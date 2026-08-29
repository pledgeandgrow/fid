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
static const char _str0[] = "called unwrap on None";
static const char _str1[] = "fn";
static const char _str2[] = "val";
static const char _str3[] = "var";
static const char _str4[] = "mut";
static const char _str5[] = "const";
static const char _str6[] = "given";
static const char _str7[] = "ensure";
static const char _str8[] = "cap";
static const char _str9[] = "type";
static const char _str10[] = "return";
static const char _str11[] = "if";
static const char _str12[] = "else";
static const char _str13[] = "match";
static const char _str14[] = "select";
static const char _str15[] = "struct";
static const char _str16[] = "enum";
static const char _str17[] = "impl";
static const char _str18[] = "pub";
static const char _str19[] = "use";
static const char _str20[] = "module";
static const char _str21[] = "with";
static const char _str22[] = "async";
static const char _str23[] = "await";
static const char _str24[] = "comptime";
static const char _str25[] = "defer";
static const char _str26[] = "spawn";
static const char _str27[] = "move";
static const char _str28[] = "static";
static const char _str29[] = "try";
static const char _str30[] = "in";
static const char _str31[] = "as";
static const char _str32[] = "dyn";
static const char _str33[] = "where";
static const char _str34[] = "region";
static const char _str35[] = "sandbox";
static const char _str36[] = "unsafe";
static const char _str37[] = "txn";
static const char _str38[] = "persist";
static const char _str39[] = "for";
static const char _str40[] = "while";
static const char _str41[] = "loop";
static const char _str42[] = "break";
static const char _str43[] = "continue";
static const char _str44[] = "handle";
static const char _str45[] = "do";
static const char _str46[] = "interrupt";
static const char _str47[] = "extern";
static const char _str48[] = "guard";
static const char _str49[] = "is";
static const char _str50[] = "true";
static const char _str51[] = "false";
static const char _str52[] = "layer";
static const char _str53[] = "model";
static const char _str54[] = "grad";
static const char _str55[] = "backward";
static const char _str56[] = "kernel";
static const char _str57[] = "component";
static const char _str58[] = "system";
static const char _str59[] = "query";
static const char _str60[] = "state";
static const char _str61[] = "on";
static const char _str62[] = "FloatLit(";
static const char _str63[] = ")";
static const char _str64[] = "\"";
static const char _str65[] = "FString(\"";
static const char _str66[] = "\")";
static const char _str67[] = "EOF";
static const char _str68[] = "I8";
static const char _str69[] = "I16";
static const char _str70[] = "I32";
static const char _str71[] = "I64";
static const char _str72[] = "I128";
static const char _str73[] = "U8";
static const char _str74[] = "U16";
static const char _str75[] = "U32";
static const char _str76[] = "U64";
static const char _str77[] = "U128";
static const char _str78[] = "F32";
static const char _str79[] = "F64";
static const char _str80[] = "Bool";
static const char _str81[] = "Str";
static const char _str82[] = "Void";
static const char _str83[] = "Plus";
static const char _str84[] = "Minus";
static const char _str85[] = "Star";
static const char _str86[] = "Slash";
static const char _str87[] = "Percent";
static const char _str88[] = "Eq";
static const char _str89[] = "EqEq";
static const char _str90[] = "BangEq";
static const char _str91[] = "Lt";
static const char _str92[] = "Gt";
static const char _str93[] = "LtEq";
static const char _str94[] = "GtEq";
static const char _str95[] = "Bang";
static const char _str96[] = "And";
static const char _str97[] = "Ampersand";
static const char _str98[] = "Or";
static const char _str99[] = "Pipe";
static const char _str100[] = "PipeArrow";
static const char _str101[] = "Caret";
static const char _str102[] = "Tilde";
static const char _str103[] = "Shl";
static const char _str104[] = "Shr";
static const char _str105[] = "DotDot";
static const char _str106[] = "DotDotEq";
static const char _str107[] = "TildeArrow";
static const char _str108[] = "PlusEq";
static const char _str109[] = "MinusEq";
static const char _str110[] = "StarEq";
static const char _str111[] = "SlashEq";
static const char _str112[] = "PercentEq";
static const char _str113[] = "AmpEq";
static const char _str114[] = "PipeEq";
static const char _str115[] = "CaretEq";
static const char _str116[] = "ShlEq";
static const char _str117[] = "ShrEq";
static const char _str118[] = "Arrow";
static const char _str119[] = "FatArrow";
static const char _str120[] = "Dot";
static const char _str121[] = "Comma";
static const char _str122[] = "Colon";
static const char _str123[] = "Semicolon";
static const char _str124[] = "DoubleColon";
static const char _str125[] = "Question";
static const char _str126[] = "QuestionQuestion";
static const char _str127[] = "QuestionDot";
static const char _str128[] = "Hash";
static const char _str129[] = "At";
static const char _str130[] = "LBracket";
static const char _str131[] = "RBracket";
static const char _str132[] = "LParen";
static const char _str133[] = "RParen";
static const char _str134[] = "LBrace";
static const char _str135[] = "RBrace";
static const char _str136[] = "expected '\"' after r";
static const char _str137[] = "";
static const char _str138[] = "unterminated raw string";
static const char _str139[] = "\n";
static const char _str140[] = "Assertion failed";
static const char _str141[] = " ";
static const char _str142[] = "[";
static const char _str143[] = "]";
static const char _str144[] = ", ";
static const char _str145[] = "Error:";
static const char _str146[] = ":";
static const char _str147[] = ": ";
static const char _str148[] = ".0";
static const char _str149[] = "\\n";
static const char _str150[] = "\\t";
static const char _str151[] = "\\r";
static const char _str152[] = "\\\\";
static const char _str153[] = "\\\"";
static const char _str154[] = "\\0";
static const char _str155[] = "--";
static const char _str156[] = "usage: fidesc run test_lexer_harness.fd -- <file.fd>\n";
static const char _str157[] = "   or: ./test_lexer_harness <file.fd>\n";
static const char _str158[] = "error: could not read file or empty file: ";
static const char _str159[] = "  ";
static const char _str160[] = "q8";
static const char _str161[] = "q4";
static const char _str162[] = "i8";
static const char _str163[] = "i16";
static const char _str164[] = "i32";
static const char _str165[] = "i64";
static const char _str166[] = "i128";
static const char _str167[] = "u8";
static const char _str168[] = "u16";
static const char _str169[] = "u32";
static const char _str170[] = "u64";
static const char _str171[] = "u128";
static const char _str172[] = "f32";
static const char _str173[] = "f64";
static const char _str174[] = "bool";
static const char _str175[] = "str";
static const char _str176[] = "void";
static const char _str177[] = "Fn";
static const char _str178[] = "Val";
static const char _str179[] = "Var";
static const char _str180[] = "Mut";
static const char _str181[] = "Const";
static const char _str182[] = "Given";
static const char _str183[] = "Ensure";
static const char _str184[] = "Cap";
static const char _str185[] = "Type";
static const char _str186[] = "Return";
static const char _str187[] = "If";
static const char _str188[] = "Else";
static const char _str189[] = "Match";
static const char _str190[] = "Select";
static const char _str191[] = "Struct";
static const char _str192[] = "Enum";
static const char _str193[] = "Impl";
static const char _str194[] = "Pub";
static const char _str195[] = "Use";
static const char _str196[] = "Module";
static const char _str197[] = "With";
static const char _str198[] = "Async";
static const char _str199[] = "Await";
static const char _str200[] = "Comptime";
static const char _str201[] = "Defer";
static const char _str202[] = "Spawn";
static const char _str203[] = "Move";
static const char _str204[] = "Static";
static const char _str205[] = "Try";
static const char _str206[] = "In";
static const char _str207[] = "As";
static const char _str208[] = "Dyn";
static const char _str209[] = "Where";
static const char _str210[] = "Region";
static const char _str211[] = "Sandbox";
static const char _str212[] = "Unsafe";
static const char _str213[] = "Txn";
static const char _str214[] = "Persist";
static const char _str215[] = "For";
static const char _str216[] = "While";
static const char _str217[] = "Loop";
static const char _str218[] = "Break";
static const char _str219[] = "Continue";
static const char _str220[] = "Handle";
static const char _str221[] = "Do";
static const char _str222[] = "Interrupt";
static const char _str223[] = "Extern";
static const char _str224[] = "Guard";
static const char _str225[] = "Is";
static const char _str226[] = "True";
static const char _str227[] = "False";
static const char _str228[] = "Layer";
static const char _str229[] = "Model";
static const char _str230[] = "Grad";
static const char _str231[] = "Backward";
static const char _str232[] = "Kernel";
static const char _str233[] = "Component";
static const char _str234[] = "System";
static const char _str235[] = "Query";
static const char _str236[] = "State";
static const char _str237[] = "On";
static const char _str238[] = "IntLit(";
static const char _str239[] = "StringLit(\"";
static const char _str240[] = "Ident(\"";
static const char _str241[] = "Eof";
static const char _str242[] = "\t";
static const char _str243[] = "\r";
static const char _str244[] = "\\";
static const char _str245[] = "unknown escape sequence \\";
static const char _str246[] = "unterminated string";
static const char _str247[] = "{";
static const char _str248[] = "}";
static const char _str249[] = "unterminated f-string";
static const char _str250[] = "called unwrap on Err";
static const char _str251[] = "unexpected character '";
static const char _str252[] = "'";

// ─── Struct definitions ───
struct TokenKind_s {
    int64_t field_0;
    int64_t field_1;
};

struct Lexer_s {
    void* field_0;
    int64_t field_1;
    int64_t field_2;
    int64_t field_3;
    void* field_4;
};

struct Option_s {
    int64_t field_0;
    int64_t field_1;
};

struct ByteBuffer_s {
    void* field_0;
    int64_t field_1;
};

struct Result_s {
    int64_t field_0;
    int64_t field_1;
};

struct Token_s {
    struct TokenKind_s* field_0;
    int64_t field_1;
    int64_t field_2;
};

struct Vec_s {
    int64_t field_0;
    int64_t field_1;
    int64_t field_2;
};

struct StringBuilder_s {
    void* field_0;
    int64_t field_1;
};

// ─── Function prototypes ───
void p_thread_register(const char* name, int64_t (*fn)());
int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2);
void vec_dedup(void* v0, int64_t v1);
int64_t parse_radix(void* v0, void* v1, int64_t v2);
int64_t String_clone(void* v0, int64_t v1);
int64_t HashMap_keys(void* v0, int64_t v1);
bool is_word_keyword(void* v0, void* v1);
void* str_split(void* v0, void* v1, void* v2);
int64_t Option_unwrap(void* v0, void* v1);
void* str_to_upper(void* v0, void* v1);
bool Vec_all(void* v0, void* v1, void* v2);
void Lexer_skip_block_comment(void* v0, void* v1);
int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3);
void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3);
int64_t vec_window(void* v0, int64_t v1, int64_t v2);
bool p_str_equal(void* v0, int64_t v1, int64_t v2);
int64_t Vec_len(void* v0, void* v1);
int64_t p_str_hash(void* v0, int64_t v1);
bool Vec_contains_val(void* v0, void* v1, int64_t v2);
int64_t String_trim(void* v0, int64_t v1);
int64_t allocator_bytes_used(void* v0, void* v1);
bool String_contains(void* v0, int64_t v1, int64_t v2);
void HashMap_grow(void* v0, int64_t v1);
void* token_kind_to_str(void* v0, void* v1);
void Lexer_read_raw_string(void* v0, void* v1, void* v2);
void Option_map(void* v0, void* v1, void* v2, void* v3);
void vec_sort_by(void* v0, int64_t v1, void* v2);
void println(void* v0, void* v1);
int64_t Result_expect(void* v0, void* v1, void* v2);
void* Vec_window(void* v0, void* v1, int64_t v2);
void StringBuilder_append_char(void* v0, void* v1, int64_t v2);
int64_t ByteBuffer_len(void* v0, void* v1);
void* ByteBuffer_to_str(void* v0, void* v1);
int64_t str_compare(void* v0, void* v1, void* v2);
int64_t Vec_remove(void* v0, void* v1, int64_t v2);
void HashMap_remove(void* v0, int64_t v1, int64_t v2);
int64_t Lexer_peek_at(void* v0, void* v1, int64_t v2);
int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2);
void* str_from_bytes(void* v0, void* v1);
void* str_to_lower(void* v0, void* v1);
bool Vec_is_empty(void* v0, void* v1);
void assert(void* v0, bool v1);
int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3);
void Lexer_read_identifier_or_keyword(void* v0, void* v1, void* v2);
bool str_ends_with(void* v0, void* v1, void* v2);
int64_t Lexer_advance(void* v0, void* v1);
void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2);
void* pad_right(void* v0, int64_t v1, int64_t v2);
void StringBuilder_clear(void* v0, void* v1);
void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3);
void* pad_left(void* v0, int64_t v1, int64_t v2);
int64_t str_find(void* v0, void* v1, void* v2);
int64_t String_char_at(void* v0, int64_t v1, int64_t v2);
void* char_to_str(void* v0, int64_t v1);
int64_t Vec_pop(void* v0, void* v1);
int64_t String_repeat(void* v0, int64_t v1, int64_t v2);
void Vec_sort(void* v0, void* v1);
int64_t str_parse_hex(void* v0, void* v1);
bool HashMap_is_empty(void* v0, int64_t v1);
void HashMap_clear(void* v0, int64_t v1);
int64_t HashMap_get(void* v0, int64_t v1, int64_t v2);
bool Vec_contains(void* v0, void* v1, int64_t v2);
void* byte_to_str(void* v0, int64_t v1);
int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2);
int64_t String_find(void* v0, int64_t v1, int64_t v2);
bool lex_is_digit(void* v0, int64_t v1);
int64_t String_bytes(void* v0, int64_t v1);
void* Vec_to_str(void* v0, int64_t v1);
int64_t Vec_last(void* v0, void* v1);
int64_t String_to_string(void* v0, int64_t v1);
int64_t str_byte_at(void* v0, void* v1, int64_t v2);
int64_t Lexer_peek(void* v0, void* v1);
void* Lexer_read_utf8(void* v0, void* v1);
bool char_is_upper(void* v0, int64_t v1);
int64_t Vec_fold(void* v0, void* v1, int64_t v2, void* v3);
void Vec_clear(void* v0, void* v1);
bool str_equal(void* v0, void* v1, void* v2);
void* lex_error(void* v0, int64_t v1, int64_t v2, void* v3);
void* float_to_str(void* v0, double v1);
void Lexer_read_number(void* v0, void* v1, void* v2);
bool Vec_any(void* v0, void* v1, void* v2);
void Vec_dedup(void* v0, void* v1);
void* repeat_char(void* v0, int64_t v1, int64_t v2);
int64_t Vec_capacity(void* v0, void* v1);
void* float_debug(void* v0, double v1);
int64_t Vec_get(void* v0, void* v1, int64_t v2);
void Vec_sort_by(void* v0, void* v1, void* v2);
int64_t Vec_find(void* v0, void* v1, void* v2);
void* Vec_enumerate(void* v0, void* v1);
bool Option_is_none(void* v0, void* v1);
void Vec_insert(void* v0, void* v1, int64_t v2, int64_t v3);
void Option_and_then(void* v0, void* v1, void* v2, void* v3);
void Option_or_else(void* v0, void* v1, void* v2, void* v3);
int64_t String_rfind(void* v0, int64_t v1, int64_t v2);
int64_t Option_expect(void* v0, void* v1, void* v2);
void Vec_set(void* v0, void* v1, int64_t v2, int64_t v3);
void Vec_reverse(void* v0, void* v1);
bool Result_is_err(void* v0, void* v1);
void Result_err(void* v0, void* v1, void* v2);
void StringBuilder_new(void* v0, void* v1);
bool lex_is_alphanumeric(void* v0, int64_t v1);
void StringBuilder_append_int(void* v0, void* v1, int64_t v2);
int64_t vec_skip(void* v0, int64_t v1, int64_t v2);
void ByteBuffer_clear(void* v0, void* v1);
int64_t Vec_position(void* v0, void* v1, void* v2);
int64_t p_max(void* v0, int64_t v1, int64_t v2);
void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_len(void* v0, int64_t v1);
void* create_allocator(void* v0, void* v1);
bool str_is_empty(void* v0, void* v1);
int64_t p_min(void* v0, int64_t v1, int64_t v2);
int64_t allocator_alloc(void* v0, void* v1, int64_t v2);
int64_t vec_enumerate(void* v0, int64_t v1);
int64_t str_parse_int(void* v0, void* v1);
int64_t p_str_length(void* v0, int64_t v1);
void* Vec_chunk(void* v0, void* v1, int64_t v2);
void* str_escape(void* v0, void* v1);
int64_t vec_flat_map(void* v0, int64_t v1, void* v2);
bool HashMap_contains(void* v0, int64_t v1, int64_t v2);
void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3);
void Vec_push(void* v0, void* v1, int64_t v2);
void* str_bytes(void* v0, void* v1);
bool str_contains(void* v0, void* v1, void* v2);
int64_t main_entry(void* v0);
void keyword_or_ident(void* v0, void* v1, void* v2);
int64_t String_char_count(void* v0, int64_t v1);
int64_t str_rfind(void* v0, void* v1, void* v2);
void* str_trim_start(void* v0, void* v1);
void token_new(void* v0, void* v1, void* v2, int64_t v3, int64_t v4);
bool char_is_whitespace(void* v0, int64_t v1);
void* int_to_str(void* v0, int64_t v1);
void Lexer_new(void* v0, void* v1, void* v2);
void Lexer_skip_line_comment(void* v0, void* v1);
void* token_kind_debug(void* v0, void* v1);
void Lexer_read_string(void* v0, void* v1, void* v2);
int64_t HashMap_clone(void* v0, int64_t v1);
bool char_is_digit(void* v0, int64_t v1);
void* Vec_flat_map(void* v0, void* v1, void* v2);
int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2);
void* Vec_map(void* v0, void* v1, void* v2);
int64_t String_to_uppercase(void* v0, int64_t v1);
void Result_map_err(void* v0, void* v1, void* v2, void* v3);
int64_t StringBuilder_len(void* v0, void* v1);
bool StringBuilder_is_empty(void* v0, void* v1);
void ByteBuffer_new(void* v0, void* v1, int64_t v2);
void* Vec_filter(void* v0, void* v1, void* v2);
void* Lexer_tokenize(void* v0, void* v1);
void* Vec_skip(void* v0, void* v1, int64_t v2);
int64_t String_byte_at(void* v0, int64_t v1, int64_t v2);
int64_t Vec_first(void* v0, void* v1);
bool Result_is_ok(void* v0, void* v1);
void Vec_truncate(void* v0, void* v1, int64_t v2);
void Result_ok(void* v0, void* v1, void* v2);
int64_t HashMap_len(void* v0, int64_t v1);
bool String_ends_with(void* v0, int64_t v1, int64_t v2);
int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool char_is_lower(void* v0, int64_t v1);
void Lexer_read_fstring(void* v0, void* v1, void* v2);
bool lex_is_hexdigit(void* v0, int64_t v1);
int64_t vec_zip(void* v0, int64_t v1, int64_t v2);
int64_t HashMap_values(void* v0, int64_t v1);
void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3);
bool String_is_empty(void* v0, int64_t v1);
int64_t char_to_lower(void* v0, int64_t v1);
int64_t vec_take(void* v0, int64_t v1, int64_t v2);
void* str_replace(void* v0, void* v1, void* v2, void* v3);
void* str_trim_end(void* v0, void* v1);
void* str_repeat(void* v0, void* v1, int64_t v2);
int64_t String_push_str(void* v0, int64_t v1, int64_t v2);
void* HashMap_to_str(void* v0, int64_t v1);
int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3);
int64_t String_parse_int(void* v0, int64_t v1);
int64_t Result_unwrap(void* v0, void* v1);
void Result_map(void* v0, void* v1, void* v2, void* v3);
void assert_msg(void* v0, bool v1, void* v2);
bool ByteBuffer_is_empty(void* v0, void* v1);
void Vec_for_each(void* v0, void* v1, void* v2);
int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2);
void* str_trim(void* v0, void* v1);
void Vec_extend(void* v0, void* v1, void* v2);
int64_t HashMap_new(void* v0);
void ByteBuffer_from_string(void* v0, void* v1, void* v2);
int64_t str_len(void* v0, void* v1);
bool String_starts_with(void* v0, int64_t v1, int64_t v2);
int64_t String_from(void* v0, int64_t v1);
bool char_is_alpha(void* v0, int64_t v1);
int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2);
int64_t token_kind_tag(void* v0, void* v1);
int64_t char_to_upper(void* v0, int64_t v1);
void Result_and_then(void* v0, void* v1, void* v2, void* v3);
void* StringBuilder_to_str(void* v0, void* v1);
bool str_starts_with(void* v0, void* v1, void* v2);
void StringBuilder_append_str(void* v0, void* v1, void* v2);
int64_t vec_chunk(void* v0, int64_t v1, int64_t v2);
double str_parse_float(void* v0, void* v1);
int64_t String_to_lowercase(void* v0, int64_t v1);
bool lex_is_whitespace(void* v0, int64_t v1);
int64_t p_abs(void* v0, int64_t v1);
void* p_str_from_ptr(void* v0, int64_t v1);
void* str_split_char(void* v0, void* v1, int64_t v2);
bool token_kind_eq(void* v0, void* v1, void* v2);
void allocator_free(void* v0, void* v1, int64_t v2);
void* str_concat(void* v0, void* v1, void* v2);
void Lexer_read_operator(void* v0, void* v1, void* v2);
void* Vec_take(void* v0, void* v1, int64_t v2);
void* Vec_zip(void* v0, void* v1, void* v2);
int64_t String_split(void* v0, int64_t v1, int64_t v2);
int64_t p_str_compare(void* v0, int64_t v1, int64_t v2);
bool Option_is_some(void* v0, void* v1);
void* Vec_clone(void* v0, void* v1);
void* Vec_new(void* v0);
bool lex_is_alpha(void* v0, int64_t v1);
bool vec_contains_val(void* v0, int64_t v1, int64_t v2);

int64_t ByteBuffer_read_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_read_byte");
    int64_t v9; /* placeholder for void */
    void* v5;
    bool v8;
    int64_t v10;
    void* v15;
    int64_t v13; /* placeholder for void */
    int64_t v16;
    int64_t v12; /* placeholder for void */
    int64_t v17;
    int64_t v11;
    int64_t v3;
    void* v14;
    bool v4;
    int64_t v6;
    bool v7;
    int64_t v18; /* placeholder for void */
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

void vec_dedup(void* v0, int64_t v1) {
    p_call_stack_push("vec_dedup");
    int64_t v17; /* placeholder for void */
    int64_t v19;
    int64_t v23;
    int64_t v27;
    int64_t v33;
    bool v21;
    int64_t v34;
    int64_t v39;
    int64_t v42; /* placeholder for void */
    int64_t v45;
    int64_t v15;
    int64_t v37;
    int64_t v26;
    int64_t v46;
    int64_t v47;
    int64_t v48; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v16;
    int64_t v22; /* placeholder for void */
    int64_t v35;
    int64_t v51; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v36;
    int64_t v7;
    void* v4;
    int64_t v41;
    int64_t v12;
    int64_t v25;
    int64_t v38; /* placeholder for void */
    bool v31;
    int64_t v40;
    int64_t v50;
    void* v13;
    int64_t v10; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    bool v8;
    int64_t v5; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v3;
    int64_t v6;
    int64_t v24;
    int64_t v2;
    int64_t v29;
    int64_t v30;
    int64_t v9; /* placeholder for void */
    int64_t v20;
    int64_t v28;
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

int64_t parse_radix(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("parse_radix");
    void* v21;
    int64_t v72; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v6;
    int64_t v12; /* placeholder for void */
    bool v46;
    int64_t v30;
    int64_t v74;
    int64_t v83; /* placeholder for void */
    bool v29;
    bool v45;
    int64_t v38; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    bool v42;
    void* v4;
    bool v55;
    int64_t v85; /* placeholder for void */
    bool v70;
    int64_t v76;
    int64_t v79;
    bool v16;
    bool v59;
    int64_t v62;
    int64_t v51; /* placeholder for void */
    int64_t v54;
    int64_t v84;
    int64_t v50;
    void* v11;
    int64_t v8; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v48;
    int64_t v49;
    int64_t v13; /* placeholder for void */
    int64_t v19;
    int64_t v18;
    int64_t v10;
    int64_t v31;
    int64_t v52; /* placeholder for void */
    int64_t v57;
    int64_t v67; /* placeholder for void */
    int64_t v80;
    int64_t v47; /* placeholder for void */
    int64_t v41;
    int64_t v9;
    int64_t v68; /* placeholder for void */
    int64_t v14;
    int64_t v63;
    int64_t v34; /* placeholder for void */
    int64_t v37;
    int64_t v43;
    int64_t v28;
    int64_t v81;
    int64_t v26; /* placeholder for void */
    int64_t v75;
    int64_t v40;
    int64_t v66; /* placeholder for void */
    int64_t v69;
    int64_t v73; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v35;
    int64_t v77;
    int64_t v53;
    int64_t v27;
    int64_t v24;
    int64_t v61;
    int64_t v39; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v32;
    bool v33;
    void* v7;
    int64_t v23;
    int64_t v60; /* placeholder for void */
    bool v58;
    int64_t v56;
    int64_t v82; /* placeholder for void */
    int64_t v44;
    int64_t v65; /* placeholder for void */
    void* v25;
    int64_t v22; /* placeholder for void */
    int64_t v36;
    int64_t v15;
    int64_t v20;
    int64_t v3;
    v3 = 0LL;
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = 0LL;
    v10 = (int64_t)str_len((void*)v9, v1);
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
    v20 = (int64_t)str_byte_at((void*)v19, v1, v18);
    v21 = malloc(8);
    *(int64_t*)v21 = (int64_t)v20;
    v23 = 1LL;
    v24 = -v23;
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v21;
    v28 = 48LL;
    v29 = v27 >= v28;
    v30 = *(int64_t*)v21;
    v31 = 57LL;
    v32 = v30 <= v31;
    v33 = v29 && v32;
    if (v33) goto bb4;
    else goto bb5;
  bb3:
    v84 = *(int64_t*)v4;
    p_call_stack_pop();
    return v84;
  bb4:
    v35 = *(int64_t*)v21;
    v36 = 48LL;
    v37 = v35 - v36;
    *(int64_t*)v25 = (int64_t)v37;
    goto bb6;
  bb5:
    v40 = *(int64_t*)v21;
    v41 = 65LL;
    v42 = v40 >= v41;
    v43 = *(int64_t*)v21;
    v44 = 90LL;
    v45 = v43 <= v44;
    v46 = v42 && v45;
    if (v46) goto bb7;
    else goto bb8;
  bb6:
    v69 = *(int64_t*)v25;
    v70 = v69 >= v2;
    if (v70) goto bb13;
    else goto bb14;
  bb7:
    v48 = *(int64_t*)v21;
    v49 = 55LL;
    v50 = v48 - v49;
    *(int64_t*)v25 = (int64_t)v50;
    goto bb9;
  bb8:
    v53 = *(int64_t*)v21;
    v54 = 97LL;
    v55 = v53 >= v54;
    v56 = *(int64_t*)v21;
    v57 = 122LL;
    v58 = v56 <= v57;
    v59 = v55 && v58;
    if (v59) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v61 = *(int64_t*)v21;
    v62 = 87LL;
    v63 = v61 - v62;
    *(int64_t*)v25 = (int64_t)v63;
    goto bb12;
  bb11:
    goto bb3;
  bb12:
    goto bb9;
  bb13:
    goto bb3;
  bb14:
    goto bb15;
  bb15:
    v74 = *(int64_t*)v4;
    v75 = v74 * v2;
    v76 = *(int64_t*)v25;
    v77 = v75 + v76;
    *(int64_t*)v4 = (int64_t)v77;
    v79 = *(int64_t*)v7;
    v80 = 1LL;
    v81 = v79 + v80;
    *(int64_t*)v7 = (int64_t)v81;
    goto bb1;
    return 0;
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

int64_t HashMap_keys(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_keys");
    int64_t v10; /* placeholder for void */
    void* v53;
    bool v32;
    int64_t v62; /* placeholder for void */
    bool v70;
    int64_t v57; /* placeholder for void */
    int64_t v64;
    int64_t v81; /* placeholder for void */
    int64_t v2;
    void* v50;
    int64_t v23;
    bool v43;
    int64_t v11;
    int64_t v67;
    int64_t v85; /* placeholder for void */
    int64_t v41;
    int64_t v22;
    int64_t v27;
    int64_t v8;
    int64_t v48;
    int64_t v42;
    int64_t v51; /* placeholder for void */
    void* v4;
    int64_t v55;
    bool v65;
    int64_t v76; /* placeholder for void */
    int64_t v84;
    int64_t v73;
    int64_t v30;
    int64_t v52;
    int64_t v20; /* placeholder for void */
    void* v56;
    int64_t v7; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v68;
    int64_t v78;
    int64_t v17; /* placeholder for void */
    int64_t v58; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v46;
    void* v6;
    int64_t v34;
    void* v38;
    int64_t v3;
    int64_t v63;
    int64_t v66;
    void* v9;
    int64_t v19;
    bool v31;
    int64_t v37;
    int64_t v35; /* placeholder for void */
    bool v69;
    int64_t v13; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v79;
    void* v12;
    int64_t v39; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v45;
    int64_t v49;
    void* v59;
    int64_t v72;
    int64_t v44; /* placeholder for void */
    void* v24;
    int64_t v77; /* placeholder for void */
    int64_t v47;
    int64_t v33; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v26;
    int64_t v60;
    void* v15;
    void* v18;
    int64_t v25; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v16;
    int64_t v71; /* placeholder for void */
    int64_t v74;
    int64_t v83;
    int64_t v87; /* placeholder for void */
    bool v28;
    int64_t v86;
    int64_t v80;
    int64_t v14; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v29;
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

bool is_word_keyword(void* v0, void* v1) {
    p_call_stack_push("is_word_keyword");
    bool v745;
    bool v869;
    bool v829;
    void* v593;
    int64_t v805;
    int64_t v679;
    int64_t v186;
    int64_t v648; /* placeholder for void */
    bool v946;
    int64_t v139; /* placeholder for void */
    int64_t v242;
    int64_t v396;
    int64_t v302; /* placeholder for void */
    int64_t v283;
    int64_t v823; /* placeholder for void */
    bool v724;
    int64_t v865; /* placeholder for void */
    bool v610;
    bool v63;
    void* v142;
    int64_t v284;
    void* v607;
    bool v843;
    bool v91;
    int64_t v136;
    bool v532;
    void* v422;
    int64_t v776;
    int64_t v174; /* placeholder for void */
    int64_t v162; /* placeholder for void */
    bool v287;
    int64_t v636;
    bool v231;
    bool v925;
    bool v313;
    int64_t v347;
    bool v904;
    int64_t v123;
    int64_t v167; /* placeholder for void */
    void* v2;
    int64_t v111; /* placeholder for void */
    void* v663;
    int64_t v749;
    bool v392;
    int64_t v798;
    int64_t v143;
    void* v415;
    int64_t v288; /* placeholder for void */
    void* v733;
    void* v782;
    bool v582;
    bool v168;
    bool v446;
    int64_t v644;
    int64_t v793; /* placeholder for void */
    void* v359;
    int64_t v83; /* placeholder for void */
    bool v301;
    void* v614;
    int64_t v730; /* placeholder for void */
    bool v939;
    bool v941;
    int64_t v29; /* placeholder for void */
    int64_t v276;
    bool v294;
    int64_t v328; /* placeholder for void */
    bool v633;
    int64_t v398; /* placeholder for void */
    bool v638;
    bool v871;
    bool v320;
    void* v261;
    int64_t v368;
    int64_t v622;
    void* v499;
    int64_t v517; /* placeholder for void */
    bool v626;
    int64_t v811;
    int64_t v910;
    int64_t v249;
    void* v485;
    int64_t v311;
    bool v432;
    void* v570;
    bool v773;
    bool v827;
    bool v173;
    int64_t v150;
    int64_t v377; /* placeholder for void */
    bool v766;
    bool v897;
    bool v958;
    void* v268;
    int64_t v403;
    int64_t v435; /* placeholder for void */
    int64_t v171;
    int64_t v384; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    bool v834;
    void* v635;
    int64_t v674; /* placeholder for void */
    bool v292;
    bool v369;
    bool v425;
    int64_t v615;
    bool v652;
    bool v913;
    int64_t v919; /* placeholder for void */
    int64_t v480;
    void* v831;
    int64_t v452;
    int64_t v514;
    int64_t v949; /* placeholder for void */
    bool v40;
    bool v792;
    bool v462;
    bool v890;
    void* v58;
    void* v908;
    int64_t v132; /* placeholder for void */
    void* v642;
    void* v51;
    bool v357;
    int64_t v94;
    void* v450;
    int64_t v783;
    bool v906;
    int64_t v606; /* placeholder for void */
    int64_t v571;
    int64_t v137;
    int64_t v877; /* placeholder for void */
    int64_t v847;
    bool v250;
    int64_t v896;
    bool v701;
    bool v673;
    int64_t v951;
    bool v327;
    int64_t v758; /* placeholder for void */
    int64_t v574; /* placeholder for void */
    int64_t v209; /* placeholder for void */
    int64_t v959; /* placeholder for void */
    int64_t v854;
    int64_t v874;
    int64_t v655; /* placeholder for void */
    bool v19;
    int64_t v87;
    bool v736;
    int64_t v844; /* placeholder for void */
    int64_t v409;
    int64_t v48; /* placeholder for void */
    bool v383;
    void* v289;
    bool v469;
    bool v343;
    bool v411;
    bool v439;
    int64_t v237; /* placeholder for void */
    void* v561;
    int64_t v430;
    int64_t v760; /* placeholder for void */
    int64_t v630;
    int64_t v459;
    bool v892;
    int64_t v903;
    void* v275;
    bool v110;
    int64_t v938;
    bool v481;
    bool v434;
    int64_t v379; /* placeholder for void */
    int64_t v67;
    int64_t v95;
    void* v212;
    int64_t v176; /* placeholder for void */
    void* v845;
    int64_t v933; /* placeholder for void */
    int64_t v643;
    int64_t v90; /* placeholder for void */
    int64_t v279; /* placeholder for void */
    bool v857;
    int64_t v522;
    bool v161;
    int64_t v608;
    int64_t v889;
    bool v413;
    void* v824;
    bool v42;
    int64_t v246; /* placeholder for void */
    int64_t v314; /* placeholder for void */
    int64_t v116;
    int64_t v164;
    int64_t v860;
    int64_t v244; /* placeholder for void */
    int64_t v179;
    int64_t v856; /* placeholder for void */
    int64_t v17;
    bool v743;
    bool v147;
    int64_t v71; /* placeholder for void */
    void* v936;
    void* v513;
    int64_t v594;
    bool v273;
    int64_t v449; /* placeholder for void */
    int64_t v875;
    bool v573;
    void* v894;
    int64_t v267; /* placeholder for void */
    int64_t v199;
    bool v694;
    int64_t v262;
    int64_t v391; /* placeholder for void */
    bool v474;
    void* v838;
    int64_t v272; /* placeholder for void */
    int64_t v553;
    void* v86;
    int64_t v125; /* placeholder for void */
    bool v210;
    int64_t v611; /* placeholder for void */
    int64_t v195; /* placeholder for void */
    void* v796;
    int64_t v650;
    int64_t v800; /* placeholder for void */
    bool v245;
    void* v457;
    int64_t v365; /* placeholder for void */
    void* v740;
    int64_t v298;
    int64_t v898; /* placeholder for void */
    void* v65;
    bool v82;
    int64_t v510; /* placeholder for void */
    void* v621;
    int64_t v944;
    int64_t v886; /* placeholder for void */
    int64_t v554;
    int64_t v688; /* placeholder for void */
    bool v453;
    int64_t v228;
    int64_t v551; /* placeholder for void */
    int64_t v581;
    int64_t v169; /* placeholder for void */
    int64_t v825;
    int64_t v206;
    int64_t v660; /* placeholder for void */
    int64_t v672;
    int64_t v952;
    void* v705;
    int64_t v671;
    int64_t v18;
    int64_t v297;
    int64_t v129;
    bool v278;
    bool v112;
    int64_t v560; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v867;
    int64_t v683; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v912; /* placeholder for void */
    int64_t v74;
    int64_t v190; /* placeholder for void */
    int64_t v424;
    int64_t v704; /* placeholder for void */
    int64_t v11;
    int64_t v342; /* placeholder for void */
    void* v719;
    int64_t v828; /* placeholder for void */
    void* v324;
    int64_t v706;
    void* v37;
    int64_t v66;
    int64_t v141; /* placeholder for void */
    void* v163;
    bool v371;
    int64_t v445;
    int64_t v802; /* placeholder for void */
    bool v189;
    int64_t v709; /* placeholder for void */
    int64_t v909;
    int64_t v234;
    bool v364;
    int64_t v185;
    int64_t v335; /* placeholder for void */
    int64_t v220;
    int64_t v482; /* placeholder for void */
    bool v771;
    int64_t v807; /* placeholder for void */
    void* v72;
    int64_t v721;
    int64_t v505; /* placeholder for void */
    bool v124;
    bool v927;
    int64_t v274; /* placeholder for void */
    int64_t v562;
    int64_t v41; /* placeholder for void */
    int64_t v270;
    void* v338;
    void* v177;
    void* v775;
    int64_t v319;
    int64_t v468; /* placeholder for void */
    int64_t v101;
    int64_t v428; /* placeholder for void */
    int64_t v556; /* placeholder for void */
    int64_t v681; /* placeholder for void */
    bool v710;
    bool v631;
    bool v934;
    int64_t v629;
    int64_t v784;
    int64_t v786; /* placeholder for void */
    int64_t v830; /* placeholder for void */
    bool v35;
    int64_t v849; /* placeholder for void */
    int64_t v753; /* placeholder for void */
    void* v331;
    void* v254;
    int64_t v444;
    bool v33;
    bool v5;
    int64_t v183; /* placeholder for void */
    int64_t v382;
    bool v376;
    int64_t v870; /* placeholder for void */
    int64_t v493;
    bool v140;
    int64_t v609;
    int64_t v627; /* placeholder for void */
    int64_t v491; /* placeholder for void */
    int64_t v53;
    bool v696;
    int64_t v122;
    int64_t v678;
    bool v154;
    int64_t v917;
    bool v175;
    int64_t v790;
    bool v236;
    bool v70;
    int64_t v104; /* placeholder for void */
    int64_t v10;
    int64_t v32;
    void* v93;
    int64_t v431;
    bool v119;
    bool v525;
    int64_t v762;
    bool v708;
    bool v215;
    bool v238;
    int64_t v699;
    int64_t v20; /* placeholder for void */
    void* v670;
    int64_t v8; /* placeholder for void */
    bool v511;
    int64_t v693;
    int64_t v725; /* placeholder for void */
    bool v77;
    int64_t v804;
    int64_t v814; /* placeholder for void */
    int64_t v835; /* placeholder for void */
    int64_t v839;
    int64_t v756;
    int64_t v473;
    bool v264;
    int64_t v569; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    bool v848;
    bool v878;
    int64_t v146; /* placeholder for void */
    bool v722;
    int64_t v833;
    void* v915;
    int64_t v602;
    int64_t v601;
    int64_t v916;
    bool v624;
    int64_t v118; /* placeholder for void */
    int64_t v748;
    int64_t v604; /* placeholder for void */
    int64_t v407; /* placeholder for void */
    bool v948;
    void* v506;
    int64_t v193;
    int64_t v230; /* placeholder for void */
    bool v448;
    bool v362;
    int64_t v265; /* placeholder for void */
    int64_t v370; /* placeholder for void */
    void* v649;
    bool v266;
    void* v901;
    void* v345;
    int64_t v595;
    bool v187;
    int64_t v797;
    bool v133;
    bool v523;
    bool v752;
    int64_t v821; /* placeholder for void */
    bool v920;
    bool v584;
    int64_t v842; /* placeholder for void */
    int64_t v325;
    int64_t v6; /* placeholder for void */
    void* v128;
    void* v922;
    int64_t v374;
    int64_t v931;
    int64_t v316; /* placeholder for void */
    int64_t v340;
    int64_t v309; /* placeholder for void */
    int64_t v326;
    bool v659;
    int64_t v358; /* placeholder for void */
    void* v464;
    bool v750;
    int64_t v765; /* placeholder for void */
    int64_t v477; /* placeholder for void */
    int64_t v902;
    bool v280;
    int64_t v286; /* placeholder for void */
    bool v518;
    int64_t v225; /* placeholder for void */
    bool v738;
    void* v887;
    int64_t v148; /* placeholder for void */
    int64_t v669; /* placeholder for void */
    int64_t v31;
    int64_t v779; /* placeholder for void */
    bool v815;
    int64_t v197; /* placeholder for void */
    bool v341;
    int64_t v55; /* placeholder for void */
    int64_t v24;
    bool v49;
    bool v201;
    int64_t v410;
    void* v114;
    void* v436;
    bool v61;
    bool v217;
    int64_t v791;
    int64_t v115;
    bool v645;
    int64_t v421; /* placeholder for void */
    int64_t v227;
    int64_t v218; /* placeholder for void */
    bool v404;
    bool v640;
    bool v504;
    int64_t v521;
    void* v950;
    void* v492;
    int64_t v500;
    int64_t v690; /* placeholder for void */
    int64_t v888;
    int64_t v528;
    void* v566;
    int64_t v741;
    bool v222;
    int64_t v27; /* placeholder for void */
    int64_t v945;
    int64_t v178;
    void* v539;
    int64_t v858; /* placeholder for void */
    int64_t v356; /* placeholder for void */
    bool v509;
    int64_t v127; /* placeholder for void */
    int64_t v440; /* placeholder for void */
    bool v182;
    int64_t v697; /* placeholder for void */
    int64_t v207;
    void* v929;
    int64_t v130;
    bool v661;
    int64_t v300; /* placeholder for void */
    bool v495;
    int64_t v475; /* placeholder for void */
    int64_t v846;
    void* v552;
    int64_t v172;
    int64_t v437;
    bool v759;
    bool v490;
    int64_t v620; /* placeholder for void */
    void* v567;
    int64_t v618; /* placeholder for void */
    int64_t v926; /* placeholder for void */
    bool v619;
    int64_t v36; /* placeholder for void */
    bool v441;
    int64_t v192;
    void* v429;
    int64_t v891; /* placeholder for void */
    bool v530;
    bool v390;
    int64_t v723; /* placeholder for void */
    bool v689;
    int64_t v333;
    bool v418;
    int64_t v695; /* placeholder for void */
    int64_t v349; /* placeholder for void */
    bool v806;
    int64_t v92; /* placeholder for void */
    int64_t v412; /* placeholder for void */
    int64_t v536;
    int64_t v737; /* placeholder for void */
    int64_t v414; /* placeholder for void */
    int64_t v486;
    bool v559;
    void* v121;
    bool v126;
    void* v712;
    int64_t v466;
    int64_t v565; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    void* v557;
    bool v257;
    bool v306;
    bool v794;
    void* v44;
    void* v317;
    bool v56;
    int64_t v812;
    bool v98;
    void* v296;
    bool v378;
    int64_t v826;
    bool v799;
    int64_t v255;
    int64_t v547; /* placeholder for void */
    int64_t v646; /* placeholder for void */
    bool v675;
    bool v550;
    bool v666;
    void* v600;
    bool v780;
    int64_t v515;
    bool v131;
    int64_t v503; /* placeholder for void */
    int64_t v613; /* placeholder for void */
    bool v715;
    bool v84;
    bool v778;
    bool v836;
    bool v541;
    int64_t v924;
    int64_t v942; /* placeholder for void */
    int64_t v947; /* placeholder for void */
    int64_t v144;
    bool v654;
    int64_t v393; /* placeholder for void */
    bool v89;
    int64_t v221;
    int64_t v330; /* placeholder for void */
    void* v548;
    void* v107;
    bool v271;
    bool v589;
    bool v955;
    int64_t v402;
    void* v726;
    int64_t v465;
    int64_t v388;
    int64_t v734;
    int64_t v43; /* placeholder for void */
    int64_t v954; /* placeholder for void */
    int64_t v746; /* placeholder for void */
    int64_t v290;
    bool v537;
    int64_t v727;
    void* v310;
    int64_t v454; /* placeholder for void */
    int64_t v400; /* placeholder for void */
    bool v488;
    void* v135;
    int64_t v73;
    int64_t v419; /* placeholder for void */
    bool v605;
    bool v682;
    void* v575;
    void* v817;
    int64_t v809; /* placeholder for void */
    void* v754;
    int64_t v837; /* placeholder for void */
    bool v315;
    int64_t v498; /* placeholder for void */
    int64_t v235;
    int64_t v585; /* placeholder for void */
    int64_t v816; /* placeholder for void */
    int64_t v685;
    int64_t v367;
    int64_t v508;
    void* v352;
    int64_t v438;
    bool v47;
    int64_t v458;
    int64_t v578; /* placeholder for void */
    int64_t v639; /* placeholder for void */
    int64_t v735;
    int64_t v213;
    int64_t v714;
    int64_t v700;
    int64_t v868;
    int64_t v433; /* placeholder for void */
    bool v224;
    int64_t v658;
    int64_t v769;
    bool v822;
    bool v932;
    int64_t v363; /* placeholder for void */
    int64_t v781; /* placeholder for void */
    void* v677;
    int64_t v588;
    int64_t v312;
    int64_t v470; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    bool v259;
    int64_t v260; /* placeholder for void */
    int64_t v872; /* placeholder for void */
    int64_t v616;
    int64_t v346;
    int64_t v742;
    void* v628;
    int64_t v258; /* placeholder for void */
    int64_t v239; /* placeholder for void */
    bool v329;
    int64_t v158;
    void* v478;
    bool v603;
    int64_t v381;
    bool v355;
    bool v243;
    bool v680;
    int64_t v479;
    int64_t v304;
    int64_t v461; /* placeholder for void */
    void* v534;
    void* v408;
    int64_t v360;
    int64_t v248;
    void* v698;
    int64_t v524; /* placeholder for void */
    int64_t v716; /* placeholder for void */
    void* v366;
    bool v577;
    int64_t v293; /* placeholder for void */
    int64_t v456; /* placeholder for void */
    int64_t v728;
    void* v859;
    void* v943;
    bool v757;
    void* v443;
    int64_t v375;
    bool v668;
    int64_t v113; /* placeholder for void */
    int64_t v372; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v472;
    bool v516;
    int64_t v732; /* placeholder for void */
    int64_t v720;
    int64_t v120; /* placeholder for void */
    int64_t v339;
    int64_t v487;
    int64_t v772; /* placeholder for void */
    int64_t v713;
    int64_t v361;
    int64_t v451;
    int64_t v590; /* placeholder for void */
    int64_t v923;
    void* v586;
    int64_t v59;
    int64_t v657;
    int64_t v108;
    bool v785;
    int64_t v930;
    int64_t v519; /* placeholder for void */
    int64_t v935; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    bool v117;
    int64_t v155; /* placeholder for void */
    void* v240;
    void* v684;
    int64_t v702; /* placeholder for void */
    void* v170;
    int64_t v426; /* placeholder for void */
    bool v467;
    int64_t v893; /* placeholder for void */
    int64_t v632; /* placeholder for void */
    double v549;
    int64_t v232; /* placeholder for void */
    bool v820;
    void* v880;
    bool v138;
    int64_t v905; /* placeholder for void */
    int64_t v395;
    int64_t v386; /* placeholder for void */
    int64_t v879; /* placeholder for void */
    int64_t v777;
    int64_t v819;
    void* v198;
    int64_t v592; /* placeholder for void */
    int64_t v763;
    int64_t v81;
    int64_t v545;
    int64_t v416;
    bool v194;
    void* v233;
    int64_t v940; /* placeholder for void */
    bool v427;
    bool v483;
    void* v100;
    int64_t v587;
    int64_t v533; /* placeholder for void */
    bool v855;
    int64_t v78; /* placeholder for void */
    void* v282;
    int64_t v25;
    void* v520;
    void* v527;
    int64_t v39;
    int64_t v405; /* placeholder for void */
    int64_t v651;
    bool v918;
    bool v555;
    void* v852;
    int64_t v770;
    void* v873;
    bool v502;
    bool v455;
    int64_t v214;
    int64_t v744; /* placeholder for void */
    int64_t v501;
    int64_t v921; /* placeholder for void */
    int64_t v354;
    int64_t v305;
    int64_t v840;
    bool v14;
    int64_t v216; /* placeholder for void */
    bool v350;
    int64_t v463; /* placeholder for void */
    bool v862;
    int64_t v900; /* placeholder for void */
    bool v54;
    int64_t v686;
    void* v768;
    bool v899;
    bool v75;
    int64_t v442; /* placeholder for void */
    int64_t v529;
    bool v406;
    void* v16;
    int64_t v956; /* placeholder for void */
    void* v471;
    int64_t v337; /* placeholder for void */
    int64_t v489; /* placeholder for void */
    bool v883;
    bool v596;
    int64_t v321; /* placeholder for void */
    int64_t v157;
    int64_t v4;
    int64_t v538; /* placeholder for void */
    int64_t v707;
    bool v813;
    int64_t v52;
    int64_t v653; /* placeholder for void */
    int64_t v881;
    int64_t v494;
    int64_t v85; /* placeholder for void */
    int64_t v3;
    int64_t v739; /* placeholder for void */
    int64_t v344; /* placeholder for void */
    void* v656;
    bool v336;
    int64_t v895;
    void* v9;
    int64_t v751; /* placeholder for void */
    int64_t v937;
    int64_t v50; /* placeholder for void */
    bool v105;
    int64_t v851; /* placeholder for void */
    bool v285;
    bool v348;
    int64_t v774; /* placeholder for void */
    void* v579;
    int64_t v307; /* placeholder for void */
    int64_t v884; /* placeholder for void */
    bool v420;
    bool v546;
    bool v196;
    bool v787;
    int64_t v60;
    int64_t v634; /* placeholder for void */
    bool v299;
    bool v28;
    void* v205;
    int64_t v256;
    int64_t v447; /* placeholder for void */
    int64_t v526; /* placeholder for void */
    void* v558;
    bool v598;
    void* v156;
    bool v460;
    bool v717;
    bool v476;
    int64_t v542; /* placeholder for void */
    int64_t v540;
    int64_t v188; /* placeholder for void */
    int64_t v134; /* placeholder for void */
    int64_t v718; /* placeholder for void */
    int64_t v88;
    int64_t v544;
    int64_t v641; /* placeholder for void */
    bool v152;
    bool v21;
    int64_t v223; /* placeholder for void */
    bool v385;
    int64_t v389;
    int64_t v623;
    bool v647;
    bool v731;
    void* v747;
    bool v876;
    int64_t v202; /* placeholder for void */
    void* v149;
    bool v308;
    void* v191;
    int64_t v62; /* placeholder for void */
    int64_t v818;
    int64_t v863; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    bool v145;
    void* v866;
    int64_t v907; /* placeholder for void */
    bool v103;
    int64_t v318;
    int64_t v531; /* placeholder for void */
    void* v576;
    int64_t v599; /* placeholder for void */
    bool v166;
    bool v397;
    int64_t v664;
    int64_t v957; /* placeholder for void */
    void* v184;
    void* v303;
    int64_t v535;
    int64_t v323; /* placeholder for void */
    int64_t v572;
    bool v808;
    bool v764;
    int64_t v832;
    int64_t v109;
    bool v7;
    int64_t v80;
    int64_t v512; /* placeholder for void */
    bool v885;
    int64_t v204; /* placeholder for void */
    int64_t v269;
    void* v247;
    int64_t v882;
    int64_t v496; /* placeholder for void */
    int64_t v928; /* placeholder for void */
    int64_t v667; /* placeholder for void */
    bool v841;
    bool v208;
    void* v219;
    int64_t v755;
    bool v96;
    int64_t v277;
    int64_t v211; /* placeholder for void */
    bool v564;
    int64_t v34; /* placeholder for void */
    int64_t v580;
    void* v394;
    bool v911;
    int64_t v295; /* placeholder for void */
    bool v864;
    void* v387;
    bool v850;
    int64_t v38;
    int64_t v788; /* placeholder for void */
    int64_t v692;
    void* v373;
    bool v12;
    void* v543;
    void* v401;
    int64_t v200;
    bool v497;
    int64_t v423;
    int64_t v417;
    void* v810;
    void* v226;
    int64_t v251; /* placeholder for void */
    int64_t v795; /* placeholder for void */
    int64_t v484; /* placeholder for void */
    int64_t v676; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    bool v703;
    int64_t v853;
    bool v729;
    int64_t v332;
    bool v180;
    int64_t v637;
    bool v568;
    int64_t v563;
    void* v803;
    int64_t v711; /* placeholder for void */
    void* v691;
    int64_t v165;
    int64_t v281; /* placeholder for void */
    int64_t v597; /* placeholder for void */
    int64_t v665;
    bool v159;
    int64_t v46;
    int64_t v241;
    void* v761;
    int64_t v861;
    int64_t v45;
    int64_t v767; /* placeholder for void */
    bool v617;
    void* v23;
    bool v801;
    void* v30;
    void* v380;
    bool v26;
    bool v953;
    int64_t v583; /* placeholder for void */
    int64_t v914; /* placeholder for void */
    bool v334;
    int64_t v625; /* placeholder for void */
    int64_t v353;
    bool v612;
    void* v79;
    bool v322;
    void* v789;
    bool v591;
    int64_t v351; /* placeholder for void */
    int64_t v160; /* placeholder for void */
    bool v687;
    int64_t v507;
    bool v399;
    int64_t v151;
    int64_t v263;
    int64_t v102;
    int64_t v253; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    bool v252;
    int64_t v291;
    bool v203;
    int64_t v662; /* placeholder for void */
    bool v68;
    bool v229;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v958 = false;
    p_call_stack_pop();
    return v958;
  bb2:
    v7 = true;
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
    v14 = true;
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
    v21 = true;
    p_call_stack_pop();
    return v21;
  bb7:
    v23 = (char*)v1 + 0;
    v24 = *(int64_t*)v23;
    v25 = 3LL;
    v26 = v24 == v25;
    if (v26) goto bb8;
    else goto bb9;
  bb8:
    v28 = true;
    p_call_stack_pop();
    return v28;
  bb9:
    v30 = (char*)v1 + 0;
    v31 = *(int64_t*)v30;
    v32 = 4LL;
    v33 = v31 == v32;
    if (v33) goto bb10;
    else goto bb11;
  bb10:
    v35 = true;
    p_call_stack_pop();
    return v35;
  bb11:
    v37 = (char*)v1 + 0;
    v38 = *(int64_t*)v37;
    v39 = 5LL;
    v40 = v38 == v39;
    if (v40) goto bb12;
    else goto bb13;
  bb12:
    v42 = true;
    p_call_stack_pop();
    return v42;
  bb13:
    v44 = (char*)v1 + 0;
    v45 = *(int64_t*)v44;
    v46 = 6LL;
    v47 = v45 == v46;
    if (v47) goto bb14;
    else goto bb15;
  bb14:
    v49 = true;
    p_call_stack_pop();
    return v49;
  bb15:
    v51 = (char*)v1 + 0;
    v52 = *(int64_t*)v51;
    v53 = 7LL;
    v54 = v52 == v53;
    if (v54) goto bb16;
    else goto bb17;
  bb16:
    v56 = true;
    p_call_stack_pop();
    return v56;
  bb17:
    v58 = (char*)v1 + 0;
    v59 = *(int64_t*)v58;
    v60 = 8LL;
    v61 = v59 == v60;
    if (v61) goto bb18;
    else goto bb19;
  bb18:
    v63 = true;
    p_call_stack_pop();
    return v63;
  bb19:
    v65 = (char*)v1 + 0;
    v66 = *(int64_t*)v65;
    v67 = 9LL;
    v68 = v66 == v67;
    if (v68) goto bb20;
    else goto bb21;
  bb20:
    v70 = true;
    p_call_stack_pop();
    return v70;
  bb21:
    v72 = (char*)v1 + 0;
    v73 = *(int64_t*)v72;
    v74 = 10LL;
    v75 = v73 == v74;
    if (v75) goto bb22;
    else goto bb23;
  bb22:
    v77 = true;
    p_call_stack_pop();
    return v77;
  bb23:
    v79 = (char*)v1 + 0;
    v80 = *(int64_t*)v79;
    v81 = 11LL;
    v82 = v80 == v81;
    if (v82) goto bb24;
    else goto bb25;
  bb24:
    v84 = true;
    p_call_stack_pop();
    return v84;
  bb25:
    v86 = (char*)v1 + 0;
    v87 = *(int64_t*)v86;
    v88 = 12LL;
    v89 = v87 == v88;
    if (v89) goto bb26;
    else goto bb27;
  bb26:
    v91 = true;
    p_call_stack_pop();
    return v91;
  bb27:
    v93 = (char*)v1 + 0;
    v94 = *(int64_t*)v93;
    v95 = 13LL;
    v96 = v94 == v95;
    if (v96) goto bb28;
    else goto bb29;
  bb28:
    v98 = true;
    p_call_stack_pop();
    return v98;
  bb29:
    v100 = (char*)v1 + 0;
    v101 = *(int64_t*)v100;
    v102 = 14LL;
    v103 = v101 == v102;
    if (v103) goto bb30;
    else goto bb31;
  bb30:
    v105 = true;
    p_call_stack_pop();
    return v105;
  bb31:
    v107 = (char*)v1 + 0;
    v108 = *(int64_t*)v107;
    v109 = 15LL;
    v110 = v108 == v109;
    if (v110) goto bb32;
    else goto bb33;
  bb32:
    v112 = true;
    p_call_stack_pop();
    return v112;
  bb33:
    v114 = (char*)v1 + 0;
    v115 = *(int64_t*)v114;
    v116 = 16LL;
    v117 = v115 == v116;
    if (v117) goto bb34;
    else goto bb35;
  bb34:
    v119 = true;
    p_call_stack_pop();
    return v119;
  bb35:
    v121 = (char*)v1 + 0;
    v122 = *(int64_t*)v121;
    v123 = 17LL;
    v124 = v122 == v123;
    if (v124) goto bb36;
    else goto bb37;
  bb36:
    v126 = true;
    p_call_stack_pop();
    return v126;
  bb37:
    v128 = (char*)v1 + 0;
    v129 = *(int64_t*)v128;
    v130 = 18LL;
    v131 = v129 == v130;
    if (v131) goto bb38;
    else goto bb39;
  bb38:
    v133 = true;
    p_call_stack_pop();
    return v133;
  bb39:
    v135 = (char*)v1 + 0;
    v136 = *(int64_t*)v135;
    v137 = 19LL;
    v138 = v136 == v137;
    if (v138) goto bb40;
    else goto bb41;
  bb40:
    v140 = true;
    p_call_stack_pop();
    return v140;
  bb41:
    v142 = (char*)v1 + 0;
    v143 = *(int64_t*)v142;
    v144 = 20LL;
    v145 = v143 == v144;
    if (v145) goto bb42;
    else goto bb43;
  bb42:
    v147 = true;
    p_call_stack_pop();
    return v147;
  bb43:
    v149 = (char*)v1 + 0;
    v150 = *(int64_t*)v149;
    v151 = 21LL;
    v152 = v150 == v151;
    if (v152) goto bb44;
    else goto bb45;
  bb44:
    v154 = true;
    p_call_stack_pop();
    return v154;
  bb45:
    v156 = (char*)v1 + 0;
    v157 = *(int64_t*)v156;
    v158 = 22LL;
    v159 = v157 == v158;
    if (v159) goto bb46;
    else goto bb47;
  bb46:
    v161 = true;
    p_call_stack_pop();
    return v161;
  bb47:
    v163 = (char*)v1 + 0;
    v164 = *(int64_t*)v163;
    v165 = 23LL;
    v166 = v164 == v165;
    if (v166) goto bb48;
    else goto bb49;
  bb48:
    v168 = true;
    p_call_stack_pop();
    return v168;
  bb49:
    v170 = (char*)v1 + 0;
    v171 = *(int64_t*)v170;
    v172 = 24LL;
    v173 = v171 == v172;
    if (v173) goto bb50;
    else goto bb51;
  bb50:
    v175 = true;
    p_call_stack_pop();
    return v175;
  bb51:
    v177 = (char*)v1 + 0;
    v178 = *(int64_t*)v177;
    v179 = 25LL;
    v180 = v178 == v179;
    if (v180) goto bb52;
    else goto bb53;
  bb52:
    v182 = true;
    p_call_stack_pop();
    return v182;
  bb53:
    v184 = (char*)v1 + 0;
    v185 = *(int64_t*)v184;
    v186 = 26LL;
    v187 = v185 == v186;
    if (v187) goto bb54;
    else goto bb55;
  bb54:
    v189 = true;
    p_call_stack_pop();
    return v189;
  bb55:
    v191 = (char*)v1 + 0;
    v192 = *(int64_t*)v191;
    v193 = 27LL;
    v194 = v192 == v193;
    if (v194) goto bb56;
    else goto bb57;
  bb56:
    v196 = true;
    p_call_stack_pop();
    return v196;
  bb57:
    v198 = (char*)v1 + 0;
    v199 = *(int64_t*)v198;
    v200 = 28LL;
    v201 = v199 == v200;
    if (v201) goto bb58;
    else goto bb59;
  bb58:
    v203 = true;
    p_call_stack_pop();
    return v203;
  bb59:
    v205 = (char*)v1 + 0;
    v206 = *(int64_t*)v205;
    v207 = 29LL;
    v208 = v206 == v207;
    if (v208) goto bb60;
    else goto bb61;
  bb60:
    v210 = true;
    p_call_stack_pop();
    return v210;
  bb61:
    v212 = (char*)v1 + 0;
    v213 = *(int64_t*)v212;
    v214 = 30LL;
    v215 = v213 == v214;
    if (v215) goto bb62;
    else goto bb63;
  bb62:
    v217 = true;
    p_call_stack_pop();
    return v217;
  bb63:
    v219 = (char*)v1 + 0;
    v220 = *(int64_t*)v219;
    v221 = 31LL;
    v222 = v220 == v221;
    if (v222) goto bb64;
    else goto bb65;
  bb64:
    v224 = true;
    p_call_stack_pop();
    return v224;
  bb65:
    v226 = (char*)v1 + 0;
    v227 = *(int64_t*)v226;
    v228 = 32LL;
    v229 = v227 == v228;
    if (v229) goto bb66;
    else goto bb67;
  bb66:
    v231 = true;
    p_call_stack_pop();
    return v231;
  bb67:
    v233 = (char*)v1 + 0;
    v234 = *(int64_t*)v233;
    v235 = 33LL;
    v236 = v234 == v235;
    if (v236) goto bb68;
    else goto bb69;
  bb68:
    v238 = true;
    p_call_stack_pop();
    return v238;
  bb69:
    v240 = (char*)v1 + 0;
    v241 = *(int64_t*)v240;
    v242 = 34LL;
    v243 = v241 == v242;
    if (v243) goto bb70;
    else goto bb71;
  bb70:
    v245 = true;
    p_call_stack_pop();
    return v245;
  bb71:
    v247 = (char*)v1 + 0;
    v248 = *(int64_t*)v247;
    v249 = 35LL;
    v250 = v248 == v249;
    if (v250) goto bb72;
    else goto bb73;
  bb72:
    v252 = true;
    p_call_stack_pop();
    return v252;
  bb73:
    v254 = (char*)v1 + 0;
    v255 = *(int64_t*)v254;
    v256 = 36LL;
    v257 = v255 == v256;
    if (v257) goto bb74;
    else goto bb75;
  bb74:
    v259 = true;
    p_call_stack_pop();
    return v259;
  bb75:
    v261 = (char*)v1 + 0;
    v262 = *(int64_t*)v261;
    v263 = 37LL;
    v264 = v262 == v263;
    if (v264) goto bb76;
    else goto bb77;
  bb76:
    v266 = true;
    p_call_stack_pop();
    return v266;
  bb77:
    v268 = (char*)v1 + 0;
    v269 = *(int64_t*)v268;
    v270 = 38LL;
    v271 = v269 == v270;
    if (v271) goto bb78;
    else goto bb79;
  bb78:
    v273 = true;
    p_call_stack_pop();
    return v273;
  bb79:
    v275 = (char*)v1 + 0;
    v276 = *(int64_t*)v275;
    v277 = 39LL;
    v278 = v276 == v277;
    if (v278) goto bb80;
    else goto bb81;
  bb80:
    v280 = true;
    p_call_stack_pop();
    return v280;
  bb81:
    v282 = (char*)v1 + 0;
    v283 = *(int64_t*)v282;
    v284 = 40LL;
    v285 = v283 == v284;
    if (v285) goto bb82;
    else goto bb83;
  bb82:
    v287 = true;
    p_call_stack_pop();
    return v287;
  bb83:
    v289 = (char*)v1 + 0;
    v290 = *(int64_t*)v289;
    v291 = 41LL;
    v292 = v290 == v291;
    if (v292) goto bb84;
    else goto bb85;
  bb84:
    v294 = true;
    p_call_stack_pop();
    return v294;
  bb85:
    v296 = (char*)v1 + 0;
    v297 = *(int64_t*)v296;
    v298 = 42LL;
    v299 = v297 == v298;
    if (v299) goto bb86;
    else goto bb87;
  bb86:
    v301 = true;
    p_call_stack_pop();
    return v301;
  bb87:
    v303 = (char*)v1 + 0;
    v304 = *(int64_t*)v303;
    v305 = 43LL;
    v306 = v304 == v305;
    if (v306) goto bb88;
    else goto bb89;
  bb88:
    v308 = true;
    p_call_stack_pop();
    return v308;
  bb89:
    v310 = (char*)v1 + 0;
    v311 = *(int64_t*)v310;
    v312 = 44LL;
    v313 = v311 == v312;
    if (v313) goto bb90;
    else goto bb91;
  bb90:
    v315 = true;
    p_call_stack_pop();
    return v315;
  bb91:
    v317 = (char*)v1 + 0;
    v318 = *(int64_t*)v317;
    v319 = 45LL;
    v320 = v318 == v319;
    if (v320) goto bb92;
    else goto bb93;
  bb92:
    v322 = true;
    p_call_stack_pop();
    return v322;
  bb93:
    v324 = (char*)v1 + 0;
    v325 = *(int64_t*)v324;
    v326 = 46LL;
    v327 = v325 == v326;
    if (v327) goto bb94;
    else goto bb95;
  bb94:
    v329 = true;
    p_call_stack_pop();
    return v329;
  bb95:
    v331 = (char*)v1 + 0;
    v332 = *(int64_t*)v331;
    v333 = 47LL;
    v334 = v332 == v333;
    if (v334) goto bb96;
    else goto bb97;
  bb96:
    v336 = true;
    p_call_stack_pop();
    return v336;
  bb97:
    v338 = (char*)v1 + 0;
    v339 = *(int64_t*)v338;
    v340 = 48LL;
    v341 = v339 == v340;
    if (v341) goto bb98;
    else goto bb99;
  bb98:
    v343 = true;
    p_call_stack_pop();
    return v343;
  bb99:
    v345 = (char*)v1 + 0;
    v346 = *(int64_t*)v345;
    v347 = 49LL;
    v348 = v346 == v347;
    if (v348) goto bb100;
    else goto bb101;
  bb100:
    v350 = true;
    p_call_stack_pop();
    return v350;
  bb101:
    v352 = (char*)v1 + 0;
    v353 = *(int64_t*)v352;
    v354 = 50LL;
    v355 = v353 == v354;
    if (v355) goto bb102;
    else goto bb103;
  bb102:
    v357 = true;
    p_call_stack_pop();
    return v357;
  bb103:
    v359 = (char*)v1 + 0;
    v360 = *(int64_t*)v359;
    v361 = 51LL;
    v362 = v360 == v361;
    if (v362) goto bb104;
    else goto bb105;
  bb104:
    v364 = true;
    p_call_stack_pop();
    return v364;
  bb105:
    v366 = (char*)v1 + 0;
    v367 = *(int64_t*)v366;
    v368 = 52LL;
    v369 = v367 == v368;
    if (v369) goto bb106;
    else goto bb107;
  bb106:
    v371 = true;
    p_call_stack_pop();
    return v371;
  bb107:
    v373 = (char*)v1 + 0;
    v374 = *(int64_t*)v373;
    v375 = 53LL;
    v376 = v374 == v375;
    if (v376) goto bb108;
    else goto bb109;
  bb108:
    v378 = true;
    p_call_stack_pop();
    return v378;
  bb109:
    v380 = (char*)v1 + 0;
    v381 = *(int64_t*)v380;
    v382 = 54LL;
    v383 = v381 == v382;
    if (v383) goto bb110;
    else goto bb111;
  bb110:
    v385 = true;
    p_call_stack_pop();
    return v385;
  bb111:
    v387 = (char*)v1 + 0;
    v388 = *(int64_t*)v387;
    v389 = 55LL;
    v390 = v388 == v389;
    if (v390) goto bb112;
    else goto bb113;
  bb112:
    v392 = true;
    p_call_stack_pop();
    return v392;
  bb113:
    v394 = (char*)v1 + 0;
    v395 = *(int64_t*)v394;
    v396 = 56LL;
    v397 = v395 == v396;
    if (v397) goto bb114;
    else goto bb115;
  bb114:
    v399 = true;
    p_call_stack_pop();
    return v399;
  bb115:
    v401 = (char*)v1 + 0;
    v402 = *(int64_t*)v401;
    v403 = 57LL;
    v404 = v402 == v403;
    if (v404) goto bb116;
    else goto bb117;
  bb116:
    v406 = true;
    p_call_stack_pop();
    return v406;
  bb117:
    v408 = (char*)v1 + 0;
    v409 = *(int64_t*)v408;
    v410 = 58LL;
    v411 = v409 == v410;
    if (v411) goto bb118;
    else goto bb119;
  bb118:
    v413 = true;
    p_call_stack_pop();
    return v413;
  bb119:
    v415 = (char*)v1 + 0;
    v416 = *(int64_t*)v415;
    v417 = 59LL;
    v418 = v416 == v417;
    if (v418) goto bb120;
    else goto bb121;
  bb120:
    v420 = true;
    p_call_stack_pop();
    return v420;
  bb121:
    v422 = (char*)v1 + 0;
    v423 = *(int64_t*)v422;
    v424 = 60LL;
    v425 = v423 == v424;
    if (v425) goto bb122;
    else goto bb123;
  bb122:
    v427 = true;
    p_call_stack_pop();
    return v427;
  bb123:
    v429 = (char*)v1 + 0;
    v430 = *(int64_t*)v429;
    v431 = 61LL;
    v432 = v430 == v431;
    if (v432) goto bb124;
    else goto bb125;
  bb124:
    v434 = false;
    p_call_stack_pop();
    return v434;
  bb125:
    v436 = (char*)v1 + 0;
    v437 = *(int64_t*)v436;
    v438 = 62LL;
    v439 = v437 == v438;
    if (v439) goto bb126;
    else goto bb127;
  bb126:
    v441 = false;
    p_call_stack_pop();
    return v441;
  bb127:
    v443 = (char*)v1 + 0;
    v444 = *(int64_t*)v443;
    v445 = 63LL;
    v446 = v444 == v445;
    if (v446) goto bb128;
    else goto bb129;
  bb128:
    v448 = false;
    p_call_stack_pop();
    return v448;
  bb129:
    v450 = (char*)v1 + 0;
    v451 = *(int64_t*)v450;
    v452 = 64LL;
    v453 = v451 == v452;
    if (v453) goto bb130;
    else goto bb131;
  bb130:
    v455 = false;
    p_call_stack_pop();
    return v455;
  bb131:
    v457 = (char*)v1 + 0;
    v458 = *(int64_t*)v457;
    v459 = 65LL;
    v460 = v458 == v459;
    if (v460) goto bb132;
    else goto bb133;
  bb132:
    v462 = false;
    p_call_stack_pop();
    return v462;
  bb133:
    v464 = (char*)v1 + 0;
    v465 = *(int64_t*)v464;
    v466 = 66LL;
    v467 = v465 == v466;
    if (v467) goto bb134;
    else goto bb135;
  bb134:
    v469 = false;
    p_call_stack_pop();
    return v469;
  bb135:
    v471 = (char*)v1 + 0;
    v472 = *(int64_t*)v471;
    v473 = 67LL;
    v474 = v472 == v473;
    if (v474) goto bb136;
    else goto bb137;
  bb136:
    v476 = false;
    p_call_stack_pop();
    return v476;
  bb137:
    v478 = (char*)v1 + 0;
    v479 = *(int64_t*)v478;
    v480 = 68LL;
    v481 = v479 == v480;
    if (v481) goto bb138;
    else goto bb139;
  bb138:
    v483 = false;
    p_call_stack_pop();
    return v483;
  bb139:
    v485 = (char*)v1 + 0;
    v486 = *(int64_t*)v485;
    v487 = 69LL;
    v488 = v486 == v487;
    if (v488) goto bb140;
    else goto bb141;
  bb140:
    v490 = false;
    p_call_stack_pop();
    return v490;
  bb141:
    v492 = (char*)v1 + 0;
    v493 = *(int64_t*)v492;
    v494 = 70LL;
    v495 = v493 == v494;
    if (v495) goto bb142;
    else goto bb143;
  bb142:
    v497 = false;
    p_call_stack_pop();
    return v497;
  bb143:
    v499 = (char*)v1 + 0;
    v500 = *(int64_t*)v499;
    v501 = 71LL;
    v502 = v500 == v501;
    if (v502) goto bb144;
    else goto bb145;
  bb144:
    v504 = false;
    p_call_stack_pop();
    return v504;
  bb145:
    v506 = (char*)v1 + 0;
    v507 = *(int64_t*)v506;
    v508 = 72LL;
    v509 = v507 == v508;
    if (v509) goto bb146;
    else goto bb147;
  bb146:
    v511 = false;
    p_call_stack_pop();
    return v511;
  bb147:
    v513 = (char*)v1 + 0;
    v514 = *(int64_t*)v513;
    v515 = 73LL;
    v516 = v514 == v515;
    if (v516) goto bb148;
    else goto bb149;
  bb148:
    v518 = false;
    p_call_stack_pop();
    return v518;
  bb149:
    v520 = (char*)v1 + 0;
    v521 = *(int64_t*)v520;
    v522 = 74LL;
    v523 = v521 == v522;
    if (v523) goto bb150;
    else goto bb151;
  bb150:
    v525 = false;
    p_call_stack_pop();
    return v525;
  bb151:
    v527 = (char*)v1 + 0;
    v528 = *(int64_t*)v527;
    v529 = 75LL;
    v530 = v528 == v529;
    if (v530) goto bb152;
    else goto bb153;
  bb152:
    v532 = false;
    p_call_stack_pop();
    return v532;
  bb153:
    v534 = (char*)v1 + 0;
    v535 = *(int64_t*)v534;
    v536 = 76LL;
    v537 = v535 == v536;
    if (v537) goto bb154;
    else goto bb155;
  bb154:
    v539 = (char*)v1 + 8;
    v540 = *(int64_t*)v539;
    v541 = false;
    p_call_stack_pop();
    return v541;
  bb155:
    v543 = (char*)v1 + 0;
    v544 = *(int64_t*)v543;
    v545 = 77LL;
    v546 = v544 == v545;
    if (v546) goto bb156;
    else goto bb157;
  bb156:
    v548 = (char*)v1 + 8;
    v549 = *(double*)v548;
    v550 = false;
    p_call_stack_pop();
    return v550;
  bb157:
    v552 = (char*)v1 + 0;
    v553 = *(int64_t*)v552;
    v554 = 78LL;
    v555 = v553 == v554;
    if (v555) goto bb158;
    else goto bb159;
  bb158:
    v557 = (char*)v1 + 8;
    v558 = *(void**)v557;
    v559 = false;
    p_call_stack_pop();
    return v559;
  bb159:
    v561 = (char*)v1 + 0;
    v562 = *(int64_t*)v561;
    v563 = 79LL;
    v564 = v562 == v563;
    if (v564) goto bb160;
    else goto bb161;
  bb160:
    v566 = (char*)v1 + 8;
    v567 = *(void**)v566;
    v568 = false;
    p_call_stack_pop();
    return v568;
  bb161:
    v570 = (char*)v1 + 0;
    v571 = *(int64_t*)v570;
    v572 = 80LL;
    v573 = v571 == v572;
    if (v573) goto bb162;
    else goto bb163;
  bb162:
    v575 = (char*)v1 + 8;
    v576 = *(void**)v575;
    v577 = false;
    p_call_stack_pop();
    return v577;
  bb163:
    v579 = (char*)v1 + 0;
    v580 = *(int64_t*)v579;
    v581 = 81LL;
    v582 = v580 == v581;
    if (v582) goto bb164;
    else goto bb165;
  bb164:
    v584 = false;
    p_call_stack_pop();
    return v584;
  bb165:
    v586 = (char*)v1 + 0;
    v587 = *(int64_t*)v586;
    v588 = 82LL;
    v589 = v587 == v588;
    if (v589) goto bb166;
    else goto bb167;
  bb166:
    v591 = false;
    p_call_stack_pop();
    return v591;
  bb167:
    v593 = (char*)v1 + 0;
    v594 = *(int64_t*)v593;
    v595 = 83LL;
    v596 = v594 == v595;
    if (v596) goto bb168;
    else goto bb169;
  bb168:
    v598 = false;
    p_call_stack_pop();
    return v598;
  bb169:
    v600 = (char*)v1 + 0;
    v601 = *(int64_t*)v600;
    v602 = 84LL;
    v603 = v601 == v602;
    if (v603) goto bb170;
    else goto bb171;
  bb170:
    v605 = false;
    p_call_stack_pop();
    return v605;
  bb171:
    v607 = (char*)v1 + 0;
    v608 = *(int64_t*)v607;
    v609 = 85LL;
    v610 = v608 == v609;
    if (v610) goto bb172;
    else goto bb173;
  bb172:
    v612 = false;
    p_call_stack_pop();
    return v612;
  bb173:
    v614 = (char*)v1 + 0;
    v615 = *(int64_t*)v614;
    v616 = 86LL;
    v617 = v615 == v616;
    if (v617) goto bb174;
    else goto bb175;
  bb174:
    v619 = false;
    p_call_stack_pop();
    return v619;
  bb175:
    v621 = (char*)v1 + 0;
    v622 = *(int64_t*)v621;
    v623 = 87LL;
    v624 = v622 == v623;
    if (v624) goto bb176;
    else goto bb177;
  bb176:
    v626 = false;
    p_call_stack_pop();
    return v626;
  bb177:
    v628 = (char*)v1 + 0;
    v629 = *(int64_t*)v628;
    v630 = 88LL;
    v631 = v629 == v630;
    if (v631) goto bb178;
    else goto bb179;
  bb178:
    v633 = false;
    p_call_stack_pop();
    return v633;
  bb179:
    v635 = (char*)v1 + 0;
    v636 = *(int64_t*)v635;
    v637 = 89LL;
    v638 = v636 == v637;
    if (v638) goto bb180;
    else goto bb181;
  bb180:
    v640 = false;
    p_call_stack_pop();
    return v640;
  bb181:
    v642 = (char*)v1 + 0;
    v643 = *(int64_t*)v642;
    v644 = 90LL;
    v645 = v643 == v644;
    if (v645) goto bb182;
    else goto bb183;
  bb182:
    v647 = false;
    p_call_stack_pop();
    return v647;
  bb183:
    v649 = (char*)v1 + 0;
    v650 = *(int64_t*)v649;
    v651 = 91LL;
    v652 = v650 == v651;
    if (v652) goto bb184;
    else goto bb185;
  bb184:
    v654 = false;
    p_call_stack_pop();
    return v654;
  bb185:
    v656 = (char*)v1 + 0;
    v657 = *(int64_t*)v656;
    v658 = 92LL;
    v659 = v657 == v658;
    if (v659) goto bb186;
    else goto bb187;
  bb186:
    v661 = false;
    p_call_stack_pop();
    return v661;
  bb187:
    v663 = (char*)v1 + 0;
    v664 = *(int64_t*)v663;
    v665 = 93LL;
    v666 = v664 == v665;
    if (v666) goto bb188;
    else goto bb189;
  bb188:
    v668 = false;
    p_call_stack_pop();
    return v668;
  bb189:
    v670 = (char*)v1 + 0;
    v671 = *(int64_t*)v670;
    v672 = 94LL;
    v673 = v671 == v672;
    if (v673) goto bb190;
    else goto bb191;
  bb190:
    v675 = false;
    p_call_stack_pop();
    return v675;
  bb191:
    v677 = (char*)v1 + 0;
    v678 = *(int64_t*)v677;
    v679 = 95LL;
    v680 = v678 == v679;
    if (v680) goto bb192;
    else goto bb193;
  bb192:
    v682 = false;
    p_call_stack_pop();
    return v682;
  bb193:
    v684 = (char*)v1 + 0;
    v685 = *(int64_t*)v684;
    v686 = 96LL;
    v687 = v685 == v686;
    if (v687) goto bb194;
    else goto bb195;
  bb194:
    v689 = false;
    p_call_stack_pop();
    return v689;
  bb195:
    v691 = (char*)v1 + 0;
    v692 = *(int64_t*)v691;
    v693 = 97LL;
    v694 = v692 == v693;
    if (v694) goto bb196;
    else goto bb197;
  bb196:
    v696 = false;
    p_call_stack_pop();
    return v696;
  bb197:
    v698 = (char*)v1 + 0;
    v699 = *(int64_t*)v698;
    v700 = 98LL;
    v701 = v699 == v700;
    if (v701) goto bb198;
    else goto bb199;
  bb198:
    v703 = false;
    p_call_stack_pop();
    return v703;
  bb199:
    v705 = (char*)v1 + 0;
    v706 = *(int64_t*)v705;
    v707 = 99LL;
    v708 = v706 == v707;
    if (v708) goto bb200;
    else goto bb201;
  bb200:
    v710 = false;
    p_call_stack_pop();
    return v710;
  bb201:
    v712 = (char*)v1 + 0;
    v713 = *(int64_t*)v712;
    v714 = 100LL;
    v715 = v713 == v714;
    if (v715) goto bb202;
    else goto bb203;
  bb202:
    v717 = false;
    p_call_stack_pop();
    return v717;
  bb203:
    v719 = (char*)v1 + 0;
    v720 = *(int64_t*)v719;
    v721 = 101LL;
    v722 = v720 == v721;
    if (v722) goto bb204;
    else goto bb205;
  bb204:
    v724 = false;
    p_call_stack_pop();
    return v724;
  bb205:
    v726 = (char*)v1 + 0;
    v727 = *(int64_t*)v726;
    v728 = 102LL;
    v729 = v727 == v728;
    if (v729) goto bb206;
    else goto bb207;
  bb206:
    v731 = false;
    p_call_stack_pop();
    return v731;
  bb207:
    v733 = (char*)v1 + 0;
    v734 = *(int64_t*)v733;
    v735 = 103LL;
    v736 = v734 == v735;
    if (v736) goto bb208;
    else goto bb209;
  bb208:
    v738 = false;
    p_call_stack_pop();
    return v738;
  bb209:
    v740 = (char*)v1 + 0;
    v741 = *(int64_t*)v740;
    v742 = 104LL;
    v743 = v741 == v742;
    if (v743) goto bb210;
    else goto bb211;
  bb210:
    v745 = false;
    p_call_stack_pop();
    return v745;
  bb211:
    v747 = (char*)v1 + 0;
    v748 = *(int64_t*)v747;
    v749 = 105LL;
    v750 = v748 == v749;
    if (v750) goto bb212;
    else goto bb213;
  bb212:
    v752 = false;
    p_call_stack_pop();
    return v752;
  bb213:
    v754 = (char*)v1 + 0;
    v755 = *(int64_t*)v754;
    v756 = 106LL;
    v757 = v755 == v756;
    if (v757) goto bb214;
    else goto bb215;
  bb214:
    v759 = false;
    p_call_stack_pop();
    return v759;
  bb215:
    v761 = (char*)v1 + 0;
    v762 = *(int64_t*)v761;
    v763 = 107LL;
    v764 = v762 == v763;
    if (v764) goto bb216;
    else goto bb217;
  bb216:
    v766 = false;
    p_call_stack_pop();
    return v766;
  bb217:
    v768 = (char*)v1 + 0;
    v769 = *(int64_t*)v768;
    v770 = 108LL;
    v771 = v769 == v770;
    if (v771) goto bb218;
    else goto bb219;
  bb218:
    v773 = false;
    p_call_stack_pop();
    return v773;
  bb219:
    v775 = (char*)v1 + 0;
    v776 = *(int64_t*)v775;
    v777 = 109LL;
    v778 = v776 == v777;
    if (v778) goto bb220;
    else goto bb221;
  bb220:
    v780 = false;
    p_call_stack_pop();
    return v780;
  bb221:
    v782 = (char*)v1 + 0;
    v783 = *(int64_t*)v782;
    v784 = 110LL;
    v785 = v783 == v784;
    if (v785) goto bb222;
    else goto bb223;
  bb222:
    v787 = false;
    p_call_stack_pop();
    return v787;
  bb223:
    v789 = (char*)v1 + 0;
    v790 = *(int64_t*)v789;
    v791 = 111LL;
    v792 = v790 == v791;
    if (v792) goto bb224;
    else goto bb225;
  bb224:
    v794 = false;
    p_call_stack_pop();
    return v794;
  bb225:
    v796 = (char*)v1 + 0;
    v797 = *(int64_t*)v796;
    v798 = 112LL;
    v799 = v797 == v798;
    if (v799) goto bb226;
    else goto bb227;
  bb226:
    v801 = false;
    p_call_stack_pop();
    return v801;
  bb227:
    v803 = (char*)v1 + 0;
    v804 = *(int64_t*)v803;
    v805 = 113LL;
    v806 = v804 == v805;
    if (v806) goto bb228;
    else goto bb229;
  bb228:
    v808 = false;
    p_call_stack_pop();
    return v808;
  bb229:
    v810 = (char*)v1 + 0;
    v811 = *(int64_t*)v810;
    v812 = 114LL;
    v813 = v811 == v812;
    if (v813) goto bb230;
    else goto bb231;
  bb230:
    v815 = false;
    p_call_stack_pop();
    return v815;
  bb231:
    v817 = (char*)v1 + 0;
    v818 = *(int64_t*)v817;
    v819 = 115LL;
    v820 = v818 == v819;
    if (v820) goto bb232;
    else goto bb233;
  bb232:
    v822 = false;
    p_call_stack_pop();
    return v822;
  bb233:
    v824 = (char*)v1 + 0;
    v825 = *(int64_t*)v824;
    v826 = 116LL;
    v827 = v825 == v826;
    if (v827) goto bb234;
    else goto bb235;
  bb234:
    v829 = false;
    p_call_stack_pop();
    return v829;
  bb235:
    v831 = (char*)v1 + 0;
    v832 = *(int64_t*)v831;
    v833 = 117LL;
    v834 = v832 == v833;
    if (v834) goto bb236;
    else goto bb237;
  bb236:
    v836 = false;
    p_call_stack_pop();
    return v836;
  bb237:
    v838 = (char*)v1 + 0;
    v839 = *(int64_t*)v838;
    v840 = 118LL;
    v841 = v839 == v840;
    if (v841) goto bb238;
    else goto bb239;
  bb238:
    v843 = false;
    p_call_stack_pop();
    return v843;
  bb239:
    v845 = (char*)v1 + 0;
    v846 = *(int64_t*)v845;
    v847 = 119LL;
    v848 = v846 == v847;
    if (v848) goto bb240;
    else goto bb241;
  bb240:
    v850 = false;
    p_call_stack_pop();
    return v850;
  bb241:
    v852 = (char*)v1 + 0;
    v853 = *(int64_t*)v852;
    v854 = 120LL;
    v855 = v853 == v854;
    if (v855) goto bb242;
    else goto bb243;
  bb242:
    v857 = false;
    p_call_stack_pop();
    return v857;
  bb243:
    v859 = (char*)v1 + 0;
    v860 = *(int64_t*)v859;
    v861 = 121LL;
    v862 = v860 == v861;
    if (v862) goto bb244;
    else goto bb245;
  bb244:
    v864 = false;
    p_call_stack_pop();
    return v864;
  bb245:
    v866 = (char*)v1 + 0;
    v867 = *(int64_t*)v866;
    v868 = 122LL;
    v869 = v867 == v868;
    if (v869) goto bb246;
    else goto bb247;
  bb246:
    v871 = false;
    p_call_stack_pop();
    return v871;
  bb247:
    v873 = (char*)v1 + 0;
    v874 = *(int64_t*)v873;
    v875 = 123LL;
    v876 = v874 == v875;
    if (v876) goto bb248;
    else goto bb249;
  bb248:
    v878 = false;
    p_call_stack_pop();
    return v878;
  bb249:
    v880 = (char*)v1 + 0;
    v881 = *(int64_t*)v880;
    v882 = 124LL;
    v883 = v881 == v882;
    if (v883) goto bb250;
    else goto bb251;
  bb250:
    v885 = false;
    p_call_stack_pop();
    return v885;
  bb251:
    v887 = (char*)v1 + 0;
    v888 = *(int64_t*)v887;
    v889 = 125LL;
    v890 = v888 == v889;
    if (v890) goto bb252;
    else goto bb253;
  bb252:
    v892 = false;
    p_call_stack_pop();
    return v892;
  bb253:
    v894 = (char*)v1 + 0;
    v895 = *(int64_t*)v894;
    v896 = 126LL;
    v897 = v895 == v896;
    if (v897) goto bb254;
    else goto bb255;
  bb254:
    v899 = false;
    p_call_stack_pop();
    return v899;
  bb255:
    v901 = (char*)v1 + 0;
    v902 = *(int64_t*)v901;
    v903 = 127LL;
    v904 = v902 == v903;
    if (v904) goto bb256;
    else goto bb257;
  bb256:
    v906 = false;
    p_call_stack_pop();
    return v906;
  bb257:
    v908 = (char*)v1 + 0;
    v909 = *(int64_t*)v908;
    v910 = 128LL;
    v911 = v909 == v910;
    if (v911) goto bb258;
    else goto bb259;
  bb258:
    v913 = false;
    p_call_stack_pop();
    return v913;
  bb259:
    v915 = (char*)v1 + 0;
    v916 = *(int64_t*)v915;
    v917 = 129LL;
    v918 = v916 == v917;
    if (v918) goto bb260;
    else goto bb261;
  bb260:
    v920 = false;
    p_call_stack_pop();
    return v920;
  bb261:
    v922 = (char*)v1 + 0;
    v923 = *(int64_t*)v922;
    v924 = 130LL;
    v925 = v923 == v924;
    if (v925) goto bb262;
    else goto bb263;
  bb262:
    v927 = false;
    p_call_stack_pop();
    return v927;
  bb263:
    v929 = (char*)v1 + 0;
    v930 = *(int64_t*)v929;
    v931 = 131LL;
    v932 = v930 == v931;
    if (v932) goto bb264;
    else goto bb265;
  bb264:
    v934 = false;
    p_call_stack_pop();
    return v934;
  bb265:
    v936 = (char*)v1 + 0;
    v937 = *(int64_t*)v936;
    v938 = 132LL;
    v939 = v937 == v938;
    if (v939) goto bb266;
    else goto bb267;
  bb266:
    v941 = false;
    p_call_stack_pop();
    return v941;
  bb267:
    v943 = (char*)v1 + 0;
    v944 = *(int64_t*)v943;
    v945 = 133LL;
    v946 = v944 == v945;
    if (v946) goto bb268;
    else goto bb269;
  bb268:
    v948 = false;
    p_call_stack_pop();
    return v948;
  bb269:
    v950 = (char*)v1 + 0;
    v951 = *(int64_t*)v950;
    v952 = 134LL;
    v953 = v951 == v952;
    if (v953) goto bb270;
    else goto bb271;
  bb270:
    v955 = false;
    p_call_stack_pop();
    return v955;
  bb271:
    goto bb1;
    return 0;
}

void* str_split(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_split");
    int64_t v72; /* placeholder for void */
    int64_t v29;
    void* v30;
    void* v73;
    int64_t v51;
    uint8_t v77;
    int64_t v3;
    int64_t v68; /* placeholder for void */
    int64_t v47;
    int64_t v117;
    bool v39;
    int64_t v16; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v46;
    int64_t v49; /* placeholder for void */
    bool v82;
    int64_t v94; /* placeholder for void */
    uint8_t v74;
    int64_t v80;
    int64_t v120;
    int64_t v103;
    int64_t v56;
    int64_t v89;
    int64_t v21;
    int64_t v87; /* placeholder for void */
    void* v12;
    int64_t v88;
    int64_t v98;
    void* v27;
    int64_t v52;
    void* v115;
    int64_t v85; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v69;
    int64_t v34;
    bool v84;
    void* v122;
    int64_t v62; /* placeholder for void */
    void* v5;
    int64_t v105; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v118;
    int64_t v113; /* placeholder for void */
    void* v58;
    void* v67;
    int64_t v57;
    int64_t v38; /* placeholder for void */
    int64_t v104;
    int64_t v99;
    int64_t v121; /* placeholder for void */
    int64_t v119;
    int64_t v106;
    void* v95;
    int64_t v123; /* placeholder for void */
    int64_t v60;
    int64_t v81;
    int64_t v35;
    int64_t v53;
    void* v4;
    int64_t v109;
    void* v54;
    bool v93;
    int64_t v112; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v9;
    int64_t v42;
    int64_t v28; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v102;
    int64_t v65; /* placeholder for void */
    int64_t v13;
    int64_t v26;
    int64_t v116;
    int64_t v83; /* placeholder for void */
    int64_t v111;
    void* v64;
    int64_t v33;
    int64_t v7;
    int64_t v17; /* placeholder for void */
    bool v37;
    bool v48;
    int64_t v50;
    void* v76;
    int64_t v59; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    void* v61;
    int64_t v96;
    void* v40;
    int64_t v55; /* placeholder for void */
    int64_t v97;
    int64_t v32; /* placeholder for void */
    int64_t v66;
    int64_t v100;
    int64_t v44; /* placeholder for void */
    int64_t v22;
    int64_t v92; /* placeholder for void */
    int64_t v90;
    int64_t v110;
    int64_t v75; /* placeholder for void */
    int64_t v8;
    int64_t v25;
    void* v19;
    void* v23;
    int64_t v18;
    void* v43;
    int64_t v24; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    void* v15;
    int64_t v6; /* placeholder for void */
    int64_t v36;
    bool v10;
    int64_t v107; /* placeholder for void */
    void* v70;
    int64_t v63;
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

int64_t Option_unwrap(void* v0, void* v1) {
    p_call_stack_push("Option_unwrap");
    int64_t v17; /* placeholder for void */
    bool v5;
    int64_t v6; /* placeholder for void */
    void* v7;
    int64_t v16; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v3;
    void* v15;
    int64_t v12;
    void* v2;
    int64_t v18; /* placeholder for void */
    int64_t v19;
    int64_t v8;
    int64_t v4;
    void* v10;
    int64_t v9; /* placeholder for void */
    int64_t v11;
    int64_t v14; /* placeholder for void */
    bool v13;
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
    v15 = _str0;
    p_panic(v15);
    goto bb1;
  bb5:
    goto bb1;
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

bool Vec_all(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_all");
    int64_t v7;
    int64_t v26; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v28;
    int64_t v31; /* placeholder for void */
    bool v32;
    int64_t v20;
    int64_t v23; /* placeholder for void */
    int64_t v15;
    int64_t v27;
    int64_t v3;
    int64_t v22;
    int64_t v25; /* placeholder for void */
    int64_t v12;
    void* v8;
    int64_t v9; /* placeholder for void */
    int64_t v11;
    int64_t v10; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v17;
    void* v18;
    void* v4;
    int64_t v19; /* placeholder for void */
    int64_t v6;
    int64_t v16;
    bool v21;
    bool v13;
    int64_t v29;
    int64_t v33; /* placeholder for void */
    bool v24;
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
        if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_digit") == 0) {
            v21 = lex_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_hexdigit") == 0) {
            v21 = lex_is_hexdigit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_whitespace") == 0) {
            v21 = lex_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "is_word_keyword") == 0) {
            v21 = is_word_keyword((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "lex_is_alpha") == 0) {
            v21 = lex_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_alphanumeric") == 0) {
            v21 = lex_is_alphanumeric((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
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

void Lexer_skip_block_comment(void* v0, void* v1) {
    p_call_stack_push("Lexer_skip_block_comment");
    bool v24;
    int64_t v11;
    int64_t v35; /* placeholder for void */
    void* v9;
    int64_t v2;
    int64_t v22;
    void* v10;
    int64_t v21;
    int64_t v15;
    bool v13;
    int64_t v28;
    int64_t v29;
    int64_t v34; /* placeholder for void */
    int64_t v4;
    int64_t v12;
    int64_t v25; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v17;
    int64_t v32;
    int64_t v6; /* placeholder for void */
    int64_t v33;
    int64_t v3;
    int64_t v19;
    int64_t v26;
    int64_t v27;
    bool v18;
    int64_t v31; /* placeholder for void */
    void* v7;
    int64_t v20;
    bool v23;
    int64_t v16;
    int64_t v5;
    int64_t v8;
    int64_t v14; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)Lexer_advance((void*)v2, v1);
    v4 = 0LL;
    v5 = (int64_t)Lexer_advance((void*)v4, v1);
    goto bb1;
  bb1:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = (char*)v1 + 0;
    v10 = *(void**)v9;
    v11 = 0LL;
    v12 = (int64_t)str_len((void*)v11, v10);
    v13 = v8 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = 0LL;
    v16 = (int64_t)Lexer_peek((void*)v15, v1);
    v17 = 42LL;
    v18 = v16 == v17;
    v19 = 1LL;
    v20 = 0LL;
    v21 = (int64_t)Lexer_peek_at((void*)v20, v1, v19);
    v22 = 47LL;
    v23 = v21 == v22;
    v24 = v18 && v23;
    if (v24) goto bb4;
    else goto bb5;
  bb3:
    p_call_stack_pop();
    return;
  bb4:
    v26 = 0LL;
    v27 = (int64_t)Lexer_advance((void*)v26, v1);
    v28 = 0LL;
    v29 = (int64_t)Lexer_advance((void*)v28, v1);
    p_call_stack_pop();
    return;
  bb5:
    goto bb6;
  bb6:
    v32 = 0LL;
    v33 = (int64_t)Lexer_advance((void*)v32, v1);
    goto bb1;
    return;
}

int64_t str_parse_int_range(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_parse_int_range");
    int64_t v18; /* placeholder for void */
    int64_t v74;
    int64_t v59; /* placeholder for void */
    bool v30;
    int64_t v55; /* placeholder for void */
    int64_t v16;
    int64_t v68; /* placeholder for void */
    void* v10;
    int64_t v17; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    bool v44;
    int64_t v12;
    int64_t v80;
    int64_t v83; /* placeholder for void */
    bool v14;
    int64_t v69; /* placeholder for void */
    int64_t v71;
    void* v39;
    int64_t v64;
    int64_t v45; /* placeholder for void */
    int64_t v61;
    void* v48;
    bool v63;
    int64_t v65;
    int64_t v38;
    int64_t v42;
    int64_t v47;
    void* v57;
    int64_t v70; /* placeholder for void */
    int64_t v72;
    int64_t v78; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v85; /* placeholder for void */
    int64_t v13;
    int64_t v23; /* placeholder for void */
    int64_t v50;
    int64_t v33; /* placeholder for void */
    int64_t v76;
    int64_t v15; /* placeholder for void */
    void* v36;
    int64_t v26; /* placeholder for void */
    bool v22;
    int64_t v20;
    int64_t v56; /* placeholder for void */
    int64_t v4;
    int64_t v41; /* placeholder for void */
    void* v54;
    bool v66;
    int64_t v77;
    int64_t v84;
    int64_t v28;
    int64_t v11; /* placeholder for void */
    void* v6;
    int64_t v29;
    int64_t v43;
    int64_t v5;
    int64_t v21;
    int64_t v40; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v75;
    bool v67;
    int64_t v7; /* placeholder for void */
    int64_t v46;
    int64_t v52; /* placeholder for void */
    void* v8;
    int64_t v37; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v24;
    int64_t v79;
    void* v51;
    int64_t v53;
    int64_t v25; /* placeholder for void */
    int64_t v35;
    uint8_t v58;
    int64_t v81;
    int64_t v62;
    int64_t v73;
    int64_t v32;
    int64_t v9; /* placeholder for void */
    int64_t v34; /* placeholder for void */
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

void ByteBuffer_write_bytes(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_write_bytes");
    int64_t v25; /* placeholder for void */
    void* v5;
    int64_t v6; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v4;
    uint8_t v23;
    int64_t v12;
    int64_t v14; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    bool v9;
    void* v16;
    void* v22;
    int64_t v8;
    int64_t v17; /* placeholder for void */
    int64_t v27;
    int64_t v10; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v11;
    int64_t v29;
    int64_t v18;
    void* v13;
    int64_t v31;
    void* v19;
    int64_t v32; /* placeholder for void */
    int64_t v30;
    int64_t v33; /* placeholder for void */
    int64_t v15;
    int64_t v20; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v26;
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

int64_t vec_window(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_window");
    int64_t v11;
    int64_t v21;
    int64_t v30;
    bool v24;
    int64_t v20; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v23;
    void* v9;
    int64_t v14;
    int64_t v17;
    void* v5;
    int64_t v25; /* placeholder for void */
    int64_t v31;
    int64_t v26;
    int64_t v27;
    int64_t v4;
    int64_t v19; /* placeholder for void */
    int64_t v22;
    int64_t v32;
    int64_t v33; /* placeholder for void */
    int64_t v35;
    int64_t v3;
    bool v12;
    int64_t v28;
    int64_t v36; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v7;
    int64_t v8;
    int64_t v15; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    void* v18;
    int64_t v13; /* placeholder for void */
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

bool p_str_equal(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_equal");
    int64_t v13; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    void* v35;
    int64_t v58; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v60;
    int64_t v61;
    int64_t v65;
    int64_t v27;
    int64_t v19; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    bool v15;
    int64_t v40;
    int64_t v16; /* placeholder for void */
    uint8_t v32;
    uint8_t v71;
    int64_t v48;
    void* v37;
    int64_t v34; /* placeholder for void */
    bool v50;
    bool v9;
    int64_t v10; /* placeholder for void */
    bool v5;
    bool v46;
    int64_t v66; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    void* v33;
    void* v31;
    int64_t v56;
    uint8_t v36;
    bool v11;
    int64_t v8;
    bool v42;
    int64_t v24;
    int64_t v44;
    int64_t v14;
    int64_t v4; /* placeholder for void */
    int64_t v74;
    bool v76;
    void* v20;
    int64_t v26; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v55;
    int64_t v63;
    int64_t v29; /* placeholder for void */
    bool v52;
    int64_t v59;
    int64_t v54; /* placeholder for void */
    int64_t v57;
    void* v25;
    int64_t v51; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v75;
    int64_t v18; /* placeholder for void */
    int64_t v43;
    bool v17;
    bool v45;
    void* v67;
    bool v3;
    int64_t v7; /* placeholder for void */
    void* v28;
    int64_t v41;
    int64_t v38; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v49;
    int64_t v73; /* placeholder for void */
    uint8_t v68;
    int64_t v12; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v22;
    void* v70;
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

int64_t Vec_len(void* v0, void* v1) {
    p_call_stack_push("Vec_len");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_len(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t p_str_hash(void* v0, int64_t v1) {
    p_call_stack_push("p_str_hash");
    int64_t v39; /* placeholder for void */
    int64_t v23;
    void* v6;
    int64_t v38; /* placeholder for void */
    uint8_t v36;
    int64_t v11; /* placeholder for void */
    uint8_t v13;
    int64_t v45;
    int64_t v16; /* placeholder for void */
    void* v35;
    int64_t v40;
    int64_t v44;
    void* v14;
    int64_t v15; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v4;
    int64_t v46; /* placeholder for void */
    void* v12;
    int64_t v37; /* placeholder for void */
    bool v19;
    int64_t v30;
    int64_t v2;
    void* v9;
    int64_t v21;
    int64_t v22;
    int64_t v41;
    int64_t v47; /* placeholder for void */
    int64_t v3;
    int64_t v7; /* placeholder for void */
    int64_t v8;
    int64_t v26;
    int64_t v5; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v42;
    int64_t v48;
    int64_t v49; /* placeholder for void */
    int64_t v33;
    int64_t v25;
    int64_t v27;
    int64_t v29;
    int64_t v10; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v31;
    int64_t v32; /* placeholder for void */
    int64_t v17;
    int64_t v18;
    int64_t v43; /* placeholder for void */
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

int64_t String_trim(void* v0, int64_t v1) {
    p_call_stack_push("String_trim");
    int64_t v170;
    int64_t v93;
    int64_t v137;
    int64_t v174;
    int64_t v45;
    int64_t v35; /* placeholder for void */
    int64_t v131; /* placeholder for void */
    int64_t v185;
    int64_t v48;
    int64_t v71; /* placeholder for void */
    bool v120;
    void* v33;
    int64_t v64; /* placeholder for void */
    int64_t v157;
    int64_t v16; /* placeholder for void */
    int64_t v164; /* placeholder for void */
    int64_t v150;
    int64_t v184; /* placeholder for void */
    void* v186;
    int64_t v5;
    int64_t v177;
    void* v161;
    int64_t v12; /* placeholder for void */
    uint8_t v140;
    int64_t v162;
    int64_t v27;
    int64_t v63;
    int64_t v83; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v57;
    void* v95;
    int64_t v132;
    int64_t v144;
    int64_t v66; /* placeholder for void */
    int64_t v148; /* placeholder for void */
    bool v54;
    void* v81;
    int64_t v85;
    void* v36;
    int64_t v49;
    int64_t v56; /* placeholder for void */
    bool v116;
    int64_t v149;
    int64_t v153;
    int64_t v160; /* placeholder for void */
    int64_t v169;
    int64_t v175;
    int64_t v52;
    void* v172;
    int64_t v166; /* placeholder for void */
    int64_t v178;
    void* v183;
    int64_t v191; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v113;
    int64_t v75; /* placeholder for void */
    int64_t v143; /* placeholder for void */
    int64_t v107;
    int64_t v180;
    int64_t v89; /* placeholder for void */
    int64_t v134;
    int64_t v156; /* placeholder for void */
    int64_t v97;
    int64_t v168;
    void* v6;
    void* v22;
    int64_t v11; /* placeholder for void */
    int64_t v141; /* placeholder for void */
    int64_t v32;
    bool v111;
    int64_t v123;
    int64_t v114;
    int64_t v135;
    int64_t v105; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    void* v158;
    int64_t v179; /* placeholder for void */
    uint8_t v102;
    int64_t v167; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v190;
    bool v119;
    int64_t v31; /* placeholder for void */
    void* v98;
    int64_t v125; /* placeholder for void */
    bool v151;
    int64_t v20;
    int64_t v62;
    int64_t v88;
    int64_t v69; /* placeholder for void */
    void* v7;
    void* v77;
    void* v78;
    int64_t v15; /* placeholder for void */
    bool v115;
    int64_t v142; /* placeholder for void */
    int64_t v194; /* placeholder for void */
    int64_t v80;
    bool v108;
    void* v154;
    int64_t v44;
    int64_t v159; /* placeholder for void */
    void* v30;
    bool v46;
    int64_t v39; /* placeholder for void */
    uint8_t v68;
    int64_t v129; /* placeholder for void */
    void* v25;
    void* v101;
    void* v147;
    int64_t v104; /* placeholder for void */
    int64_t v165;
    int64_t v181;
    int64_t v9;
    int64_t v29; /* placeholder for void */
    int64_t v145;
    void* v155;
    bool v55;
    int64_t v70; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v92;
    int64_t v41;
    int64_t v109;
    int64_t v24;
    int64_t v126;
    int64_t v26; /* placeholder for void */
    uint8_t v37;
    int64_t v72;
    bool v112;
    void* v139;
    int64_t v59;
    void* v189;
    int64_t v187; /* placeholder for void */
    bool v3;
    int64_t v110;
    void* v171;
    void* v84;
    bool v128;
    int64_t v18; /* placeholder for void */
    int64_t v94;
    int64_t v53;
    bool v74;
    int64_t v19; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v65;
    int64_t v42;
    int64_t v176;
    int64_t v8; /* placeholder for void */
    int64_t v133;
    int64_t v193;
    int64_t v17;
    int64_t v163; /* placeholder for void */
    int64_t v91;
    int64_t v4; /* placeholder for void */
    void* v28;
    int64_t v118;
    int64_t v146;
    int64_t v96; /* placeholder for void */
    bool v47;
    int64_t v14;
    bool v50;
    void* v67;
    int64_t v121; /* placeholder for void */
    bool v51;
    int64_t v130; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v138; /* placeholder for void */
    bool v43;
    int64_t v127;
    int64_t v61;
    void* v103;
    int64_t v106;
    int64_t v182;
    int64_t v192; /* placeholder for void */
    int64_t v117;
    int64_t v124;
    int64_t v21;
    int64_t v23; /* placeholder for void */
    void* v38;
    int64_t v73;
    int64_t v76;
    void* v10;
    int64_t v173; /* placeholder for void */
    void* v13;
    int64_t v58;
    int64_t v2;
    int64_t v122;
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

int64_t allocator_bytes_used(void* v0, void* v1) {
    p_call_stack_push("allocator_bytes_used");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_allocator_stats(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool String_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_contains");
    int64_t v86;
    int64_t v101; /* placeholder for void */
    bool v89;
    bool v6;
    int64_t v33; /* placeholder for void */
    bool v98;
    int64_t v50; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    void* v72;
    void* v59;
    int64_t v91; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v16;
    int64_t v30; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    void* v66;
    int64_t v21;
    uint8_t v79;
    bool v100;
    int64_t v32; /* placeholder for void */
    int64_t v39;
    void* v35;
    bool v53;
    int64_t v5;
    int64_t v71;
    void* v45;
    int64_t v90; /* placeholder for void */
    void* v69;
    int64_t v28;
    int64_t v19; /* placeholder for void */
    int64_t v52;
    int64_t v67; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    void* v81;
    int64_t v93;
    int64_t v23; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    void* v78;
    int64_t v27;
    int64_t v46; /* placeholder for void */
    int64_t v58;
    int64_t v80; /* placeholder for void */
    int64_t v12;
    int64_t v84; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v47;
    void* v18;
    bool v29;
    void* v63;
    int64_t v76; /* placeholder for void */
    int64_t v38;
    int64_t v99; /* placeholder for void */
    int64_t v55;
    int64_t v68;
    int64_t v106; /* placeholder for void */
    int64_t v51;
    int64_t v17;
    int64_t v64; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    bool v108;
    int64_t v13;
    int64_t v36; /* placeholder for void */
    uint8_t v82;
    bool v42;
    int64_t v43; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v103;
    int64_t v25; /* placeholder for void */
    int64_t v74;
    int64_t v3;
    bool v7;
    bool v22;
    int64_t v56;
    int64_t v94;
    int64_t v96; /* placeholder for void */
    int64_t v104;
    bool v31;
    int64_t v10; /* placeholder for void */
    int64_t v34;
    int64_t v109; /* placeholder for void */
    bool v24;
    int64_t v105;
    int64_t v62;
    int64_t v15; /* placeholder for void */
    void* v75;
    int64_t v57;
    int64_t v8; /* placeholder for void */
    int64_t v61;
    int64_t v40;
    int64_t v70; /* placeholder for void */
    bool v87;
    void* v48;
    int64_t v26; /* placeholder for void */
    int64_t v20;
    bool v9;
    bool v4;
    bool v44;
    int64_t v85;
    void* v14;
    int64_t v92; /* placeholder for void */
    int64_t v95;
    int64_t v37; /* placeholder for void */
    int64_t v41;
    int64_t v65;
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

void HashMap_grow(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_grow");
    int64_t v227; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    int64_t v140;
    int64_t v5; /* placeholder for void */
    void* v127;
    int64_t v54; /* placeholder for void */
    int64_t v123;
    int64_t v162; /* placeholder for void */
    int64_t v110;
    int64_t v141;
    int64_t v264; /* placeholder for void */
    int64_t v257;
    void* v166;
    int64_t v25; /* placeholder for void */
    void* v103;
    int64_t v13;
    int64_t v205;
    int64_t v235; /* placeholder for void */
    int64_t v44;
    int64_t v69;
    void* v6;
    int64_t v100; /* placeholder for void */
    int64_t v202; /* placeholder for void */
    void* v77;
    int64_t v158;
    int64_t v15; /* placeholder for void */
    int64_t v221;
    void* v52;
    void* v20;
    int64_t v55;
    int64_t v109;
    int64_t v40;
    int64_t v85; /* placeholder for void */
    int64_t v225;
    int64_t v16;
    int64_t v157; /* placeholder for void */
    bool v67;
    int64_t v81;
    int64_t v48; /* placeholder for void */
    int64_t v36;
    int64_t v261; /* placeholder for void */
    void* v53;
    int64_t v204;
    void* v118;
    int64_t v266;
    void* v10;
    bool v196;
    int64_t v201; /* placeholder for void */
    int64_t v262; /* placeholder for void */
    int64_t v8;
    int64_t v212; /* placeholder for void */
    int64_t v254; /* placeholder for void */
    int64_t v170; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v43;
    int64_t v180;
    void* v145;
    int64_t v116;
    int64_t v46; /* placeholder for void */
    int64_t v200;
    int64_t v126; /* placeholder for void */
    int64_t v160;
    void* v199;
    void* v58;
    int64_t v92; /* placeholder for void */
    int64_t v207;
    int64_t v179;
    void* v87;
    int64_t v186;
    int64_t v146; /* placeholder for void */
    int64_t v275;
    void* v187;
    int64_t v102;
    int64_t v218; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    bool v41;
    int64_t v215;
    int64_t v112; /* placeholder for void */
    int64_t v147;
    int64_t v31; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v173;
    int64_t v234;
    int64_t v98;
    int64_t v233;
    int64_t v22;
    int64_t v230; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    void* v274;
    int64_t v2;
    int64_t v278; /* placeholder for void */
    void* v23;
    int64_t v222;
    int64_t v82;
    int64_t v131;
    int64_t v168;
    int64_t v260;
    int64_t v34; /* placeholder for void */
    int64_t v136;
    void* v208;
    int64_t v104; /* placeholder for void */
    int64_t v56;
    int64_t v128;
    int64_t v242; /* placeholder for void */
    int64_t v114;
    int64_t v134;
    int64_t v108; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    bool v258;
    bool v111;
    int64_t v143;
    int64_t v185; /* placeholder for void */
    int64_t v232;
    int64_t v88; /* placeholder for void */
    int64_t v192; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v105;
    int64_t v115;
    int64_t v164;
    bool v226;
    int64_t v229; /* placeholder for void */
    int64_t v238;
    int64_t v273; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v150;
    int64_t v144;
    int64_t v122; /* placeholder for void */
    void* v37;
    bool v138;
    int64_t v189; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    void* v148;
    void* v190;
    int64_t v253; /* placeholder for void */
    void* v268;
    int64_t v171; /* placeholder for void */
    int64_t v65;
    void* v90;
    int64_t v130; /* placeholder for void */
    int64_t v51;
    int64_t v177;
    int64_t v252;
    void* v154;
    bool v250;
    int64_t v209; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    bool v174;
    int64_t v237;
    int64_t v167; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    int64_t v9;
    int64_t v99;
    int64_t v175; /* placeholder for void */
    int64_t v70;
    void* v14;
    int64_t v249;
    int64_t v259; /* placeholder for void */
    void* v151;
    int64_t v156; /* placeholder for void */
    void* v124;
    int64_t v96; /* placeholder for void */
    void* v32;
    int64_t v49;
    int64_t v240; /* placeholder for void */
    int64_t v210;
    void* v62;
    int64_t v224;
    int64_t v142;
    int64_t v198; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v30;
    void* v59;
    int64_t v117;
    int64_t v57;
    int64_t v79;
    void* v93;
    int64_t v194;
    int64_t v149; /* placeholder for void */
    int64_t v193; /* placeholder for void */
    void* v181;
    void* v74;
    int64_t v89; /* placeholder for void */
    int64_t v246; /* placeholder for void */
    int64_t v50;
    int64_t v163;
    int64_t v7; /* placeholder for void */
    int64_t v86;
    int64_t v220;
    int64_t v213; /* placeholder for void */
    void* v214;
    int64_t v47; /* placeholder for void */
    int64_t v228;
    int64_t v39;
    int64_t v155;
    void* v265;
    int64_t v219; /* placeholder for void */
    bool v133;
    int64_t v76;
    int64_t v80;
    int64_t v255; /* placeholder for void */
    int64_t v243;
    int64_t v66;
    int64_t v248;
    int64_t v38; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    int64_t v132;
    void* v211;
    int64_t v277; /* placeholder for void */
    void* v29;
    int64_t v71;
    int64_t v73;
    int64_t v183;
    void* v26;
    void* v17;
    int64_t v45;
    int64_t v95; /* placeholder for void */
    int64_t v241; /* placeholder for void */
    int64_t v3;
    int64_t v139; /* placeholder for void */
    int64_t v172;
    int64_t v182; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v113;
    void* v271;
    int64_t v119; /* placeholder for void */
    int64_t v165;
    int64_t v267; /* placeholder for void */
    int64_t v97;
    int64_t v239;
    int64_t v263; /* placeholder for void */
    int64_t v197; /* placeholder for void */
    int64_t v247; /* placeholder for void */
    bool v137;
    int64_t v269;
    int64_t v135;
    int64_t v159;
    int64_t v178;
    int64_t v245;
    void* v184;
    int64_t v191;
    int64_t v61;
    int64_t v27;
    int64_t v216; /* placeholder for void */
    int64_t v33;
    int64_t v203;
    int64_t v19;
    int64_t v83;
    int64_t v244;
    void* v169;
    int64_t v91;
    int64_t v195;
    int64_t v206;
    int64_t v236; /* placeholder for void */
    int64_t v256;
    int64_t v176;
    int64_t v72;
    void* v121;
    int64_t v217; /* placeholder for void */
    int64_t v94;
    int64_t v42; /* placeholder for void */
    int64_t v12;
    void* v161;
    int64_t v272;
    int64_t v231; /* placeholder for void */
    int64_t v223; /* placeholder for void */
    void* v4;
    int64_t v24; /* placeholder for void */
    int64_t v251; /* placeholder for void */
    int64_t v270; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v120;
    void* v106;
    void* v84;
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

void* token_kind_to_str(void* v0, void* v1) {
    p_call_stack_push("token_kind_to_str");
    void* v30;
    int64_t v220;
    int64_t v795;
    void* v674;
    bool v68;
    int64_t v564;
    void* v77;
    int64_t v377; /* placeholder for void */
    void* v798;
    int64_t v909; /* placeholder for void */
    int64_t v685; /* placeholder for void */
    void* v273;
    int64_t v447;
    void* v14;
    int64_t v755; /* placeholder for void */
    bool v334;
    int64_t v69; /* placeholder for void */
    int64_t v808;
    int64_t v864;
    void* v107;
    int64_t v962;
    void* v933;
    int64_t v794;
    void* v458;
    int64_t v696;
    void* v254;
    int64_t v433; /* placeholder for void */
    bool v936;
    void* v910;
    void* v938;
    int64_t v440;
    int64_t v809;
    int64_t v246; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    bool v705;
    int64_t v801;
    int64_t v848; /* placeholder for void */
    int64_t v753;
    void* v415;
    void* v385;
    int64_t v535;
    void* v882;
    int64_t v228;
    void* v287;
    int64_t v937; /* placeholder for void */
    bool v516;
    void* v112;
    void* v226;
    void* v819;
    int64_t v935;
    int64_t v151;
    int64_t v762; /* placeholder for void */
    int64_t v435;
    void* v378;
    void* v653;
    int64_t v729; /* placeholder for void */
    bool v656;
    void* v444;
    void* v807;
    int64_t v197; /* placeholder for void */
    void* v520;
    int64_t v211; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    bool v215;
    int64_t v213;
    int64_t v386; /* placeholder for void */
    int64_t v481; /* placeholder for void */
    int64_t v4;
    int64_t v676;
    bool v313;
    int64_t v540; /* placeholder for void */
    void* v756;
    bool v726;
    bool v26;
    int64_t v202; /* placeholder for void */
    void* v163;
    void* v373;
    void* v499;
    int64_t v613;
    int64_t v675;
    int64_t v853; /* placeholder for void */
    bool v355;
    int64_t v365; /* placeholder for void */
    int64_t v605;
    int64_t v626;
    bool v712;
    int64_t v554; /* placeholder for void */
    int64_t v703;
    int64_t v830;
    bool v250;
    void* v446;
    int64_t v283;
    int64_t v139; /* placeholder for void */
    int64_t v199;
    bool v278;
    void* v338;
    int64_t v104; /* placeholder for void */
    void* v721;
    int64_t v549;
    int64_t v640;
    void* v940;
    void* v623;
    void* v595;
    int64_t v400; /* placeholder for void */
    int64_t v46;
    int64_t v342; /* placeholder for void */
    void* v485;
    void* v672;
    void* v723;
    int64_t v918; /* placeholder for void */
    void* v912;
    int64_t v204; /* placeholder for void */
    int64_t v311;
    int64_t v585;
    double v445;
    bool v131;
    int64_t v255;
    void* v525;
    int64_t v713; /* placeholder for void */
    bool v264;
    void* v597;
    int64_t v970; /* placeholder for void */
    int64_t v169; /* placeholder for void */
    int64_t v410;
    int64_t v491; /* placeholder for void */
    bool v397;
    int64_t v699; /* placeholder for void */
    bool v803;
    int64_t v381;
    int64_t v528;
    void* v926;
    int64_t v8; /* placeholder for void */
    int64_t v251; /* placeholder for void */
    int64_t v860; /* placeholder for void */
    int64_t v452; /* placeholder for void */
    int64_t v622; /* placeholder for void */
    int64_t v416;
    int64_t v722; /* placeholder for void */
    void* v707;
    int64_t v274; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v556;
    bool v89;
    int64_t v132; /* placeholder for void */
    int64_t v234;
    int64_t v265; /* placeholder for void */
    int64_t v130;
    int64_t v314; /* placeholder for void */
    bool v775;
    int64_t v428; /* placeholder for void */
    void* v401;
    void* v611;
    int64_t v787;
    void* v828;
    int64_t v689;
    int64_t v349; /* placeholder for void */
    void* v749;
    int64_t v780;
    int64_t v725;
    int64_t v441;
    void* v658;
    int64_t v624; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    int64_t v895; /* placeholder for void */
    bool v929;
    int64_t v297;
    void* v835;
    int64_t v421; /* placeholder for void */
    int64_t v876; /* placeholder for void */
    int64_t v368;
    bool v663;
    void* v16;
    void* v590;
    int64_t v648;
    void* v473;
    int64_t v599;
    bool v880;
    int64_t v822;
    void* v247;
    int64_t v690;
    int64_t v62; /* placeholder for void */
    void* v282;
    bool v915;
    int64_t v885;
    int64_t v790; /* placeholder for void */
    int64_t v846; /* placeholder for void */
    bool v383;
    int64_t v526; /* placeholder for void */
    bool v614;
    void* v842;
    int64_t v269;
    int64_t v738;
    void* v470;
    int64_t v781;
    void* v931;
    void* v506;
    int64_t v869; /* placeholder for void */
    bool v152;
    int64_t v967; /* placeholder for void */
    int64_t v230; /* placeholder for void */
    int64_t v759;
    bool v40;
    void* v170;
    int64_t v659; /* placeholder for void */
    void* v576;
    bool v796;
    int64_t v328; /* placeholder for void */
    void* v861;
    int64_t v774;
    bool v957;
    bool v12;
    int64_t v31;
    bool v572;
    void* v826;
    int64_t v773;
    int64_t v783; /* placeholder for void */
    void* v303;
    int64_t v344; /* placeholder for void */
    int64_t v171;
    int64_t v291;
    bool v5;
    void* v380;
    int64_t v53;
    bool v761;
    int64_t v769; /* placeholder for void */
    int64_t v375;
    void* v21;
    int64_t v181; /* placeholder for void */
    void* v7;
    bool v943;
    int64_t v155; /* placeholder for void */
    int64_t v162; /* placeholder for void */
    int64_t v694; /* placeholder for void */
    int64_t v127; /* placeholder for void */
    int64_t v888; /* placeholder for void */
    int64_t v550;
    void* v65;
    int64_t v144;
    int64_t v157;
    int64_t v286; /* placeholder for void */
    void* v429;
    void* v268;
    bool v236;
    bool v243;
    int64_t v731;
    int64_t v825; /* placeholder for void */
    int64_t v655;
    void* v289;
    int64_t v813; /* placeholder for void */
    int64_t v372; /* placeholder for void */
    bool v348;
    int64_t v102;
    void* v618;
    int64_t v668;
    int64_t v902; /* placeholder for void */
    int64_t v836;
    int64_t v207;
    void* v805;
    void* v252;
    bool v593;
    bool v187;
    int64_t v752;
    void* v786;
    bool v852;
    int64_t v235;
    void* v175;
    int64_t v248;
    bool v551;
    int64_t v367;
    void* v168;
    void* v331;
    void* v140;
    int64_t v340;
    void* v266;
    int64_t v111; /* placeholder for void */
    void* v681;
    int64_t v116;
    int64_t v309; /* placeholder for void */
    int64_t v186;
    int64_t v123;
    int64_t v356; /* placeholder for void */
    bool v456;
    int64_t v419; /* placeholder for void */
    int64_t v661;
    void* v891;
    void* v539;
    int64_t v17;
    int64_t v430;
    void* v567;
    int64_t v571;
    int64_t v944; /* placeholder for void */
    void* v126;
    int64_t v594; /* placeholder for void */
    void* v205;
    int64_t v242;
    int64_t v398; /* placeholder for void */
    void* v477;
    void* v875;
    void* v637;
    int64_t v899;
    void* v28;
    void* v63;
    void* v546;
    bool v691;
    int64_t v18;
    int64_t v108;
    int64_t v122;
    int64_t v743; /* placeholder for void */
    int64_t v949;
    int64_t v636; /* placeholder for void */
    int64_t v778; /* placeholder for void */
    int64_t v206;
    void* v854;
    int64_t v402;
    int64_t v536;
    bool v222;
    void* v688;
    int64_t v802;
    int64_t v249;
    void* v560;
    int64_t v925; /* placeholder for void */
    int64_t v476; /* placeholder for void */
    void* v686;
    int64_t v923; /* placeholder for void */
    int64_t v647;
    bool v698;
    void* v553;
    int64_t v834; /* placeholder for void */
    int64_t v708; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    bool v208;
    int64_t v704;
    int64_t v326;
    int64_t v146; /* placeholder for void */
    int64_t v172;
    int64_t v101;
    void* v667;
    int64_t v158;
    int64_t v711;
    bool v677;
    bool v61;
    int64_t v258; /* placeholder for void */
    void* v357;
    bool v306;
    int64_t v183; /* placeholder for void */
    int64_t v346;
    void* v969;
    int64_t v13; /* placeholder for void */
    int64_t v512; /* placeholder for void */
    bool v404;
    int64_t v960; /* placeholder for void */
    int64_t v244; /* placeholder for void */
    void* v408;
    int64_t v608; /* placeholder for void */
    void* v352;
    int64_t v164;
    void* v182;
    bool v47;
    int64_t v941;
    void* v581;
    int64_t v160; /* placeholder for void */
    int64_t v379; /* placeholder for void */
    int64_t v631; /* placeholder for void */
    int64_t v134; /* placeholder for void */
    bool v418;
    void* v947;
    int64_t v687; /* placeholder for void */
    int64_t v253; /* placeholder for void */
    int64_t v115;
    bool v684;
    bool v320;
    void* v709;
    int64_t v454;
    int64_t v195; /* placeholder for void */
    void* v765;
    int64_t v857;
    int64_t v32;
    void* v714;
    bool v628;
    int64_t v841; /* placeholder for void */
    void* v280;
    void* v203;
    void* v849;
    int64_t v423;
    int64_t v27; /* placeholder for void */
    void* v296;
    int64_t v20; /* placeholder for void */
    int64_t v223; /* placeholder for void */
    int64_t v358; /* placeholder for void */
    void* v463;
    int64_t v493;
    bool v285;
    int64_t v610; /* placeholder for void */
    int64_t v900;
    void* v147;
    int64_t v174; /* placeholder for void */
    void* v301;
    void* v793;
    void* v751;
    int64_t v148; /* placeholder for void */
    int64_t v827; /* placeholder for void */
    bool v558;
    int64_t v557;
    int64_t v587; /* placeholder for void */
    int64_t v293; /* placeholder for void */
    int64_t v939; /* placeholder for void */
    bool v442;
    int64_t v596; /* placeholder for void */
    int64_t v650; /* placeholder for void */
    void* v105;
    int64_t v74;
    int64_t v570;
    int64_t v354;
    int64_t v417;
    void* v465;
    bool v887;
    int64_t v185;
    int64_t v87;
    void* v534;
    bool v369;
    bool v754;
    int64_t v545; /* placeholder for void */
    int64_t v638; /* placeholder for void */
    int64_t v484; /* placeholder for void */
    int64_t v874; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v370; /* placeholder for void */
    void* v518;
    int64_t v629; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v588;
    void* v35;
    void* v79;
    bool v432;
    void* v639;
    bool v96;
    int64_t v515;
    int64_t v760;
    bool v719;
    bool v922;
    int64_t v872;
    bool v166;
    void* v275;
    int64_t v797; /* placeholder for void */
    bool v747;
    int64_t v764; /* placeholder for void */
    bool v859;
    int64_t v60;
    bool v635;
    void* v210;
    void* v399;
    int64_t v323; /* placeholder for void */
    int64_t v260; /* placeholder for void */
    void* v884;
    int64_t v443; /* placeholder for void */
    int64_t v816;
    void* v966;
    int64_t v218; /* placeholder for void */
    bool v586;
    void* v609;
    int64_t v41; /* placeholder for void */
    void* v784;
    int64_t v150;
    int64_t v494;
    int64_t v811; /* placeholder for void */
    void* v777;
    int64_t v318;
    int64_t v431;
    int64_t v776; /* placeholder for void */
    void* v604;
    void* v219;
    int64_t v353;
    int64_t v750; /* placeholder for void */
    void* v569;
    int64_t v883; /* placeholder for void */
    int64_t v94;
    void* v770;
    void* v651;
    bool v229;
    int64_t v403;
    void* v665;
    void* v763;
    int64_t v267; /* placeholder for void */
    int64_t v662;
    int64_t v15; /* placeholder for void */
    bool v138;
    int64_t v395;
    int64_t v968; /* placeholder for void */
    void* v679;
    void* v9;
    void* v462;
    int64_t v832; /* placeholder for void */
    int64_t v948;
    void* v439;
    int64_t v766;
    void* v394;
    int64_t v335; /* placeholder for void */
    int64_t v225; /* placeholder for void */
    int64_t v321; /* placeholder for void */
    int64_t v916; /* placeholder for void */
    bool v789;
    int64_t v503; /* placeholder for void */
    int64_t v927;
    void* v461;
    int64_t v867; /* placeholder for void */
    int64_t v911; /* placeholder for void */
    int64_t v612;
    bool v292;
    int64_t v718;
    bool v299;
    void* v877;
    bool v425;
    int64_t v276;
    void* v475;
    void* v161;
    int64_t v347;
    bool v480;
    int64_t v634;
    void* v392;
    int64_t v136;
    bool v782;
    int64_t v374;
    int64_t v706; /* placeholder for void */
    int64_t v615; /* placeholder for void */
    int64_t v478;
    int64_t v81;
    int64_t v424;
    int64_t v221;
    int64_t v455;
    void* v772;
    int64_t v426; /* placeholder for void */
    int64_t v479;
    void* v98;
    void* v156;
    int64_t v946; /* placeholder for void */
    void* v504;
    int64_t v165;
    bool v866;
    int64_t v141; /* placeholder for void */
    bool v831;
    int64_t v878;
    int64_t v11;
    int64_t v710;
    bool v810;
    void* v119;
    void* v184;
    void* v471;
    int64_t v396;
    bool v649;
    bool v950;
    int64_t v106; /* placeholder for void */
    bool v488;
    void* v919;
    void* v322;
    int64_t v727; /* placeholder for void */
    int64_t v178;
    void* v482;
    int64_t v720; /* placeholder for void */
    int64_t v241;
    void* v133;
    void* v56;
    bool v670;
    int64_t v73;
    void* v84;
    int64_t v304;
    int64_t v508;
    void* v924;
    int64_t v281; /* placeholder for void */
    void* v231;
    bool v824;
    int64_t v487;
    bool v894;
    int64_t v496; /* placeholder for void */
    void* v562;
    void* v460;
    int64_t v942;
    void* v44;
    bool v495;
    void* v189;
    int64_t v748; /* placeholder for void */
    int64_t v601; /* placeholder for void */
    int64_t v734; /* placeholder for void */
    int64_t v914;
    int64_t v333;
    int64_t v237; /* placeholder for void */
    void* v191;
    void* v574;
    void* v2;
    void* v294;
    void* v224;
    void* v350;
    int64_t v118; /* placeholder for void */
    int64_t v871;
    int64_t v29; /* placeholder for void */
    bool v544;
    void* v959;
    void* v177;
    int64_t v330; /* placeholder for void */
    void* v548;
    int64_t v109;
    int64_t v153; /* placeholder for void */
    int64_t v262;
    bool v117;
    void* v142;
    void* v310;
    int64_t v542;
    void* v555;
    void* v345;
    void* v450;
    int64_t v573; /* placeholder for void */
    void* v896;
    void* v198;
    int64_t v913;
    void* v735;
    int64_t v533; /* placeholder for void */
    int64_t v806; /* placeholder for void */
    int64_t v851;
    void* v317;
    int64_t v865;
    int64_t v71; /* placeholder for void */
    int64_t v279; /* placeholder for void */
    void* v898;
    int64_t v10;
    bool v327;
    int64_t v543;
    int64_t v284;
    int64_t v388;
    bool v201;
    int64_t v307; /* placeholder for void */
    int64_t v232; /* placeholder for void */
    int64_t v209; /* placeholder for void */
    int64_t v578;
    int64_t v97; /* placeholder for void */
    void* v387;
    int64_t v519; /* placeholder for void */
    int64_t v746;
    int64_t v193;
    int64_t v893;
    int64_t v886;
    int64_t v514;
    void* v730;
    void* v128;
    int64_t v589; /* placeholder for void */
    int64_t v39;
    bool v509;
    int64_t v850;
    int64_t v95;
    int64_t v467;
    int64_t v3;
    void* v511;
    bool v411;
    int64_t v844;
    void* v324;
    int64_t v951; /* placeholder for void */
    void* v217;
    bool v838;
    int64_t v788;
    void* v427;
    void* v149;
    void* v135;
    void* v917;
    int64_t v697;
    void* v212;
    void* v261;
    bool v502;
    void* v240;
    int64_t v393; /* placeholder for void */
    int64_t v645; /* placeholder for void */
    int64_t v907;
    void* v91;
    void* v492;
    void* v93;
    void* v728;
    int64_t v489; /* placeholder for void */
    void* v800;
    void* v870;
    void* v245;
    void* v583;
    int64_t v701; /* placeholder for void */
    int64_t v188; /* placeholder for void */
    void* v541;
    bool v733;
    void* v70;
    void* v238;
    bool v271;
    int64_t v510; /* placeholder for void */
    void* v359;
    int64_t v619;
    int64_t v617; /* placeholder for void */
    bool v194;
    int64_t v272; /* placeholder for void */
    int64_t v890; /* placeholder for void */
    int64_t v319;
    int64_t v591;
    int64_t v325;
    bool v600;
    void* v945;
    void* v114;
    int64_t v892;
    int64_t v930; /* placeholder for void */
    int64_t v559; /* placeholder for void */
    int64_t v256;
    int64_t v36; /* placeholder for void */
    int64_t v521;
    int64_t v592;
    int64_t v906;
    int64_t v436;
    int64_t v438; /* placeholder for void */
    int64_t v529;
    int64_t v500;
    void* v437;
    int64_t v920;
    int64_t v862; /* placeholder for void */
    int64_t v682;
    bool v124;
    int64_t v580; /* placeholder for void */
    int64_t v584;
    int64_t v501;
    int64_t v143;
    int64_t v179;
    void* v233;
    void* v259;
    void* v453;
    int64_t v958; /* placeholder for void */
    void* v329;
    void* v154;
    void* v840;
    int64_t v818; /* placeholder for void */
    int64_t v955;
    void* v660;
    int64_t v277;
    int64_t v176; /* placeholder for void */
    int64_t v897; /* placeholder for void */
    int64_t v239; /* placeholder for void */
    bool v257;
    int64_t v547; /* placeholder for void */
    void* v812;
    void* v474;
    int64_t v167; /* placeholder for void */
    int64_t v270;
    void* v51;
    int64_t v52;
    bool v390;
    int64_t v785; /* placeholder for void */
    int64_t v295; /* placeholder for void */
    int64_t v741; /* placeholder for void */
    void* v833;
    int64_t v389;
    bool v173;
    bool v565;
    void* v308;
    void* v42;
    void* v472;
    int64_t v837;
    int64_t v745;
    int64_t v620;
    int64_t v792; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v757; /* placeholder for void */
    int64_t v855; /* placeholder for void */
    void* v513;
    int64_t v80;
    int64_t v671; /* placeholder for void */
    void* v952;
    void* v602;
    void* v371;
    void* v758;
    int64_t v92; /* placeholder for void */
    int64_t v391; /* placeholder for void */
    void* v644;
    bool v901;
    void* v490;
    void* v791;
    void* v716;
    int64_t v820; /* placeholder for void */
    void* v779;
    int64_t v724;
    void* v37;
    void* v954;
    int64_t v312;
    bool v530;
    void* v100;
    int64_t v507;
    bool v468;
    int64_t v339;
    int64_t v129;
    bool v607;
    bool v845;
    int64_t v263;
    int64_t v316; /* placeholder for void */
    int64_t v412; /* placeholder for void */
    void* v413;
    bool v873;
    void* v905;
    bool v740;
    int64_t v45;
    int64_t v332;
    int64_t v736; /* placeholder for void */
    int64_t v566; /* placeholder for void */
    int64_t v67;
    int64_t v732;
    void* v483;
    void* v86;
    int64_t v804; /* placeholder for void */
    int64_t v568; /* placeholder for void */
    int64_t v414; /* placeholder for void */
    void* v366;
    int64_t v678; /* placeholder for void */
    void* v868;
    int64_t v76; /* placeholder for void */
    bool v964;
    int64_t v739;
    int64_t v382;
    int64_t v99; /* placeholder for void */
    void* v630;
    int64_t v524; /* placeholder for void */
    void* v532;
    int64_t v486;
    void* v814;
    bool v19;
    int64_t v337; /* placeholder for void */
    int64_t v407; /* placeholder for void */
    bool v103;
    void* v315;
    void* v434;
    int64_t v839; /* placeholder for void */
    int64_t v522;
    void* v856;
    bool v621;
    int64_t v190; /* placeholder for void */
    bool v54;
    bool v180;
    bool v75;
    void* v497;
    int64_t v405; /* placeholder for void */
    void* v449;
    void* v737;
    int64_t v881; /* placeholder for void */
    bool v110;
    int64_t v216; /* placeholder for void */
    int64_t v464; /* placeholder for void */
    int64_t v641;
    void* v903;
    int64_t v137;
    void* v336;
    void* v364;
    int64_t v652; /* placeholder for void */
    void* v695;
    int64_t v858;
    void* v343;
    int64_t v298;
    int64_t v288; /* placeholder for void */
    int64_t v715; /* placeholder for void */
    bool v523;
    void* v646;
    void* v821;
    int64_t v305;
    int64_t v921;
    int64_t v505; /* placeholder for void */
    void* v406;
    int64_t v531; /* placeholder for void */
    void* v625;
    void* v702;
    int64_t v113; /* placeholder for void */
    int64_t v66;
    int64_t v351; /* placeholder for void */
    void* v847;
    int64_t v879;
    int64_t v38;
    int64_t v363; /* placeholder for void */
    int64_t v666; /* placeholder for void */
    void* v742;
    int64_t v603; /* placeholder for void */
    int64_t v934;
    int64_t v290;
    void* v527;
    void* v420;
    int64_t v582; /* placeholder for void */
    int64_t v823;
    int64_t v64; /* placeholder for void */
    int64_t v692; /* placeholder for void */
    bool v82;
    int64_t v200;
    int64_t v85; /* placeholder for void */
    int64_t v538; /* placeholder for void */
    void* v744;
    int64_t v302; /* placeholder for void */
    bool v908;
    void* v451;
    bool v642;
    void* v700;
    int64_t v563;
    int64_t v50; /* placeholder for void */
    bool v362;
    int64_t v683;
    int64_t v654;
    int64_t v606;
    int64_t v932; /* placeholder for void */
    int64_t v227;
    int64_t v83; /* placeholder for void */
    int64_t v673; /* placeholder for void */
    void* v448;
    int64_t v192;
    void* v632;
    int64_t v643; /* placeholder for void */
    int64_t v904; /* placeholder for void */
    int64_t v409;
    int64_t v466;
    int64_t v498; /* placeholder for void */
    int64_t v956;
    int64_t v517; /* placeholder for void */
    int64_t v717;
    int64_t v561; /* placeholder for void */
    bool v537;
    int64_t v300; /* placeholder for void */
    void* v196;
    void* v58;
    int64_t v384; /* placeholder for void */
    int64_t v664; /* placeholder for void */
    int64_t v767;
    int64_t v963;
    int64_t v360;
    bool v579;
    int64_t v59;
    int64_t v799; /* placeholder for void */
    int64_t v88;
    int64_t v552; /* placeholder for void */
    int64_t v598;
    int64_t v965; /* placeholder for void */
    int64_t v627;
    bool v33;
    int64_t v771; /* placeholder for void */
    bool v376;
    int64_t v843;
    int64_t v953; /* placeholder for void */
    int64_t v361;
    int64_t v829;
    void* v889;
    void* v961;
    int64_t v633;
    int64_t v669;
    void* v863;
    int64_t v457; /* placeholder for void */
    void* v49;
    int64_t v657; /* placeholder for void */
    bool v817;
    int64_t v55; /* placeholder for void */
    void* v422;
    int64_t v120; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    bool v145;
    void* v459;
    int64_t v25;
    int64_t v469; /* placeholder for void */
    int64_t v577;
    int64_t v815;
    int64_t v24;
    bool v341;
    bool v768;
    int64_t v214;
    bool v159;
    int64_t v680; /* placeholder for void */
    void* v121;
    void* v23;
    int64_t v928;
    void* v616;
    void* v693;
    void* v72;
    int64_t v575; /* placeholder for void */
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v969 = 0LL;
    p_call_stack_pop();
    return v969;
  bb2:
    v7 = _str1;
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
    v14 = _str2;
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
    v21 = _str3;
    p_call_stack_pop();
    return v21;
  bb7:
    v23 = (char*)v1 + 0;
    v24 = *(int64_t*)v23;
    v25 = 3LL;
    v26 = v24 == v25;
    if (v26) goto bb8;
    else goto bb9;
  bb8:
    v28 = _str4;
    p_call_stack_pop();
    return v28;
  bb9:
    v30 = (char*)v1 + 0;
    v31 = *(int64_t*)v30;
    v32 = 4LL;
    v33 = v31 == v32;
    if (v33) goto bb10;
    else goto bb11;
  bb10:
    v35 = _str5;
    p_call_stack_pop();
    return v35;
  bb11:
    v37 = (char*)v1 + 0;
    v38 = *(int64_t*)v37;
    v39 = 5LL;
    v40 = v38 == v39;
    if (v40) goto bb12;
    else goto bb13;
  bb12:
    v42 = _str6;
    p_call_stack_pop();
    return v42;
  bb13:
    v44 = (char*)v1 + 0;
    v45 = *(int64_t*)v44;
    v46 = 6LL;
    v47 = v45 == v46;
    if (v47) goto bb14;
    else goto bb15;
  bb14:
    v49 = _str7;
    p_call_stack_pop();
    return v49;
  bb15:
    v51 = (char*)v1 + 0;
    v52 = *(int64_t*)v51;
    v53 = 7LL;
    v54 = v52 == v53;
    if (v54) goto bb16;
    else goto bb17;
  bb16:
    v56 = _str8;
    p_call_stack_pop();
    return v56;
  bb17:
    v58 = (char*)v1 + 0;
    v59 = *(int64_t*)v58;
    v60 = 8LL;
    v61 = v59 == v60;
    if (v61) goto bb18;
    else goto bb19;
  bb18:
    v63 = _str9;
    p_call_stack_pop();
    return v63;
  bb19:
    v65 = (char*)v1 + 0;
    v66 = *(int64_t*)v65;
    v67 = 9LL;
    v68 = v66 == v67;
    if (v68) goto bb20;
    else goto bb21;
  bb20:
    v70 = _str10;
    p_call_stack_pop();
    return v70;
  bb21:
    v72 = (char*)v1 + 0;
    v73 = *(int64_t*)v72;
    v74 = 10LL;
    v75 = v73 == v74;
    if (v75) goto bb22;
    else goto bb23;
  bb22:
    v77 = _str11;
    p_call_stack_pop();
    return v77;
  bb23:
    v79 = (char*)v1 + 0;
    v80 = *(int64_t*)v79;
    v81 = 11LL;
    v82 = v80 == v81;
    if (v82) goto bb24;
    else goto bb25;
  bb24:
    v84 = _str12;
    p_call_stack_pop();
    return v84;
  bb25:
    v86 = (char*)v1 + 0;
    v87 = *(int64_t*)v86;
    v88 = 12LL;
    v89 = v87 == v88;
    if (v89) goto bb26;
    else goto bb27;
  bb26:
    v91 = _str13;
    p_call_stack_pop();
    return v91;
  bb27:
    v93 = (char*)v1 + 0;
    v94 = *(int64_t*)v93;
    v95 = 13LL;
    v96 = v94 == v95;
    if (v96) goto bb28;
    else goto bb29;
  bb28:
    v98 = _str14;
    p_call_stack_pop();
    return v98;
  bb29:
    v100 = (char*)v1 + 0;
    v101 = *(int64_t*)v100;
    v102 = 14LL;
    v103 = v101 == v102;
    if (v103) goto bb30;
    else goto bb31;
  bb30:
    v105 = _str15;
    p_call_stack_pop();
    return v105;
  bb31:
    v107 = (char*)v1 + 0;
    v108 = *(int64_t*)v107;
    v109 = 15LL;
    v110 = v108 == v109;
    if (v110) goto bb32;
    else goto bb33;
  bb32:
    v112 = _str16;
    p_call_stack_pop();
    return v112;
  bb33:
    v114 = (char*)v1 + 0;
    v115 = *(int64_t*)v114;
    v116 = 16LL;
    v117 = v115 == v116;
    if (v117) goto bb34;
    else goto bb35;
  bb34:
    v119 = _str17;
    p_call_stack_pop();
    return v119;
  bb35:
    v121 = (char*)v1 + 0;
    v122 = *(int64_t*)v121;
    v123 = 17LL;
    v124 = v122 == v123;
    if (v124) goto bb36;
    else goto bb37;
  bb36:
    v126 = _str18;
    p_call_stack_pop();
    return v126;
  bb37:
    v128 = (char*)v1 + 0;
    v129 = *(int64_t*)v128;
    v130 = 18LL;
    v131 = v129 == v130;
    if (v131) goto bb38;
    else goto bb39;
  bb38:
    v133 = _str19;
    p_call_stack_pop();
    return v133;
  bb39:
    v135 = (char*)v1 + 0;
    v136 = *(int64_t*)v135;
    v137 = 19LL;
    v138 = v136 == v137;
    if (v138) goto bb40;
    else goto bb41;
  bb40:
    v140 = _str20;
    p_call_stack_pop();
    return v140;
  bb41:
    v142 = (char*)v1 + 0;
    v143 = *(int64_t*)v142;
    v144 = 20LL;
    v145 = v143 == v144;
    if (v145) goto bb42;
    else goto bb43;
  bb42:
    v147 = _str21;
    p_call_stack_pop();
    return v147;
  bb43:
    v149 = (char*)v1 + 0;
    v150 = *(int64_t*)v149;
    v151 = 21LL;
    v152 = v150 == v151;
    if (v152) goto bb44;
    else goto bb45;
  bb44:
    v154 = _str22;
    p_call_stack_pop();
    return v154;
  bb45:
    v156 = (char*)v1 + 0;
    v157 = *(int64_t*)v156;
    v158 = 22LL;
    v159 = v157 == v158;
    if (v159) goto bb46;
    else goto bb47;
  bb46:
    v161 = _str23;
    p_call_stack_pop();
    return v161;
  bb47:
    v163 = (char*)v1 + 0;
    v164 = *(int64_t*)v163;
    v165 = 23LL;
    v166 = v164 == v165;
    if (v166) goto bb48;
    else goto bb49;
  bb48:
    v168 = _str24;
    p_call_stack_pop();
    return v168;
  bb49:
    v170 = (char*)v1 + 0;
    v171 = *(int64_t*)v170;
    v172 = 24LL;
    v173 = v171 == v172;
    if (v173) goto bb50;
    else goto bb51;
  bb50:
    v175 = _str25;
    p_call_stack_pop();
    return v175;
  bb51:
    v177 = (char*)v1 + 0;
    v178 = *(int64_t*)v177;
    v179 = 25LL;
    v180 = v178 == v179;
    if (v180) goto bb52;
    else goto bb53;
  bb52:
    v182 = _str26;
    p_call_stack_pop();
    return v182;
  bb53:
    v184 = (char*)v1 + 0;
    v185 = *(int64_t*)v184;
    v186 = 26LL;
    v187 = v185 == v186;
    if (v187) goto bb54;
    else goto bb55;
  bb54:
    v189 = _str27;
    p_call_stack_pop();
    return v189;
  bb55:
    v191 = (char*)v1 + 0;
    v192 = *(int64_t*)v191;
    v193 = 27LL;
    v194 = v192 == v193;
    if (v194) goto bb56;
    else goto bb57;
  bb56:
    v196 = _str28;
    p_call_stack_pop();
    return v196;
  bb57:
    v198 = (char*)v1 + 0;
    v199 = *(int64_t*)v198;
    v200 = 28LL;
    v201 = v199 == v200;
    if (v201) goto bb58;
    else goto bb59;
  bb58:
    v203 = _str29;
    p_call_stack_pop();
    return v203;
  bb59:
    v205 = (char*)v1 + 0;
    v206 = *(int64_t*)v205;
    v207 = 29LL;
    v208 = v206 == v207;
    if (v208) goto bb60;
    else goto bb61;
  bb60:
    v210 = _str30;
    p_call_stack_pop();
    return v210;
  bb61:
    v212 = (char*)v1 + 0;
    v213 = *(int64_t*)v212;
    v214 = 30LL;
    v215 = v213 == v214;
    if (v215) goto bb62;
    else goto bb63;
  bb62:
    v217 = _str31;
    p_call_stack_pop();
    return v217;
  bb63:
    v219 = (char*)v1 + 0;
    v220 = *(int64_t*)v219;
    v221 = 31LL;
    v222 = v220 == v221;
    if (v222) goto bb64;
    else goto bb65;
  bb64:
    v224 = _str32;
    p_call_stack_pop();
    return v224;
  bb65:
    v226 = (char*)v1 + 0;
    v227 = *(int64_t*)v226;
    v228 = 32LL;
    v229 = v227 == v228;
    if (v229) goto bb66;
    else goto bb67;
  bb66:
    v231 = _str33;
    p_call_stack_pop();
    return v231;
  bb67:
    v233 = (char*)v1 + 0;
    v234 = *(int64_t*)v233;
    v235 = 33LL;
    v236 = v234 == v235;
    if (v236) goto bb68;
    else goto bb69;
  bb68:
    v238 = _str34;
    p_call_stack_pop();
    return v238;
  bb69:
    v240 = (char*)v1 + 0;
    v241 = *(int64_t*)v240;
    v242 = 34LL;
    v243 = v241 == v242;
    if (v243) goto bb70;
    else goto bb71;
  bb70:
    v245 = _str35;
    p_call_stack_pop();
    return v245;
  bb71:
    v247 = (char*)v1 + 0;
    v248 = *(int64_t*)v247;
    v249 = 35LL;
    v250 = v248 == v249;
    if (v250) goto bb72;
    else goto bb73;
  bb72:
    v252 = _str36;
    p_call_stack_pop();
    return v252;
  bb73:
    v254 = (char*)v1 + 0;
    v255 = *(int64_t*)v254;
    v256 = 36LL;
    v257 = v255 == v256;
    if (v257) goto bb74;
    else goto bb75;
  bb74:
    v259 = _str37;
    p_call_stack_pop();
    return v259;
  bb75:
    v261 = (char*)v1 + 0;
    v262 = *(int64_t*)v261;
    v263 = 37LL;
    v264 = v262 == v263;
    if (v264) goto bb76;
    else goto bb77;
  bb76:
    v266 = _str38;
    p_call_stack_pop();
    return v266;
  bb77:
    v268 = (char*)v1 + 0;
    v269 = *(int64_t*)v268;
    v270 = 38LL;
    v271 = v269 == v270;
    if (v271) goto bb78;
    else goto bb79;
  bb78:
    v273 = _str39;
    p_call_stack_pop();
    return v273;
  bb79:
    v275 = (char*)v1 + 0;
    v276 = *(int64_t*)v275;
    v277 = 39LL;
    v278 = v276 == v277;
    if (v278) goto bb80;
    else goto bb81;
  bb80:
    v280 = _str40;
    p_call_stack_pop();
    return v280;
  bb81:
    v282 = (char*)v1 + 0;
    v283 = *(int64_t*)v282;
    v284 = 40LL;
    v285 = v283 == v284;
    if (v285) goto bb82;
    else goto bb83;
  bb82:
    v287 = _str41;
    p_call_stack_pop();
    return v287;
  bb83:
    v289 = (char*)v1 + 0;
    v290 = *(int64_t*)v289;
    v291 = 41LL;
    v292 = v290 == v291;
    if (v292) goto bb84;
    else goto bb85;
  bb84:
    v294 = _str42;
    p_call_stack_pop();
    return v294;
  bb85:
    v296 = (char*)v1 + 0;
    v297 = *(int64_t*)v296;
    v298 = 42LL;
    v299 = v297 == v298;
    if (v299) goto bb86;
    else goto bb87;
  bb86:
    v301 = _str43;
    p_call_stack_pop();
    return v301;
  bb87:
    v303 = (char*)v1 + 0;
    v304 = *(int64_t*)v303;
    v305 = 43LL;
    v306 = v304 == v305;
    if (v306) goto bb88;
    else goto bb89;
  bb88:
    v308 = _str44;
    p_call_stack_pop();
    return v308;
  bb89:
    v310 = (char*)v1 + 0;
    v311 = *(int64_t*)v310;
    v312 = 44LL;
    v313 = v311 == v312;
    if (v313) goto bb90;
    else goto bb91;
  bb90:
    v315 = _str45;
    p_call_stack_pop();
    return v315;
  bb91:
    v317 = (char*)v1 + 0;
    v318 = *(int64_t*)v317;
    v319 = 45LL;
    v320 = v318 == v319;
    if (v320) goto bb92;
    else goto bb93;
  bb92:
    v322 = _str46;
    p_call_stack_pop();
    return v322;
  bb93:
    v324 = (char*)v1 + 0;
    v325 = *(int64_t*)v324;
    v326 = 46LL;
    v327 = v325 == v326;
    if (v327) goto bb94;
    else goto bb95;
  bb94:
    v329 = _str47;
    p_call_stack_pop();
    return v329;
  bb95:
    v331 = (char*)v1 + 0;
    v332 = *(int64_t*)v331;
    v333 = 47LL;
    v334 = v332 == v333;
    if (v334) goto bb96;
    else goto bb97;
  bb96:
    v336 = _str48;
    p_call_stack_pop();
    return v336;
  bb97:
    v338 = (char*)v1 + 0;
    v339 = *(int64_t*)v338;
    v340 = 48LL;
    v341 = v339 == v340;
    if (v341) goto bb98;
    else goto bb99;
  bb98:
    v343 = _str49;
    p_call_stack_pop();
    return v343;
  bb99:
    v345 = (char*)v1 + 0;
    v346 = *(int64_t*)v345;
    v347 = 49LL;
    v348 = v346 == v347;
    if (v348) goto bb100;
    else goto bb101;
  bb100:
    v350 = _str50;
    p_call_stack_pop();
    return v350;
  bb101:
    v352 = (char*)v1 + 0;
    v353 = *(int64_t*)v352;
    v354 = 50LL;
    v355 = v353 == v354;
    if (v355) goto bb102;
    else goto bb103;
  bb102:
    v357 = _str51;
    p_call_stack_pop();
    return v357;
  bb103:
    v359 = (char*)v1 + 0;
    v360 = *(int64_t*)v359;
    v361 = 51LL;
    v362 = v360 == v361;
    if (v362) goto bb104;
    else goto bb105;
  bb104:
    v364 = _str52;
    p_call_stack_pop();
    return v364;
  bb105:
    v366 = (char*)v1 + 0;
    v367 = *(int64_t*)v366;
    v368 = 52LL;
    v369 = v367 == v368;
    if (v369) goto bb106;
    else goto bb107;
  bb106:
    v371 = _str53;
    p_call_stack_pop();
    return v371;
  bb107:
    v373 = (char*)v1 + 0;
    v374 = *(int64_t*)v373;
    v375 = 53LL;
    v376 = v374 == v375;
    if (v376) goto bb108;
    else goto bb109;
  bb108:
    v378 = _str54;
    p_call_stack_pop();
    return v378;
  bb109:
    v380 = (char*)v1 + 0;
    v381 = *(int64_t*)v380;
    v382 = 54LL;
    v383 = v381 == v382;
    if (v383) goto bb110;
    else goto bb111;
  bb110:
    v385 = _str55;
    p_call_stack_pop();
    return v385;
  bb111:
    v387 = (char*)v1 + 0;
    v388 = *(int64_t*)v387;
    v389 = 55LL;
    v390 = v388 == v389;
    if (v390) goto bb112;
    else goto bb113;
  bb112:
    v392 = _str56;
    p_call_stack_pop();
    return v392;
  bb113:
    v394 = (char*)v1 + 0;
    v395 = *(int64_t*)v394;
    v396 = 56LL;
    v397 = v395 == v396;
    if (v397) goto bb114;
    else goto bb115;
  bb114:
    v399 = _str57;
    p_call_stack_pop();
    return v399;
  bb115:
    v401 = (char*)v1 + 0;
    v402 = *(int64_t*)v401;
    v403 = 57LL;
    v404 = v402 == v403;
    if (v404) goto bb116;
    else goto bb117;
  bb116:
    v406 = _str58;
    p_call_stack_pop();
    return v406;
  bb117:
    v408 = (char*)v1 + 0;
    v409 = *(int64_t*)v408;
    v410 = 58LL;
    v411 = v409 == v410;
    if (v411) goto bb118;
    else goto bb119;
  bb118:
    v413 = _str59;
    p_call_stack_pop();
    return v413;
  bb119:
    v415 = (char*)v1 + 0;
    v416 = *(int64_t*)v415;
    v417 = 59LL;
    v418 = v416 == v417;
    if (v418) goto bb120;
    else goto bb121;
  bb120:
    v420 = _str60;
    p_call_stack_pop();
    return v420;
  bb121:
    v422 = (char*)v1 + 0;
    v423 = *(int64_t*)v422;
    v424 = 60LL;
    v425 = v423 == v424;
    if (v425) goto bb122;
    else goto bb123;
  bb122:
    v427 = _str61;
    p_call_stack_pop();
    return v427;
  bb123:
    v429 = (char*)v1 + 0;
    v430 = *(int64_t*)v429;
    v431 = 76LL;
    v432 = v430 == v431;
    if (v432) goto bb124;
    else goto bb125;
  bb124:
    v434 = (char*)v1 + 8;
    v435 = *(int64_t*)v434;
    v436 = 0LL;
    v437 = int_to_str((void*)v436, v435);
    p_call_stack_pop();
    return v437;
  bb125:
    v439 = (char*)v1 + 0;
    v440 = *(int64_t*)v439;
    v441 = 77LL;
    v442 = v440 == v441;
    if (v442) goto bb126;
    else goto bb127;
  bb126:
    v444 = (char*)v1 + 8;
    v445 = *(double*)v444;
    v446 = _str62;
    v447 = 0LL;
    v448 = float_to_str((void*)v447, v445);
    v449 = p_str_concat(v446, v448);
    v450 = _str63;
    v451 = p_str_concat(v449, v450);
    p_call_stack_pop();
    return v451;
  bb127:
    v453 = (char*)v1 + 0;
    v454 = *(int64_t*)v453;
    v455 = 78LL;
    v456 = v454 == v455;
    if (v456) goto bb128;
    else goto bb129;
  bb128:
    v458 = (char*)v1 + 8;
    v459 = *(void**)v458;
    v460 = _str64;
    v461 = p_str_concat(v460, v459);
    v462 = _str64;
    v463 = p_str_concat(v461, v462);
    p_call_stack_pop();
    return v463;
  bb129:
    v465 = (char*)v1 + 0;
    v466 = *(int64_t*)v465;
    v467 = 79LL;
    v468 = v466 == v467;
    if (v468) goto bb130;
    else goto bb131;
  bb130:
    v470 = (char*)v1 + 8;
    v471 = *(void**)v470;
    v472 = _str65;
    v473 = p_str_concat(v472, v471);
    v474 = _str66;
    v475 = p_str_concat(v473, v474);
    p_call_stack_pop();
    return v475;
  bb131:
    v477 = (char*)v1 + 0;
    v478 = *(int64_t*)v477;
    v479 = 80LL;
    v480 = v478 == v479;
    if (v480) goto bb132;
    else goto bb133;
  bb132:
    v482 = (char*)v1 + 8;
    v483 = *(void**)v482;
    p_call_stack_pop();
    return v483;
  bb133:
    v485 = (char*)v1 + 0;
    v486 = *(int64_t*)v485;
    v487 = 134LL;
    v488 = v486 == v487;
    if (v488) goto bb134;
    else goto bb135;
  bb134:
    v490 = _str67;
    p_call_stack_pop();
    return v490;
  bb135:
    v492 = (char*)v1 + 0;
    v493 = *(int64_t*)v492;
    v494 = 61LL;
    v495 = v493 == v494;
    if (v495) goto bb136;
    else goto bb137;
  bb136:
    v497 = _str68;
    p_call_stack_pop();
    return v497;
  bb137:
    v499 = (char*)v1 + 0;
    v500 = *(int64_t*)v499;
    v501 = 62LL;
    v502 = v500 == v501;
    if (v502) goto bb138;
    else goto bb139;
  bb138:
    v504 = _str69;
    p_call_stack_pop();
    return v504;
  bb139:
    v506 = (char*)v1 + 0;
    v507 = *(int64_t*)v506;
    v508 = 63LL;
    v509 = v507 == v508;
    if (v509) goto bb140;
    else goto bb141;
  bb140:
    v511 = _str70;
    p_call_stack_pop();
    return v511;
  bb141:
    v513 = (char*)v1 + 0;
    v514 = *(int64_t*)v513;
    v515 = 64LL;
    v516 = v514 == v515;
    if (v516) goto bb142;
    else goto bb143;
  bb142:
    v518 = _str71;
    p_call_stack_pop();
    return v518;
  bb143:
    v520 = (char*)v1 + 0;
    v521 = *(int64_t*)v520;
    v522 = 65LL;
    v523 = v521 == v522;
    if (v523) goto bb144;
    else goto bb145;
  bb144:
    v525 = _str72;
    p_call_stack_pop();
    return v525;
  bb145:
    v527 = (char*)v1 + 0;
    v528 = *(int64_t*)v527;
    v529 = 66LL;
    v530 = v528 == v529;
    if (v530) goto bb146;
    else goto bb147;
  bb146:
    v532 = _str73;
    p_call_stack_pop();
    return v532;
  bb147:
    v534 = (char*)v1 + 0;
    v535 = *(int64_t*)v534;
    v536 = 67LL;
    v537 = v535 == v536;
    if (v537) goto bb148;
    else goto bb149;
  bb148:
    v539 = _str74;
    p_call_stack_pop();
    return v539;
  bb149:
    v541 = (char*)v1 + 0;
    v542 = *(int64_t*)v541;
    v543 = 68LL;
    v544 = v542 == v543;
    if (v544) goto bb150;
    else goto bb151;
  bb150:
    v546 = _str75;
    p_call_stack_pop();
    return v546;
  bb151:
    v548 = (char*)v1 + 0;
    v549 = *(int64_t*)v548;
    v550 = 69LL;
    v551 = v549 == v550;
    if (v551) goto bb152;
    else goto bb153;
  bb152:
    v553 = _str76;
    p_call_stack_pop();
    return v553;
  bb153:
    v555 = (char*)v1 + 0;
    v556 = *(int64_t*)v555;
    v557 = 70LL;
    v558 = v556 == v557;
    if (v558) goto bb154;
    else goto bb155;
  bb154:
    v560 = _str77;
    p_call_stack_pop();
    return v560;
  bb155:
    v562 = (char*)v1 + 0;
    v563 = *(int64_t*)v562;
    v564 = 71LL;
    v565 = v563 == v564;
    if (v565) goto bb156;
    else goto bb157;
  bb156:
    v567 = _str78;
    p_call_stack_pop();
    return v567;
  bb157:
    v569 = (char*)v1 + 0;
    v570 = *(int64_t*)v569;
    v571 = 72LL;
    v572 = v570 == v571;
    if (v572) goto bb158;
    else goto bb159;
  bb158:
    v574 = _str79;
    p_call_stack_pop();
    return v574;
  bb159:
    v576 = (char*)v1 + 0;
    v577 = *(int64_t*)v576;
    v578 = 73LL;
    v579 = v577 == v578;
    if (v579) goto bb160;
    else goto bb161;
  bb160:
    v581 = _str80;
    p_call_stack_pop();
    return v581;
  bb161:
    v583 = (char*)v1 + 0;
    v584 = *(int64_t*)v583;
    v585 = 74LL;
    v586 = v584 == v585;
    if (v586) goto bb162;
    else goto bb163;
  bb162:
    v588 = _str81;
    p_call_stack_pop();
    return v588;
  bb163:
    v590 = (char*)v1 + 0;
    v591 = *(int64_t*)v590;
    v592 = 75LL;
    v593 = v591 == v592;
    if (v593) goto bb164;
    else goto bb165;
  bb164:
    v595 = _str82;
    p_call_stack_pop();
    return v595;
  bb165:
    v597 = (char*)v1 + 0;
    v598 = *(int64_t*)v597;
    v599 = 81LL;
    v600 = v598 == v599;
    if (v600) goto bb166;
    else goto bb167;
  bb166:
    v602 = _str83;
    p_call_stack_pop();
    return v602;
  bb167:
    v604 = (char*)v1 + 0;
    v605 = *(int64_t*)v604;
    v606 = 82LL;
    v607 = v605 == v606;
    if (v607) goto bb168;
    else goto bb169;
  bb168:
    v609 = _str84;
    p_call_stack_pop();
    return v609;
  bb169:
    v611 = (char*)v1 + 0;
    v612 = *(int64_t*)v611;
    v613 = 83LL;
    v614 = v612 == v613;
    if (v614) goto bb170;
    else goto bb171;
  bb170:
    v616 = _str85;
    p_call_stack_pop();
    return v616;
  bb171:
    v618 = (char*)v1 + 0;
    v619 = *(int64_t*)v618;
    v620 = 84LL;
    v621 = v619 == v620;
    if (v621) goto bb172;
    else goto bb173;
  bb172:
    v623 = _str86;
    p_call_stack_pop();
    return v623;
  bb173:
    v625 = (char*)v1 + 0;
    v626 = *(int64_t*)v625;
    v627 = 85LL;
    v628 = v626 == v627;
    if (v628) goto bb174;
    else goto bb175;
  bb174:
    v630 = _str87;
    p_call_stack_pop();
    return v630;
  bb175:
    v632 = (char*)v1 + 0;
    v633 = *(int64_t*)v632;
    v634 = 86LL;
    v635 = v633 == v634;
    if (v635) goto bb176;
    else goto bb177;
  bb176:
    v637 = _str88;
    p_call_stack_pop();
    return v637;
  bb177:
    v639 = (char*)v1 + 0;
    v640 = *(int64_t*)v639;
    v641 = 87LL;
    v642 = v640 == v641;
    if (v642) goto bb178;
    else goto bb179;
  bb178:
    v644 = _str89;
    p_call_stack_pop();
    return v644;
  bb179:
    v646 = (char*)v1 + 0;
    v647 = *(int64_t*)v646;
    v648 = 88LL;
    v649 = v647 == v648;
    if (v649) goto bb180;
    else goto bb181;
  bb180:
    v651 = _str90;
    p_call_stack_pop();
    return v651;
  bb181:
    v653 = (char*)v1 + 0;
    v654 = *(int64_t*)v653;
    v655 = 89LL;
    v656 = v654 == v655;
    if (v656) goto bb182;
    else goto bb183;
  bb182:
    v658 = _str91;
    p_call_stack_pop();
    return v658;
  bb183:
    v660 = (char*)v1 + 0;
    v661 = *(int64_t*)v660;
    v662 = 90LL;
    v663 = v661 == v662;
    if (v663) goto bb184;
    else goto bb185;
  bb184:
    v665 = _str92;
    p_call_stack_pop();
    return v665;
  bb185:
    v667 = (char*)v1 + 0;
    v668 = *(int64_t*)v667;
    v669 = 91LL;
    v670 = v668 == v669;
    if (v670) goto bb186;
    else goto bb187;
  bb186:
    v672 = _str93;
    p_call_stack_pop();
    return v672;
  bb187:
    v674 = (char*)v1 + 0;
    v675 = *(int64_t*)v674;
    v676 = 92LL;
    v677 = v675 == v676;
    if (v677) goto bb188;
    else goto bb189;
  bb188:
    v679 = _str94;
    p_call_stack_pop();
    return v679;
  bb189:
    v681 = (char*)v1 + 0;
    v682 = *(int64_t*)v681;
    v683 = 93LL;
    v684 = v682 == v683;
    if (v684) goto bb190;
    else goto bb191;
  bb190:
    v686 = _str95;
    p_call_stack_pop();
    return v686;
  bb191:
    v688 = (char*)v1 + 0;
    v689 = *(int64_t*)v688;
    v690 = 94LL;
    v691 = v689 == v690;
    if (v691) goto bb192;
    else goto bb193;
  bb192:
    v693 = _str96;
    p_call_stack_pop();
    return v693;
  bb193:
    v695 = (char*)v1 + 0;
    v696 = *(int64_t*)v695;
    v697 = 95LL;
    v698 = v696 == v697;
    if (v698) goto bb194;
    else goto bb195;
  bb194:
    v700 = _str97;
    p_call_stack_pop();
    return v700;
  bb195:
    v702 = (char*)v1 + 0;
    v703 = *(int64_t*)v702;
    v704 = 96LL;
    v705 = v703 == v704;
    if (v705) goto bb196;
    else goto bb197;
  bb196:
    v707 = _str98;
    p_call_stack_pop();
    return v707;
  bb197:
    v709 = (char*)v1 + 0;
    v710 = *(int64_t*)v709;
    v711 = 97LL;
    v712 = v710 == v711;
    if (v712) goto bb198;
    else goto bb199;
  bb198:
    v714 = _str99;
    p_call_stack_pop();
    return v714;
  bb199:
    v716 = (char*)v1 + 0;
    v717 = *(int64_t*)v716;
    v718 = 98LL;
    v719 = v717 == v718;
    if (v719) goto bb200;
    else goto bb201;
  bb200:
    v721 = _str100;
    p_call_stack_pop();
    return v721;
  bb201:
    v723 = (char*)v1 + 0;
    v724 = *(int64_t*)v723;
    v725 = 99LL;
    v726 = v724 == v725;
    if (v726) goto bb202;
    else goto bb203;
  bb202:
    v728 = _str101;
    p_call_stack_pop();
    return v728;
  bb203:
    v730 = (char*)v1 + 0;
    v731 = *(int64_t*)v730;
    v732 = 100LL;
    v733 = v731 == v732;
    if (v733) goto bb204;
    else goto bb205;
  bb204:
    v735 = _str102;
    p_call_stack_pop();
    return v735;
  bb205:
    v737 = (char*)v1 + 0;
    v738 = *(int64_t*)v737;
    v739 = 101LL;
    v740 = v738 == v739;
    if (v740) goto bb206;
    else goto bb207;
  bb206:
    v742 = _str103;
    p_call_stack_pop();
    return v742;
  bb207:
    v744 = (char*)v1 + 0;
    v745 = *(int64_t*)v744;
    v746 = 102LL;
    v747 = v745 == v746;
    if (v747) goto bb208;
    else goto bb209;
  bb208:
    v749 = _str104;
    p_call_stack_pop();
    return v749;
  bb209:
    v751 = (char*)v1 + 0;
    v752 = *(int64_t*)v751;
    v753 = 103LL;
    v754 = v752 == v753;
    if (v754) goto bb210;
    else goto bb211;
  bb210:
    v756 = _str105;
    p_call_stack_pop();
    return v756;
  bb211:
    v758 = (char*)v1 + 0;
    v759 = *(int64_t*)v758;
    v760 = 104LL;
    v761 = v759 == v760;
    if (v761) goto bb212;
    else goto bb213;
  bb212:
    v763 = _str106;
    p_call_stack_pop();
    return v763;
  bb213:
    v765 = (char*)v1 + 0;
    v766 = *(int64_t*)v765;
    v767 = 105LL;
    v768 = v766 == v767;
    if (v768) goto bb214;
    else goto bb215;
  bb214:
    v770 = _str107;
    p_call_stack_pop();
    return v770;
  bb215:
    v772 = (char*)v1 + 0;
    v773 = *(int64_t*)v772;
    v774 = 106LL;
    v775 = v773 == v774;
    if (v775) goto bb216;
    else goto bb217;
  bb216:
    v777 = _str108;
    p_call_stack_pop();
    return v777;
  bb217:
    v779 = (char*)v1 + 0;
    v780 = *(int64_t*)v779;
    v781 = 107LL;
    v782 = v780 == v781;
    if (v782) goto bb218;
    else goto bb219;
  bb218:
    v784 = _str109;
    p_call_stack_pop();
    return v784;
  bb219:
    v786 = (char*)v1 + 0;
    v787 = *(int64_t*)v786;
    v788 = 108LL;
    v789 = v787 == v788;
    if (v789) goto bb220;
    else goto bb221;
  bb220:
    v791 = _str110;
    p_call_stack_pop();
    return v791;
  bb221:
    v793 = (char*)v1 + 0;
    v794 = *(int64_t*)v793;
    v795 = 109LL;
    v796 = v794 == v795;
    if (v796) goto bb222;
    else goto bb223;
  bb222:
    v798 = _str111;
    p_call_stack_pop();
    return v798;
  bb223:
    v800 = (char*)v1 + 0;
    v801 = *(int64_t*)v800;
    v802 = 110LL;
    v803 = v801 == v802;
    if (v803) goto bb224;
    else goto bb225;
  bb224:
    v805 = _str112;
    p_call_stack_pop();
    return v805;
  bb225:
    v807 = (char*)v1 + 0;
    v808 = *(int64_t*)v807;
    v809 = 111LL;
    v810 = v808 == v809;
    if (v810) goto bb226;
    else goto bb227;
  bb226:
    v812 = _str113;
    p_call_stack_pop();
    return v812;
  bb227:
    v814 = (char*)v1 + 0;
    v815 = *(int64_t*)v814;
    v816 = 112LL;
    v817 = v815 == v816;
    if (v817) goto bb228;
    else goto bb229;
  bb228:
    v819 = _str114;
    p_call_stack_pop();
    return v819;
  bb229:
    v821 = (char*)v1 + 0;
    v822 = *(int64_t*)v821;
    v823 = 113LL;
    v824 = v822 == v823;
    if (v824) goto bb230;
    else goto bb231;
  bb230:
    v826 = _str115;
    p_call_stack_pop();
    return v826;
  bb231:
    v828 = (char*)v1 + 0;
    v829 = *(int64_t*)v828;
    v830 = 114LL;
    v831 = v829 == v830;
    if (v831) goto bb232;
    else goto bb233;
  bb232:
    v833 = _str116;
    p_call_stack_pop();
    return v833;
  bb233:
    v835 = (char*)v1 + 0;
    v836 = *(int64_t*)v835;
    v837 = 115LL;
    v838 = v836 == v837;
    if (v838) goto bb234;
    else goto bb235;
  bb234:
    v840 = _str117;
    p_call_stack_pop();
    return v840;
  bb235:
    v842 = (char*)v1 + 0;
    v843 = *(int64_t*)v842;
    v844 = 116LL;
    v845 = v843 == v844;
    if (v845) goto bb236;
    else goto bb237;
  bb236:
    v847 = _str118;
    p_call_stack_pop();
    return v847;
  bb237:
    v849 = (char*)v1 + 0;
    v850 = *(int64_t*)v849;
    v851 = 117LL;
    v852 = v850 == v851;
    if (v852) goto bb238;
    else goto bb239;
  bb238:
    v854 = _str119;
    p_call_stack_pop();
    return v854;
  bb239:
    v856 = (char*)v1 + 0;
    v857 = *(int64_t*)v856;
    v858 = 118LL;
    v859 = v857 == v858;
    if (v859) goto bb240;
    else goto bb241;
  bb240:
    v861 = _str120;
    p_call_stack_pop();
    return v861;
  bb241:
    v863 = (char*)v1 + 0;
    v864 = *(int64_t*)v863;
    v865 = 119LL;
    v866 = v864 == v865;
    if (v866) goto bb242;
    else goto bb243;
  bb242:
    v868 = _str121;
    p_call_stack_pop();
    return v868;
  bb243:
    v870 = (char*)v1 + 0;
    v871 = *(int64_t*)v870;
    v872 = 120LL;
    v873 = v871 == v872;
    if (v873) goto bb244;
    else goto bb245;
  bb244:
    v875 = _str122;
    p_call_stack_pop();
    return v875;
  bb245:
    v877 = (char*)v1 + 0;
    v878 = *(int64_t*)v877;
    v879 = 121LL;
    v880 = v878 == v879;
    if (v880) goto bb246;
    else goto bb247;
  bb246:
    v882 = _str123;
    p_call_stack_pop();
    return v882;
  bb247:
    v884 = (char*)v1 + 0;
    v885 = *(int64_t*)v884;
    v886 = 122LL;
    v887 = v885 == v886;
    if (v887) goto bb248;
    else goto bb249;
  bb248:
    v889 = _str124;
    p_call_stack_pop();
    return v889;
  bb249:
    v891 = (char*)v1 + 0;
    v892 = *(int64_t*)v891;
    v893 = 123LL;
    v894 = v892 == v893;
    if (v894) goto bb250;
    else goto bb251;
  bb250:
    v896 = _str125;
    p_call_stack_pop();
    return v896;
  bb251:
    v898 = (char*)v1 + 0;
    v899 = *(int64_t*)v898;
    v900 = 124LL;
    v901 = v899 == v900;
    if (v901) goto bb252;
    else goto bb253;
  bb252:
    v903 = _str126;
    p_call_stack_pop();
    return v903;
  bb253:
    v905 = (char*)v1 + 0;
    v906 = *(int64_t*)v905;
    v907 = 125LL;
    v908 = v906 == v907;
    if (v908) goto bb254;
    else goto bb255;
  bb254:
    v910 = _str127;
    p_call_stack_pop();
    return v910;
  bb255:
    v912 = (char*)v1 + 0;
    v913 = *(int64_t*)v912;
    v914 = 126LL;
    v915 = v913 == v914;
    if (v915) goto bb256;
    else goto bb257;
  bb256:
    v917 = _str128;
    p_call_stack_pop();
    return v917;
  bb257:
    v919 = (char*)v1 + 0;
    v920 = *(int64_t*)v919;
    v921 = 127LL;
    v922 = v920 == v921;
    if (v922) goto bb258;
    else goto bb259;
  bb258:
    v924 = _str129;
    p_call_stack_pop();
    return v924;
  bb259:
    v926 = (char*)v1 + 0;
    v927 = *(int64_t*)v926;
    v928 = 128LL;
    v929 = v927 == v928;
    if (v929) goto bb260;
    else goto bb261;
  bb260:
    v931 = _str130;
    p_call_stack_pop();
    return v931;
  bb261:
    v933 = (char*)v1 + 0;
    v934 = *(int64_t*)v933;
    v935 = 129LL;
    v936 = v934 == v935;
    if (v936) goto bb262;
    else goto bb263;
  bb262:
    v938 = _str131;
    p_call_stack_pop();
    return v938;
  bb263:
    v940 = (char*)v1 + 0;
    v941 = *(int64_t*)v940;
    v942 = 130LL;
    v943 = v941 == v942;
    if (v943) goto bb264;
    else goto bb265;
  bb264:
    v945 = _str132;
    p_call_stack_pop();
    return v945;
  bb265:
    v947 = (char*)v1 + 0;
    v948 = *(int64_t*)v947;
    v949 = 131LL;
    v950 = v948 == v949;
    if (v950) goto bb266;
    else goto bb267;
  bb266:
    v952 = _str133;
    p_call_stack_pop();
    return v952;
  bb267:
    v954 = (char*)v1 + 0;
    v955 = *(int64_t*)v954;
    v956 = 132LL;
    v957 = v955 == v956;
    if (v957) goto bb268;
    else goto bb269;
  bb268:
    v959 = _str134;
    p_call_stack_pop();
    return v959;
  bb269:
    v961 = (char*)v1 + 0;
    v962 = *(int64_t*)v961;
    v963 = 133LL;
    v964 = v962 == v963;
    if (v964) goto bb270;
    else goto bb271;
  bb270:
    v966 = _str135;
    p_call_stack_pop();
    return v966;
  bb271:
    goto bb1;
    return 0;
}

void Lexer_read_raw_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_raw_string");
    int64_t v141;
    int64_t v115;
    int64_t v130;
    int64_t v154;
    int64_t v157; /* placeholder for void */
    void* v67;
    int64_t v122;
    int64_t v128; /* placeholder for void */
    void* v45;
    void* v9;
    int64_t v166;
    void* v218;
    int64_t v39;
    void* v201;
    void* v43;
    int64_t v113;
    int64_t v117; /* placeholder for void */
    int64_t v140;
    void* v169;
    void* v208;
    int64_t v19;
    int64_t v65; /* placeholder for void */
    int64_t v18;
    int64_t v112;
    int64_t v189;
    bool v97;
    int64_t v79; /* placeholder for void */
    int64_t v220;
    int64_t v225; /* placeholder for void */
    int64_t v124;
    void* v214;
    int64_t v207; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    void* v164;
    bool v92;
    void* v132;
    int64_t v175; /* placeholder for void */
    int64_t v8;
    int64_t v51; /* placeholder for void */
    int64_t v152; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v212;
    void* v75;
    int64_t v123;
    void* v165;
    void* v168;
    void* v172;
    void* v48;
    void* v74;
    void* v151;
    int64_t v13;
    int64_t v95;
    void* v61;
    bool v116;
    void* v66;
    int64_t v107;
    int64_t v144; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    void* v184;
    bool v118;
    void* v200;
    int64_t v106;
    int64_t v204;
    void* v159;
    int64_t v156;
    void* v83;
    int64_t v58;
    void* v210;
    int64_t v186; /* placeholder for void */
    void* v60;
    void* v57;
    void* v160;
    int64_t v31;
    int64_t v195; /* placeholder for void */
    int64_t v44;
    int64_t v199; /* placeholder for void */
    void* v3;
    void* v34;
    void* v88;
    int64_t v36;
    void* v158;
    int64_t v72;
    void* v7;
    void* v71;
    int64_t v49;
    int64_t v138; /* placeholder for void */
    int64_t v102;
    void* v86;
    int64_t v93; /* placeholder for void */
    void* v211;
    int64_t v131;
    int64_t v78; /* placeholder for void */
    void* v53;
    void* v42;
    int64_t v178; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    void* v70;
    int64_t v47; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    void* v223;
    int64_t v30;
    bool v108;
    int64_t v226; /* placeholder for void */
    int64_t v22;
    int64_t v33; /* placeholder for void */
    void* v62;
    int64_t v76;
    int64_t v69; /* placeholder for void */
    int64_t v142;
    int64_t v64;
    int64_t v10; /* placeholder for void */
    int64_t v194;
    int64_t v213; /* placeholder for void */
    void* v14;
    int64_t v55;
    int64_t v179; /* placeholder for void */
    int64_t v167; /* placeholder for void */
    void* v52;
    int64_t v126; /* placeholder for void */
    int64_t v96;
    int64_t v135;
    int64_t v206;
    void* v146;
    void* v155;
    int64_t v77; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    void* v161;
    int64_t v81;
    void* v173;
    bool v20;
    bool v32;
    void* v187;
    void* v182;
    int64_t v6; /* placeholder for void */
    int64_t v149; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v192;
    void* v188;
    int64_t v101; /* placeholder for void */
    void* v100;
    void* v35;
    int64_t v25;
    void* v82;
    int64_t v110;
    int64_t v111;
    int64_t v12;
    void* v150;
    int64_t v40;
    int64_t v143;
    int64_t v37;
    int64_t v176; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    bool v99;
    int64_t v21; /* placeholder for void */
    int64_t v90;
    int64_t v73; /* placeholder for void */
    int64_t v216;
    int64_t v224;
    int64_t v185; /* placeholder for void */
    void* v198;
    void* v222;
    int64_t v121; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v50;
    void* v63;
    void* v103;
    bool v137;
    int64_t v183;
    int64_t v11;
    int64_t v41;
    int64_t v26;
    int64_t v177; /* placeholder for void */
    void* v148;
    int64_t v153;
    void* v215;
    int64_t v125; /* placeholder for void */
    int64_t v24;
    void* v196;
    int64_t v17;
    void* v38;
    int64_t v87;
    void* v205;
    int64_t v170;
    int64_t v28; /* placeholder for void */
    void* v209;
    int64_t v202; /* placeholder for void */
    int64_t v174;
    void* v219;
    int64_t v23;
    int64_t v4;
    int64_t v162;
    void* v180;
    int64_t v190;
    int64_t v136;
    int64_t v147;
    int64_t v16; /* placeholder for void */
    int64_t v94;
    void* v193;
    int64_t v114;
    int64_t v46;
    int64_t v104; /* placeholder for void */
    int64_t v181;
    int64_t v197;
    int64_t v129;
    int64_t v68;
    int64_t v91;
    void* v5;
    int64_t v80;
    int64_t v56;
    int64_t v221; /* placeholder for void */
    int64_t v139;
    int64_t v203;
    int64_t v85; /* placeholder for void */
    bool v127;
    void* v89;
    int64_t v163; /* placeholder for void */
    int64_t v134; /* placeholder for void */
    int64_t v29;
    void* v191;
    int64_t v119; /* placeholder for void */
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Lexer_advance((void*)v11, v2);
    v13 = 0LL;
    v14 = malloc(8);
    *(int64_t*)v14 = (int64_t)v13;
    goto bb1;
  bb1:
    v17 = 0LL;
    v18 = (int64_t)Lexer_peek((void*)v17, v2);
    v19 = 35LL;
    v20 = v18 == v19;
    if (v20) goto bb2;
    else goto bb3;
  bb2:
    v22 = 0LL;
    v23 = (int64_t)Lexer_advance((void*)v22, v2);
    v24 = *(int64_t*)v14;
    v25 = 1LL;
    v26 = v24 + v25;
    *(int64_t*)v14 = (int64_t)v26;
    goto bb1;
  bb3:
    v29 = 0LL;
    v30 = (int64_t)Lexer_peek((void*)v29, v2);
    v31 = 34LL;
    v32 = v30 != v31;
    if (v32) goto bb4;
    else goto bb5;
  bb4:
    v34 = (char*)v2 + 32;
    v35 = *(void**)v34;
    v36 = *(int64_t*)v5;
    v37 = *(int64_t*)v9;
    v38 = _str136;
    v39 = 35LL;
    v40 = *(int64_t*)v14;
    v41 = 0LL;
    v42 = repeat_char((void*)v41, v39, v40);
    v43 = p_str_concat(v38, v42);
    v44 = 0LL;
    v45 = lex_error((void*)v44, v36, v37, v43);
    v46 = 0LL;
    Vec_push((void*)v46, v35, (int64_t)v45);
    v48 = malloc(16);
    v49 = 78LL;
    v50 = (char*)v48 + 0;
    *(int64_t*)v50 = (int64_t)v49;
    v52 = _str137;
    v53 = (char*)v48 + 8;
    *(void**)v53 = (void*)v52;
    v55 = *(int64_t*)v5;
    v56 = *(int64_t*)v9;
    v57 = malloc(32);
    v58 = 0LL;
    token_new(v57, (void*)v58, v48, v55, v56);
    v60 = (char*)v57 + 0;
    v61 = (char*)v0 + 0;
    v62 = (char*)v60 + 0;
    v63 = (char*)v61 + 0;
    v64 = *(int64_t*)v62;
    *(int64_t*)v63 = (int64_t)v64;
    v66 = (char*)v60 + 8;
    v67 = (char*)v61 + 8;
    v68 = *(int64_t*)v66;
    *(int64_t*)v67 = (int64_t)v68;
    v70 = (char*)v57 + 16;
    v71 = (char*)v0 + 16;
    v72 = *(int64_t*)v70;
    *(int64_t*)v71 = (int64_t)v72;
    v74 = (char*)v57 + 24;
    v75 = (char*)v0 + 24;
    v76 = *(int64_t*)v74;
    *(int64_t*)v75 = (int64_t)v76;
    p_call_stack_pop();
    return;
  bb5:
    goto bb6;
  bb6:
    v80 = 0LL;
    v81 = (int64_t)Lexer_advance((void*)v80, v2);
    v82 = _str137;
    v83 = malloc(8);
    *(void**)v83 = (void*)v82;
    goto bb7;
  bb7:
    v86 = (char*)v2 + 8;
    v87 = *(int64_t*)v86;
    v88 = (char*)v2 + 0;
    v89 = *(void**)v88;
    v90 = 0LL;
    v91 = (int64_t)str_len((void*)v90, v89);
    v92 = v87 < v91;
    if (v92) goto bb8;
    else goto bb9;
  bb8:
    v94 = 0LL;
    v95 = (int64_t)Lexer_peek((void*)v94, v2);
    v96 = 34LL;
    v97 = v95 == v96;
    if (v97) goto bb10;
    else goto bb11;
  bb9:
    v187 = (char*)v2 + 32;
    v188 = *(void**)v187;
    v189 = *(int64_t*)v5;
    v190 = *(int64_t*)v9;
    v191 = _str138;
    v192 = 0LL;
    v193 = lex_error((void*)v192, v189, v190, v191);
    v194 = 0LL;
    Vec_push((void*)v194, v188, (int64_t)v193);
    v196 = malloc(16);
    v197 = 78LL;
    v198 = (char*)v196 + 0;
    *(int64_t*)v198 = (int64_t)v197;
    v200 = *(void**)v83;
    v201 = (char*)v196 + 8;
    *(void**)v201 = (void*)v200;
    v203 = *(int64_t*)v5;
    v204 = *(int64_t*)v9;
    v205 = malloc(32);
    v206 = 0LL;
    token_new(v205, (void*)v206, v196, v203, v204);
    v208 = (char*)v205 + 0;
    v209 = (char*)v0 + 0;
    v210 = (char*)v208 + 0;
    v211 = (char*)v209 + 0;
    v212 = *(int64_t*)v210;
    *(int64_t*)v211 = (int64_t)v212;
    v214 = (char*)v208 + 8;
    v215 = (char*)v209 + 8;
    v216 = *(int64_t*)v214;
    *(int64_t*)v215 = (int64_t)v216;
    v218 = (char*)v205 + 16;
    v219 = (char*)v0 + 16;
    v220 = *(int64_t*)v218;
    *(int64_t*)v219 = (int64_t)v220;
    v222 = (char*)v205 + 24;
    v223 = (char*)v0 + 24;
    v224 = *(int64_t*)v222;
    *(int64_t*)v223 = (int64_t)v224;
    p_call_stack_pop();
    return;
  bb10:
    v99 = true;
    v100 = malloc(1);
    *(bool*)v100 = v99;
    v102 = 0LL;
    v103 = malloc(8);
    *(int64_t*)v103 = (int64_t)v102;
    goto bb13;
  bb11:
    goto bb12;
  bb12:
    v180 = *(void**)v83;
    v181 = 0LL;
    v182 = Lexer_read_utf8((void*)v181, v2);
    v183 = 0LL;
    v184 = str_concat((void*)v183, v180, v182);
    *(void**)v83 = (void*)v184;
    goto bb7;
  bb13:
    v106 = *(int64_t*)v103;
    v107 = *(int64_t*)v14;
    v108 = v106 < v107;
    if (v108) goto bb14;
    else goto bb15;
  bb14:
    v110 = 1LL;
    v111 = *(int64_t*)v103;
    v112 = v110 + v111;
    v113 = 0LL;
    v114 = (int64_t)Lexer_peek_at((void*)v113, v2, v112);
    v115 = 35LL;
    v116 = v114 != v115;
    if (v116) goto bb16;
    else goto bb17;
  bb15:
    v127 = *(bool*)v100;
    if (v127) goto bb19;
    else goto bb20;
  bb16:
    v118 = false;
    *(bool*)v100 = v118;
    goto bb15;
  bb17:
    goto bb18;
  bb18:
    v122 = *(int64_t*)v103;
    v123 = 1LL;
    v124 = v122 + v123;
    *(int64_t*)v103 = (int64_t)v124;
    goto bb13;
  bb19:
    v129 = 0LL;
    v130 = (int64_t)Lexer_advance((void*)v129, v2);
    v131 = 0LL;
    v132 = malloc(8);
    *(int64_t*)v132 = (int64_t)v131;
    goto bb22;
  bb20:
    goto bb21;
  bb21:
    goto bb12;
  bb22:
    v135 = *(int64_t*)v132;
    v136 = *(int64_t*)v14;
    v137 = v135 < v136;
    if (v137) goto bb23;
    else goto bb24;
  bb23:
    v139 = 0LL;
    v140 = (int64_t)Lexer_advance((void*)v139, v2);
    v141 = *(int64_t*)v132;
    v142 = 1LL;
    v143 = v141 + v142;
    *(int64_t*)v132 = (int64_t)v143;
    goto bb22;
  bb24:
    v146 = malloc(16);
    v147 = 78LL;
    v148 = (char*)v146 + 0;
    *(int64_t*)v148 = (int64_t)v147;
    v150 = *(void**)v83;
    v151 = (char*)v146 + 8;
    *(void**)v151 = (void*)v150;
    v153 = *(int64_t*)v5;
    v154 = *(int64_t*)v9;
    v155 = malloc(32);
    v156 = 0LL;
    token_new(v155, (void*)v156, v146, v153, v154);
    v158 = (char*)v155 + 0;
    v159 = (char*)v0 + 0;
    v160 = (char*)v158 + 0;
    v161 = (char*)v159 + 0;
    v162 = *(int64_t*)v160;
    *(int64_t*)v161 = (int64_t)v162;
    v164 = (char*)v158 + 8;
    v165 = (char*)v159 + 8;
    v166 = *(int64_t*)v164;
    *(int64_t*)v165 = (int64_t)v166;
    v168 = (char*)v155 + 16;
    v169 = (char*)v0 + 16;
    v170 = *(int64_t*)v168;
    *(int64_t*)v169 = (int64_t)v170;
    v172 = (char*)v155 + 24;
    v173 = (char*)v0 + 24;
    v174 = *(int64_t*)v172;
    *(int64_t*)v173 = (int64_t)v174;
    p_call_stack_pop();
    return;
    return;
}

void Option_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_map");
    int64_t v43; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    void* v34;
    void* v19;
    int64_t v29;
    void* v9;
    void* v16;
    int64_t v5;
    int64_t v14; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v18;
    int64_t v38;
    bool v30;
    int64_t v33;
    int64_t v17; /* placeholder for void */
    void* v23;
    int64_t v35; /* placeholder for void */
    void* v4;
    void* v22;
    void* v37;
    int64_t v39; /* placeholder for void */
    int64_t v42;
    int64_t v28;
    void* v40;
    void* v41;
    int64_t v45; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v32;
    int64_t v6;
    void* v27;
    void* v11;
    int64_t v15;
    int64_t v31; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v10;
    void* v36;
    int64_t v12;
    int64_t v21; /* placeholder for void */
    int64_t v20;
    int64_t v24;
    void* v13;
    bool v7;
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v15 = Lexer_peek((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v15 = token_kind_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v15 = Lexer_advance((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
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

void vec_sort_by(void* v0, int64_t v1, void* v2) {
    p_call_stack_push("vec_sort_by");
    void* v23;
    int64_t v34;
    int64_t v64; /* placeholder for void */
    int64_t v53;
    int64_t v60;
    void* v18;
    int64_t v40;
    int64_t v26;
    int64_t v14; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v59;
    int64_t v38; /* placeholder for void */
    int64_t v7;
    int64_t v63; /* placeholder for void */
    int64_t v50;
    int64_t v51; /* placeholder for void */
    int64_t v32;
    int64_t v27;
    int64_t v22;
    int64_t v44;
    int64_t v52; /* placeholder for void */
    int64_t v21;
    int64_t v56;
    int64_t v57;
    int64_t v33;
    int64_t v20;
    int64_t v10; /* placeholder for void */
    int64_t v3;
    int64_t v9; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v16;
    int64_t v15;
    int64_t v17;
    int64_t v35;
    bool v36;
    void* v5;
    int64_t v29; /* placeholder for void */
    int64_t v46;
    bool v13;
    int64_t v55;
    int64_t v30;
    int64_t v24; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v49;
    void* v8;
    int64_t v11;
    int64_t v54;
    bool v28;
    int64_t v37; /* placeholder for void */
    int64_t v48;
    int64_t v12;
    int64_t v31;
    int64_t v41;
    int64_t v43;
    int64_t v45;
    int64_t v58; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v4;
    int64_t v42;
    int64_t v39; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v61;
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
        if (strcmp(_fn_name, "String_byte_at") == 0) {
            v34 = String_byte_at((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Result_expect") == 0) {
            v34 = Result_expect((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_find") == 0) {
            v34 = String_find((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_push_str") == 0) {
            v34 = String_push_str((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_find") == 0) {
            v34 = Vec_find((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "max") == 0) {
            v34 = p_max((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_take") == 0) {
            v34 = vec_take((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Option_unwrap_or_else") == 0) {
            v34 = Option_unwrap_or_else((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "vec_skip") == 0) {
            v34 = vec_skip((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "min") == 0) {
            v34 = p_min((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_chunk") == 0) {
            v34 = vec_chunk((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_repeat") == 0) {
            v34 = String_repeat((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "String_split") == 0) {
            v34 = String_split((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "HashMap_get") == 0) {
            v34 = HashMap_get((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Lexer_peek_at") == 0) {
            v34 = Lexer_peek_at((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "ByteBuffer_read_byte") == 0) {
            v34 = ByteBuffer_read_byte((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_get") == 0) {
            v34 = Vec_get((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Result_unwrap_or_else") == 0) {
            v34 = Result_unwrap_or_else((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "parse_radix") == 0) {
            v34 = parse_radix((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "HashMap_find_slot") == 0) {
            v34 = HashMap_find_slot((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_position") == 0) {
            v34 = Vec_position((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_char_at") == 0) {
            v34 = String_char_at((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_rfind") == 0) {
            v34 = str_rfind((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "Result_unwrap_or") == 0) {
            v34 = Result_unwrap_or((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "p_str_compare") == 0) {
            v34 = p_str_compare((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_compare") == 0) {
            v34 = str_compare((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "Option_unwrap_or") == 0) {
            v34 = Option_unwrap_or((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_flat_map") == 0) {
            v34 = vec_flat_map((void*)v2, (int64_t)v32, (void*)v33);
        } else if (strcmp(_fn_name, "allocator_alloc") == 0) {
            v34 = allocator_alloc((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Vec_remove") == 0) {
            v34 = Vec_remove((void*)v2, (void*)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "str_find") == 0) {
            v34 = str_find((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "String_rfind") == 0) {
            v34 = String_rfind((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_zip") == 0) {
            v34 = vec_zip((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "vec_window") == 0) {
            v34 = vec_window((void*)v2, (int64_t)v32, (int64_t)v33);
        } else if (strcmp(_fn_name, "Option_expect") == 0) {
            v34 = Option_expect((void*)v2, (void*)v32, (void*)v33);
        } else if (strcmp(_fn_name, "str_byte_at") == 0) {
            v34 = str_byte_at((void*)v2, (void*)v32, (int64_t)v33);
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

void println(void* v0, void* v1) {
    p_call_stack_push("println");
    int64_t v4; /* placeholder for void */
    void* v3;
    int64_t v5; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    p_print_str(v1);
    v3 = _str139;
    p_print_str(v3);
    p_call_stack_pop();
    return;
    return;
}

int64_t Result_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_expect");
    void* v3;
    bool v14;
    bool v6;
    int64_t v7; /* placeholder for void */
    int64_t v5;
    int64_t v20; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v9;
    int64_t v18; /* placeholder for void */
    int64_t v21;
    int64_t v13;
    int64_t v12;
    int64_t v10; /* placeholder for void */
    int64_t v17;
    void* v11;
    void* v16;
    void* v8;
    int64_t v4;
    int64_t v15; /* placeholder for void */
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

void StringBuilder_append_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_char");
    void* v9;
    void* v3;
    int64_t v12;
    int64_t v11;
    int64_t v8; /* placeholder for void */
    void* v7;
    void* v4;
    int64_t v15; /* placeholder for void */
    void* v5;
    void* v10;
    void* v6;
    int64_t v14; /* placeholder for void */
    int64_t v13;
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

void* ByteBuffer_to_str(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_to_str");
    int64_t v17;
    void* v24;
    void* v23;
    int64_t v30; /* placeholder for void */
    int64_t v27;
    void* v21;
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v20; /* placeholder for void */
    void* v6;
    int64_t v4; /* placeholder for void */
    int64_t v22;
    int64_t v32; /* placeholder for void */
    int64_t v5;
    int64_t v9;
    void* v15;
    int64_t v13; /* placeholder for void */
    void* v2;
    int64_t v8; /* placeholder for void */
    void* v19;
    void* v31;
    int64_t v11;
    int64_t v16;
    void* v14;
    bool v12;
    int64_t v18;
    int64_t v25; /* placeholder for void */
    int64_t v28;
    int64_t v26;
    int64_t v29; /* placeholder for void */
    void* v10;
    v2 = _str137;
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

int64_t Vec_remove(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_remove");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_vec_remove(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

void HashMap_remove(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_remove");
    int64_t v49;
    int64_t v74;
    int64_t v65; /* placeholder for void */
    int64_t v16;
    int64_t v48; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v3;
    void* v75;
    int64_t v55; /* placeholder for void */
    void* v27;
    int64_t v45; /* placeholder for void */
    int64_t v20;
    void* v50;
    int64_t v26;
    int64_t v73; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v11;
    int64_t v38; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v40;
    int64_t v82;
    void* v44;
    int64_t v46;
    int64_t v54;
    int64_t v90; /* placeholder for void */
    int64_t v4;
    int64_t v92; /* placeholder for void */
    int64_t v62;
    int64_t v68; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v7;
    int64_t v17; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    int64_t v42;
    int64_t v87; /* placeholder for void */
    void* v15;
    int64_t v34; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v36;
    int64_t v25;
    int64_t v58;
    void* v12;
    int64_t v67; /* placeholder for void */
    int64_t v8;
    int64_t v31;
    bool v64;
    void* v78;
    int64_t v14; /* placeholder for void */
    int64_t v84;
    bool v63;
    int64_t v23; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v60;
    int64_t v43;
    void* v29;
    bool v59;
    bool v21;
    int64_t v22; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v39;
    void* v69;
    int64_t v19;
    int64_t v83;
    void* v88;
    int64_t v24; /* placeholder for void */
    void* v5;
    int64_t v57;
    int64_t v89;
    void* v35;
    int64_t v6; /* placeholder for void */
    int64_t v79;
    void* v32;
    void* v9;
    int64_t v13; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v47;
    void* v53;
    int64_t v37; /* placeholder for void */
    int64_t v41;
    int64_t v61;
    int64_t v70;
    int64_t v71;
    void* v85;
    int64_t v91; /* placeholder for void */
    int64_t v77; /* placeholder for void */
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

int64_t Lexer_peek_at(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Lexer_peek_at");
    int64_t v21;
    bool v13;
    int64_t v22;
    int64_t v7; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    void* v3;
    void* v10;
    int64_t v12;
    int64_t v23;
    void* v20;
    int64_t v5;
    int64_t v8;
    int64_t v16;
    int64_t v4;
    void* v6;
    int64_t v15;
    void* v9;
    int64_t v14; /* placeholder for void */
    void* v19;
    int64_t v18; /* placeholder for void */
    int64_t v11;
    int64_t v24; /* placeholder for void */
    v3 = (char*)v1 + 8;
    v4 = *(int64_t*)v3;
    v5 = v4 + v2;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = *(int64_t*)v6;
    v9 = (char*)v1 + 0;
    v10 = *(void**)v9;
    v11 = 0LL;
    v12 = (int64_t)str_len((void*)v11, v10);
    v13 = v8 >= v12;
    if (v13) goto bb1;
    else goto bb2;
  bb1:
    v15 = 1LL;
    v16 = -v15;
    p_call_stack_pop();
    return v16;
  bb2:
    goto bb3;
  bb3:
    v19 = (char*)v1 + 0;
    v20 = *(void**)v19;
    v21 = *(int64_t*)v6;
    v22 = 0LL;
    v23 = (int64_t)str_byte_at((void*)v22, v20, v21);
    p_call_stack_pop();
    return v23;
    return 0;
}

int64_t HashMap_find_slot(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_find_slot");
    int64_t v125;
    int64_t v31; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v56;
    int64_t v59; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v132; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v17;
    void* v28;
    int64_t v103;
    void* v13;
    void* v25;
    int64_t v79;
    int64_t v94;
    void* v10;
    int64_t v91;
    void* v64;
    int64_t v105; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    bool v88;
    int64_t v97; /* placeholder for void */
    int64_t v68;
    int64_t v76;
    int64_t v111;
    int64_t v9;
    int64_t v38;
    int64_t v33;
    int64_t v60;
    int64_t v66; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v109;
    int64_t v117; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    int64_t v128;
    int64_t v131;
    void* v7;
    int64_t v8; /* placeholder for void */
    int64_t v87;
    int64_t v98; /* placeholder for void */
    int64_t v41;
    int64_t v48; /* placeholder for void */
    int64_t v49;
    int64_t v47; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v123; /* placeholder for void */
    int64_t v99;
    int64_t v104; /* placeholder for void */
    bool v126;
    int64_t v24;
    bool v77;
    int64_t v15; /* placeholder for void */
    int64_t v112;
    int64_t v130; /* placeholder for void */
    int64_t v120;
    int64_t v12; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v90;
    int64_t v108;
    void* v46;
    void* v58;
    int64_t v36;
    int64_t v27; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v67;
    int64_t v71;
    void* v34;
    int64_t v40; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v121;
    int64_t v127; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v42;
    int64_t v22; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v86;
    int64_t v54;
    int64_t v63;
    bool v73;
    bool v19;
    int64_t v69; /* placeholder for void */
    bool v113;
    int64_t v16; /* placeholder for void */
    int64_t v119;
    int64_t v93; /* placeholder for void */
    bool v51;
    int64_t v21;
    int64_t v32;
    int64_t v85;
    int64_t v26; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    int64_t v37;
    int64_t v100;
    int64_t v18;
    int64_t v55;
    int64_t v50;
    int64_t v45;
    int64_t v29;
    int64_t v53;
    void* v43;
    void* v5;
    int64_t v110; /* placeholder for void */
    int64_t v115;
    int64_t v62; /* placeholder for void */
    int64_t v107;
    int64_t v57;
    int64_t v116; /* placeholder for void */
    bool v92;
    bool v101;
    void* v39;
    int64_t v122; /* placeholder for void */
    int64_t v3;
    int64_t v89; /* placeholder for void */
    int64_t v4;
    int64_t v96; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v75;
    int64_t v124;
    int64_t v35; /* placeholder for void */
    int64_t v82;
    int64_t v106; /* placeholder for void */
    int64_t v14;
    void* v61;
    int64_t v72;
    int64_t v30; /* placeholder for void */
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

void* str_from_bytes(void* v0, void* v1) {
    p_call_stack_push("str_from_bytes");
    void* v17;
    int64_t v11; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    void* v3;
    void* v2;
    int64_t v27; /* placeholder for void */
    int64_t v20;
    int64_t v30; /* placeholder for void */
    bool v15;
    void* v22;
    int64_t v4; /* placeholder for void */
    int64_t v19;
    int64_t v25;
    int64_t v23; /* placeholder for void */
    void* v6;
    int64_t v9;
    void* v21;
    void* v29;
    int64_t v7; /* placeholder for void */
    int64_t v24;
    int64_t v8;
    int64_t v5;
    int64_t v26;
    int64_t v28; /* placeholder for void */
    void* v10;
    int64_t v12; /* placeholder for void */
    int64_t v14;
    int64_t v13;
    int64_t v18;
    v2 = _str137;
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

void* str_to_lower(void* v0, void* v1) {
    p_call_stack_push("str_to_lower");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_to_lowercase((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

bool Vec_is_empty(void* v0, void* v1) {
    p_call_stack_push("Vec_is_empty");
    bool v2;
    int64_t v3; /* placeholder for void */
    v2 = (bool)p_vec_is_empty(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void assert(void* v0, bool v1) {
    p_call_stack_push("assert");
    int64_t v2;
    void* v4;
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    v2 = !v1;
    if (v2) goto bb1;
    else goto bb2;
  bb1:
    v4 = _str140;
    p_panic(v4);
    goto bb3;
  bb2:
    goto bb3;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_get_or(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_get_or");
    void* v32;
    int64_t v74;
    int64_t v75; /* placeholder for void */
    void* v47;
    int64_t v30; /* placeholder for void */
    bool v61;
    int64_t v72; /* placeholder for void */
    int64_t v43;
    bool v56;
    void* v26;
    void* v91;
    int64_t v45; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v23;
    void* v12;
    int64_t v15; /* placeholder for void */
    void* v29;
    int64_t v76; /* placeholder for void */
    int64_t v92;
    void* v50;
    int64_t v81;
    int64_t v63; /* placeholder for void */
    int64_t v22;
    int64_t v58;
    void* v88;
    int64_t v59;
    int64_t v10; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v13;
    int64_t v4;
    void* v6;
    int64_t v36;
    int64_t v87;
    int64_t v93; /* placeholder for void */
    void* v44;
    bool v18;
    int64_t v78;
    int64_t v5;
    void* v82;
    int64_t v51;
    int64_t v80;
    int64_t v20; /* placeholder for void */
    void* v85;
    int64_t v94; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    void* v41;
    void* v73;
    int64_t v84;
    void* v9;
    int64_t v83; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v95;
    int64_t v39;
    bool v60;
    void* v24;
    int64_t v11; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v17;
    int64_t v57;
    int64_t v37;
    int64_t v62; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v33;
    int64_t v69;
    int64_t v64; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v77;
    int64_t v46;
    int64_t v52; /* placeholder for void */
    void* v70;
    int64_t v35; /* placeholder for void */
    void* v67;
    int64_t v25; /* placeholder for void */
    int64_t v40;
    int64_t v28;
    int64_t v65;
    int64_t v8;
    int64_t v55;
    int64_t v27; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v79;
    int64_t v21; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v16;
    int64_t v66;
    int64_t v54;
    int64_t v38;
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

void Lexer_read_identifier_or_keyword(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_identifier_or_keyword");
    void* v15;
    void* v25;
    int64_t v14; /* placeholder for void */
    void* v9;
    int64_t v58; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v27;
    int64_t v22; /* placeholder for void */
    void* v62;
    void* v7;
    int64_t v68; /* placeholder for void */
    int64_t v20;
    int64_t v13; /* placeholder for void */
    int64_t v28;
    void* v74;
    void* v73;
    void* v3;
    int64_t v6; /* placeholder for void */
    void* v12;
    int64_t v76; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    void* v35;
    void* v56;
    int64_t v26; /* placeholder for void */
    int64_t v24;
    int64_t v55;
    int64_t v71;
    int64_t v44; /* placeholder for void */
    int64_t v49;
    void* v40;
    void* v66;
    void* v18;
    void* v69;
    int64_t v16;
    int64_t v45; /* placeholder for void */
    int64_t v23;
    int64_t v42;
    void* v59;
    int64_t v37;
    void* v61;
    int64_t v67;
    int64_t v64; /* placeholder for void */
    void* v70;
    void* v5;
    bool v33;
    int64_t v19;
    void* v48;
    bool v29;
    int64_t v4;
    int64_t v36;
    int64_t v31;
    bool v21;
    void* v38;
    int64_t v39;
    int64_t v41; /* placeholder for void */
    int64_t v43;
    void* v51;
    int64_t v57;
    void* v47;
    void* v17;
    int64_t v8;
    void* v60;
    void* v65;
    int64_t v77; /* placeholder for void */
    int64_t v75;
    void* v53;
    bool v32;
    int64_t v54;
    void* v11;
    int64_t v63;
    int64_t v72; /* placeholder for void */
    int64_t v30;
    int64_t v46; /* placeholder for void */
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = _str137;
    v12 = malloc(8);
    *(void**)v12 = (void*)v11;
    goto bb1;
  bb1:
    v15 = (char*)v2 + 8;
    v16 = *(int64_t*)v15;
    v17 = (char*)v2 + 0;
    v18 = *(void**)v17;
    v19 = 0LL;
    v20 = (int64_t)str_len((void*)v19, v18);
    v21 = v16 < v20;
    if (v21) goto bb2;
    else goto bb3;
  bb2:
    v23 = 0LL;
    v24 = (int64_t)Lexer_peek((void*)v23, v2);
    v25 = malloc(8);
    *(int64_t*)v25 = (int64_t)v24;
    v27 = *(int64_t*)v25;
    v28 = 0LL;
    v29 = (bool)lex_is_alphanumeric((void*)v28, v27);
    v30 = *(int64_t*)v25;
    v31 = 95LL;
    v32 = v30 == v31;
    v33 = v29 || v32;
    if (v33) goto bb4;
    else goto bb5;
  bb3:
    v47 = *(void**)v12;
    v48 = malloc(16);
    v49 = 0LL;
    keyword_or_ident(v48, (void*)v49, v47);
    v51 = malloc(8);
    *(void**)v51 = (void*)v48;
    v53 = *(void**)v51;
    v54 = *(int64_t*)v5;
    v55 = *(int64_t*)v9;
    v56 = malloc(32);
    v57 = 0LL;
    token_new(v56, (void*)v57, v53, v54, v55);
    v59 = (char*)v56 + 0;
    v60 = (char*)v0 + 0;
    v61 = (char*)v59 + 0;
    v62 = (char*)v60 + 0;
    v63 = *(int64_t*)v61;
    *(int64_t*)v62 = (int64_t)v63;
    v65 = (char*)v59 + 8;
    v66 = (char*)v60 + 8;
    v67 = *(int64_t*)v65;
    *(int64_t*)v66 = (int64_t)v67;
    v69 = (char*)v56 + 16;
    v70 = (char*)v0 + 16;
    v71 = *(int64_t*)v69;
    *(int64_t*)v70 = (int64_t)v71;
    v73 = (char*)v56 + 24;
    v74 = (char*)v0 + 24;
    v75 = *(int64_t*)v73;
    *(int64_t*)v74 = (int64_t)v75;
    p_call_stack_pop();
    return;
  bb4:
    v35 = *(void**)v12;
    v36 = *(int64_t*)v25;
    v37 = 0LL;
    v38 = byte_to_str((void*)v37, v36);
    v39 = 0LL;
    v40 = str_concat((void*)v39, v35, v38);
    *(void**)v12 = (void*)v40;
    v42 = 0LL;
    v43 = (int64_t)Lexer_advance((void*)v42, v2);
    goto bb6;
  bb5:
    goto bb3;
  bb6:
    goto bb1;
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

int64_t Lexer_advance(void* v0, void* v1) {
    p_call_stack_push("Lexer_advance");
    void* v16;
    void* v47;
    int64_t v2;
    int64_t v61;
    int64_t v13;
    bool v65;
    int64_t v73; /* placeholder for void */
    int64_t v79;
    void* v31;
    int64_t v42; /* placeholder for void */
    bool v50;
    int64_t v71;
    void* v52;
    int64_t v72;
    int64_t v78; /* placeholder for void */
    int64_t v23;
    int64_t v40;
    int64_t v10;
    int64_t v25; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v6;
    int64_t v45;
    void* v46;
    int64_t v55;
    int64_t v11; /* placeholder for void */
    bool v62;
    void* v68;
    int64_t v74; /* placeholder for void */
    int64_t v12;
    void* v38;
    int64_t v3;
    int64_t v39;
    void* v58;
    int64_t v63;
    int64_t v5; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    bool v66;
    int64_t v8;
    int64_t v48;
    void* v22;
    void* v4;
    int64_t v19;
    int64_t v27;
    bool v28;
    int64_t v36; /* placeholder for void */
    void* v30;
    int64_t v41;
    int64_t v67; /* placeholder for void */
    void* v37;
    void* v54;
    int64_t v33;
    int64_t v64;
    int64_t v70;
    int64_t v76; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    void* v17;
    int64_t v32;
    bool v14;
    int64_t v15; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v57;
    int64_t v18;
    int64_t v20;
    void* v69;
    int64_t v75; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v9;
    int64_t v21; /* placeholder for void */
    void* v44;
    int64_t v34;
    int64_t v26;
    void* v7;
    int64_t v56;
    int64_t v24; /* placeholder for void */
    int64_t v60;
    void* v53;
    int64_t v29; /* placeholder for void */
    int64_t v49;
    v2 = 0LL;
    v3 = (int64_t)Lexer_peek((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = (char*)v1 + 8;
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = 1LL;
    v10 = v8 + v9;
    *(int64_t*)v6 = (int64_t)v10;
    v12 = *(int64_t*)v4;
    v13 = 10LL;
    v14 = v12 == v13;
    if (v14) goto bb1;
    else goto bb2;
  bb1:
    v16 = (char*)v1 + 16;
    v17 = (char*)v1 + 16;
    v18 = *(int64_t*)v17;
    v19 = 1LL;
    v20 = v18 + v19;
    *(int64_t*)v16 = (int64_t)v20;
    v22 = (char*)v1 + 24;
    v23 = 1LL;
    *(int64_t*)v22 = (int64_t)v23;
    goto bb3;
  bb2:
    v26 = *(int64_t*)v4;
    v27 = 128LL;
    v28 = v26 < v27;
    if (v28) goto bb4;
    else goto bb5;
  bb3:
    v79 = *(int64_t*)v4;
    p_call_stack_pop();
    return v79;
  bb4:
    v30 = (char*)v1 + 24;
    v31 = (char*)v1 + 24;
    v32 = *(int64_t*)v31;
    v33 = 1LL;
    v34 = v32 + v33;
    *(int64_t*)v30 = (int64_t)v34;
    goto bb6;
  bb5:
    v37 = (char*)v1 + 24;
    v38 = (char*)v1 + 24;
    v39 = *(int64_t*)v38;
    v40 = 1LL;
    v41 = v39 + v40;
    *(int64_t*)v37 = (int64_t)v41;
    goto bb7;
  bb6:
    goto bb3;
  bb7:
    v44 = (char*)v1 + 8;
    v45 = *(int64_t*)v44;
    v46 = (char*)v1 + 0;
    v47 = *(void**)v46;
    v48 = 0LL;
    v49 = (int64_t)str_len((void*)v48, v47);
    v50 = v45 < v49;
    if (v50) goto bb8;
    else goto bb9;
  bb8:
    v52 = (char*)v1 + 0;
    v53 = *(void**)v52;
    v54 = (char*)v1 + 8;
    v55 = *(int64_t*)v54;
    v56 = 0LL;
    v57 = (int64_t)str_byte_at((void*)v56, v53, v55);
    v58 = malloc(8);
    *(int64_t*)v58 = (int64_t)v57;
    v60 = *(int64_t*)v58;
    v61 = 128LL;
    v62 = v60 >= v61;
    v63 = *(int64_t*)v58;
    v64 = 192LL;
    v65 = v63 < v64;
    v66 = v62 && v65;
    if (v66) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v68 = (char*)v1 + 8;
    v69 = (char*)v1 + 8;
    v70 = *(int64_t*)v69;
    v71 = 1LL;
    v72 = v70 + v71;
    *(int64_t*)v68 = (int64_t)v72;
    goto bb12;
  bb11:
    goto bb9;
  bb12:
    goto bb7;
    return 0;
}

void ByteBuffer_write_byte(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_write_byte");
    void* v4;
    int64_t v6; /* placeholder for void */
    int64_t v9;
    int64_t v10;
    void* v3;
    void* v7;
    int64_t v11;
    int64_t v12; /* placeholder for void */
    void* v8;
    int64_t v13; /* placeholder for void */
    int64_t v5;
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

void* pad_right(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("pad_right");
    void* v10;
    int64_t v6; /* placeholder for void */
    int64_t v9;
    int64_t v11; /* placeholder for void */
    bool v14;
    int64_t v13;
    int64_t v8;
    void* v17;
    int64_t v22;
    void* v16;
    void* v4;
    void* v5;
    int64_t v15; /* placeholder for void */
    int64_t v18;
    void* v19;
    int64_t v23;
    int64_t v27; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v26;
    int64_t v3;
    void* v7;
    int64_t v20; /* placeholder for void */
    int64_t v21;
    int64_t v25; /* placeholder for void */
    v3 = 0LL;
    v4 = int_to_str((void*)v3, v1);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = *(void**)v5;
    v8 = 0LL;
    v9 = (int64_t)str_len((void*)v8, v7);
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v9;
    goto bb1;
  bb1:
    v13 = *(int64_t*)v10;
    v14 = v13 < v2;
    if (v14) goto bb2;
    else goto bb3;
  bb2:
    v16 = _str141;
    v17 = *(void**)v5;
    v18 = 0LL;
    v19 = str_concat((void*)v18, v16, v17);
    *(void**)v5 = (void*)v19;
    v21 = *(int64_t*)v10;
    v22 = 1LL;
    v23 = v21 + v22;
    *(int64_t*)v10 = (int64_t)v23;
    goto bb1;
  bb3:
    v26 = *(void**)v5;
    p_call_stack_pop();
    return v26;
    return 0;
}

void StringBuilder_clear(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_clear");
    void* v3;
    int64_t v7; /* placeholder for void */
    void* v2;
    int64_t v4; /* placeholder for void */
    int64_t v6;
    int64_t v8; /* placeholder for void */
    void* v5;
    v2 = (char*)v1 + 0;
    v3 = _str137;
    *(void**)v2 = (void*)v3;
    v5 = (char*)v1 + 8;
    v6 = 0LL;
    *(int64_t*)v5 = (int64_t)v6;
    p_call_stack_pop();
    return;
    return;
}

void ByteBuffer_set_byte(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("ByteBuffer_set_byte");
    void* v6;
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v7;
    void* v13;
    bool v8;
    int64_t v11; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v5;
    int64_t v15;
    int64_t v16; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v9;
    void* v14;
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

void* pad_left(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("pad_left");
    int64_t v12; /* placeholder for void */
    void* v16;
    int64_t v21;
    void* v26;
    int64_t v9;
    int64_t v22;
    int64_t v24; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v7;
    int64_t v11; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v13;
    bool v14;
    void* v10;
    void* v17;
    int64_t v8;
    int64_t v23;
    void* v5;
    int64_t v6; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v18;
    int64_t v27; /* placeholder for void */
    void* v19;
    int64_t v3;
    void* v4;
    v3 = 0LL;
    v4 = int_to_str((void*)v3, v1);
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = *(void**)v5;
    v8 = 0LL;
    v9 = (int64_t)str_len((void*)v8, v7);
    v10 = malloc(8);
    *(int64_t*)v10 = (int64_t)v9;
    goto bb1;
  bb1:
    v13 = *(int64_t*)v10;
    v14 = v13 < v2;
    if (v14) goto bb2;
    else goto bb3;
  bb2:
    v16 = *(void**)v5;
    v17 = _str141;
    v18 = 0LL;
    v19 = str_concat((void*)v18, v16, v17);
    *(void**)v5 = (void*)v19;
    v21 = *(int64_t*)v10;
    v22 = 1LL;
    v23 = v21 + v22;
    *(int64_t*)v10 = (int64_t)v23;
    goto bb1;
  bb3:
    v26 = *(void**)v5;
    p_call_stack_pop();
    return v26;
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

int64_t String_char_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_char_at");
    int64_t v8; /* placeholder for void */
    int64_t v28;
    int64_t v19;
    bool v16;
    int64_t v45;
    int64_t v10;
    int64_t v15;
    int64_t v36;
    int64_t v43; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v25;
    void* v51;
    int64_t v13;
    bool v4;
    int64_t v34; /* placeholder for void */
    int64_t v58;
    int64_t v3;
    int64_t v35; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v50;
    int64_t v59; /* placeholder for void */
    void* v41;
    int64_t v60; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v49;
    int64_t v46; /* placeholder for void */
    int64_t v61;
    int64_t v42; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v14;
    uint8_t v33;
    int64_t v40;
    int64_t v22;
    int64_t v48;
    int64_t v6;
    void* v26;
    int64_t v18; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v9;
    bool v17;
    int64_t v24; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v38;
    void* v54;
    void* v57;
    int64_t v53;
    void* v23;
    int64_t v31; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v44;
    void* v32;
    int64_t v56; /* placeholder for void */
    void* v11;
    int64_t v12; /* placeholder for void */
    void* v37;
    void* v29;
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

void* char_to_str(void* v0, int64_t v1) {
    p_call_stack_push("char_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = p_char_to_str(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t Vec_pop(void* v0, void* v1) {
    p_call_stack_push("Vec_pop");
    int64_t v3; /* placeholder for void */
    int64_t v2;
    v2 = (int64_t)p_vec_pop(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t String_repeat(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_repeat");
    int64_t v43;
    bool v4;
    int64_t v20; /* placeholder for void */
    int64_t v25;
    void* v10;
    void* v33;
    int64_t v31;
    int64_t v46;
    int64_t v68; /* placeholder for void */
    int64_t v21;
    int64_t v55;
    void* v59;
    int64_t v64; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    int64_t v5;
    void* v36;
    int64_t v42;
    int64_t v30;
    int64_t v18;
    int64_t v66;
    int64_t v9;
    int64_t v12; /* placeholder for void */
    bool v40;
    int64_t v69;
    bool v6;
    int64_t v41; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v48;
    int64_t v53; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v32;
    int64_t v34; /* placeholder for void */
    int64_t v47;
    void* v26;
    int64_t v3;
    int64_t v56;
    int64_t v52;
    int64_t v8; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v65;
    int64_t v54; /* placeholder for void */
    int64_t v35;
    int64_t v70; /* placeholder for void */
    int64_t v44;
    int64_t v37; /* placeholder for void */
    int64_t v28;
    int64_t v29;
    void* v14;
    int64_t v22; /* placeholder for void */
    void* v62;
    int64_t v15; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v50;
    int64_t v51;
    int64_t v45;
    int64_t v24;
    void* v17;
    void* v11;
    int64_t v39;
    bool v7;
    int64_t v57;
    int64_t v60; /* placeholder for void */
    int64_t v58;
    int64_t v13;
    int64_t v49; /* placeholder for void */
    int64_t v61;
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

void Vec_sort(void* v0, void* v1) {
    p_call_stack_push("Vec_sort");
    int64_t v3; /* placeholder for void */
    int64_t v2; /* placeholder for void */
    p_vec_sort(v1);
    p_call_stack_pop();
    return;
    return;
}

int64_t str_parse_hex(void* v0, void* v1) {
    p_call_stack_push("str_parse_hex");
    int64_t v188;
    int64_t v49;
    int64_t v97; /* placeholder for void */
    void* v132;
    int64_t v91;
    int64_t v13;
    int64_t v52; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v171; /* placeholder for void */
    int64_t v113; /* placeholder for void */
    int64_t v77;
    bool v78;
    int64_t v37; /* placeholder for void */
    int64_t v120;
    void* v32;
    int64_t v31;
    int64_t v135; /* placeholder for void */
    uint8_t v39;
    int64_t v152; /* placeholder for void */
    int64_t v207;
    int64_t v216;
    int64_t v229;
    bool v233;
    int64_t v234; /* placeholder for void */
    int64_t v10;
    int64_t v71; /* placeholder for void */
    int64_t v105;
    int64_t v122;
    int64_t v99;
    int64_t v148;
    int64_t v82;
    uint8_t v133;
    bool v211;
    int64_t v93;
    bool v144;
    int64_t v98;
    void* v163;
    int64_t v192; /* placeholder for void */
    int64_t v100;
    int64_t v184;
    int64_t v167; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v181;
    int64_t v36; /* placeholder for void */
    int64_t v230;
    int64_t v40; /* placeholder for void */
    void* v20;
    int64_t v62;
    int64_t v23;
    int64_t v227; /* placeholder for void */
    void* v17;
    int64_t v117; /* placeholder for void */
    int64_t v155; /* placeholder for void */
    int64_t v170; /* placeholder for void */
    bool v185;
    int64_t v30; /* placeholder for void */
    void* v126;
    int64_t v202;
    void* v178;
    int64_t v50;
    int64_t v157;
    int64_t v12; /* placeholder for void */
    int64_t v51;
    int64_t v154; /* placeholder for void */
    int64_t v194;
    int64_t v87;
    int64_t v60; /* placeholder for void */
    uint8_t v73;
    int64_t v205; /* placeholder for void */
    int64_t v225;
    bool v208;
    int64_t v84;
    int64_t v59; /* placeholder for void */
    int64_t v55;
    bool v25;
    int64_t v220; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v231; /* placeholder for void */
    int64_t v54;
    int64_t v143;
    int64_t v86; /* placeholder for void */
    int64_t v177;
    bool v80;
    int64_t v81; /* placeholder for void */
    int64_t v61;
    int64_t v232; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v111;
    bool v145;
    void* v166;
    int64_t v236;
    int64_t v121;
    void* v72;
    int64_t v119;
    int64_t v2;
    void* v106;
    int64_t v190;
    void* v115;
    int64_t v223;
    int64_t v226;
    int64_t v187; /* placeholder for void */
    int64_t v228;
    int64_t v176;
    int64_t v101;
    int64_t v239; /* placeholder for void */
    bool v141;
    void* v129;
    int64_t v114; /* placeholder for void */
    bool v199;
    int64_t v34;
    bool v102;
    int64_t v183;
    int64_t v110; /* placeholder for void */
    int64_t v213; /* placeholder for void */
    int64_t v224;
    int64_t v191; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v150;
    int64_t v203;
    int64_t v92;
    bool v44;
    int64_t v161;
    int64_t v140;
    int64_t v162;
    int64_t v9; /* placeholder for void */
    void* v123;
    int64_t v131; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v38;
    int64_t v48; /* placeholder for void */
    int64_t v88;
    bool v19;
    int64_t v134; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    int64_t v149;
    int64_t v107; /* placeholder for void */
    bool v56;
    void* v169;
    void* v66;
    int64_t v200; /* placeholder for void */
    int64_t v237;
    int64_t v242; /* placeholder for void */
    int64_t v3;
    void* v109;
    bool v138;
    void* v35;
    int64_t v11; /* placeholder for void */
    int64_t v204; /* placeholder for void */
    int64_t v6;
    int64_t v151; /* placeholder for void */
    int64_t v201;
    bool v212;
    int64_t v215;
    int64_t v7;
    int64_t v235;
    int64_t v210;
    int64_t v175; /* placeholder for void */
    void* v29;
    int64_t v127; /* placeholder for void */
    int64_t v209;
    bool v186;
    int64_t v94; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v112;
    int64_t v221; /* placeholder for void */
    int64_t v193;
    int64_t v28;
    int64_t v147; /* placeholder for void */
    uint8_t v173;
    int64_t v217; /* placeholder for void */
    int64_t v24;
    int64_t v165;
    int64_t v64; /* placeholder for void */
    int64_t v174; /* placeholder for void */
    int64_t v124; /* placeholder for void */
    int64_t v128;
    int64_t v168;
    int64_t v219; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v222;
    int64_t v65;
    int64_t v206;
    int64_t v26; /* placeholder for void */
    int64_t v42;
    int64_t v58;
    int64_t v125;
    int64_t v158;
    int64_t v75; /* placeholder for void */
    uint8_t v116;
    int64_t v160; /* placeholder for void */
    void* v4;
    bool v195;
    int64_t v197;
    int64_t v240; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v156; /* placeholder for void */
    bool v159;
    bool v8;
    int64_t v218; /* placeholder for void */
    void* v69;
    int64_t v76;
    int64_t v90; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    int64_t v196;
    int64_t v137;
    int64_t v27;
    int64_t v16;
    int64_t v41; /* placeholder for void */
    int64_t v214;
    int64_t v83;
    void* v172;
    int64_t v136;
    int64_t v104;
    int64_t v108;
    int64_t v67; /* placeholder for void */
    int64_t v238; /* placeholder for void */
    void* v63;
    int64_t v164; /* placeholder for void */
    int64_t v68;
    int64_t v142;
    int64_t v241;
    int64_t v139;
    bool v198;
    int64_t v33; /* placeholder for void */
    int64_t v43;
    int64_t v45;
    int64_t v189;
    bool v146;
    void* v14;
    int64_t v18; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v180;
    int64_t v85; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    int64_t v179; /* placeholder for void */
    bool v89;
    int64_t v153; /* placeholder for void */
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

bool HashMap_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_is_empty");
    int64_t v5; /* placeholder for void */
    int64_t v15;
    bool v16;
    int64_t v8; /* placeholder for void */
    int64_t v11;
    int64_t v12; /* placeholder for void */
    void* v4;
    int64_t v13; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v3;
    int64_t v6;
    void* v7;
    int64_t v2;
    void* v10;
    int64_t v9; /* placeholder for void */
    int64_t v14;
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

void HashMap_clear(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clear");
    int64_t v6; /* placeholder for void */
    void* v18;
    int64_t v42; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    void* v7;
    void* v58;
    int64_t v23; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    bool v36;
    int64_t v17;
    int64_t v69; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v13;
    void* v52;
    int64_t v49;
    int64_t v3;
    int64_t v54;
    int64_t v8;
    int64_t v44;
    int64_t v2;
    int64_t v59;
    bool v33;
    int64_t v64;
    int64_t v39;
    int64_t v32;
    int64_t v43;
    void* v4;
    int64_t v53; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v11;
    int64_t v30; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v34;
    bool v45;
    int64_t v57; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v28;
    void* v15;
    int64_t v41; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v27;
    int64_t v16; /* placeholder for void */
    void* v55;
    int64_t v68; /* placeholder for void */
    bool v37;
    int64_t v51;
    int64_t v14;
    int64_t v61; /* placeholder for void */
    int64_t v50;
    int64_t v67; /* placeholder for void */
    int64_t v25;
    int64_t v20;
    int64_t v56; /* placeholder for void */
    void* v24;
    int64_t v46; /* placeholder for void */
    int64_t v47;
    int64_t v26; /* placeholder for void */
    void* v40;
    int64_t v22; /* placeholder for void */
    int64_t v48;
    int64_t v31;
    void* v21;
    int64_t v35;
    int64_t v62;
    int64_t v63;
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

int64_t HashMap_get(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_get");
    int64_t v15;
    void* v71;
    void* v74;
    void* v44;
    int64_t v39;
    bool v17;
    int64_t v78;
    int64_t v82;
    int64_t v72; /* placeholder for void */
    int64_t v38;
    int64_t v14; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    void* v11;
    int64_t v80;
    bool v56;
    int64_t v28;
    int64_t v36;
    int64_t v77; /* placeholder for void */
    int64_t v4;
    int64_t v93;
    void* v8;
    int64_t v21; /* placeholder for void */
    int64_t v63;
    void* v47;
    int64_t v54;
    int64_t v51;
    int64_t v43;
    bool v60;
    int64_t v69; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    void* v89;
    int64_t v27; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v58;
    int64_t v87; /* placeholder for void */
    int64_t v16;
    int64_t v31; /* placeholder for void */
    int64_t v46;
    int64_t v18; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    bool v61;
    int64_t v81;
    int64_t v91; /* placeholder for void */
    int64_t v85;
    void* v83;
    void* v92;
    int64_t v65; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v3;
    int64_t v13; /* placeholder for void */
    void* v41;
    int64_t v49; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v88;
    int64_t v76; /* placeholder for void */
    int64_t v96;
    int64_t v55;
    int64_t v45; /* placeholder for void */
    int64_t v7;
    void* v32;
    int64_t v48; /* placeholder for void */
    int64_t v66;
    int64_t v59;
    int64_t v67;
    int64_t v19;
    int64_t v40;
    int64_t v23;
    void* v86;
    int64_t v37;
    int64_t v75;
    void* v68;
    int64_t v97; /* placeholder for void */
    int64_t v79;
    void* v24;
    void* v29;
    int64_t v22;
    int64_t v57;
    int64_t v70;
    void* v26;
    int64_t v12;
    int64_t v34; /* placeholder for void */
    void* v50;
    void* v5;
    int64_t v33;
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

bool Vec_contains(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_contains");
    bool v3;
    int64_t v4; /* placeholder for void */
    v3 = (bool)p_vec_contains(v1, (void*)v2);
    p_call_stack_pop();
    return v3;
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

int64_t Result_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_unwrap_or_else");
    void* v3;
    int64_t v4;
    int64_t v15; /* placeholder for void */
    int64_t v18;
    int64_t v21;
    int64_t v22; /* placeholder for void */
    bool v6;
    int64_t v12;
    int64_t v17;
    void* v8;
    int64_t v10; /* placeholder for void */
    int64_t v13;
    int64_t v9;
    bool v14;
    void* v16;
    int64_t v20; /* placeholder for void */
    void* v11;
    int64_t v7; /* placeholder for void */
    int64_t v19; /* placeholder for void */
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
        if (strcmp(_fn_name, "main") == 0) {
            v18 = main_entry((void*)v2);
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

int64_t String_find(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_find");
    void* v71;
    int64_t v10;
    int64_t v59;
    int64_t v90; /* placeholder for void */
    bool v55;
    int64_t v78; /* placeholder for void */
    uint8_t v84;
    int64_t v39; /* placeholder for void */
    int64_t v9;
    int64_t v69; /* placeholder for void */
    bool v44;
    int64_t v53;
    void* v77;
    int64_t v97;
    int64_t v64;
    int64_t v31; /* placeholder for void */
    bool v89;
    int64_t v40;
    int64_t v12; /* placeholder for void */
    int64_t v73;
    void* v83;
    int64_t v62; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v108; /* placeholder for void */
    bool v30;
    int64_t v72; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    int64_t v21;
    void* v74;
    int64_t v110;
    bool v6;
    bool v46;
    int64_t v52; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v67;
    int64_t v104; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v29;
    int64_t v57;
    int64_t v95;
    bool v4;
    int64_t v85; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v36;
    void* v50;
    int64_t v111;
    int64_t v14;
    int64_t v20; /* placeholder for void */
    int64_t v54;
    int64_t v33;
    int64_t v70;
    int64_t v49;
    int64_t v93; /* placeholder for void */
    int64_t v76;
    int64_t v42;
    int64_t v11; /* placeholder for void */
    void* v61;
    void* v47;
    int64_t v26; /* placeholder for void */
    void* v37;
    int64_t v79; /* placeholder for void */
    int64_t v105;
    int64_t v38; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    int64_t v63;
    int64_t v24; /* placeholder for void */
    int64_t v87;
    int64_t v41;
    int64_t v58;
    int64_t v34; /* placeholder for void */
    bool v7;
    int64_t v35; /* placeholder for void */
    int64_t v88;
    int64_t v101; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    void* v65;
    bool v91;
    int64_t v96;
    void* v15;
    int64_t v43;
    int64_t v60;
    void* v68;
    int64_t v28;
    void* v19;
    int64_t v45; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v17;
    void* v80;
    int64_t v5;
    int64_t v102;
    bool v23;
    int64_t v99; /* placeholder for void */
    int64_t v106;
    int64_t v103; /* placeholder for void */
    int64_t v22;
    int64_t v32;
    uint8_t v81;
    int64_t v86; /* placeholder for void */
    int64_t v107;
    int64_t v18;
    int64_t v25;
    int64_t v13;
    bool v100;
    int64_t v16; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v3;
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

bool lex_is_digit(void* v0, int64_t v1) {
    p_call_stack_push("lex_is_digit");
    bool v6;
    bool v3;
    int64_t v4;
    bool v5;
    int64_t v2;
    int64_t v7; /* placeholder for void */
    v2 = 48LL;
    v3 = v1 >= v2;
    v4 = 57LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

int64_t String_bytes(void* v0, int64_t v1) {
    p_call_stack_push("String_bytes");
    int64_t v3;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

void* Vec_to_str(void* v0, int64_t v1) {
    p_call_stack_push("Vec_to_str");
    int64_t v17;
    void* v21;
    void* v37;
    void* v6;
    int64_t v74; /* placeholder for void */
    void* v73;
    int64_t v60;
    int64_t v38; /* placeholder for void */
    int64_t v63; /* placeholder for void */
    void* v71;
    void* v61;
    void* v8;
    int64_t v15; /* placeholder for void */
    int64_t v56;
    int64_t v40; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v65;
    int64_t v22; /* placeholder for void */
    int64_t v41;
    void* v4;
    int64_t v27;
    int64_t v3;
    int64_t v13; /* placeholder for void */
    bool v29;
    int64_t v9; /* placeholder for void */
    bool v33;
    void* v36;
    int64_t v43;
    int64_t v44;
    int64_t v50; /* placeholder for void */
    void* v69;
    void* v70;
    int64_t v57; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v2;
    int64_t v67; /* placeholder for void */
    int64_t v31;
    int64_t v48;
    void* v46;
    void* v24;
    int64_t v28;
    int64_t v64;
    int64_t v23;
    void* v59;
    int64_t v12; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    void* v20;
    int64_t v54; /* placeholder for void */
    void* v35;
    int64_t v18; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v51;
    void* v16;
    int64_t v45;
    void* v62;
    void* v10;
    int64_t v42;
    void* v49;
    int64_t v30; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v32;
    int64_t v58; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v55;
    void* v52;
    int64_t v72; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v66;
    int64_t v39; /* placeholder for void */
    void* v14;
    int64_t v19; /* placeholder for void */
    int64_t v11;
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
    v20 = _str142;
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
    v70 = _str143;
    v71 = p_str_concat(v69, v70);
    *(void**)v21 = (void*)v71;
    v73 = *(void**)v21;
    p_call_stack_pop();
    return v73;
  bb4:
    v35 = *(void**)v21;
    v36 = _str144;
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

int64_t Vec_last(void* v0, void* v1) {
    p_call_stack_push("Vec_last");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_last(v1);
    p_call_stack_pop();
    return v2;
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

int64_t Lexer_peek(void* v0, void* v1) {
    p_call_stack_push("Lexer_peek");
    void* v4;
    int64_t v6;
    int64_t v10;
    int64_t v11; /* placeholder for void */
    int64_t v18;
    void* v2;
    void* v5;
    void* v15;
    int64_t v3;
    int64_t v7;
    int64_t v12; /* placeholder for void */
    bool v8;
    int64_t v9; /* placeholder for void */
    void* v14;
    int64_t v16;
    int64_t v17;
    void* v13;
    int64_t v19; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    v4 = (char*)v1 + 0;
    v5 = *(void**)v4;
    v6 = 0LL;
    v7 = (int64_t)str_len((void*)v6, v5);
    v8 = v3 >= v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = 0LL;
    p_call_stack_pop();
    return v10;
  bb2:
    goto bb3;
  bb3:
    v13 = (char*)v1 + 0;
    v14 = *(void**)v13;
    v15 = (char*)v1 + 8;
    v16 = *(int64_t*)v15;
    v17 = 0LL;
    v18 = (int64_t)str_byte_at((void*)v17, v14, v16);
    p_call_stack_pop();
    return v18;
    return 0;
}

void* Lexer_read_utf8(void* v0, void* v1) {
    p_call_stack_push("Lexer_read_utf8");
    int64_t v42;
    int64_t v50;
    int64_t v61;
    int64_t v16;
    int64_t v68;
    int64_t v73;
    int64_t v80; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    int64_t v84;
    int64_t v9; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v3;
    int64_t v85;
    void* v87;
    int64_t v59;
    int64_t v23;
    int64_t v94;
    int64_t v97; /* placeholder for void */
    void* v91;
    void* v62;
    void* v98;
    void* v48;
    bool v79;
    int64_t v26;
    int64_t v29; /* placeholder for void */
    int64_t v74;
    void* v56;
    void* v17;
    void* v39;
    void* v45;
    int64_t v11;
    int64_t v49;
    int64_t v40; /* placeholder for void */
    int64_t v78;
    int64_t v86; /* placeholder for void */
    void* v92;
    bool v28;
    int64_t v93;
    int64_t v99; /* placeholder for void */
    bool v8;
    int64_t v27;
    void* v57;
    int64_t v30;
    int64_t v19;
    int64_t v7;
    int64_t v71; /* placeholder for void */
    void* v35;
    int64_t v83;
    void* v13;
    int64_t v88;
    int64_t v38;
    void* v82;
    void* v67;
    int64_t v58;
    int64_t v6;
    void* v36;
    void* v64;
    void* v66;
    int64_t v70;
    int64_t v5; /* placeholder for void */
    int64_t v2;
    void* v4;
    int64_t v25; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    bool v52;
    int64_t v95;
    int64_t v37; /* placeholder for void */
    int64_t v53; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    void* v54;
    int64_t v77;
    int64_t v75; /* placeholder for void */
    void* v81;
    int64_t v34; /* placeholder for void */
    int64_t v10;
    void* v55;
    int64_t v12;
    int64_t v24; /* placeholder for void */
    void* v47;
    int64_t v22; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v63;
    int64_t v69;
    int64_t v72;
    int64_t v89; /* placeholder for void */
    bool v51;
    int64_t v46;
    int64_t v15; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    bool v44;
    bool v21;
    int64_t v41; /* placeholder for void */
    int64_t v20;
    int64_t v60;
    int64_t v43;
    v2 = 0LL;
    v3 = (int64_t)Lexer_peek((void*)v2, v1);
    v4 = malloc(8);
    *(int64_t*)v4 = (int64_t)v3;
    v6 = *(int64_t*)v4;
    v7 = 128LL;
    v8 = v6 < v7;
    if (v8) goto bb1;
    else goto bb2;
  bb1:
    v10 = 0LL;
    v11 = (int64_t)Lexer_advance((void*)v10, v1);
    v12 = 0LL;
    v13 = byte_to_str((void*)v12, v11);
    p_call_stack_pop();
    return v13;
  bb2:
    goto bb3;
  bb3:
    v16 = 2LL;
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(int64_t*)v4;
    v20 = 240LL;
    v21 = v19 >= v20;
    if (v21) goto bb4;
    else goto bb5;
  bb4:
    v23 = 4LL;
    *(int64_t*)v17 = (int64_t)v23;
    goto bb6;
  bb5:
    v26 = *(int64_t*)v4;
    v27 = 224LL;
    v28 = v26 >= v27;
    if (v28) goto bb7;
    else goto bb8;
  bb6:
    v35 = _str137;
    v36 = malloc(8);
    *(void**)v36 = (void*)v35;
    v38 = 0LL;
    v39 = malloc(8);
    *(int64_t*)v39 = (int64_t)v38;
    goto bb10;
  bb7:
    v30 = 3LL;
    *(int64_t*)v17 = (int64_t)v30;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    goto bb6;
  bb10:
    v42 = *(int64_t*)v39;
    v43 = *(int64_t*)v17;
    v44 = v42 < v43;
    v45 = (char*)v1 + 8;
    v46 = *(int64_t*)v45;
    v47 = (char*)v1 + 0;
    v48 = *(void**)v47;
    v49 = 0LL;
    v50 = (int64_t)str_len((void*)v49, v48);
    v51 = v46 < v50;
    v52 = v44 && v51;
    if (v52) goto bb11;
    else goto bb12;
  bb11:
    v54 = *(void**)v36;
    v55 = (char*)v1 + 0;
    v56 = *(void**)v55;
    v57 = (char*)v1 + 8;
    v58 = *(int64_t*)v57;
    v59 = 0LL;
    v60 = (int64_t)str_byte_at((void*)v59, v56, v58);
    v61 = 0LL;
    v62 = byte_to_str((void*)v61, v60);
    v63 = 0LL;
    v64 = str_concat((void*)v63, v54, v62);
    *(void**)v36 = (void*)v64;
    v66 = (char*)v1 + 8;
    v67 = (char*)v1 + 8;
    v68 = *(int64_t*)v67;
    v69 = 1LL;
    v70 = v68 + v69;
    *(int64_t*)v66 = (int64_t)v70;
    v72 = *(int64_t*)v39;
    v73 = 1LL;
    v74 = v72 + v73;
    *(int64_t*)v39 = (int64_t)v74;
    goto bb10;
  bb12:
    v77 = *(int64_t*)v4;
    v78 = 10LL;
    v79 = v77 == v78;
    if (v79) goto bb13;
    else goto bb14;
  bb13:
    v81 = (char*)v1 + 16;
    v82 = (char*)v1 + 16;
    v83 = *(int64_t*)v82;
    v84 = 1LL;
    v85 = v83 + v84;
    *(int64_t*)v81 = (int64_t)v85;
    v87 = (char*)v1 + 24;
    v88 = 1LL;
    *(int64_t*)v87 = (int64_t)v88;
    goto bb15;
  bb14:
    v91 = (char*)v1 + 24;
    v92 = (char*)v1 + 24;
    v93 = *(int64_t*)v92;
    v94 = 1LL;
    v95 = v93 + v94;
    *(int64_t*)v91 = (int64_t)v95;
    goto bb15;
  bb15:
    v98 = *(void**)v36;
    p_call_stack_pop();
    return v98;
    return 0;
}

bool char_is_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_is_upper");
    int64_t v2;
    bool v6;
    int64_t v7; /* placeholder for void */
    bool v3;
    int64_t v4;
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

int64_t Vec_fold(void* v0, void* v1, int64_t v2, void* v3) {
    p_call_stack_push("Vec_fold");
    int64_t v10;
    int64_t v18;
    int64_t v23;
    int64_t v28;
    int64_t v26; /* placeholder for void */
    void* v7;
    int64_t v29;
    int64_t v22; /* placeholder for void */
    int64_t v19;
    int64_t v6;
    int64_t v13; /* placeholder for void */
    int64_t v20;
    int64_t v17; /* placeholder for void */
    int64_t v15;
    bool v16;
    int64_t v12; /* placeholder for void */
    int64_t v25;
    int64_t v9;
    int64_t v14;
    void* v4;
    int64_t v24;
    int64_t v30; /* placeholder for void */
    int64_t v32;
    void* v21;
    int64_t v31; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v27;
    int64_t v33; /* placeholder for void */
    void* v11;
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
        if (strcmp(_fn_name, "String_byte_at") == 0) {
            v25 = String_byte_at((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_expect") == 0) {
            v25 = Result_expect((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_find") == 0) {
            v25 = String_find((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_push_str") == 0) {
            v25 = String_push_str((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_find") == 0) {
            v25 = Vec_find((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "max") == 0) {
            v25 = p_max((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_take") == 0) {
            v25 = vec_take((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_unwrap_or_else") == 0) {
            v25 = Option_unwrap_or_else((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "vec_skip") == 0) {
            v25 = vec_skip((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "min") == 0) {
            v25 = p_min((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_chunk") == 0) {
            v25 = vec_chunk((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_repeat") == 0) {
            v25 = String_repeat((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_split") == 0) {
            v25 = String_split((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_get") == 0) {
            v25 = HashMap_get((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Lexer_peek_at") == 0) {
            v25 = Lexer_peek_at((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_read_byte") == 0) {
            v25 = ByteBuffer_read_byte((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_get") == 0) {
            v25 = Vec_get((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_unwrap_or_else") == 0) {
            v25 = Result_unwrap_or_else((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "parse_radix") == 0) {
            v25 = parse_radix((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_find_slot") == 0) {
            v25 = HashMap_find_slot((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_position") == 0) {
            v25 = Vec_position((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_char_at") == 0) {
            v25 = String_char_at((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_rfind") == 0) {
            v25 = str_rfind((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "Result_unwrap_or") == 0) {
            v25 = Result_unwrap_or((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "p_str_compare") == 0) {
            v25 = p_str_compare((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_compare") == 0) {
            v25 = str_compare((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "Option_unwrap_or") == 0) {
            v25 = Option_unwrap_or((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_flat_map") == 0) {
            v25 = vec_flat_map((void*)v3, (int64_t)v23, (void*)v24);
        } else if (strcmp(_fn_name, "allocator_alloc") == 0) {
            v25 = allocator_alloc((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_remove") == 0) {
            v25 = Vec_remove((void*)v3, (void*)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_find") == 0) {
            v25 = str_find((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "String_rfind") == 0) {
            v25 = String_rfind((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_zip") == 0) {
            v25 = vec_zip((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "vec_window") == 0) {
            v25 = vec_window((void*)v3, (int64_t)v23, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_expect") == 0) {
            v25 = Option_expect((void*)v3, (void*)v23, (void*)v24);
        } else if (strcmp(_fn_name, "str_byte_at") == 0) {
            v25 = str_byte_at((void*)v3, (void*)v23, (int64_t)v24);
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

void Vec_clear(void* v0, void* v1) {
    p_call_stack_push("Vec_clear");
    int64_t v2; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_clear(v1);
    p_call_stack_pop();
    return;
    return;
}

bool str_equal(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_equal");
    int64_t v3;
    bool v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (bool)p_str_equal((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* lex_error(void* v0, int64_t v1, int64_t v2, void* v3) {
    p_call_stack_push("lex_error");
    void* v4;
    void* v13;
    int64_t v16; /* placeholder for void */
    void* v9;
    void* v7;
    void* v6;
    void* v8;
    void* v15;
    void* v11;
    void* v14;
    int64_t v5;
    void* v12;
    int64_t v10;
    v4 = _str145;
    v5 = 0LL;
    v6 = int_to_str((void*)v5, v1);
    v7 = p_str_concat(v4, v6);
    v8 = _str146;
    v9 = p_str_concat(v7, v8);
    v10 = 0LL;
    v11 = int_to_str((void*)v10, v2);
    v12 = p_str_concat(v9, v11);
    v13 = _str147;
    v14 = p_str_concat(v12, v13);
    v15 = p_str_concat(v14, v3);
    p_call_stack_pop();
    return v15;
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

void Lexer_read_number(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_number");
    void* v91;
    int64_t v249;
    bool v422;
    int64_t v402;
    int64_t v229;
    int64_t v458;
    int64_t v223;
    int64_t v374;
    int64_t v310;
    bool v26;
    bool v241;
    int64_t v396; /* placeholder for void */
    bool v381;
    int64_t v244;
    int64_t v255;
    void* v122;
    void* v279;
    int64_t v295; /* placeholder for void */
    int64_t v37;
    int64_t v173;
    bool v371;
    int64_t v184; /* placeholder for void */
    int64_t v242;
    int64_t v357;
    void* v520;
    int64_t v110;
    void* v197;
    int64_t v227;
    void* v3;
    void* v338;
    int64_t v514;
    int64_t v271;
    int64_t v18; /* placeholder for void */
    int64_t v207; /* placeholder for void */
    void* v11;
    int64_t v307; /* placeholder for void */
    void* v163;
    int64_t v14;
    int64_t v500; /* placeholder for void */
    int64_t v41;
    bool v353;
    void* v491;
    void* v84;
    int64_t v498; /* placeholder for void */
    int64_t v206;
    void* v71;
    int64_t v517; /* placeholder for void */
    int64_t v412;
    int64_t v63;
    int64_t v31; /* placeholder for void */
    void* v139;
    bool v427;
    void* v417;
    int64_t v405;
    int64_t v522;
    void* v336;
    void* v49;
    int64_t v65;
    int64_t v434;
    int64_t v331; /* placeholder for void */
    int64_t v531; /* placeholder for void */
    int64_t v20;
    int64_t v21;
    void* v360;
    int64_t v461; /* placeholder for void */
    int64_t v4;
    void* v292;
    int64_t v114;
    int64_t v490; /* placeholder for void */
    int64_t v234; /* placeholder for void */
    int64_t v148;
    int64_t v510;
    int64_t v346;
    void* v46;
    int64_t v228;
    int64_t v423;
    void* v190;
    int64_t v391;
    int64_t v129; /* placeholder for void */
    void* v481;
    int64_t v117;
    void* v168;
    int64_t v239;
    int64_t v460; /* placeholder for void */
    int64_t v225; /* placeholder for void */
    int64_t v78;
    int64_t v355; /* placeholder for void */
    int64_t v72;
    int64_t v364; /* placeholder for void */
    int64_t v366;
    int64_t v160;
    void* v301;
    int64_t v441;
    int64_t v221;
    int64_t v269;
    void* v506;
    int64_t v499; /* placeholder for void */
    void* v287;
    int64_t v358;
    int64_t v454;
    void* v104;
    int64_t v86;
    void* v288;
    void* v5;
    int64_t v350;
    int64_t v316;
    int64_t v489;
    int64_t v465;
    int64_t v432;
    void* v51;
    int64_t v24;
    void* v134;
    int64_t v56;
    bool v145;
    int64_t v186;
    void* v274;
    int64_t v526;
    void* v471;
    void* v62;
    int64_t v479;
    int64_t v407;
    int64_t v261; /* placeholder for void */
    int64_t v477;
    bool v330;
    int64_t v146;
    int64_t v240;
    bool v155;
    int64_t v169;
    void* v440;
    void* v504;
    void* v524;
    int64_t v275;
    int64_t v535; /* placeholder for void */
    int64_t v347;
    void* v312;
    int64_t v133;
    int64_t v179;
    void* v525;
    void* v289;
    int64_t v10; /* placeholder for void */
    void* v276;
    int64_t v507;
    int64_t v328;
    int64_t v19;
    void* v176;
    int64_t v472; /* placeholder for void */
    void* v478;
    int64_t v137; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    bool v216;
    int64_t v372;
    int64_t v403;
    int64_t v314;
    int64_t v98;
    int64_t v317;
    void* v218;
    void* v496;
    int64_t v267;
    void* v191;
    int64_t v320;
    void* v224;
    int64_t v387; /* placeholder for void */
    bool v251;
    void* v193;
    int64_t v329;
    void* v415;
    int64_t v156; /* placeholder for void */
    void* v484;
    int64_t v448;
    void* v109;
    void* v469;
    int64_t v42;
    int64_t v252; /* placeholder for void */
    int64_t v373;
    int64_t v39; /* placeholder for void */
    int64_t v220;
    bool v116;
    void* v368;
    int64_t v527; /* placeholder for void */
    void* v80;
    int64_t v171;
    int64_t v135;
    int64_t v174;
    void* v166;
    int64_t v236;
    int64_t v74;
    int64_t v290;
    void* v12;
    int64_t v308; /* placeholder for void */
    int64_t v89;
    bool v323;
    void* v332;
    void* v100;
    void* v44;
    bool v149;
    void* v268;
    int64_t v383;
    int64_t v385;
    int64_t v399;
    int64_t v401;
    bool v55;
    int64_t v93; /* placeholder for void */
    void* v136;
    int64_t v393;
    int64_t v103; /* placeholder for void */
    int64_t v359;
    int64_t v463; /* placeholder for void */
    int64_t v534;
    int64_t v99; /* placeholder for void */
    int64_t v150;
    void* v205;
    bool v120;
    void* v392;
    void* v97;
    void* v511;
    int64_t v438; /* placeholder for void */
    void* v183;
    int64_t v516;
    void* v36;
    int64_t v211;
    int64_t v53;
    int64_t v124;
    int64_t v456;
    int64_t v263;
    int64_t v304; /* placeholder for void */
    int64_t v468; /* placeholder for void */
    int64_t v303; /* placeholder for void */
    int64_t v318;
    int64_t v144;
    void* v200;
    int64_t v159;
    int64_t v25;
    int64_t v202;
    int64_t v61; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    void* v529;
    bool v250;
    int64_t v199; /* placeholder for void */
    bool v154;
    void* v492;
    int64_t v143;
    int64_t v536; /* placeholder for void */
    void* v101;
    void* v257;
    int64_t v208; /* placeholder for void */
    void* v180;
    int64_t v203; /* placeholder for void */
    int64_t v342;
    void* v95;
    void* v82;
    int64_t v185;
    bool v404;
    int64_t v210;
    void* v259;
    int64_t v198;
    void* v68;
    int64_t v90;
    int64_t v102;
    void* v40;
    void* v411;
    int64_t v299; /* placeholder for void */
    void* v394;
    int64_t v48; /* placeholder for void */
    int64_t v83;
    bool v451;
    int64_t v175;
    void* v515;
    int64_t v219;
    void* v501;
    void* v309;
    int64_t v217; /* placeholder for void */
    int64_t v363; /* placeholder for void */
    int64_t v76;
    bool v354;
    void* v87;
    bool v60;
    void* v262;
    bool v379;
    void* v474;
    void* v533;
    int64_t v406;
    bool v450;
    void* v96;
    int64_t v416;
    int64_t v475; /* placeholder for void */
    bool v29;
    void* v365;
    int64_t v444;
    int64_t v138; /* placeholder for void */
    void* v457;
    void* v235;
    int64_t v162;
    int64_t v505; /* placeholder for void */
    int64_t v455;
    void* v172;
    int64_t v54;
    int64_t v414;
    bool v30;
    int64_t v370;
    int64_t v343;
    int64_t v452; /* placeholder for void */
    int64_t v476;
    int64_t v158;
    int64_t v231;
    int64_t v280; /* placeholder for void */
    int64_t v306; /* placeholder for void */
    int64_t v480; /* placeholder for void */
    int64_t v298;
    int64_t v419;
    bool v59;
    int64_t v335;
    void* v264;
    int64_t v260; /* placeholder for void */
    void* v73;
    int64_t v170;
    int64_t v294;
    void* v77;
    void* v296;
    int64_t v140;
    void* v232;
    bool v408;
    void* v237;
    int64_t v497;
    int64_t v33;
    void* v519;
    int64_t v437; /* placeholder for void */
    int64_t v395; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v189; /* placeholder for void */
    void* v128;
    int64_t v67;
    void* v161;
    bool v319;
    void* v141;
    int64_t v164; /* placeholder for void */
    int64_t v147;
    bool v246;
    int64_t v177; /* placeholder for void */
    int64_t v8;
    int64_t v256;
    void* v459;
    void* v464;
    void* v9;
    bool v215;
    int64_t v345;
    void* v22;
    int64_t v284;
    int64_t v125;
    int64_t v512; /* placeholder for void */
    void* v528;
    int64_t v337;
    int64_t v378;
    bool v212;
    void* v362;
    int64_t v50;
    int64_t v195; /* placeholder for void */
    bool v446;
    int64_t v420;
    int64_t v92;
    int64_t v213;
    bool v324;
    int64_t v58;
    void* v443;
    void* v453;
    int64_t v81; /* placeholder for void */
    int64_t v390;
    int64_t v265;
    double v466;
    int64_t v165; /* placeholder for void */
    int64_t v254;
    void* v467;
    int64_t v6; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    void* v178;
    int64_t v152;
    void* v196;
    void* v204;
    int64_t v258;
    void* v286;
    void* v311;
    bool v119;
    int64_t v327;
    int64_t v214;
    int64_t v400; /* placeholder for void */
    int64_t v428; /* placeholder for void */
    int64_t v79;
    int64_t v382; /* placeholder for void */
    bool v325;
    int64_t v182;
    int64_t v281;
    void* v388;
    int64_t v45;
    int64_t v64;
    int64_t v339; /* placeholder for void */
    int64_t v449;
    int64_t v333;
    void* v201;
    int64_t v233; /* placeholder for void */
    bool v315;
    int64_t v352;
    void* v367;
    int64_t v167;
    void* v518;
    void* v297;
    int64_t v513;
    int64_t v106;
    int64_t v349;
    bool v375;
    int64_t v418; /* placeholder for void */
    bool v409;
    int64_t v493;
    bool v153;
    void* v253;
    int64_t v462; /* placeholder for void */
    void* v300;
    void* v142;
    int64_t v377;
    bool v17;
    int64_t v70; /* placeholder for void */
    int64_t v291; /* placeholder for void */
    int64_t v321;
    double v473;
    int64_t v445;
    int64_t v270;
    int64_t v277; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v273; /* placeholder for void */
    void* v521;
    void* v293;
    int64_t v15;
    int64_t v194;
    int64_t v351;
    int64_t v503;
    int64_t v85; /* placeholder for void */
    void* v187;
    void* v52;
    void* v7;
    int64_t v340; /* placeholder for void */
    int64_t v132;
    int64_t v430;
    int64_t v436; /* placeholder for void */
    void* v192;
    int64_t v485;
    void* v429;
    void* v105;
    int64_t v16;
    int64_t v131;
    void* v487;
    void* v435;
    int64_t v47; /* placeholder for void */
    int64_t v75;
    void* v157;
    int64_t v57;
    void* v283;
    int64_t v361;
    void* v38;
    void* v238;
    int64_t v431;
    int64_t v384;
    int64_t v313;
    int64_t v421;
    int64_t v509; /* placeholder for void */
    int64_t v188;
    int64_t v494; /* placeholder for void */
    int64_t v247;
    bool v380;
    bool v426;
    int64_t v115;
    int64_t v266;
    int64_t v302;
    void* v66;
    void* v488;
    int64_t v285; /* placeholder for void */
    int64_t v278;
    int64_t v424;
    int64_t v369;
    int64_t v334;
    int64_t v123;
    int64_t v118;
    int64_t v322;
    int64_t v439; /* placeholder for void */
    int64_t v389;
    int64_t v447;
    void* v272;
    void* v226;
    int64_t v326; /* placeholder for void */
    int64_t v121; /* placeholder for void */
    int64_t v43;
    int64_t v470;
    bool v245;
    int64_t v27;
    int64_t v305; /* placeholder for void */
    int64_t v486; /* placeholder for void */
    int64_t v35;
    int64_t v344; /* placeholder for void */
    void* v483;
    int64_t v413;
    int64_t v151;
    int64_t v376;
    int64_t v127;
    void* v230;
    void* v94;
    bool v386;
    int64_t v397; /* placeholder for void */
    int64_t v410; /* placeholder for void */
    void* v482;
    void* v532;
    int64_t v523; /* placeholder for void */
    void* v356;
    int64_t v425;
    int64_t v69; /* placeholder for void */
    void* v32;
    int64_t v34;
    int64_t v113; /* placeholder for void */
    int64_t v502;
    void* v108;
    void* v130;
    int64_t v282;
    int64_t v341; /* placeholder for void */
    void* v442;
    int64_t v248;
    int64_t v530;
    void* v222;
    void* v126;
    void* v495;
    int64_t v243;
    int64_t v209; /* placeholder for void */
    int64_t v28;
    int64_t v398;
    void* v433;
    void* v508;
    bool v348;
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = _str137;
    v12 = malloc(8);
    *(void**)v12 = (void*)v11;
    v14 = 0LL;
    v15 = (int64_t)Lexer_peek((void*)v14, v2);
    v16 = 48LL;
    v17 = v15 == v16;
    if (v17) goto bb1;
    else goto bb2;
  bb1:
    v19 = 1LL;
    v20 = 0LL;
    v21 = (int64_t)Lexer_peek_at((void*)v20, v2, v19);
    v22 = malloc(8);
    *(int64_t*)v22 = (int64_t)v21;
    v24 = *(int64_t*)v22;
    v25 = 120LL;
    v26 = v24 == v25;
    v27 = *(int64_t*)v22;
    v28 = 88LL;
    v29 = v27 == v28;
    v30 = v26 || v29;
    if (v30) goto bb4;
    else goto bb5;
  bb2:
    goto bb3;
  bb3:
    goto bb22;
  bb4:
    v32 = *(void**)v12;
    v33 = 0LL;
    v34 = (int64_t)Lexer_advance((void*)v33, v2);
    v35 = 0LL;
    v36 = byte_to_str((void*)v35, v34);
    v37 = 0LL;
    v38 = str_concat((void*)v37, v32, v36);
    *(void**)v12 = (void*)v38;
    v40 = *(void**)v12;
    v41 = 0LL;
    v42 = (int64_t)Lexer_advance((void*)v41, v2);
    v43 = 0LL;
    v44 = byte_to_str((void*)v43, v42);
    v45 = 0LL;
    v46 = str_concat((void*)v45, v40, v44);
    *(void**)v12 = (void*)v46;
    goto bb7;
  bb5:
    goto bb6;
  bb6:
    v114 = *(int64_t*)v22;
    v115 = 98LL;
    v116 = v114 == v115;
    v117 = *(int64_t*)v22;
    v118 = 66LL;
    v119 = v117 == v118;
    v120 = v116 || v119;
    if (v120) goto bb10;
    else goto bb11;
  bb7:
    v49 = (char*)v2 + 8;
    v50 = *(int64_t*)v49;
    v51 = (char*)v2 + 0;
    v52 = *(void**)v51;
    v53 = 0LL;
    v54 = (int64_t)str_len((void*)v53, v52);
    v55 = v50 < v54;
    v56 = 0LL;
    v57 = (int64_t)Lexer_peek((void*)v56, v2);
    v58 = 0LL;
    v59 = (bool)lex_is_hexdigit((void*)v58, v57);
    v60 = v55 && v59;
    if (v60) goto bb8;
    else goto bb9;
  bb8:
    v62 = *(void**)v12;
    v63 = 0LL;
    v64 = (int64_t)Lexer_advance((void*)v63, v2);
    v65 = 0LL;
    v66 = byte_to_str((void*)v65, v64);
    v67 = 0LL;
    v68 = str_concat((void*)v67, v62, v66);
    *(void**)v12 = (void*)v68;
    goto bb7;
  bb9:
    v71 = *(void**)v12;
    v72 = 2LL;
    v73 = *(void**)v12;
    v74 = 0LL;
    v75 = (int64_t)str_len((void*)v74, v73);
    v76 = 0LL;
    v77 = str_slice((void*)v76, v71, v72, v75);
    v78 = 0LL;
    v79 = (int64_t)str_parse_hex((void*)v78, v77);
    v80 = malloc(8);
    *(int64_t*)v80 = (int64_t)v79;
    v82 = malloc(16);
    v83 = 76LL;
    v84 = (char*)v82 + 0;
    *(int64_t*)v84 = (int64_t)v83;
    v86 = *(int64_t*)v80;
    v87 = (char*)v82 + 8;
    *(int64_t*)v87 = (int64_t)v86;
    v89 = *(int64_t*)v5;
    v90 = *(int64_t*)v9;
    v91 = malloc(32);
    v92 = 0LL;
    token_new(v91, (void*)v92, v82, v89, v90);
    v94 = (char*)v91 + 0;
    v95 = (char*)v0 + 0;
    v96 = (char*)v94 + 0;
    v97 = (char*)v95 + 0;
    v98 = *(int64_t*)v96;
    *(int64_t*)v97 = (int64_t)v98;
    v100 = (char*)v94 + 8;
    v101 = (char*)v95 + 8;
    v102 = *(int64_t*)v100;
    *(int64_t*)v101 = (int64_t)v102;
    v104 = (char*)v91 + 16;
    v105 = (char*)v0 + 16;
    v106 = *(int64_t*)v104;
    *(int64_t*)v105 = (int64_t)v106;
    v108 = (char*)v91 + 24;
    v109 = (char*)v0 + 24;
    v110 = *(int64_t*)v108;
    *(int64_t*)v109 = (int64_t)v110;
    p_call_stack_pop();
    return;
  bb10:
    v122 = *(void**)v12;
    v123 = 0LL;
    v124 = (int64_t)Lexer_advance((void*)v123, v2);
    v125 = 0LL;
    v126 = byte_to_str((void*)v125, v124);
    v127 = 0LL;
    v128 = str_concat((void*)v127, v122, v126);
    *(void**)v12 = (void*)v128;
    v130 = *(void**)v12;
    v131 = 0LL;
    v132 = (int64_t)Lexer_advance((void*)v131, v2);
    v133 = 0LL;
    v134 = byte_to_str((void*)v133, v132);
    v135 = 0LL;
    v136 = str_concat((void*)v135, v130, v134);
    *(void**)v12 = (void*)v136;
    goto bb13;
  bb11:
    goto bb12;
  bb12:
    v210 = *(int64_t*)v22;
    v211 = 111LL;
    v212 = v210 == v211;
    v213 = *(int64_t*)v22;
    v214 = 79LL;
    v215 = v213 == v214;
    v216 = v212 || v215;
    if (v216) goto bb16;
    else goto bb17;
  bb13:
    v139 = (char*)v2 + 8;
    v140 = *(int64_t*)v139;
    v141 = (char*)v2 + 0;
    v142 = *(void**)v141;
    v143 = 0LL;
    v144 = (int64_t)str_len((void*)v143, v142);
    v145 = v140 < v144;
    v146 = 0LL;
    v147 = (int64_t)Lexer_peek((void*)v146, v2);
    v148 = 48LL;
    v149 = v147 == v148;
    v150 = 0LL;
    v151 = (int64_t)Lexer_peek((void*)v150, v2);
    v152 = 49LL;
    v153 = v151 == v152;
    v154 = v149 || v153;
    v155 = v145 && v154;
    if (v155) goto bb14;
    else goto bb15;
  bb14:
    v157 = *(void**)v12;
    v158 = 0LL;
    v159 = (int64_t)Lexer_advance((void*)v158, v2);
    v160 = 0LL;
    v161 = byte_to_str((void*)v160, v159);
    v162 = 0LL;
    v163 = str_concat((void*)v162, v157, v161);
    *(void**)v12 = (void*)v163;
    goto bb13;
  bb15:
    v166 = *(void**)v12;
    v167 = 2LL;
    v168 = *(void**)v12;
    v169 = 0LL;
    v170 = (int64_t)str_len((void*)v169, v168);
    v171 = 0LL;
    v172 = str_slice((void*)v171, v166, v167, v170);
    v173 = 2LL;
    v174 = 0LL;
    v175 = (int64_t)parse_radix((void*)v174, v172, v173);
    v176 = malloc(8);
    *(int64_t*)v176 = (int64_t)v175;
    v178 = malloc(16);
    v179 = 76LL;
    v180 = (char*)v178 + 0;
    *(int64_t*)v180 = (int64_t)v179;
    v182 = *(int64_t*)v176;
    v183 = (char*)v178 + 8;
    *(int64_t*)v183 = (int64_t)v182;
    v185 = *(int64_t*)v5;
    v186 = *(int64_t*)v9;
    v187 = malloc(32);
    v188 = 0LL;
    token_new(v187, (void*)v188, v178, v185, v186);
    v190 = (char*)v187 + 0;
    v191 = (char*)v0 + 0;
    v192 = (char*)v190 + 0;
    v193 = (char*)v191 + 0;
    v194 = *(int64_t*)v192;
    *(int64_t*)v193 = (int64_t)v194;
    v196 = (char*)v190 + 8;
    v197 = (char*)v191 + 8;
    v198 = *(int64_t*)v196;
    *(int64_t*)v197 = (int64_t)v198;
    v200 = (char*)v187 + 16;
    v201 = (char*)v0 + 16;
    v202 = *(int64_t*)v200;
    *(int64_t*)v201 = (int64_t)v202;
    v204 = (char*)v187 + 24;
    v205 = (char*)v0 + 24;
    v206 = *(int64_t*)v204;
    *(int64_t*)v205 = (int64_t)v206;
    p_call_stack_pop();
    return;
  bb16:
    v218 = *(void**)v12;
    v219 = 0LL;
    v220 = (int64_t)Lexer_advance((void*)v219, v2);
    v221 = 0LL;
    v222 = byte_to_str((void*)v221, v220);
    v223 = 0LL;
    v224 = str_concat((void*)v223, v218, v222);
    *(void**)v12 = (void*)v224;
    v226 = *(void**)v12;
    v227 = 0LL;
    v228 = (int64_t)Lexer_advance((void*)v227, v2);
    v229 = 0LL;
    v230 = byte_to_str((void*)v229, v228);
    v231 = 0LL;
    v232 = str_concat((void*)v231, v226, v230);
    *(void**)v12 = (void*)v232;
    goto bb19;
  bb17:
    goto bb18;
  bb18:
    goto bb3;
  bb19:
    v235 = (char*)v2 + 8;
    v236 = *(int64_t*)v235;
    v237 = (char*)v2 + 0;
    v238 = *(void**)v237;
    v239 = 0LL;
    v240 = (int64_t)str_len((void*)v239, v238);
    v241 = v236 < v240;
    v242 = 0LL;
    v243 = (int64_t)Lexer_peek((void*)v242, v2);
    v244 = 48LL;
    v245 = v243 >= v244;
    v246 = v241 && v245;
    v247 = 0LL;
    v248 = (int64_t)Lexer_peek((void*)v247, v2);
    v249 = 55LL;
    v250 = v248 <= v249;
    v251 = v246 && v250;
    if (v251) goto bb20;
    else goto bb21;
  bb20:
    v253 = *(void**)v12;
    v254 = 0LL;
    v255 = (int64_t)Lexer_advance((void*)v254, v2);
    v256 = 0LL;
    v257 = byte_to_str((void*)v256, v255);
    v258 = 0LL;
    v259 = str_concat((void*)v258, v253, v257);
    *(void**)v12 = (void*)v259;
    goto bb19;
  bb21:
    v262 = *(void**)v12;
    v263 = 2LL;
    v264 = *(void**)v12;
    v265 = 0LL;
    v266 = (int64_t)str_len((void*)v265, v264);
    v267 = 0LL;
    v268 = str_slice((void*)v267, v262, v263, v266);
    v269 = 8LL;
    v270 = 0LL;
    v271 = (int64_t)parse_radix((void*)v270, v268, v269);
    v272 = malloc(8);
    *(int64_t*)v272 = (int64_t)v271;
    v274 = malloc(16);
    v275 = 76LL;
    v276 = (char*)v274 + 0;
    *(int64_t*)v276 = (int64_t)v275;
    v278 = *(int64_t*)v272;
    v279 = (char*)v274 + 8;
    *(int64_t*)v279 = (int64_t)v278;
    v281 = *(int64_t*)v5;
    v282 = *(int64_t*)v9;
    v283 = malloc(32);
    v284 = 0LL;
    token_new(v283, (void*)v284, v274, v281, v282);
    v286 = (char*)v283 + 0;
    v287 = (char*)v0 + 0;
    v288 = (char*)v286 + 0;
    v289 = (char*)v287 + 0;
    v290 = *(int64_t*)v288;
    *(int64_t*)v289 = (int64_t)v290;
    v292 = (char*)v286 + 8;
    v293 = (char*)v287 + 8;
    v294 = *(int64_t*)v292;
    *(int64_t*)v293 = (int64_t)v294;
    v296 = (char*)v283 + 16;
    v297 = (char*)v0 + 16;
    v298 = *(int64_t*)v296;
    *(int64_t*)v297 = (int64_t)v298;
    v300 = (char*)v283 + 24;
    v301 = (char*)v0 + 24;
    v302 = *(int64_t*)v300;
    *(int64_t*)v301 = (int64_t)v302;
    p_call_stack_pop();
    return;
  bb22:
    v309 = (char*)v2 + 8;
    v310 = *(int64_t*)v309;
    v311 = (char*)v2 + 0;
    v312 = *(void**)v311;
    v313 = 0LL;
    v314 = (int64_t)str_len((void*)v313, v312);
    v315 = v310 < v314;
    v316 = 0LL;
    v317 = (int64_t)Lexer_peek((void*)v316, v2);
    v318 = 0LL;
    v319 = (bool)lex_is_digit((void*)v318, v317);
    v320 = 0LL;
    v321 = (int64_t)Lexer_peek((void*)v320, v2);
    v322 = 95LL;
    v323 = v321 == v322;
    v324 = v319 || v323;
    v325 = v315 && v324;
    if (v325) goto bb23;
    else goto bb24;
  bb23:
    v327 = 0LL;
    v328 = (int64_t)Lexer_peek((void*)v327, v2);
    v329 = 95LL;
    v330 = v328 != v329;
    if (v330) goto bb25;
    else goto bb26;
  bb24:
    v345 = 0LL;
    v346 = (int64_t)Lexer_peek((void*)v345, v2);
    v347 = 46LL;
    v348 = v346 == v347;
    v349 = 1LL;
    v350 = 0LL;
    v351 = (int64_t)Lexer_peek_at((void*)v350, v2, v349);
    v352 = 0LL;
    v353 = (bool)lex_is_digit((void*)v352, v351);
    v354 = v348 && v353;
    if (v354) goto bb28;
    else goto bb29;
  bb25:
    v332 = *(void**)v12;
    v333 = 0LL;
    v334 = (int64_t)Lexer_peek((void*)v333, v2);
    v335 = 0LL;
    v336 = byte_to_str((void*)v335, v334);
    v337 = 0LL;
    v338 = str_concat((void*)v337, v332, v336);
    *(void**)v12 = (void*)v338;
    goto bb27;
  bb26:
    goto bb27;
  bb27:
    v342 = 0LL;
    v343 = (int64_t)Lexer_advance((void*)v342, v2);
    goto bb22;
  bb28:
    v356 = *(void**)v12;
    v357 = 0LL;
    v358 = (int64_t)Lexer_advance((void*)v357, v2);
    v359 = 0LL;
    v360 = byte_to_str((void*)v359, v358);
    v361 = 0LL;
    v362 = str_concat((void*)v361, v356, v360);
    *(void**)v12 = (void*)v362;
    goto bb31;
  bb29:
    goto bb30;
  bb30:
    v501 = *(void**)v12;
    v502 = 0LL;
    v503 = (int64_t)str_parse_int((void*)v502, v501);
    v504 = malloc(8);
    *(int64_t*)v504 = (int64_t)v503;
    v506 = malloc(16);
    v507 = 76LL;
    v508 = (char*)v506 + 0;
    *(int64_t*)v508 = (int64_t)v507;
    v510 = *(int64_t*)v504;
    v511 = (char*)v506 + 8;
    *(int64_t*)v511 = (int64_t)v510;
    v513 = *(int64_t*)v5;
    v514 = *(int64_t*)v9;
    v515 = malloc(32);
    v516 = 0LL;
    token_new(v515, (void*)v516, v506, v513, v514);
    v518 = (char*)v515 + 0;
    v519 = (char*)v0 + 0;
    v520 = (char*)v518 + 0;
    v521 = (char*)v519 + 0;
    v522 = *(int64_t*)v520;
    *(int64_t*)v521 = (int64_t)v522;
    v524 = (char*)v518 + 8;
    v525 = (char*)v519 + 8;
    v526 = *(int64_t*)v524;
    *(int64_t*)v525 = (int64_t)v526;
    v528 = (char*)v515 + 16;
    v529 = (char*)v0 + 16;
    v530 = *(int64_t*)v528;
    *(int64_t*)v529 = (int64_t)v530;
    v532 = (char*)v515 + 24;
    v533 = (char*)v0 + 24;
    v534 = *(int64_t*)v532;
    *(int64_t*)v533 = (int64_t)v534;
    p_call_stack_pop();
    return;
  bb31:
    v365 = (char*)v2 + 8;
    v366 = *(int64_t*)v365;
    v367 = (char*)v2 + 0;
    v368 = *(void**)v367;
    v369 = 0LL;
    v370 = (int64_t)str_len((void*)v369, v368);
    v371 = v366 < v370;
    v372 = 0LL;
    v373 = (int64_t)Lexer_peek((void*)v372, v2);
    v374 = 0LL;
    v375 = (bool)lex_is_digit((void*)v374, v373);
    v376 = 0LL;
    v377 = (int64_t)Lexer_peek((void*)v376, v2);
    v378 = 95LL;
    v379 = v377 == v378;
    v380 = v375 || v379;
    v381 = v371 && v380;
    if (v381) goto bb32;
    else goto bb33;
  bb32:
    v383 = 0LL;
    v384 = (int64_t)Lexer_peek((void*)v383, v2);
    v385 = 95LL;
    v386 = v384 != v385;
    if (v386) goto bb34;
    else goto bb35;
  bb33:
    v401 = 0LL;
    v402 = (int64_t)Lexer_peek((void*)v401, v2);
    v403 = 101LL;
    v404 = v402 == v403;
    v405 = 0LL;
    v406 = (int64_t)Lexer_peek((void*)v405, v2);
    v407 = 69LL;
    v408 = v406 == v407;
    v409 = v404 || v408;
    if (v409) goto bb37;
    else goto bb38;
  bb34:
    v388 = *(void**)v12;
    v389 = 0LL;
    v390 = (int64_t)Lexer_peek((void*)v389, v2);
    v391 = 0LL;
    v392 = byte_to_str((void*)v391, v390);
    v393 = 0LL;
    v394 = str_concat((void*)v393, v388, v392);
    *(void**)v12 = (void*)v394;
    goto bb36;
  bb35:
    goto bb36;
  bb36:
    v398 = 0LL;
    v399 = (int64_t)Lexer_advance((void*)v398, v2);
    goto bb31;
  bb37:
    v411 = *(void**)v12;
    v412 = 0LL;
    v413 = (int64_t)Lexer_advance((void*)v412, v2);
    v414 = 0LL;
    v415 = byte_to_str((void*)v414, v413);
    v416 = 0LL;
    v417 = str_concat((void*)v416, v411, v415);
    *(void**)v12 = (void*)v417;
    v419 = 0LL;
    v420 = (int64_t)Lexer_peek((void*)v419, v2);
    v421 = 43LL;
    v422 = v420 == v421;
    v423 = 0LL;
    v424 = (int64_t)Lexer_peek((void*)v423, v2);
    v425 = 45LL;
    v426 = v424 == v425;
    v427 = v422 || v426;
    if (v427) goto bb40;
    else goto bb41;
  bb38:
    goto bb39;
  bb39:
    v464 = *(void**)v12;
    v465 = 0LL;
    v466 = (double)str_parse_float((void*)v465, v464);
    v467 = malloc(8);
    *(double*)v467 = v466;
    v469 = malloc(16);
    v470 = 77LL;
    v471 = (char*)v469 + 0;
    *(int64_t*)v471 = (int64_t)v470;
    v473 = *(double*)v467;
    v474 = (char*)v469 + 8;
    *(double*)v474 = v473;
    v476 = *(int64_t*)v5;
    v477 = *(int64_t*)v9;
    v478 = malloc(32);
    v479 = 0LL;
    token_new(v478, (void*)v479, v469, v476, v477);
    v481 = (char*)v478 + 0;
    v482 = (char*)v0 + 0;
    v483 = (char*)v481 + 0;
    v484 = (char*)v482 + 0;
    v485 = *(int64_t*)v483;
    *(int64_t*)v484 = (int64_t)v485;
    v487 = (char*)v481 + 8;
    v488 = (char*)v482 + 8;
    v489 = *(int64_t*)v487;
    *(int64_t*)v488 = (int64_t)v489;
    v491 = (char*)v478 + 16;
    v492 = (char*)v0 + 16;
    v493 = *(int64_t*)v491;
    *(int64_t*)v492 = (int64_t)v493;
    v495 = (char*)v478 + 24;
    v496 = (char*)v0 + 24;
    v497 = *(int64_t*)v495;
    *(int64_t*)v496 = (int64_t)v497;
    p_call_stack_pop();
    return;
  bb40:
    v429 = *(void**)v12;
    v430 = 0LL;
    v431 = (int64_t)Lexer_advance((void*)v430, v2);
    v432 = 0LL;
    v433 = byte_to_str((void*)v432, v431);
    v434 = 0LL;
    v435 = str_concat((void*)v434, v429, v433);
    *(void**)v12 = (void*)v435;
    goto bb42;
  bb41:
    goto bb42;
  bb42:
    goto bb43;
  bb43:
    v440 = (char*)v2 + 8;
    v441 = *(int64_t*)v440;
    v442 = (char*)v2 + 0;
    v443 = *(void**)v442;
    v444 = 0LL;
    v445 = (int64_t)str_len((void*)v444, v443);
    v446 = v441 < v445;
    v447 = 0LL;
    v448 = (int64_t)Lexer_peek((void*)v447, v2);
    v449 = 0LL;
    v450 = (bool)lex_is_digit((void*)v449, v448);
    v451 = v446 && v450;
    if (v451) goto bb44;
    else goto bb45;
  bb44:
    v453 = *(void**)v12;
    v454 = 0LL;
    v455 = (int64_t)Lexer_advance((void*)v454, v2);
    v456 = 0LL;
    v457 = byte_to_str((void*)v456, v455);
    v458 = 0LL;
    v459 = str_concat((void*)v458, v453, v457);
    *(void**)v12 = (void*)v459;
    goto bb43;
  bb45:
    goto bb39;
    return;
}

bool Vec_any(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_any");
    bool v23;
    int64_t v22; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v3;
    int64_t v12;
    int64_t v28;
    int64_t v6;
    int64_t v24; /* placeholder for void */
    bool v31;
    void* v4;
    int64_t v7;
    int64_t v20;
    int64_t v15;
    int64_t v26;
    int64_t v16;
    int64_t v17;
    void* v8;
    int64_t v11;
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v27;
    int64_t v32; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    bool v13;
    void* v18;
    int64_t v10; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    bool v21;
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
        if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_digit") == 0) {
            v21 = lex_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_hexdigit") == 0) {
            v21 = lex_is_hexdigit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_whitespace") == 0) {
            v21 = lex_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "is_word_keyword") == 0) {
            v21 = is_word_keyword((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "lex_is_alpha") == 0) {
            v21 = lex_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_alphanumeric") == 0) {
            v21 = lex_is_alphanumeric((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
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

void Vec_dedup(void* v0, void* v1) {
    p_call_stack_push("Vec_dedup");
    int64_t v4; /* placeholder for void */
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = 0LL;
    vec_dedup((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return;
    return;
}

void* repeat_char(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("repeat_char");
    void* v7;
    void* v17;
    int64_t v21;
    void* v3;
    bool v11;
    int64_t v12; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v19;
    int64_t v9; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v20;
    int64_t v6;
    int64_t v16;
    int64_t v23; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v10;
    void* v24;
    int64_t v25; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v14;
    void* v13;
    void* v4;
    void* v15;
    v3 = _str137;
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    goto bb1;
  bb1:
    v10 = *(int64_t*)v7;
    v11 = v10 < v2;
    if (v11) goto bb2;
    else goto bb3;
  bb2:
    v13 = *(void**)v4;
    v14 = 0LL;
    v15 = byte_to_str((void*)v14, v1);
    v16 = 0LL;
    v17 = str_concat((void*)v16, v13, v15);
    *(void**)v4 = (void*)v17;
    v19 = *(int64_t*)v7;
    v20 = 1LL;
    v21 = v19 + v20;
    *(int64_t*)v7 = (int64_t)v21;
    goto bb1;
  bb3:
    v24 = *(void**)v4;
    p_call_stack_pop();
    return v24;
    return 0;
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

void* float_debug(void* v0, double v1) {
    p_call_stack_push("float_debug");
    int64_t v18; /* placeholder for void */
    int64_t v20;
    int64_t v27; /* placeholder for void */
    void* v10;
    int64_t v13;
    int64_t v6;
    void* v3;
    bool v26;
    int64_t v31;
    void* v45;
    int64_t v29; /* placeholder for void */
    int64_t v30;
    int64_t v46; /* placeholder for void */
    int64_t v40;
    int64_t v2;
    void* v14;
    bool v9;
    void* v7;
    int64_t v15;
    bool v24;
    int64_t v36;
    int64_t v22;
    void* v39;
    int64_t v43; /* placeholder for void */
    bool v17;
    int64_t v44; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v32;
    int64_t v12; /* placeholder for void */
    int64_t v23;
    int64_t v33; /* placeholder for void */
    int64_t v16;
    int64_t v8; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v38;
    void* v41;
    void* v19;
    int64_t v25; /* placeholder for void */
    void* v4;
    int64_t v34; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    bool v35;
    int64_t v5; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v21;
    v2 = 0LL;
    v3 = float_to_str((void*)v2, v1);
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    v6 = 0LL;
    v7 = malloc(8);
    *(int64_t*)v7 = (int64_t)v6;
    v9 = false;
    v10 = malloc(1);
    *(bool*)v10 = v9;
    goto bb1;
  bb1:
    v13 = *(int64_t*)v7;
    v14 = *(void**)v4;
    v15 = 0LL;
    v16 = (int64_t)str_len((void*)v15, v14);
    v17 = v13 < v16;
    if (v17) goto bb2;
    else goto bb3;
  bb2:
    v19 = *(void**)v4;
    v20 = *(int64_t*)v7;
    v21 = 0LL;
    v22 = (int64_t)str_byte_at((void*)v21, v19, v20);
    v23 = 46LL;
    v24 = v22 == v23;
    if (v24) goto bb4;
    else goto bb5;
  bb3:
    v35 = *(bool*)v10;
    v36 = !v35;
    if (v36) goto bb7;
    else goto bb8;
  bb4:
    v26 = true;
    *(bool*)v10 = v26;
    goto bb3;
  bb5:
    goto bb6;
  bb6:
    v30 = *(int64_t*)v7;
    v31 = 1LL;
    v32 = v30 + v31;
    *(int64_t*)v7 = (int64_t)v32;
    goto bb1;
  bb7:
    v38 = *(void**)v4;
    v39 = _str148;
    v40 = 0LL;
    v41 = str_concat((void*)v40, v38, v39);
    *(void**)v4 = (void*)v41;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    v45 = *(void**)v4;
    p_call_stack_pop();
    return v45;
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

void Vec_sort_by(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_sort_by");
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    vec_sort_by((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return;
    return;
}

int64_t Vec_find(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_find");
    int64_t v20;
    int64_t v14; /* placeholder for void */
    void* v18;
    int64_t v17;
    int64_t v22; /* placeholder for void */
    int64_t v23;
    int64_t v26;
    int64_t v10; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v15;
    int64_t v12;
    int64_t v16;
    int64_t v3;
    int64_t v6;
    int64_t v19; /* placeholder for void */
    void* v8;
    int64_t v33; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v28;
    bool v21;
    int64_t v7;
    int64_t v11;
    void* v4;
    int64_t v24; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v31;
    bool v13;
    int64_t v30; /* placeholder for void */
    int64_t v32;
    int64_t v27;
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
        if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_digit") == 0) {
            v21 = lex_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_hexdigit") == 0) {
            v21 = lex_is_hexdigit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_whitespace") == 0) {
            v21 = lex_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "is_word_keyword") == 0) {
            v21 = is_word_keyword((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "lex_is_alpha") == 0) {
            v21 = lex_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_alphanumeric") == 0) {
            v21 = lex_is_alphanumeric((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
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

void* Vec_enumerate(void* v0, void* v1) {
    p_call_stack_push("Vec_enumerate");
    int64_t v4; /* placeholder for void */
    int64_t v2;
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)vec_enumerate((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return (void*)v3;
    return 0;
}

bool Option_is_none(void* v0, void* v1) {
    p_call_stack_push("Option_is_none");
    int64_t v15; /* placeholder for void */
    int64_t v12;
    bool v5;
    int64_t v18; /* placeholder for void */
    void* v7;
    int64_t v3;
    bool v16;
    void* v11;
    bool v9;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v19;
    int64_t v13;
    int64_t v20; /* placeholder for void */
    void* v2;
    int64_t v8;
    bool v14;
    int64_t v10; /* placeholder for void */
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

void Vec_insert(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_insert");
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    p_vec_insert(v1, v2, (void*)v3);
    p_call_stack_pop();
    return;
    return;
}

void Option_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_and_then");
    int64_t v38;
    int64_t v42;
    int64_t v43; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v9;
    void* v22;
    void* v41;
    int64_t v44; /* placeholder for void */
    void* v4;
    int64_t v8; /* placeholder for void */
    int64_t v15;
    void* v40;
    int64_t v46; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    bool v7;
    int64_t v21; /* placeholder for void */
    bool v30;
    int64_t v28;
    void* v32;
    int64_t v12;
    int64_t v39; /* placeholder for void */
    void* v11;
    int64_t v24;
    int64_t v6;
    int64_t v10;
    int64_t v17; /* placeholder for void */
    void* v19;
    int64_t v33;
    void* v36;
    void* v23;
    int64_t v31; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v29;
    void* v34;
    void* v37;
    int64_t v5;
    int64_t v14; /* placeholder for void */
    void* v27;
    void* v16;
    void* v18;
    int64_t v20;
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v15 = Lexer_peek((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v15 = token_kind_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v15 = Lexer_advance((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
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

void Option_or_else(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Option_or_else");
    int64_t v16; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v15;
    void* v11;
    bool v29;
    void* v33;
    int64_t v27;
    int64_t v19;
    int64_t v30; /* placeholder for void */
    void* v9;
    int64_t v23;
    void* v21;
    int64_t v34; /* placeholder for void */
    void* v36;
    bool v7;
    int64_t v46; /* placeholder for void */
    int64_t v6;
    int64_t v14; /* placeholder for void */
    int64_t v28;
    int64_t v12;
    int64_t v8; /* placeholder for void */
    void* v38;
    int64_t v20; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v5;
    void* v13;
    void* v22;
    void* v31;
    int64_t v37; /* placeholder for void */
    int64_t v32;
    void* v18;
    void* v26;
    void* v17;
    int64_t v40;
    int64_t v41; /* placeholder for void */
    void* v39;
    void* v4;
    int64_t v24; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v10;
    void* v43;
    int64_t v35;
    void* v42;
    int64_t v44;
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
        if (strcmp(_fn_name, "main") == 0) {
            v35 = main_entry((void*)v3);
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

int64_t String_rfind(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_rfind");
    bool v55;
    int64_t v101; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    int64_t v9;
    int64_t v73;
    int64_t v35; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    void* v80;
    int64_t v94; /* placeholder for void */
    int64_t v96;
    int64_t v8; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    bool v44;
    int64_t v82; /* placeholder for void */
    int64_t v107;
    int64_t v17;
    int64_t v106;
    int64_t v52; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v88;
    void* v74;
    void* v77;
    int64_t v98; /* placeholder for void */
    int64_t v49;
    int64_t v53;
    void* v39;
    bool v4;
    int64_t v43;
    int64_t v85; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    void* v65;
    int64_t v64;
    int64_t v31; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v58;
    int64_t v90; /* placeholder for void */
    int64_t v103; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v70;
    int64_t v63;
    bool v23;
    int64_t v75; /* placeholder for void */
    void* v83;
    bool v89;
    int64_t v5;
    uint8_t v81;
    int64_t v105;
    void* v47;
    int64_t v78; /* placeholder for void */
    bool v7;
    int64_t v10;
    int64_t v62; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v36;
    int64_t v45; /* placeholder for void */
    int64_t v38;
    int64_t v102;
    int64_t v108; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v21;
    int64_t v20; /* placeholder for void */
    int64_t v22;
    int64_t v54;
    void* v15;
    int64_t v51; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v76;
    void* v19;
    bool v91;
    int64_t v37;
    int64_t v57;
    void* v71;
    int64_t v95;
    bool v100;
    bool v30;
    uint8_t v84;
    int64_t v87;
    int64_t v29;
    int64_t v99; /* placeholder for void */
    int64_t v59;
    int64_t v97;
    int64_t v42;
    int64_t v111;
    int64_t v3;
    int64_t v13;
    int64_t v28;
    int64_t v27; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    void* v50;
    int64_t v110;
    void* v61;
    int64_t v67;
    bool v6;
    int64_t v48; /* placeholder for void */
    int64_t v25;
    int64_t v69; /* placeholder for void */
    int64_t v18;
    int64_t v79; /* placeholder for void */
    int64_t v14;
    bool v46;
    int64_t v33;
    int64_t v32;
    void* v68;
    int64_t v60;
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

int64_t Option_expect(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_expect");
    int64_t v16; /* placeholder for void */
    void* v3;
    int64_t v12;
    int64_t v18; /* placeholder for void */
    int64_t v4;
    bool v6;
    int64_t v7; /* placeholder for void */
    int64_t v9;
    void* v11;
    void* v8;
    int64_t v19;
    int64_t v10; /* placeholder for void */
    int64_t v5;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v14;
    int64_t v17; /* placeholder for void */
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

void Vec_set(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("Vec_set");
    int64_t v5; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_vec_set(v1, v2, (void*)v3);
    p_call_stack_pop();
    return;
    return;
}

void Vec_reverse(void* v0, void* v1) {
    p_call_stack_push("Vec_reverse");
    int64_t v2; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_reverse(v1);
    p_call_stack_pop();
    return;
    return;
}

bool Result_is_err(void* v0, void* v1) {
    p_call_stack_push("Result_is_err");
    bool v14;
    bool v18;
    int64_t v22; /* placeholder for void */
    void* v11;
    int64_t v12;
    int64_t v3;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    void* v7;
    void* v16;
    void* v2;
    int64_t v13;
    int64_t v17;
    bool v21;
    int64_t v10; /* placeholder for void */
    bool v5;
    bool v9;
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

void Result_err(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_err");
    int64_t v33; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v25;
    void* v8;
    void* v34;
    void* v40;
    int64_t v43; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    void* v36;
    int64_t v45; /* placeholder for void */
    void* v15;
    void* v10;
    int64_t v13; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v16;
    bool v6;
    void* v14;
    void* v30;
    int64_t v17; /* placeholder for void */
    int64_t v5;
    void* v32;
    int64_t v38;
    int64_t v9;
    int64_t v4;
    int64_t v35; /* placeholder for void */
    void* v18;
    void* v3;
    void* v28;
    void* v37;
    void* v23;
    void* v19;
    int64_t v29;
    int64_t v11;
    int64_t v20;
    int64_t v24;
    bool v26;
    int64_t v31;
    int64_t v39; /* placeholder for void */
    void* v41;
    void* v12;
    int64_t v42;
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

void StringBuilder_new(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_new");
    void* v3;
    int64_t v5; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v10;
    void* v7;
    int64_t v16; /* placeholder for void */
    void* v13;
    void* v14;
    int64_t v15;
    void* v4;
    int64_t v6;
    void* v9;
    int64_t v12; /* placeholder for void */
    void* v2;
    void* v11;
    int64_t v17; /* placeholder for void */
    v2 = malloc(16);
    v3 = _str137;
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

bool lex_is_alphanumeric(void* v0, int64_t v1) {
    p_call_stack_push("lex_is_alphanumeric");
    int64_t v4;
    bool v5;
    bool v6;
    int64_t v2;
    int64_t v7; /* placeholder for void */
    bool v3;
    v2 = 0LL;
    v3 = (bool)lex_is_alpha((void*)v2, v1);
    v4 = 0LL;
    v5 = (bool)lex_is_digit((void*)v4, v1);
    v6 = v3 || v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void StringBuilder_append_int(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("StringBuilder_append_int");
    int64_t v8; /* placeholder for void */
    void* v10;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    void* v5;
    void* v7;
    int64_t v14; /* placeholder for void */
    void* v9;
    void* v6;
    void* v3;
    void* v4;
    int64_t v12;
    int64_t v11;
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

int64_t vec_skip(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_skip");
    int64_t v30;
    int64_t v25;
    int64_t v33;
    bool v27;
    int64_t v18; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v26;
    int64_t v19; /* placeholder for void */
    int64_t v17;
    int64_t v23; /* placeholder for void */
    int64_t v29;
    int64_t v35;
    void* v11;
    int64_t v36;
    int64_t v40;
    int64_t v41; /* placeholder for void */
    void* v9;
    int64_t v13;
    int64_t v14;
    int64_t v8;
    int64_t v34; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v32;
    int64_t v21;
    int64_t v24; /* placeholder for void */
    int64_t v7;
    int64_t v6; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v5;
    bool v15;
    int64_t v10; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    int64_t v20; /* placeholder for void */
    int64_t v31;
    int64_t v37;
    int64_t v39; /* placeholder for void */
    void* v22;
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

void ByteBuffer_clear(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_clear");
    int64_t v7;
    int64_t v4;
    void* v2;
    void* v3;
    int64_t v5; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v6;
    int64_t v9; /* placeholder for void */
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

int64_t Vec_position(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_position");
    int64_t v28;
    int64_t v17;
    int64_t v14; /* placeholder for void */
    int64_t v27;
    int64_t v30; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v15;
    int64_t v33; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    bool v13;
    int64_t v11;
    bool v21;
    int64_t v6;
    int64_t v10; /* placeholder for void */
    int64_t v12;
    void* v18;
    int64_t v19; /* placeholder for void */
    int64_t v20;
    int64_t v31;
    void* v8;
    int64_t v24; /* placeholder for void */
    int64_t v7;
    int64_t v32;
    int64_t v3;
    int64_t v23;
    int64_t v26;
    void* v4;
    int64_t v16;
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
        if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v21 = char_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_digit") == 0) {
            v21 = lex_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_hexdigit") == 0) {
            v21 = lex_is_hexdigit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_whitespace") == 0) {
            v21 = lex_is_whitespace((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v21 = Result_is_ok((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v21 = char_is_digit((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v21 = char_is_upper((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v21 = ByteBuffer_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v21 = str_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "is_word_keyword") == 0) {
            v21 = is_word_keyword((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v21 = char_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v21 = Result_is_err((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v21 = Option_is_none((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v21 = char_is_lower((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v21 = Vec_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "lex_is_alpha") == 0) {
            v21 = lex_is_alpha((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v21 = HashMap_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v21 = StringBuilder_is_empty((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v21 = String_is_empty((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "lex_is_alphanumeric") == 0) {
            v21 = lex_is_alphanumeric((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v21 = Option_is_some((void*)v2, (void*)v20);
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

int64_t p_max(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("max");
    int64_t v4; /* placeholder for void */
    bool v3;
    int64_t v6; /* placeholder for void */
    int64_t v5; /* placeholder for void */
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

void HashMap_insert(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("HashMap_insert");
    int64_t v31;
    int64_t v9;
    void* v119;
    int64_t v56; /* placeholder for void */
    int64_t v102;
    int64_t v14; /* placeholder for void */
    int64_t v73;
    void* v49;
    void* v58;
    int64_t v80; /* placeholder for void */
    int64_t v74;
    int64_t v17; /* placeholder for void */
    void* v79;
    int64_t v108; /* placeholder for void */
    int64_t v127;
    int64_t v33;
    int64_t v86;
    int64_t v101; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v38;
    int64_t v70; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v128;
    int64_t v129;
    void* v6;
    int64_t v26;
    int64_t v100; /* placeholder for void */
    int64_t v20;
    int64_t v142; /* placeholder for void */
    int64_t v4;
    int64_t v42;
    int64_t v81;
    int64_t v30;
    int64_t v134;
    int64_t v34;
    int64_t v136; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v123;
    void* v85;
    int64_t v124; /* placeholder for void */
    int64_t v137; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v77; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v51;
    void* v110;
    void* v107;
    int64_t v50; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v32;
    int64_t v23;
    bool v35;
    void* v55;
    int64_t v68;
    int64_t v75;
    int64_t v104;
    bool v36;
    void* v113;
    int64_t v118;
    void* v16;
    int64_t v72;
    int64_t v126; /* placeholder for void */
    int64_t v65;
    int64_t v63; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v54;
    int64_t v78;
    int64_t v47;
    int64_t v8;
    void* v132;
    int64_t v90;
    int64_t v15;
    void* v67;
    int64_t v7; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    void* v13;
    void* v122;
    int64_t v106;
    void* v22;
    int64_t v71;
    int64_t v45; /* placeholder for void */
    int64_t v53;
    int64_t v44; /* placeholder for void */
    int64_t v140; /* placeholder for void */
    int64_t v94;
    bool v95;
    void* v61;
    int64_t v105;
    int64_t v130;
    int64_t v98; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v115; /* placeholder for void */
    int64_t v116;
    int64_t v114; /* placeholder for void */
    int64_t v89;
    int64_t v103;
    void* v64;
    bool v28;
    int64_t v60;
    int64_t v121; /* placeholder for void */
    int64_t v27;
    bool v96;
    int64_t v97; /* placeholder for void */
    int64_t v12;
    int64_t v24; /* placeholder for void */
    bool v91;
    int64_t v39; /* placeholder for void */
    void* v41;
    int64_t v66; /* placeholder for void */
    int64_t v117;
    int64_t v92;
    void* v138;
    int64_t v125; /* placeholder for void */
    int64_t v109;
    void* v99;
    int64_t v141; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v10;
    int64_t v43; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v76;
    int64_t v133; /* placeholder for void */
    void* v135;
    int64_t v93;
    int64_t v48;
    int64_t v57;
    int64_t v139; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    int64_t v131;
    void* v82;
    int64_t v11; /* placeholder for void */
    int64_t v5;
    void* v19;
    int64_t v29;
    int64_t v59; /* placeholder for void */
    int64_t v84; /* placeholder for void */
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

int64_t clamp(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("clamp");
    int64_t v9; /* placeholder for void */
    bool v8;
    int64_t v5; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    bool v4;
    int64_t v7; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v10; /* placeholder for void */
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

int64_t String_len(void* v0, int64_t v1) {
    p_call_stack_push("String_len");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
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

int64_t p_min(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("min");
    int64_t v7; /* placeholder for void */
    bool v3;
    int64_t v4; /* placeholder for void */
    int64_t v5; /* placeholder for void */
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

int64_t allocator_alloc(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_alloc");
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v3 = (int64_t)p_allocator_alloc(v1, v2);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t vec_enumerate(void* v0, int64_t v1) {
    p_call_stack_push("vec_enumerate");
    int64_t v20;
    int64_t v31; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v11;
    int64_t v10;
    int64_t v5; /* placeholder for void */
    int64_t v18;
    int64_t v22;
    int64_t v27; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v26;
    int64_t v3;
    int64_t v7;
    int64_t v28;
    int64_t v34; /* placeholder for void */
    int64_t v29;
    int64_t v24;
    int64_t v33;
    int64_t v19;
    int64_t v21; /* placeholder for void */
    int64_t v2;
    int64_t v17; /* placeholder for void */
    int64_t v25;
    int64_t v6;
    int64_t v30;
    void* v8;
    int64_t v23;
    int64_t v14;
    int64_t v15;
    bool v16;
    void* v4;
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

int64_t p_str_length(void* v0, int64_t v1) {
    p_call_stack_push("p_str_length");
    int64_t v9; /* placeholder for void */
    int64_t v2;
    int64_t v16; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v23;
    uint8_t v37;
    int64_t v42; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v34;
    int64_t v35; /* placeholder for void */
    int64_t v41;
    bool v3;
    int64_t v32;
    int64_t v40; /* placeholder for void */
    int64_t v10;
    int64_t v33; /* placeholder for void */
    void* v11;
    int64_t v26;
    int64_t v4; /* placeholder for void */
    void* v17;
    void* v14;
    int64_t v29; /* placeholder for void */
    void* v36;
    int64_t v28;
    int64_t v22;
    int64_t v25; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v5;
    uint8_t v18;
    int64_t v39; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v31;
    bool v24;
    int64_t v27;
    int64_t v30;
    void* v8;
    void* v19;
    int64_t v38; /* placeholder for void */
    int64_t v13;
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

void* Vec_chunk(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_chunk");
    int64_t v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)vec_chunk((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void* str_escape(void* v0, void* v1) {
    p_call_stack_push("str_escape");
    int64_t v43;
    int64_t v93; /* placeholder for void */
    void* v100;
    void* v76;
    int64_t v12; /* placeholder for void */
    int64_t v32;
    int64_t v51; /* placeholder for void */
    void* v69;
    int64_t v72;
    void* v36;
    int64_t v86;
    int64_t v16; /* placeholder for void */
    bool v54;
    int64_t v8;
    void* v2;
    bool v74;
    int64_t v99; /* placeholder for void */
    void* v49;
    void* v10;
    bool v24;
    void* v29;
    int64_t v48;
    int64_t v96;
    int64_t v75; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    int64_t v84;
    void* v37;
    void* v59;
    bool v64;
    int64_t v21; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v14;
    void* v39;
    void* v57;
    int64_t v23;
    int64_t v68;
    int64_t v53;
    int64_t v80; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    void* v26;
    int64_t v45; /* placeholder for void */
    void* v6;
    int64_t v19;
    int64_t v60; /* placeholder for void */
    int64_t v33;
    int64_t v28;
    int64_t v41; /* placeholder for void */
    void* v56;
    int64_t v71; /* placeholder for void */
    int64_t v73;
    void* v85;
    int64_t v95;
    int64_t v30; /* placeholder for void */
    int64_t v17;
    void* v66;
    int64_t v70; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v78;
    void* v67;
    int64_t v81; /* placeholder for void */
    int64_t v9;
    int64_t v55; /* placeholder for void */
    void* v77;
    int64_t v42;
    int64_t v13;
    int64_t v63;
    int64_t v88; /* placeholder for void */
    int64_t v97;
    int64_t v5;
    int64_t v7; /* placeholder for void */
    bool v15;
    int64_t v31; /* placeholder for void */
    int64_t v52;
    void* v46;
    void* v47;
    void* v27;
    int64_t v11; /* placeholder for void */
    int64_t v38;
    int64_t v22;
    int64_t v18;
    void* v3;
    int64_t v35; /* placeholder for void */
    bool v44;
    int64_t v61; /* placeholder for void */
    int64_t v83;
    void* v20;
    bool v34;
    int64_t v65; /* placeholder for void */
    void* v79;
    void* v82;
    int64_t v62;
    int64_t v58;
    void* v87;
    int64_t v89; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    v2 = _str137;
    v3 = malloc(8);
    *(void**)v3 = (void*)v2;
    v5 = 0LL;
    v6 = malloc(8);
    *(int64_t*)v6 = (int64_t)v5;
    v8 = 0LL;
    v9 = (int64_t)str_len((void*)v8, v1);
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
    v17 = *(int64_t*)v6;
    v18 = 0LL;
    v19 = (int64_t)str_byte_at((void*)v18, v1, v17);
    v20 = malloc(8);
    *(int64_t*)v20 = (int64_t)v19;
    v22 = *(int64_t*)v20;
    v23 = 10LL;
    v24 = v22 == v23;
    if (v24) goto bb4;
    else goto bb5;
  bb3:
    v100 = *(void**)v3;
    p_call_stack_pop();
    return v100;
  bb4:
    v26 = *(void**)v3;
    v27 = _str149;
    v28 = 0LL;
    v29 = str_concat((void*)v28, v26, v27);
    *(void**)v3 = (void*)v29;
    goto bb6;
  bb5:
    v32 = *(int64_t*)v20;
    v33 = 9LL;
    v34 = v32 == v33;
    if (v34) goto bb7;
    else goto bb8;
  bb6:
    v95 = *(int64_t*)v6;
    v96 = 1LL;
    v97 = v95 + v96;
    *(int64_t*)v6 = (int64_t)v97;
    goto bb1;
  bb7:
    v36 = *(void**)v3;
    v37 = _str150;
    v38 = 0LL;
    v39 = str_concat((void*)v38, v36, v37);
    *(void**)v3 = (void*)v39;
    goto bb9;
  bb8:
    v42 = *(int64_t*)v20;
    v43 = 13LL;
    v44 = v42 == v43;
    if (v44) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v46 = *(void**)v3;
    v47 = _str151;
    v48 = 0LL;
    v49 = str_concat((void*)v48, v46, v47);
    *(void**)v3 = (void*)v49;
    goto bb12;
  bb11:
    v52 = *(int64_t*)v20;
    v53 = 92LL;
    v54 = v52 == v53;
    if (v54) goto bb13;
    else goto bb14;
  bb12:
    goto bb9;
  bb13:
    v56 = *(void**)v3;
    v57 = _str152;
    v58 = 0LL;
    v59 = str_concat((void*)v58, v56, v57);
    *(void**)v3 = (void*)v59;
    goto bb15;
  bb14:
    v62 = *(int64_t*)v20;
    v63 = 34LL;
    v64 = v62 == v63;
    if (v64) goto bb16;
    else goto bb17;
  bb15:
    goto bb12;
  bb16:
    v66 = *(void**)v3;
    v67 = _str153;
    v68 = 0LL;
    v69 = str_concat((void*)v68, v66, v67);
    *(void**)v3 = (void*)v69;
    goto bb18;
  bb17:
    v72 = *(int64_t*)v20;
    v73 = 0LL;
    v74 = v72 == v73;
    if (v74) goto bb19;
    else goto bb20;
  bb18:
    goto bb15;
  bb19:
    v76 = *(void**)v3;
    v77 = _str154;
    v78 = 0LL;
    v79 = str_concat((void*)v78, v76, v77);
    *(void**)v3 = (void*)v79;
    goto bb21;
  bb20:
    v82 = *(void**)v3;
    v83 = *(int64_t*)v20;
    v84 = 0LL;
    v85 = byte_to_str((void*)v84, v83);
    v86 = 0LL;
    v87 = str_concat((void*)v86, v82, v85);
    *(void**)v3 = (void*)v87;
    goto bb21;
  bb21:
    goto bb18;
    return 0;
}

int64_t vec_flat_map(void* v0, int64_t v1, void* v2) {
    p_call_stack_push("vec_flat_map");
    int64_t v33; /* placeholder for void */
    int64_t v40;
    int64_t v16;
    int64_t v56; /* placeholder for void */
    int64_t v20;
    int64_t v41;
    int64_t v29; /* placeholder for void */
    int64_t v46;
    int64_t v42;
    int64_t v48; /* placeholder for void */
    int64_t v19;
    int64_t v38;
    bool v36;
    int64_t v50;
    int64_t v53; /* placeholder for void */
    int64_t v52;
    int64_t v18; /* placeholder for void */
    int64_t v45;
    bool v17;
    int64_t v32; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v22;
    int64_t v10; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v3;
    int64_t v21;
    int64_t v26;
    void* v28;
    int64_t v35;
    int64_t v4;
    int64_t v30;
    int64_t v8;
    void* v12;
    int64_t v24; /* placeholder for void */
    int64_t v15;
    int64_t v25;
    int64_t v27;
    int64_t v39;
    void* v23;
    int64_t v43;
    int64_t v13; /* placeholder for void */
    int64_t v11;
    int64_t v14; /* placeholder for void */
    int64_t v34;
    void* v9;
    int64_t v44; /* placeholder for void */
    void* v31;
    int64_t v49; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v47;
    int64_t v7;
    int64_t v55;
    int64_t v51;
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v22 = StringBuilder_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v22 = Vec_first((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v22 = HashMap_values((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v22 = char_to_upper((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v22 = Vec_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v22 = String_to_string((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v22 = str_parse_hex((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v22 = Lexer_peek((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v22 = token_kind_tag((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v22 = vec_enumerate((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v22 = Vec_capacity((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v22 = allocator_bytes_used((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v22 = Lexer_advance((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v22 = Vec_pop((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v22 = p_str_hash((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v22 = String_clone((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v22 = HashMap_keys((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v22 = str_parse_int((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v22 = HashMap_clone((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v22 = String_to_lowercase((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v22 = String_parse_int((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v22 = String_trim((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v22 = String_bytes((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v22 = Option_unwrap((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v22 = String_from((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v22 = String_to_uppercase((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v22 = char_to_lower((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v22 = HashMap_len((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v22 = String_len((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v22 = p_abs((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v22 = Vec_last((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v22 = ByteBuffer_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v22 = p_str_length((void*)v2, (int64_t)v21);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v22 = str_len((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v22 = Result_unwrap((void*)v2, (void*)v21);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v22 = String_char_count((void*)v2, (int64_t)v21);
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

bool HashMap_contains(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("HashMap_contains");
    int64_t v38;
    int64_t v49; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v46;
    int64_t v57;
    int64_t v59;
    int64_t v16;
    int64_t v6; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v39;
    bool v61;
    int64_t v43;
    int64_t v25; /* placeholder for void */
    int64_t v33;
    int64_t v36;
    int64_t v13; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v11;
    void* v29;
    int64_t v48; /* placeholder for void */
    void* v24;
    int64_t v62; /* placeholder for void */
    void* v50;
    int64_t v18; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v55;
    int64_t v58;
    void* v32;
    void* v26;
    int64_t v53; /* placeholder for void */
    int64_t v12;
    bool v60;
    int64_t v52; /* placeholder for void */
    int64_t v22;
    int64_t v31; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    void* v8;
    int64_t v37;
    int64_t v4;
    int64_t v7;
    int64_t v14; /* placeholder for void */
    int64_t v51;
    int64_t v3;
    void* v41;
    bool v56;
    void* v44;
    int64_t v9; /* placeholder for void */
    int64_t v15;
    bool v17;
    int64_t v35; /* placeholder for void */
    bool v19;
    int64_t v23;
    int64_t v34; /* placeholder for void */
    int64_t v28;
    void* v5;
    int64_t v54;
    int64_t v40;
    int64_t v20; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    void* v47;
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

void* str_slice(void* v0, void* v1, int64_t v2, int64_t v3) {
    p_call_stack_push("str_slice");
    void* v6;
    int64_t v5;
    int64_t v20;
    void* v8;
    int64_t v12;
    int64_t v27; /* placeholder for void */
    int64_t v35;
    int64_t v36;
    int64_t v15; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v38;
    int64_t v28;
    bool v22;
    int64_t v19; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v21;
    bool v14;
    int64_t v17; /* placeholder for void */
    int64_t v13;
    int64_t v23; /* placeholder for void */
    bool v30;
    void* v32;
    int64_t v34; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v16;
    int64_t v9; /* placeholder for void */
    void* v10;
    int64_t v18; /* placeholder for void */
    int64_t v29;
    int64_t v4;
    int64_t v33; /* placeholder for void */
    int64_t v37;
    int64_t v24;
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
    v32 = _str137;
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

void Vec_push(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_push");
    int64_t v4; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_push(v1, (void*)v2);
    p_call_stack_pop();
    return;
    return;
}

void* str_bytes(void* v0, void* v1) {
    p_call_stack_push("str_bytes");
    void* v23;
    int64_t v27; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v15;
    int64_t v28; /* placeholder for void */
    void* v20;
    void* v26;
    void* v33;
    int64_t v35;
    int64_t v31; /* placeholder for void */
    int64_t v37;
    int64_t v18;
    int64_t v39;
    int64_t v24; /* placeholder for void */
    uint8_t v30;
    int64_t v40; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v7;
    int64_t v21; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v29;
    void* v11;
    int64_t v12; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v22;
    void* v8;
    int64_t v34;
    void* v4;
    int64_t v6;
    int64_t v9; /* placeholder for void */
    int64_t v19;
    void* v3;
    int64_t v38;
    int64_t v25;
    int64_t v2;
    int64_t v43; /* placeholder for void */
    int64_t v10;
    int64_t v14;
    void* v42;
    bool v16;
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

int64_t main_entry(void* v0) {
    p_call_stack_push("main");
    bool v16;
    bool v25;
    bool v27;
    int64_t v127; /* placeholder for void */
    int64_t v130;
    int64_t v178;
    int64_t v151; /* placeholder for void */
    int64_t v58;
    int64_t v61; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    void* v19;
    int64_t v72; /* placeholder for void */
    void* v92;
    void* v157;
    void* v99;
    void* v171;
    int64_t v1;
    int64_t v66; /* placeholder for void */
    int64_t v164;
    int64_t v13; /* placeholder for void */
    int64_t v112; /* placeholder for void */
    void* v121;
    int64_t v132;
    bool v88;
    int64_t v158; /* placeholder for void */
    int64_t v172;
    int64_t v165;
    void* v107;
    bool v142;
    void* v80;
    int64_t v126;
    int64_t v62;
    int64_t v129; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v4;
    int64_t v47; /* placeholder for void */
    void* v153;
    void* v173;
    bool v60;
    int64_t v176; /* placeholder for void */
    int64_t v183; /* placeholder for void */
    int64_t v110;
    bool v55;
    int64_t v125;
    int64_t v104; /* placeholder for void */
    void* v150;
    bool v56;
    bool v33;
    int64_t v77;
    int64_t v93; /* placeholder for void */
    void* v67;
    int64_t v18;
    void* v81;
    void* v115;
    int64_t v133; /* placeholder for void */
    int64_t v134; /* placeholder for void */
    int64_t v141;
    int64_t v163;
    void* v51;
    void* v103;
    int64_t v96;
    int64_t v118;
    int64_t v68;
    int64_t v177;
    int64_t v24;
    int64_t v109; /* placeholder for void */
    bool v59;
    int64_t v113;
    int64_t v76; /* placeholder for void */
    int64_t v57;
    int64_t v117;
    int64_t v145;
    int64_t v54;
    void* v90;
    int64_t v156;
    void* v5;
    void* v170;
    void* v23;
    int64_t v17; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v70;
    void* v75;
    int64_t v95; /* placeholder for void */
    int64_t v46;
    int64_t v98; /* placeholder for void */
    int64_t v179;
    int64_t v87;
    void* v2;
    void* v22;
    int64_t v35;
    int64_t v42; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v32;
    int64_t v97; /* placeholder for void */
    int64_t v131;
    void* v148;
    void* v166;
    int64_t v7;
    void* v147;
    int64_t v174; /* placeholder for void */
    int64_t v182;
    int64_t v154;
    int64_t v3; /* placeholder for void */
    int64_t v52;
    int64_t v6; /* placeholder for void */
    int64_t v53;
    void* v63;
    bool v71;
    int64_t v28; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v137; /* placeholder for void */
    void* v38;
    void* v114;
    int64_t v43; /* placeholder for void */
    bool v119;
    int64_t v65; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    void* v94;
    void* v128;
    int64_t v26; /* placeholder for void */
    void* v122;
    int64_t v146;
    int64_t v169; /* placeholder for void */
    int64_t v45;
    int64_t v79; /* placeholder for void */
    int64_t v143; /* placeholder for void */
    void* v20;
    int64_t v30;
    int64_t v37;
    int64_t v41; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v175;
    void* v162;
    int64_t v106;
    int64_t v149; /* placeholder for void */
    void* v108;
    void* v144;
    void* v116;
    int64_t v50;
    void* v123;
    int64_t v101;
    int64_t v64; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v124;
    int64_t v135;
    void* v11;
    int64_t v44;
    void* v139;
    int64_t v140;
    void* v161;
    int64_t v86;
    int64_t v167; /* placeholder for void */
    int64_t v14;
    int64_t v36;
    bool v49;
    int64_t v9; /* placeholder for void */
    int64_t v29;
    int64_t v69;
    void* v73;
    int64_t v85;
    int64_t v89; /* placeholder for void */
    void* v100;
    void* v152;
    void* v8;
    void* v159;
    int64_t v48; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    void* v82;
    int64_t v155;
    int64_t v15;
    int64_t v12; /* placeholder for void */
    int64_t v31;
    bool v10;
    int64_t v138;
    void* v168;
    void* v84;
    int64_t v181; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v105;
    int64_t v160; /* placeholder for void */
    int64_t v180; /* placeholder for void */
    v1 = (int64_t)p_argc();
    v2 = malloc(8);
    *(int64_t*)v2 = (int64_t)v1;
    v4 = _str137;
    v5 = malloc(8);
    *(void**)v5 = (void*)v4;
    v7 = 0LL;
    v8 = malloc(8);
    *(int64_t*)v8 = (int64_t)v7;
    v10 = false;
    v11 = malloc(1);
    *(bool*)v11 = v10;
    goto bb1;
  bb1:
    v14 = *(int64_t*)v8;
    v15 = *(int64_t*)v2;
    v16 = v14 < v15;
    if (v16) goto bb2;
    else goto bb3;
  bb2:
    v18 = *(int64_t*)v8;
    v19 = p_argv(v18);
    v20 = malloc(8);
    *(void**)v20 = (void*)v19;
    v22 = *(void**)v20;
    v23 = _str155;
    v24 = 0LL;
    v25 = (bool)p_str_equal((void*)v24, (int64_t)v22, (int64_t)v23);
    if (v25) goto bb4;
    else goto bb5;
  bb3:
    v49 = *(bool*)v11;
    v50 = !v49;
    v51 = *(void**)v5;
    v52 = 0LL;
    v53 = (int64_t)str_len((void*)v52, v51);
    v54 = 0LL;
    v55 = v53 == v54;
    v56 = v50 && v55;
    v57 = *(int64_t*)v2;
    v58 = 2LL;
    v59 = v57 >= v58;
    v60 = v56 && v59;
    if (v60) goto bb10;
    else goto bb11;
  bb4:
    v27 = true;
    *(bool*)v11 = v27;
    v29 = *(int64_t*)v8;
    v30 = 1LL;
    v31 = v29 + v30;
    v32 = *(int64_t*)v2;
    v33 = v31 < v32;
    if (v33) goto bb7;
    else goto bb8;
  bb5:
    goto bb6;
  bb6:
    v44 = *(int64_t*)v8;
    v45 = 1LL;
    v46 = v44 + v45;
    *(int64_t*)v8 = (int64_t)v46;
    goto bb1;
  bb7:
    v35 = *(int64_t*)v8;
    v36 = 1LL;
    v37 = v35 + v36;
    v38 = p_argv(v37);
    *(void**)v5 = (void*)v38;
    goto bb9;
  bb8:
    goto bb9;
  bb9:
    goto bb3;
  bb10:
    v62 = 1LL;
    v63 = p_argv(v62);
    *(void**)v5 = (void*)v63;
    goto bb12;
  bb11:
    goto bb12;
  bb12:
    v67 = *(void**)v5;
    v68 = 0LL;
    v69 = (int64_t)str_len((void*)v68, v67);
    v70 = 0LL;
    v71 = v69 == v70;
    if (v71) goto bb13;
    else goto bb14;
  bb13:
    v73 = _str156;
    p_print_str(v73);
    v75 = _str157;
    p_print_str(v75);
    v77 = 1LL;
    p_call_stack_pop();
    return v77;
  bb14:
    goto bb15;
  bb15:
    v80 = *(void**)v5;
    v81 = p_file_read(v80);
    v82 = malloc(8);
    *(void**)v82 = (void*)v81;
    v84 = *(void**)v82;
    v85 = 0LL;
    v86 = (int64_t)str_len((void*)v85, v84);
    v87 = 0LL;
    v88 = v86 == v87;
    if (v88) goto bb16;
    else goto bb17;
  bb16:
    v90 = _str158;
    p_print_str(v90);
    v92 = *(void**)v5;
    p_print_str(v92);
    v94 = _str139;
    p_print_str(v94);
    v96 = 1LL;
    p_call_stack_pop();
    return v96;
  bb17:
    goto bb18;
  bb18:
    v99 = *(void**)v82;
    v100 = malloc(40);
    v101 = 0LL;
    Lexer_new(v100, (void*)v101, v99);
    v103 = malloc(8);
    *(void**)v103 = (void*)v100;
    v105 = *(void**)v103;
    v106 = 0LL;
    v107 = Lexer_tokenize((void*)v106, v105);
    v108 = malloc(8);
    *(void**)v108 = (void*)v107;
    v110 = 0LL;
    *(int64_t*)v8 = (int64_t)v110;
    goto bb19;
  bb19:
    v113 = *(int64_t*)v8;
    v114 = *(void**)v103;
    v115 = (char*)v114 + 32;
    v116 = *(void**)v115;
    v117 = 0LL;
    v118 = (int64_t)Vec_len((void*)v117, v116);
    v119 = v113 < v118;
    if (v119) goto bb20;
    else goto bb21;
  bb20:
    v121 = *(void**)v103;
    v122 = (char*)v121 + 32;
    v123 = *(void**)v122;
    v124 = *(int64_t*)v8;
    v125 = 0LL;
    v126 = (int64_t)Vec_get((void*)v125, v123, v124);
    p_print_int((int64_t)v126);
    v128 = _str139;
    p_print_str(v128);
    v130 = *(int64_t*)v8;
    v131 = 1LL;
    v132 = v130 + v131;
    *(int64_t*)v8 = (int64_t)v132;
    goto bb19;
  bb21:
    v135 = 0LL;
    *(int64_t*)v8 = (int64_t)v135;
    goto bb22;
  bb22:
    v138 = *(int64_t*)v8;
    v139 = *(void**)v108;
    v140 = 0LL;
    v141 = (int64_t)Vec_len((void*)v140, v139);
    v142 = v138 < v141;
    if (v142) goto bb23;
    else goto bb24;
  bb23:
    v144 = *(void**)v108;
    v145 = *(int64_t*)v8;
    v146 = 0LL;
    v147 = (void*)Vec_get((void*)v146, v144, v145);
    v148 = malloc(8);
    *(void**)v148 = (void*)v147;
    v150 = _str159;
    p_print_str(v150);
    v152 = *(void**)v148;
    v153 = (char*)v152 + 16;
    v154 = *(int64_t*)v153;
    v155 = 4LL;
    v156 = 0LL;
    v157 = pad_right((void*)v156, v154, v155);
    p_print_str(v157);
    v159 = _str146;
    p_print_str(v159);
    v161 = *(void**)v148;
    v162 = (char*)v161 + 24;
    v163 = *(int64_t*)v162;
    v164 = 3LL;
    v165 = 0LL;
    v166 = pad_left((void*)v165, v163, v164);
    p_print_str(v166);
    v168 = _str141;
    p_print_str(v168);
    v170 = *(void**)v148;
    v171 = (char*)v170 + 0;
    v172 = 0LL;
    v173 = token_kind_debug((void*)v172, v171);
    p_print_str(v173);
    v175 = _str139;
    p_print_str(v175);
    v177 = *(int64_t*)v8;
    v178 = 1LL;
    v179 = v177 + v178;
    *(int64_t*)v8 = (int64_t)v179;
    goto bb22;
  bb24:
    v182 = 0LL;
    p_call_stack_pop();
    return v182;
    return 0;
}

void keyword_or_ident(void* v0, void* v1, void* v2) {
    p_call_stack_push("keyword_or_ident");
    void* v947;
    void* v1092;
    int64_t v618; /* placeholder for void */
    void* v459;
    void* v534;
    int64_t v180; /* placeholder for void */
    void* v303;
    void* v101;
    int64_t v458;
    void* v637;
    int64_t v341;
    int64_t v212; /* placeholder for void */
    void* v519;
    void* v844;
    int64_t v919; /* placeholder for void */
    int64_t v450; /* placeholder for void */
    int64_t v964; /* placeholder for void */
    void* v1002;
    void* v1191;
    void* v1204;
    void* v390;
    void* v645;
    bool v1229;
    int64_t v1244; /* placeholder for void */
    int64_t v1304;
    int64_t v202;
    bool v1373;
    void* v1418;
    void* v1321;
    int64_t v1376;
    int64_t v146; /* placeholder for void */
    void* v659;
    void* v934;
    void* v1005;
    int64_t v452; /* placeholder for void */
    void* v51;
    int64_t v122; /* placeholder for void */
    int64_t v1048;
    int64_t v42; /* placeholder for void */
    int64_t v1098; /* placeholder for void */
    void* v192;
    void* v1159;
    void* v825;
    int64_t v859;
    int64_t v1028; /* placeholder for void */
    int64_t v359;
    void* v753;
    int64_t v870; /* placeholder for void */
    int64_t v1198; /* placeholder for void */
    int64_t v312; /* placeholder for void */
    int64_t v400;
    void* v912;
    int64_t v427;
    bool v401;
    void* v664;
    int64_t v1099; /* placeholder for void */
    void* v241;
    void* v1276;
    int64_t v586; /* placeholder for void */
    void* v45;
    int64_t v82; /* placeholder for void */
    void* v277;
    void* v699;
    int64_t v523; /* placeholder for void */
    void* v281;
    void* v983;
    void* v465;
    int64_t v734; /* placeholder for void */
    bool v1067;
    int64_t v1158; /* placeholder for void */
    void* v803;
    void* v124;
    bool v563;
    int64_t v272; /* placeholder for void */
    int64_t v649; /* placeholder for void */
    void* v819;
    void* v849;
    int64_t v1039;
    void* v1163;
    int64_t v648; /* placeholder for void */
    int64_t v1169;
    int64_t v104; /* placeholder for void */
    void* v862;
    int64_t v636; /* placeholder for void */
    void* v1231;
    void* v1384;
    void* v997;
    void* v1089;
    int64_t v1363;
    void* v489;
    bool v149;
    void* v70;
    int64_t v342; /* placeholder for void */
    int64_t v524; /* placeholder for void */
    int64_t v594; /* placeholder for void */
    int64_t v650; /* placeholder for void */
    bool v779;
    void* v1096;
    void* v123;
    void* v457;
    void* v705;
    int64_t v776; /* placeholder for void */
    int64_t v422;
    int64_t v260;
    int64_t v1336;
    void* v1335;
    int64_t v1381;
    void* v867;
    bool v815;
    void* v1059;
    bool v347;
    int64_t v247;
    int64_t v940;
    int64_t v323;
    int64_t v1296; /* placeholder for void */
    int64_t v468; /* placeholder for void */
    int64_t v1086; /* placeholder for void */
    int64_t v76;
    void* v549;
    void* v556;
    void* v34;
    int64_t v1196;
    int64_t v824; /* placeholder for void */
    int64_t v402; /* placeholder for void */
    int64_t v379; /* placeholder for void */
    void* v731;
    int64_t v112;
    void* v411;
    void* v376;
    void* v759;
    void* v201;
    int64_t v778;
    void* v789;
    int64_t v378; /* placeholder for void */
    void* v965;
    void* v1011;
    int64_t v564; /* placeholder for void */
    int64_t v1282;
    int64_t v1392; /* placeholder for void */
    void* v97;
    void* v471;
    int64_t v1066;
    int64_t v319;
    int64_t v499;
    int64_t v377;
    void* v1177;
    int64_t v1010; /* placeholder for void */
    int64_t v134;
    void* v30;
    int64_t v44;
    void* v1341;
    void* v87;
    int64_t v1202; /* placeholder for void */
    int64_t v1372;
    void* v498;
    int64_t v972; /* placeholder for void */
    bool v185;
    void* v408;
    int64_t v184;
    void* v83;
    void* v695;
    int64_t v721; /* placeholder for void */
    int64_t v1022; /* placeholder for void */
    int64_t v1415;
    int64_t v676; /* placeholder for void */
    int64_t v382;
    void* v945;
    int64_t v971;
    void* v930;
    void* v551;
    int64_t v510; /* placeholder for void */
    bool v653;
    int64_t v470; /* placeholder for void */
    int64_t v1206; /* placeholder for void */
    int64_t v973; /* placeholder for void */
    void* v178;
    int64_t v360; /* placeholder for void */
    bool v923;
    int64_t v229;
    void* v925;
    void* v1271;
    int64_t v794; /* placeholder for void */
    int64_t v1370; /* placeholder for void */
    void* v988;
    int64_t v415; /* placeholder for void */
    int64_t v139;
    void* v1145;
    int64_t v433; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    void* v660;
    bool v707;
    int64_t v301;
    void* v741;
    void* v444;
    int64_t v607;
    int64_t v1255;
    int64_t v1266; /* placeholder for void */
    int64_t v842; /* placeholder for void */
    int64_t v1421; /* placeholder for void */
    int64_t v31;
    void* v894;
    void* v1299;
    int64_t v1309;
    int64_t v654; /* placeholder for void */
    void* v921;
    void* v1213;
    int64_t v54; /* placeholder for void */
    int64_t v220;
    int64_t v208; /* placeholder for void */
    int64_t v269;
    int64_t v526;
    int64_t v476;
    int64_t v818;
    int64_t v1000; /* placeholder for void */
    int64_t v1036; /* placeholder for void */
    int64_t v438; /* placeholder for void */
    int64_t v1184; /* placeholder for void */
    void* v507;
    int64_t v1378; /* placeholder for void */
    int64_t v172; /* placeholder for void */
    void* v1035;
    int64_t v1207; /* placeholder for void */
    int64_t v1241;
    void* v1114;
    int64_t v114; /* placeholder for void */
    int64_t v811; /* placeholder for void */
    void* v232;
    int64_t v164; /* placeholder for void */
    void* v657;
    int64_t v418;
    int64_t v58;
    void* v394;
    void* v713;
    void* v195;
    void* v345;
    void* v381;
    int64_t v1004; /* placeholder for void */
    int64_t v708; /* placeholder for void */
    int64_t v701;
    int64_t v703; /* placeholder for void */
    int64_t v355;
    void* v749;
    void* v795;
    int64_t v274;
    int64_t v200; /* placeholder for void */
    void* v1074;
    bool v1211;
    void* v1168;
    void* v1239;
    int64_t v1243; /* placeholder for void */
    int64_t v1314; /* placeholder for void */
    int64_t v1360; /* placeholder for void */
    void* v975;
    int64_t v1400; /* placeholder for void */
    void* v807;
    int64_t v1405; /* placeholder for void */
    int64_t v163; /* placeholder for void */
    void* v299;
    void* v354;
    int64_t v436;
    void* v735;
    int64_t v487; /* placeholder for void */
    void* v511;
    void* v537;
    void* v16;
    int64_t v974; /* placeholder for void */
    int64_t v978; /* placeholder for void */
    int64_t v946; /* placeholder for void */
    void* v1217;
    void* v1305;
    int64_t v308; /* placeholder for void */
    void* v855;
    int64_t v1225; /* placeholder for void */
    int64_t v80;
    int64_t v248; /* placeholder for void */
    void* v777;
    int64_t v950; /* placeholder for void */
    void* v1069;
    int64_t v595; /* placeholder for void */
    void* v1199;
    void* v1379;
    void* v231;
    int64_t v397; /* placeholder for void */
    void* v369;
    int64_t v640; /* placeholder for void */
    void* v1389;
    void* v313;
    void* v106;
    void* v747;
    bool v383;
    int64_t v724;
    int64_t v922;
    void* v987;
    int64_t v991; /* placeholder for void */
    int64_t v1228;
    int64_t v836;
    int64_t v130;
    void* v585;
    void* v732;
    int64_t v1237;
    int64_t v1178;
    void* v574;
    void* v1221;
    int64_t v1018; /* placeholder for void */
    int64_t v1100; /* placeholder for void */
    void* v875;
    void* v1402;
    int64_t v582; /* placeholder for void */
    int64_t v1394;
    int64_t v881;
    int64_t v182; /* placeholder for void */
    int64_t v1277;
    int64_t v176; /* placeholder for void */
    bool v77;
    int64_t v812; /* placeholder for void */
    int64_t v1062; /* placeholder for void */
    int64_t v962;
    int64_t v1219;
    void* v624;
    void* v999;
    int64_t v1234; /* placeholder for void */
    bool v1391;
    int64_t v98;
    void* v655;
    int64_t v242;
    int64_t v742;
    int64_t v868;
    int64_t v914; /* placeholder for void */
    bool v545;
    void* v641;
    void* v897;
    void* v709;
    void* v754;
    int64_t v596; /* placeholder for void */
    int64_t v162; /* placeholder for void */
    int64_t v774; /* placeholder for void */
    bool v833;
    void* v525;
    int64_t v197;
    int64_t v442; /* placeholder for void */
    int64_t v1070;
    void* v1073;
    void* v279;
    int64_t v575;
    int64_t v1102;
    void* v1182;
    int64_t v1192;
    void* v48;
    int64_t v1076; /* placeholder for void */
    void* v1251;
    void* v538;
    void* v1307;
    int64_t v1328; /* placeholder for void */
    int64_t v1201;
    void* v943;
    void* v1330;
    int64_t v872;
    int64_t v449;
    void* v1105;
    int64_t v896; /* placeholder for void */
    int64_t v604; /* placeholder for void */
    int64_t v478; /* placeholder for void */
    bool v959;
    int64_t v1082; /* placeholder for void */
    int64_t v1297; /* placeholder for void */
    int64_t v874; /* placeholder for void */
    void* v1053;
    void* v781;
    int64_t v608; /* placeholder for void */
    int64_t v865; /* placeholder for void */
    int64_t v1388; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    bool v437;
    int64_t v127; /* placeholder for void */
    void* v696;
    int64_t v110; /* placeholder for void */
    int64_t v305;
    void* v52;
    void* v1236;
    void* v1312;
    int64_t v362; /* placeholder for void */
    void* v903;
    int64_t v1050; /* placeholder for void */
    int64_t v1261; /* placeholder for void */
    int64_t v265;
    void* v1417;
    int64_t v1063; /* placeholder for void */
    void* v189;
    void* v587;
    int64_t v1350; /* placeholder for void */
    int64_t v49;
    void* v1254;
    int64_t v665;
    int64_t v392; /* placeholder for void */
    bool v743;
    int64_t v949;
    void* v1173;
    int64_t v810; /* placeholder for void */
    int64_t v733;
    void* v880;
    int64_t v1403;
    bool v1139;
    void* v315;
    int64_t v888; /* placeholder for void */
    int64_t v22;
    int64_t v1230; /* placeholder for void */
    void* v483;
    void* v552;
    void* v876;
    int64_t v755;
    void* v831;
    int64_t v352; /* placeholder for void */
    int64_t v215;
    int64_t v562;
    int64_t v1104; /* placeholder for void */
    void* v1029;
    int64_t v474; /* placeholder for void */
    int64_t v784; /* placeholder for void */
    int64_t v998;
    int64_t v1115;
    int64_t v719;
    void* v393;
    void* v1127;
    int64_t v64; /* placeholder for void */
    int64_t v125;
    bool v1085;
    bool v1193;
    int64_t v1232;
    int64_t v1259;
    int64_t v1342; /* placeholder for void */
    int64_t v166;
    int64_t v292;
    bool v203;
    void* v1101;
    int64_t v1354;
    void* v1407;
    int64_t v86; /* placeholder for void */
    void* v255;
    int64_t v554; /* placeholder for void */
    void* v817;
    void* v371;
    bool v311;
    int64_t v1298; /* placeholder for void */
    int64_t v152;
    bool v635;
    void* v69;
    int64_t v302; /* placeholder for void */
    void* v533;
    int64_t v560; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    void* v583;
    int64_t v60; /* placeholder for void */
    bool v725;
    int64_t v917;
    int64_t v276; /* placeholder for void */
    void* v615;
    void* v808;
    void* v889;
    void* v1042;
    void* v21;
    int64_t v1270; /* placeholder for void */
    int64_t v856; /* placeholder for void */
    int64_t v1273;
    int64_t v1216; /* placeholder for void */
    int64_t v1322;
    int64_t v816; /* placeholder for void */
    void* v1267;
    int64_t v18; /* placeholder for void */
    void* v502;
    int64_t v1408;
    int64_t v955; /* placeholder for void */
    void* v1161;
    int64_t v1306; /* placeholder for void */
    int64_t v492; /* placeholder for void */
    void* v141;
    void* v120;
    void* v309;
    int64_t v508;
    int64_t v647;
    void* v115;
    int64_t v764;
    int64_t v1094; /* placeholder for void */
    int64_t v216; /* placeholder for void */
    int64_t v1288; /* placeholder for void */
    void* v1215;
    void* v497;
    bool v41;
    int64_t v629;
    int64_t v757; /* placeholder for void */
    int64_t v805;
    int64_t v235; /* placeholder for void */
    int64_t v980;
    int64_t v1260; /* placeholder for void */
    int64_t v697;
    void* v723;
    int64_t v787;
    int64_t v157;
    void* v81;
    int64_t v740; /* placeholder for void */
    int64_t v254; /* placeholder for void */
    void* v339;
    void* v429;
    int64_t v55; /* placeholder for void */
    bool v95;
    int64_t v632; /* placeholder for void */
    int64_t v838; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    void* v786;
    int64_t v108; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v1012;
    int64_t v1040; /* placeholder for void */
    void* v1047;
    void* v1095;
    void* v1131;
    bool v1175;
    int64_t v522; /* placeholder for void */
    void* v79;
    void* v441;
    void* v187;
    void* v261;
    int64_t v512;
    int64_t v908;
    void* v153;
    void* v1077;
    int64_t v13;
    void* v603;
    int64_t v253; /* placeholder for void */
    int64_t v791;
    void* v439;
    int64_t v976;
    int64_t v1172; /* placeholder for void */
    int64_t v1268;
    void* v839;
    int64_t v388; /* placeholder for void */
    int64_t v557;
    void* v27;
    int64_t v482; /* placeholder for void */
    int64_t v828; /* placeholder for void */
    void* v966;
    int64_t v848; /* placeholder for void */
    void* v475;
    void* v621;
    int64_t v782;
    int64_t v796;
    int64_t v186; /* placeholder for void */
    int64_t v326; /* placeholder for void */
    int64_t v712; /* placeholder for void */
    void* v268;
    void* v799;
    void* v1024;
    int64_t v1194; /* placeholder for void */
    int64_t v775; /* placeholder for void */
    int64_t v1205;
    void* v1293;
    int64_t v845;
    int64_t v600; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    void* v642;
    int64_t v834; /* placeholder for void */
    int64_t v860; /* placeholder for void */
    int64_t v1346; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    void* v219;
    int64_t v346;
    int64_t v258; /* placeholder for void */
    int64_t v788; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v729;
    bool v221;
    int64_t v863;
    void* v1019;
    void* v1071;
    int64_t v832;
    void* v1222;
    void* v1303;
    int64_t v298; /* placeholder for void */
    void* v210;
    int64_t v1368; /* placeholder for void */
    void* v1375;
    int64_t v937; /* placeholder for void */
    int64_t v144; /* placeholder for void */
    int64_t v1016;
    int64_t v198; /* placeholder for void */
    void* v579;
    void* v1065;
    int64_t v1134; /* placeholder for void */
    void* v993;
    int64_t v1214;
    void* v1395;
    int64_t v658; /* placeholder for void */
    int64_t v1058; /* placeholder for void */
    int64_t v715;
    void* v93;
    bool v761;
    bool v617;
    int64_t v1208; /* placeholder for void */
    int64_t v170;
    int64_t v690; /* placeholder for void */
    void* v1033;
    int64_t v1118; /* placeholder for void */
    int64_t v1279; /* placeholder for void */
    void* v969;
    int64_t v994;
    bool v1355;
    int64_t v576; /* placeholder for void */
    void* v160;
    int64_t v179;
    void* v417;
    int64_t v738; /* placeholder for void */
    void* v375;
    void* v7;
    int64_t v337;
    int64_t v410; /* placeholder for void */
    int64_t v985;
    int64_t v716; /* placeholder for void */
    int64_t v1003;
    int64_t v1147;
    int64_t v280; /* placeholder for void */
    int64_t v1187;
    int64_t v1152; /* placeholder for void */
    bool v5;
    void* v605;
    int64_t v1250;
    void* v691;
    void* v3;
    void* v246;
    int64_t v584;
    void* v939;
    int64_t v238;
    int64_t v990; /* placeholder for void */
    bool v1031;
    void* v1132;
    int64_t v490;
    int64_t v652;
    void* v835;
    void* v75;
    int64_t v488; /* placeholder for void */
    int64_t v683;
    int64_t v46; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v1064; /* placeholder for void */
    int64_t v1356; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    void* v47;
    void* v1359;
    void* v358;
    int64_t v902; /* placeholder for void */
    bool v113;
    void* v885;
    void* v529;
    int64_t v252; /* placeholder for void */
    int64_t v398; /* placeholder for void */
    int64_t v467;
    int64_t v694; /* placeholder for void */
    void* v873;
    int64_t v1174;
    int64_t v1120;
    int64_t v739; /* placeholder for void */
    int64_t v1349;
    int64_t v1014; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    bool v1049;
    bool v1265;
    int64_t v244; /* placeholder for void */
    void* v9;
    int64_t v674;
    void* v353;
    void* v929;
    int64_t v1046; /* placeholder for void */
    int64_t v622; /* placeholder for void */
    void* v763;
    bool v689;
    int64_t v1080; /* placeholder for void */
    int64_t v256;
    void* v43;
    void* v25;
    void* v1113;
    int64_t v1144; /* placeholder for void */
    void* v243;
    int64_t v266; /* placeholder for void */
    void* v569;
    int64_t v616;
    bool v905;
    bool v1247;
    int64_t v181; /* placeholder for void */
    void* v531;
    int64_t v217; /* placeholder for void */
    void* v351;
    void* v1060;
    int64_t v89;
    int64_t v1331;
    void* v399;
    void* v1087;
    bool v419;
    void* v425;
    int64_t v496; /* placeholder for void */
    int64_t v1320; /* placeholder for void */
    void* v259;
    int64_t v548;
    int64_t v571;
    void* v1308;
    void* v99;
    void* v196;
    int64_t v472;
    void* v790;
    int64_t v643;
    int64_t v726; /* placeholder for void */
    int64_t v424; /* placeholder for void */
    int64_t v17;
    void* v228;
    int64_t v558; /* placeholder for void */
    int64_t v746;
    int64_t v661;
    void* v826;
    int64_t v892; /* placeholder for void */
    int64_t v744; /* placeholder for void */
    int64_t v878; /* placeholder for void */
    void* v718;
    int64_t v704; /* placeholder for void */
    void* v879;
    int64_t v546; /* placeholder for void */
    void* v898;
    int64_t v396; /* placeholder for void */
    void* v385;
    int64_t v590; /* placeholder for void */
    int64_t v802; /* placeholder for void */
    int64_t v823;
    bool v1301;
    int64_t v1352; /* placeholder for void */
    int64_t v1075;
    int64_t v1364; /* placeholder for void */
    void* v405;
    int64_t v770; /* placeholder for void */
    int64_t v1386; /* placeholder for void */
    void* v948;
    int64_t v1316; /* placeholder for void */
    void* v1023;
    void* v804;
    int64_t v289; /* placeholder for void */
    void* v1164;
    int64_t v262; /* placeholder for void */
    int64_t v866; /* placeholder for void */
    void* v477;
    int64_t v829; /* placeholder for void */
    void* v1107;
    void* v1197;
    void* v1414;
    void* v273;
    int64_t v395;
    int64_t v528; /* placeholder for void */
    void* v84;
    int64_t v290; /* placeholder for void */
    int64_t v356; /* placeholder for void */
    int64_t v928; /* placeholder for void */
    int64_t v1274; /* placeholder for void */
    bool v365;
    int64_t v924; /* placeholder for void */
    void* v984;
    int64_t v307; /* placeholder for void */
    void* v1348;
    int64_t v1278; /* placeholder for void */
    bool v257;
    int64_t v1027; /* placeholder for void */
    bool v491;
    int64_t v1032; /* placeholder for void */
    void* v336;
    int64_t v218; /* placeholder for void */
    void* v426;
    void* v435;
    int64_t v854;
    void* v1167;
    void* v1258;
    void* v138;
    int64_t v702; /* placeholder for void */
    int64_t v294; /* placeholder for void */
    void* v750;
    void* v981;
    void* v1015;
    void* v1055;
    int64_t v271; /* placeholder for void */
    int64_t v638;
    void* v547;
    int64_t v798; /* placeholder for void */
    void* v597;
    int64_t v1190; /* placeholder for void */
    void* v1240;
    void* v209;
    bool v131;
    int64_t v679;
    int64_t v456; /* placeholder for void */
    int64_t v464; /* placeholder for void */
    int64_t v1008; /* placeholder for void */
    void* v1371;
    int64_t v420; /* placeholder for void */
    int64_t v373;
    int64_t v140; /* placeholder for void */
    int64_t v348; /* placeholder for void */
    void* v861;
    int64_t v1136; /* placeholder for void */
    int64_t v1310; /* placeholder for void */
    void* v250;
    int64_t v938; /* placeholder for void */
    void* v1078;
    void* v367;
    void* v171;
    void* v151;
    void* v286;
    void* v610;
    int64_t v504; /* placeholder for void */
    int64_t v656;
    int64_t v758; /* placeholder for void */
    int64_t v1264;
    void* v447;
    void* v479;
    int64_t v672; /* placeholder for void */
    int64_t v414; /* placeholder for void */
    void* v1353;
    int64_t v386;
    int64_t v1117; /* placeholder for void */
    int64_t v150; /* placeholder for void */
    void* v1001;
    bool v1121;
    void* v1377;
    int64_t v530;
    void* v227;
    void* v225;
    int64_t v240; /* placeholder for void */
    int64_t v432; /* placeholder for void */
    int64_t v667; /* placeholder for void */
    void* v736;
    int64_t v1210;
    int64_t v960; /* placeholder for void */
    void* v951;
    void* v1091;
    void* v173;
    void* v423;
    void* v911;
    int64_t v895;
    void* v389;
    void* v1383;
    void* v1329;
    int64_t v72; /* placeholder for void */
    int64_t v1138;
    void* v159;
    int64_t v532; /* placeholder for void */
    void* v1143;
    void* v1287;
    int64_t v226; /* placeholder for void */
    int64_t v1135; /* placeholder for void */
    void* v682;
    int64_t v374; /* placeholder for void */
    int64_t v766; /* placeholder for void */
    void* v767;
    int64_t v1057;
    int64_t v154; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v1223;
    int64_t v158; /* placeholder for void */
    int64_t v416; /* placeholder for void */
    int64_t v428; /* placeholder for void */
    void* v169;
    int64_t v391;
    int64_t v1026; /* placeholder for void */
    int64_t v1160;
    int64_t v1148; /* placeholder for void */
    void* v1362;
    void* v1272;
    int64_t v74; /* placeholder for void */
    void* v119;
    void* v249;
    void* v1366;
    bool v581;
    int64_t v1130; /* placeholder for void */
    int64_t v1068; /* placeholder for void */
    int64_t v67;
    int64_t v454;
    void* v61;
    void* v573;
    int64_t v668; /* placeholder for void */
    void* v933;
    bool v1013;
    int64_t v580;
    void* v191;
    void* v840;
    int64_t v1345;
    void* v174;
    int64_t v143;
    void* v853;
    int64_t v904;
    int64_t v32; /* placeholder for void */
    void* v1409;
    int64_t v809;
    int64_t v175;
    int64_t v236; /* placeholder for void */
    int64_t v1061;
    void* v516;
    void* v822;
    void* v561;
    int64_t v211;
    void* v480;
    int64_t v1090; /* placeholder for void */
    int64_t v1406; /* placeholder for void */
    int64_t v116;
    void* v264;
    void* v407;
    int64_t v1242; /* placeholder for void */
    int64_t v148;
    void* v1269;
    int64_t v361; /* placeholder for void */
    void* v65;
    void* v421;
    int64_t v494;
    int64_t v572; /* placeholder for void */
    int64_t v1387; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v205;
    void* v1326;
    void* v1233;
    int64_t v206;
    int64_t v224;
    int64_t v535;
    int64_t v720; /* placeholder for void */
    int64_t v760;
    void* v1146;
    int64_t v602;
    int64_t v368;
    void* v1365;
    void* v1123;
    void* v501;
    int64_t v612; /* placeholder for void */
    int64_t v136; /* placeholder for void */
    void* v961;
    void* v135;
    int64_t v107;
    int64_t v958;
    void* v693;
    void* v1128;
    void* v333;
    void* v1235;
    void* v177;
    void* v357;
    void* v565;
    int64_t v900; /* placeholder for void */
    int64_t v953;
    void* v453;
    void* v1317;
    int64_t v1416; /* placeholder for void */
    int64_t v1156;
    int64_t v1374; /* placeholder for void */
    void* v263;
    int64_t v8;
    void* v462;
    int64_t v773;
    int64_t v890;
    void* v927;
    int64_t v330; /* placeholder for void */
    int64_t v168; /* placeholder for void */
    int64_t v121;
    int64_t v384; /* placeholder for void */
    void* v448;
    int64_t v1025;
    int64_t v780; /* placeholder for void */
    void* v609;
    void* v1137;
    int64_t v161;
    int64_t v578; /* placeholder for void */
    int64_t v901; /* placeholder for void */
    int64_t v967;
    void* v871;
    void* v15;
    int64_t v1340;
    void* v813;
    void* v1411;
    int64_t v847; /* placeholder for void */
    int64_t v413;
    int64_t v344; /* placeholder for void */
    void* v285;
    bool v509;
    int64_t v287;
    void* v245;
    int64_t v325; /* placeholder for void */
    int64_t v328;
    int64_t v485;
    int64_t v518; /* placeholder for void */
    int64_t v1153; /* placeholder for void */
    void* v183;
    void* v1200;
    int64_t v956; /* placeholder for void */
    void* v1017;
    void* v1253;
    void* v1325;
    int64_t v820; /* placeholder for void */
    int64_t v1358;
    int64_t v193;
    void* v916;
    void* v1109;
    void* v295;
    void* v588;
    void* v403;
    void* v1119;
    void* v1038;
    bool v1157;
    int64_t v1404; /* placeholder for void */
    int64_t v1252; /* placeholder for void */
    int64_t v26;
    int64_t v568; /* placeholder for void */
    int64_t v109; /* placeholder for void */
    bool v599;
    int64_t v366; /* placeholder for void */
    int64_t v684; /* placeholder for void */
    int64_t v944;
    bool v995;
    void* v214;
    void* v663;
    void* v1051;
    void* v327;
    int64_t v1129;
    void* v915;
    int64_t v56; /* placeholder for void */
    int64_t v278;
    void* v466;
    int64_t v913;
    int64_t v505; /* placeholder for void */
    int64_t v1302; /* placeholder for void */
    int64_t v409;
    int64_t v553;
    void* v801;
    int64_t v882; /* placeholder for void */
    int64_t v380; /* placeholder for void */
    int64_t v846; /* placeholder for void */
    void* v1203;
    void* v1020;
    void* v291;
    int64_t v906; /* placeholder for void */
    int64_t v500; /* placeholder for void */
    int64_t v334; /* placeholder for void */
    void* v1245;
    void* v628;
    int64_t v1009; /* placeholder for void */
    int64_t v350;
    int64_t v666; /* placeholder for void */
    int64_t v751;
    void* v1290;
    int64_t v481;
    int64_t v706;
    void* v1339;
    int64_t v188;
    bool v329;
    void* v970;
    int64_t v199; /* placeholder for void */
    void* v681;
    int64_t v517;
    int64_t v1399;
    int64_t v692;
    void* v1181;
    void* v223;
    void* v443;
    int64_t v827;
    int64_t v234; /* placeholder for void */
    int64_t v936; /* placeholder for void */
    int64_t v992; /* placeholder for void */
    void* v282;
    int64_t v698; /* placeholder for void */
    int64_t v1034;
    void* v633;
    void* v1110;
    int64_t v1180; /* placeholder for void */
    int64_t v1419;
    void* v591;
    int64_t v730; /* placeholder for void */
    void* v717;
    void* v909;
    int64_t v1313;
    int64_t v1248; /* placeholder for void */
    int64_t v85;
    void* v1186;
    int64_t v94;
    int64_t v877;
    int64_t v1030;
    void* v331;
    int64_t v536; /* placeholder for void */
    int64_t v1126; /* placeholder for void */
    int64_t v1385;
    void* v783;
    int64_t v883; /* placeholder for void */
    int64_t v1007;
    int64_t v899;
    int64_t v1166; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    void* v142;
    int64_t v270; /* placeholder for void */
    void* v714;
    void* v745;
    void* v627;
    int64_t v614; /* placeholder for void */
    int64_t v503;
    int64_t v14; /* placeholder for void */
    void* v317;
    void* v555;
    bool v869;
    int64_t v942; /* placeholder for void */
    void* v771;
    int64_t v931;
    int64_t v954; /* placeholder for void */
    bool v527;
    int64_t v982; /* placeholder for void */
    int64_t v1045; /* placeholder for void */
    int64_t v1052;
    void* v907;
    void* v66;
    int64_t v460; /* placeholder for void */
    void* v1311;
    void* v700;
    int64_t v613; /* placeholder for void */
    void* v39;
    bool v59;
    void* v484;
    int64_t v370; /* placeholder for void */
    void* v711;
    int64_t v53;
    int64_t v728;
    int64_t v1106;
    int64_t v688;
    int64_t v752; /* placeholder for void */
    void* v1083;
    int64_t v910; /* placeholder for void */
    int64_t v1133;
    int64_t v364;
    int64_t v542; /* placeholder for void */
    int64_t v662; /* placeholder for void */
    void* v891;
    int64_t v1286;
    void* v147;
    void* v639;
    int64_t v1369; /* placeholder for void */
    void* v1347;
    int64_t v1256; /* placeholder for void */
    void* v155;
    int64_t v634;
    int64_t v126; /* placeholder for void */
    bool v797;
    int64_t v830; /* placeholder for void */
    void* v363;
    bool v887;
    void* v1323;
    bool v293;
    int64_t v230; /* placeholder for void */
    void* v133;
    void* v297;
    int64_t v748; /* placeholder for void */
    void* v63;
    int64_t v986; /* placeholder for void */
    int64_t v1097;
    int64_t v1280; /* placeholder for void */
    void* v1401;
    int64_t v314;
    void* v646;
    int64_t v996; /* placeholder for void */
    void* v237;
    void* v1150;
    void* v318;
    int64_t v1396; /* placeholder for void */
    int64_t v1351; /* placeholder for void */
    int64_t v1079;
    int64_t v541; /* placeholder for void */
    int64_t v190; /* placeholder for void */
    bool v1103;
    int64_t v559; /* placeholder for void */
    void* v322;
    void* v300;
    void* v493;
    int64_t v920; /* placeholder for void */
    void* v213;
    int64_t v463;
    int64_t v756; /* placeholder for void */
    int64_t v320; /* placeholder for void */
    bool v473;
    void* v12;
    int64_t v132; /* placeholder for void */
    void* v207;
    void* v687;
    int64_t v589;
    void* v1275;
    int64_t v1151;
    int64_t v451; /* placeholder for void */
    void* v1263;
    void* v412;
    void* v1037;
    int64_t v1189; /* placeholder for void */
    int64_t v194; /* placeholder for void */
    int64_t v620;
    void* v1155;
    bool v167;
    int64_t v469; /* placeholder for void */
    void* v677;
    void* v430;
    int64_t v710;
    int64_t v1072; /* placeholder for void */
    int64_t v1324; /* placeholder for void */
    int64_t v404;
    bool v239;
    int64_t v284; /* placeholder for void */
    void* v495;
    void* v1056;
    int64_t v1165;
    void* v678;
    int64_t v1084;
    int64_t v1292; /* placeholder for void */
    int64_t v1327;
    void* v520;
    int64_t v38; /* placeholder for void */
    int64_t v850;
    int64_t v806; /* placeholder for void */
    void* v1343;
    int64_t v521;
    int64_t v1390;
    int64_t v1021;
    int64_t v40;
    void* v335;
    int64_t v445;
    void* v461;
    int64_t v989;
    void* v33;
    void* v727;
    void* v111;
    void* v567;
    int64_t v1171; /* placeholder for void */
    void* v601;
    void* v1257;
    void* v137;
    void* v1179;
    int64_t v737;
    int64_t v1122; /* placeholder for void */
    void* v1397;
    int64_t v680; /* placeholder for void */
    int64_t v62;
    void* v619;
    void* v837;
    int64_t v598;
    void* v29;
    int64_t v1111;
    void* v304;
    int64_t v1338; /* placeholder for void */
    int64_t v566;
    int64_t v1154; /* placeholder for void */
    int64_t v1412; /* placeholder for void */
    int64_t v71;
    int64_t v288; /* placeholder for void */
    bool v23;
    int64_t v514; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v630; /* placeholder for void */
    void* v57;
    void* v669;
    int64_t v800;
    bool v941;
    int64_t v486; /* placeholder for void */
    bool v977;
    void* v1185;
    void* v570;
    int64_t v1262; /* placeholder for void */
    bool v1337;
    int64_t v1410; /* placeholder for void */
    int64_t v626; /* placeholder for void */
    int64_t v1291;
    void* v515;
    int64_t v73; /* placeholder for void */
    void* v1398;
    void* v88;
    void* v1041;
    void* v858;
    void* v387;
    int64_t v338; /* placeholder for void */
    int64_t v1112; /* placeholder for void */
    void* v102;
    int64_t v722; /* placeholder for void */
    void* v1195;
    int64_t v1220; /* placeholder for void */
    void* v1249;
    void* v1285;
    void* v1289;
    int64_t v324; /* placeholder for void */
    int64_t v686; /* placeholder for void */
    int64_t v222; /* placeholder for void */
    int64_t v251;
    int64_t v37; /* placeholder for void */
    int64_t v204; /* placeholder for void */
    int64_t v283;
    int64_t v1334; /* placeholder for void */
    int64_t v1162; /* placeholder for void */
    int64_t v1188; /* placeholder for void */
    bool v1283;
    int64_t v1043;
    void* v772;
    int64_t v1246;
    void* v1294;
    int64_t v446; /* placeholder for void */
    int64_t v1367;
    void* v979;
    int64_t v1212; /* placeholder for void */
    int64_t v296;
    int64_t v431;
    bool v275;
    int64_t v1081; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v1140; /* placeholder for void */
    void* v952;
    void* v1361;
    int64_t v1315; /* placeholder for void */
    void* v321;
    int64_t v440;
    void* v785;
    void* v156;
    int64_t v306; /* placeholder for void */
    int64_t v406; /* placeholder for void */
    void* v963;
    void* v623;
    int64_t v1224; /* placeholder for void */
    int64_t v1420; /* placeholder for void */
    int64_t v1332; /* placeholder for void */
    int64_t v670;
    int64_t v19; /* placeholder for void */
    int64_t v935;
    int64_t v926;
    void* v1344;
    int64_t v316; /* placeholder for void */
    int64_t v540; /* placeholder for void */
    void* v673;
    int64_t v1108; /* placeholder for void */
    void* v267;
    bool v851;
    int64_t v1382; /* placeholder for void */
    int64_t v332;
    int64_t v685; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v893;
    void* v651;
    int64_t v852; /* placeholder for void */
    int64_t v1176; /* placeholder for void */
    void* v165;
    int64_t v968; /* placeholder for void */
    int64_t v577; /* placeholder for void */
    int64_t v1183;
    void* v1209;
    void* v1227;
    int64_t v1318;
    bool v1319;
    void* v340;
    int64_t v864; /* placeholder for void */
    int64_t v1226; /* placeholder for void */
    void* v105;
    bool v455;
    int64_t v1054; /* placeholder for void */
    int64_t v544;
    int64_t v343; /* placeholder for void */
    void* v765;
    int64_t v611;
    void* v1125;
    void* v592;
    int64_t v886;
    int64_t v1300;
    int64_t v932; /* placeholder for void */
    void* v1357;
    int64_t v918; /* placeholder for void */
    int64_t v769;
    int64_t v310;
    void* v543;
    void* v675;
    void* v821;
    void* v1393;
    int64_t v103;
    void* v1141;
    int64_t v550; /* placeholder for void */
    int64_t v233;
    void* v129;
    int64_t v644; /* placeholder for void */
    int64_t v1116; /* placeholder for void */
    int64_t v1142;
    void* v11;
    int64_t v1170; /* placeholder for void */
    int64_t v593;
    void* v1380;
    int64_t v1044; /* placeholder for void */
    void* v349;
    void* v606;
    int64_t v762; /* placeholder for void */
    void* v513;
    bool v671;
    int64_t v1295;
    int64_t v506; /* placeholder for void */
    void* v372;
    int64_t v631; /* placeholder for void */
    void* v957;
    void* v1218;
    int64_t v792; /* placeholder for void */
    int64_t v35;
    int64_t v434; /* placeholder for void */
    int64_t v841;
    void* v1281;
    int64_t v1333; /* placeholder for void */
    int64_t v539;
    int64_t v625;
    int64_t v884; /* placeholder for void */
    void* v1006;
    int64_t v1093;
    int64_t v1238; /* placeholder for void */
    int64_t v1088;
    void* v843;
    void* v857;
    int64_t v1284; /* placeholder for void */
    void* v1149;
    void* v768;
    int64_t v793; /* placeholder for void */
    int64_t v1124;
    void* v117;
    void* v1413;
    int64_t v814;
    v3 = _str1;
    v4 = 0LL;
    v5 = (bool)str_equal((void*)v4, v2, v3);
    if (v5) goto bb1;
    else goto bb2;
  bb1:
    v7 = malloc(16);
    v8 = 0LL;
    v9 = (char*)v7 + 0;
    *(int64_t*)v9 = (int64_t)v8;
    v11 = (char*)v7 + 0;
    v12 = (char*)v0 + 0;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v7 + 8;
    v16 = (char*)v0 + 8;
    v17 = *(int64_t*)v15;
    *(int64_t*)v16 = (int64_t)v17;
    p_call_stack_pop();
    return;
  bb2:
    goto bb3;
  bb3:
    v21 = _str2;
    v22 = 0LL;
    v23 = (bool)str_equal((void*)v22, v2, v21);
    if (v23) goto bb4;
    else goto bb5;
  bb4:
    v25 = malloc(16);
    v26 = 1LL;
    v27 = (char*)v25 + 0;
    *(int64_t*)v27 = (int64_t)v26;
    v29 = (char*)v25 + 0;
    v30 = (char*)v0 + 0;
    v31 = *(int64_t*)v29;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = (char*)v25 + 8;
    v34 = (char*)v0 + 8;
    v35 = *(int64_t*)v33;
    *(int64_t*)v34 = (int64_t)v35;
    p_call_stack_pop();
    return;
  bb5:
    goto bb6;
  bb6:
    v39 = _str3;
    v40 = 0LL;
    v41 = (bool)str_equal((void*)v40, v2, v39);
    if (v41) goto bb7;
    else goto bb8;
  bb7:
    v43 = malloc(16);
    v44 = 2LL;
    v45 = (char*)v43 + 0;
    *(int64_t*)v45 = (int64_t)v44;
    v47 = (char*)v43 + 0;
    v48 = (char*)v0 + 0;
    v49 = *(int64_t*)v47;
    *(int64_t*)v48 = (int64_t)v49;
    v51 = (char*)v43 + 8;
    v52 = (char*)v0 + 8;
    v53 = *(int64_t*)v51;
    *(int64_t*)v52 = (int64_t)v53;
    p_call_stack_pop();
    return;
  bb8:
    goto bb9;
  bb9:
    v57 = _str4;
    v58 = 0LL;
    v59 = (bool)str_equal((void*)v58, v2, v57);
    if (v59) goto bb10;
    else goto bb11;
  bb10:
    v61 = malloc(16);
    v62 = 3LL;
    v63 = (char*)v61 + 0;
    *(int64_t*)v63 = (int64_t)v62;
    v65 = (char*)v61 + 0;
    v66 = (char*)v0 + 0;
    v67 = *(int64_t*)v65;
    *(int64_t*)v66 = (int64_t)v67;
    v69 = (char*)v61 + 8;
    v70 = (char*)v0 + 8;
    v71 = *(int64_t*)v69;
    *(int64_t*)v70 = (int64_t)v71;
    p_call_stack_pop();
    return;
  bb11:
    goto bb12;
  bb12:
    v75 = _str5;
    v76 = 0LL;
    v77 = (bool)str_equal((void*)v76, v2, v75);
    if (v77) goto bb13;
    else goto bb14;
  bb13:
    v79 = malloc(16);
    v80 = 4LL;
    v81 = (char*)v79 + 0;
    *(int64_t*)v81 = (int64_t)v80;
    v83 = (char*)v79 + 0;
    v84 = (char*)v0 + 0;
    v85 = *(int64_t*)v83;
    *(int64_t*)v84 = (int64_t)v85;
    v87 = (char*)v79 + 8;
    v88 = (char*)v0 + 8;
    v89 = *(int64_t*)v87;
    *(int64_t*)v88 = (int64_t)v89;
    p_call_stack_pop();
    return;
  bb14:
    goto bb15;
  bb15:
    v93 = _str6;
    v94 = 0LL;
    v95 = (bool)str_equal((void*)v94, v2, v93);
    if (v95) goto bb16;
    else goto bb17;
  bb16:
    v97 = malloc(16);
    v98 = 5LL;
    v99 = (char*)v97 + 0;
    *(int64_t*)v99 = (int64_t)v98;
    v101 = (char*)v97 + 0;
    v102 = (char*)v0 + 0;
    v103 = *(int64_t*)v101;
    *(int64_t*)v102 = (int64_t)v103;
    v105 = (char*)v97 + 8;
    v106 = (char*)v0 + 8;
    v107 = *(int64_t*)v105;
    *(int64_t*)v106 = (int64_t)v107;
    p_call_stack_pop();
    return;
  bb17:
    goto bb18;
  bb18:
    v111 = _str7;
    v112 = 0LL;
    v113 = (bool)str_equal((void*)v112, v2, v111);
    if (v113) goto bb19;
    else goto bb20;
  bb19:
    v115 = malloc(16);
    v116 = 6LL;
    v117 = (char*)v115 + 0;
    *(int64_t*)v117 = (int64_t)v116;
    v119 = (char*)v115 + 0;
    v120 = (char*)v0 + 0;
    v121 = *(int64_t*)v119;
    *(int64_t*)v120 = (int64_t)v121;
    v123 = (char*)v115 + 8;
    v124 = (char*)v0 + 8;
    v125 = *(int64_t*)v123;
    *(int64_t*)v124 = (int64_t)v125;
    p_call_stack_pop();
    return;
  bb20:
    goto bb21;
  bb21:
    v129 = _str8;
    v130 = 0LL;
    v131 = (bool)str_equal((void*)v130, v2, v129);
    if (v131) goto bb22;
    else goto bb23;
  bb22:
    v133 = malloc(16);
    v134 = 7LL;
    v135 = (char*)v133 + 0;
    *(int64_t*)v135 = (int64_t)v134;
    v137 = (char*)v133 + 0;
    v138 = (char*)v0 + 0;
    v139 = *(int64_t*)v137;
    *(int64_t*)v138 = (int64_t)v139;
    v141 = (char*)v133 + 8;
    v142 = (char*)v0 + 8;
    v143 = *(int64_t*)v141;
    *(int64_t*)v142 = (int64_t)v143;
    p_call_stack_pop();
    return;
  bb23:
    goto bb24;
  bb24:
    v147 = _str9;
    v148 = 0LL;
    v149 = (bool)str_equal((void*)v148, v2, v147);
    if (v149) goto bb25;
    else goto bb26;
  bb25:
    v151 = malloc(16);
    v152 = 8LL;
    v153 = (char*)v151 + 0;
    *(int64_t*)v153 = (int64_t)v152;
    v155 = (char*)v151 + 0;
    v156 = (char*)v0 + 0;
    v157 = *(int64_t*)v155;
    *(int64_t*)v156 = (int64_t)v157;
    v159 = (char*)v151 + 8;
    v160 = (char*)v0 + 8;
    v161 = *(int64_t*)v159;
    *(int64_t*)v160 = (int64_t)v161;
    p_call_stack_pop();
    return;
  bb26:
    goto bb27;
  bb27:
    v165 = _str10;
    v166 = 0LL;
    v167 = (bool)str_equal((void*)v166, v2, v165);
    if (v167) goto bb28;
    else goto bb29;
  bb28:
    v169 = malloc(16);
    v170 = 9LL;
    v171 = (char*)v169 + 0;
    *(int64_t*)v171 = (int64_t)v170;
    v173 = (char*)v169 + 0;
    v174 = (char*)v0 + 0;
    v175 = *(int64_t*)v173;
    *(int64_t*)v174 = (int64_t)v175;
    v177 = (char*)v169 + 8;
    v178 = (char*)v0 + 8;
    v179 = *(int64_t*)v177;
    *(int64_t*)v178 = (int64_t)v179;
    p_call_stack_pop();
    return;
  bb29:
    goto bb30;
  bb30:
    v183 = _str11;
    v184 = 0LL;
    v185 = (bool)str_equal((void*)v184, v2, v183);
    if (v185) goto bb31;
    else goto bb32;
  bb31:
    v187 = malloc(16);
    v188 = 10LL;
    v189 = (char*)v187 + 0;
    *(int64_t*)v189 = (int64_t)v188;
    v191 = (char*)v187 + 0;
    v192 = (char*)v0 + 0;
    v193 = *(int64_t*)v191;
    *(int64_t*)v192 = (int64_t)v193;
    v195 = (char*)v187 + 8;
    v196 = (char*)v0 + 8;
    v197 = *(int64_t*)v195;
    *(int64_t*)v196 = (int64_t)v197;
    p_call_stack_pop();
    return;
  bb32:
    goto bb33;
  bb33:
    v201 = _str12;
    v202 = 0LL;
    v203 = (bool)str_equal((void*)v202, v2, v201);
    if (v203) goto bb34;
    else goto bb35;
  bb34:
    v205 = malloc(16);
    v206 = 11LL;
    v207 = (char*)v205 + 0;
    *(int64_t*)v207 = (int64_t)v206;
    v209 = (char*)v205 + 0;
    v210 = (char*)v0 + 0;
    v211 = *(int64_t*)v209;
    *(int64_t*)v210 = (int64_t)v211;
    v213 = (char*)v205 + 8;
    v214 = (char*)v0 + 8;
    v215 = *(int64_t*)v213;
    *(int64_t*)v214 = (int64_t)v215;
    p_call_stack_pop();
    return;
  bb35:
    goto bb36;
  bb36:
    v219 = _str13;
    v220 = 0LL;
    v221 = (bool)str_equal((void*)v220, v2, v219);
    if (v221) goto bb37;
    else goto bb38;
  bb37:
    v223 = malloc(16);
    v224 = 12LL;
    v225 = (char*)v223 + 0;
    *(int64_t*)v225 = (int64_t)v224;
    v227 = (char*)v223 + 0;
    v228 = (char*)v0 + 0;
    v229 = *(int64_t*)v227;
    *(int64_t*)v228 = (int64_t)v229;
    v231 = (char*)v223 + 8;
    v232 = (char*)v0 + 8;
    v233 = *(int64_t*)v231;
    *(int64_t*)v232 = (int64_t)v233;
    p_call_stack_pop();
    return;
  bb38:
    goto bb39;
  bb39:
    v237 = _str14;
    v238 = 0LL;
    v239 = (bool)str_equal((void*)v238, v2, v237);
    if (v239) goto bb40;
    else goto bb41;
  bb40:
    v241 = malloc(16);
    v242 = 13LL;
    v243 = (char*)v241 + 0;
    *(int64_t*)v243 = (int64_t)v242;
    v245 = (char*)v241 + 0;
    v246 = (char*)v0 + 0;
    v247 = *(int64_t*)v245;
    *(int64_t*)v246 = (int64_t)v247;
    v249 = (char*)v241 + 8;
    v250 = (char*)v0 + 8;
    v251 = *(int64_t*)v249;
    *(int64_t*)v250 = (int64_t)v251;
    p_call_stack_pop();
    return;
  bb41:
    goto bb42;
  bb42:
    v255 = _str15;
    v256 = 0LL;
    v257 = (bool)str_equal((void*)v256, v2, v255);
    if (v257) goto bb43;
    else goto bb44;
  bb43:
    v259 = malloc(16);
    v260 = 14LL;
    v261 = (char*)v259 + 0;
    *(int64_t*)v261 = (int64_t)v260;
    v263 = (char*)v259 + 0;
    v264 = (char*)v0 + 0;
    v265 = *(int64_t*)v263;
    *(int64_t*)v264 = (int64_t)v265;
    v267 = (char*)v259 + 8;
    v268 = (char*)v0 + 8;
    v269 = *(int64_t*)v267;
    *(int64_t*)v268 = (int64_t)v269;
    p_call_stack_pop();
    return;
  bb44:
    goto bb45;
  bb45:
    v273 = _str16;
    v274 = 0LL;
    v275 = (bool)str_equal((void*)v274, v2, v273);
    if (v275) goto bb46;
    else goto bb47;
  bb46:
    v277 = malloc(16);
    v278 = 15LL;
    v279 = (char*)v277 + 0;
    *(int64_t*)v279 = (int64_t)v278;
    v281 = (char*)v277 + 0;
    v282 = (char*)v0 + 0;
    v283 = *(int64_t*)v281;
    *(int64_t*)v282 = (int64_t)v283;
    v285 = (char*)v277 + 8;
    v286 = (char*)v0 + 8;
    v287 = *(int64_t*)v285;
    *(int64_t*)v286 = (int64_t)v287;
    p_call_stack_pop();
    return;
  bb47:
    goto bb48;
  bb48:
    v291 = _str17;
    v292 = 0LL;
    v293 = (bool)str_equal((void*)v292, v2, v291);
    if (v293) goto bb49;
    else goto bb50;
  bb49:
    v295 = malloc(16);
    v296 = 16LL;
    v297 = (char*)v295 + 0;
    *(int64_t*)v297 = (int64_t)v296;
    v299 = (char*)v295 + 0;
    v300 = (char*)v0 + 0;
    v301 = *(int64_t*)v299;
    *(int64_t*)v300 = (int64_t)v301;
    v303 = (char*)v295 + 8;
    v304 = (char*)v0 + 8;
    v305 = *(int64_t*)v303;
    *(int64_t*)v304 = (int64_t)v305;
    p_call_stack_pop();
    return;
  bb50:
    goto bb51;
  bb51:
    v309 = _str18;
    v310 = 0LL;
    v311 = (bool)str_equal((void*)v310, v2, v309);
    if (v311) goto bb52;
    else goto bb53;
  bb52:
    v313 = malloc(16);
    v314 = 17LL;
    v315 = (char*)v313 + 0;
    *(int64_t*)v315 = (int64_t)v314;
    v317 = (char*)v313 + 0;
    v318 = (char*)v0 + 0;
    v319 = *(int64_t*)v317;
    *(int64_t*)v318 = (int64_t)v319;
    v321 = (char*)v313 + 8;
    v322 = (char*)v0 + 8;
    v323 = *(int64_t*)v321;
    *(int64_t*)v322 = (int64_t)v323;
    p_call_stack_pop();
    return;
  bb53:
    goto bb54;
  bb54:
    v327 = _str19;
    v328 = 0LL;
    v329 = (bool)str_equal((void*)v328, v2, v327);
    if (v329) goto bb55;
    else goto bb56;
  bb55:
    v331 = malloc(16);
    v332 = 18LL;
    v333 = (char*)v331 + 0;
    *(int64_t*)v333 = (int64_t)v332;
    v335 = (char*)v331 + 0;
    v336 = (char*)v0 + 0;
    v337 = *(int64_t*)v335;
    *(int64_t*)v336 = (int64_t)v337;
    v339 = (char*)v331 + 8;
    v340 = (char*)v0 + 8;
    v341 = *(int64_t*)v339;
    *(int64_t*)v340 = (int64_t)v341;
    p_call_stack_pop();
    return;
  bb56:
    goto bb57;
  bb57:
    v345 = _str20;
    v346 = 0LL;
    v347 = (bool)str_equal((void*)v346, v2, v345);
    if (v347) goto bb58;
    else goto bb59;
  bb58:
    v349 = malloc(16);
    v350 = 19LL;
    v351 = (char*)v349 + 0;
    *(int64_t*)v351 = (int64_t)v350;
    v353 = (char*)v349 + 0;
    v354 = (char*)v0 + 0;
    v355 = *(int64_t*)v353;
    *(int64_t*)v354 = (int64_t)v355;
    v357 = (char*)v349 + 8;
    v358 = (char*)v0 + 8;
    v359 = *(int64_t*)v357;
    *(int64_t*)v358 = (int64_t)v359;
    p_call_stack_pop();
    return;
  bb59:
    goto bb60;
  bb60:
    v363 = _str21;
    v364 = 0LL;
    v365 = (bool)str_equal((void*)v364, v2, v363);
    if (v365) goto bb61;
    else goto bb62;
  bb61:
    v367 = malloc(16);
    v368 = 20LL;
    v369 = (char*)v367 + 0;
    *(int64_t*)v369 = (int64_t)v368;
    v371 = (char*)v367 + 0;
    v372 = (char*)v0 + 0;
    v373 = *(int64_t*)v371;
    *(int64_t*)v372 = (int64_t)v373;
    v375 = (char*)v367 + 8;
    v376 = (char*)v0 + 8;
    v377 = *(int64_t*)v375;
    *(int64_t*)v376 = (int64_t)v377;
    p_call_stack_pop();
    return;
  bb62:
    goto bb63;
  bb63:
    v381 = _str22;
    v382 = 0LL;
    v383 = (bool)str_equal((void*)v382, v2, v381);
    if (v383) goto bb64;
    else goto bb65;
  bb64:
    v385 = malloc(16);
    v386 = 21LL;
    v387 = (char*)v385 + 0;
    *(int64_t*)v387 = (int64_t)v386;
    v389 = (char*)v385 + 0;
    v390 = (char*)v0 + 0;
    v391 = *(int64_t*)v389;
    *(int64_t*)v390 = (int64_t)v391;
    v393 = (char*)v385 + 8;
    v394 = (char*)v0 + 8;
    v395 = *(int64_t*)v393;
    *(int64_t*)v394 = (int64_t)v395;
    p_call_stack_pop();
    return;
  bb65:
    goto bb66;
  bb66:
    v399 = _str23;
    v400 = 0LL;
    v401 = (bool)str_equal((void*)v400, v2, v399);
    if (v401) goto bb67;
    else goto bb68;
  bb67:
    v403 = malloc(16);
    v404 = 22LL;
    v405 = (char*)v403 + 0;
    *(int64_t*)v405 = (int64_t)v404;
    v407 = (char*)v403 + 0;
    v408 = (char*)v0 + 0;
    v409 = *(int64_t*)v407;
    *(int64_t*)v408 = (int64_t)v409;
    v411 = (char*)v403 + 8;
    v412 = (char*)v0 + 8;
    v413 = *(int64_t*)v411;
    *(int64_t*)v412 = (int64_t)v413;
    p_call_stack_pop();
    return;
  bb68:
    goto bb69;
  bb69:
    v417 = _str24;
    v418 = 0LL;
    v419 = (bool)str_equal((void*)v418, v2, v417);
    if (v419) goto bb70;
    else goto bb71;
  bb70:
    v421 = malloc(16);
    v422 = 23LL;
    v423 = (char*)v421 + 0;
    *(int64_t*)v423 = (int64_t)v422;
    v425 = (char*)v421 + 0;
    v426 = (char*)v0 + 0;
    v427 = *(int64_t*)v425;
    *(int64_t*)v426 = (int64_t)v427;
    v429 = (char*)v421 + 8;
    v430 = (char*)v0 + 8;
    v431 = *(int64_t*)v429;
    *(int64_t*)v430 = (int64_t)v431;
    p_call_stack_pop();
    return;
  bb71:
    goto bb72;
  bb72:
    v435 = _str25;
    v436 = 0LL;
    v437 = (bool)str_equal((void*)v436, v2, v435);
    if (v437) goto bb73;
    else goto bb74;
  bb73:
    v439 = malloc(16);
    v440 = 24LL;
    v441 = (char*)v439 + 0;
    *(int64_t*)v441 = (int64_t)v440;
    v443 = (char*)v439 + 0;
    v444 = (char*)v0 + 0;
    v445 = *(int64_t*)v443;
    *(int64_t*)v444 = (int64_t)v445;
    v447 = (char*)v439 + 8;
    v448 = (char*)v0 + 8;
    v449 = *(int64_t*)v447;
    *(int64_t*)v448 = (int64_t)v449;
    p_call_stack_pop();
    return;
  bb74:
    goto bb75;
  bb75:
    v453 = _str26;
    v454 = 0LL;
    v455 = (bool)str_equal((void*)v454, v2, v453);
    if (v455) goto bb76;
    else goto bb77;
  bb76:
    v457 = malloc(16);
    v458 = 25LL;
    v459 = (char*)v457 + 0;
    *(int64_t*)v459 = (int64_t)v458;
    v461 = (char*)v457 + 0;
    v462 = (char*)v0 + 0;
    v463 = *(int64_t*)v461;
    *(int64_t*)v462 = (int64_t)v463;
    v465 = (char*)v457 + 8;
    v466 = (char*)v0 + 8;
    v467 = *(int64_t*)v465;
    *(int64_t*)v466 = (int64_t)v467;
    p_call_stack_pop();
    return;
  bb77:
    goto bb78;
  bb78:
    v471 = _str27;
    v472 = 0LL;
    v473 = (bool)str_equal((void*)v472, v2, v471);
    if (v473) goto bb79;
    else goto bb80;
  bb79:
    v475 = malloc(16);
    v476 = 26LL;
    v477 = (char*)v475 + 0;
    *(int64_t*)v477 = (int64_t)v476;
    v479 = (char*)v475 + 0;
    v480 = (char*)v0 + 0;
    v481 = *(int64_t*)v479;
    *(int64_t*)v480 = (int64_t)v481;
    v483 = (char*)v475 + 8;
    v484 = (char*)v0 + 8;
    v485 = *(int64_t*)v483;
    *(int64_t*)v484 = (int64_t)v485;
    p_call_stack_pop();
    return;
  bb80:
    goto bb81;
  bb81:
    v489 = _str28;
    v490 = 0LL;
    v491 = (bool)str_equal((void*)v490, v2, v489);
    if (v491) goto bb82;
    else goto bb83;
  bb82:
    v493 = malloc(16);
    v494 = 27LL;
    v495 = (char*)v493 + 0;
    *(int64_t*)v495 = (int64_t)v494;
    v497 = (char*)v493 + 0;
    v498 = (char*)v0 + 0;
    v499 = *(int64_t*)v497;
    *(int64_t*)v498 = (int64_t)v499;
    v501 = (char*)v493 + 8;
    v502 = (char*)v0 + 8;
    v503 = *(int64_t*)v501;
    *(int64_t*)v502 = (int64_t)v503;
    p_call_stack_pop();
    return;
  bb83:
    goto bb84;
  bb84:
    v507 = _str29;
    v508 = 0LL;
    v509 = (bool)str_equal((void*)v508, v2, v507);
    if (v509) goto bb85;
    else goto bb86;
  bb85:
    v511 = malloc(16);
    v512 = 28LL;
    v513 = (char*)v511 + 0;
    *(int64_t*)v513 = (int64_t)v512;
    v515 = (char*)v511 + 0;
    v516 = (char*)v0 + 0;
    v517 = *(int64_t*)v515;
    *(int64_t*)v516 = (int64_t)v517;
    v519 = (char*)v511 + 8;
    v520 = (char*)v0 + 8;
    v521 = *(int64_t*)v519;
    *(int64_t*)v520 = (int64_t)v521;
    p_call_stack_pop();
    return;
  bb86:
    goto bb87;
  bb87:
    v525 = _str30;
    v526 = 0LL;
    v527 = (bool)str_equal((void*)v526, v2, v525);
    if (v527) goto bb88;
    else goto bb89;
  bb88:
    v529 = malloc(16);
    v530 = 29LL;
    v531 = (char*)v529 + 0;
    *(int64_t*)v531 = (int64_t)v530;
    v533 = (char*)v529 + 0;
    v534 = (char*)v0 + 0;
    v535 = *(int64_t*)v533;
    *(int64_t*)v534 = (int64_t)v535;
    v537 = (char*)v529 + 8;
    v538 = (char*)v0 + 8;
    v539 = *(int64_t*)v537;
    *(int64_t*)v538 = (int64_t)v539;
    p_call_stack_pop();
    return;
  bb89:
    goto bb90;
  bb90:
    v543 = _str31;
    v544 = 0LL;
    v545 = (bool)str_equal((void*)v544, v2, v543);
    if (v545) goto bb91;
    else goto bb92;
  bb91:
    v547 = malloc(16);
    v548 = 30LL;
    v549 = (char*)v547 + 0;
    *(int64_t*)v549 = (int64_t)v548;
    v551 = (char*)v547 + 0;
    v552 = (char*)v0 + 0;
    v553 = *(int64_t*)v551;
    *(int64_t*)v552 = (int64_t)v553;
    v555 = (char*)v547 + 8;
    v556 = (char*)v0 + 8;
    v557 = *(int64_t*)v555;
    *(int64_t*)v556 = (int64_t)v557;
    p_call_stack_pop();
    return;
  bb92:
    goto bb93;
  bb93:
    v561 = _str32;
    v562 = 0LL;
    v563 = (bool)str_equal((void*)v562, v2, v561);
    if (v563) goto bb94;
    else goto bb95;
  bb94:
    v565 = malloc(16);
    v566 = 31LL;
    v567 = (char*)v565 + 0;
    *(int64_t*)v567 = (int64_t)v566;
    v569 = (char*)v565 + 0;
    v570 = (char*)v0 + 0;
    v571 = *(int64_t*)v569;
    *(int64_t*)v570 = (int64_t)v571;
    v573 = (char*)v565 + 8;
    v574 = (char*)v0 + 8;
    v575 = *(int64_t*)v573;
    *(int64_t*)v574 = (int64_t)v575;
    p_call_stack_pop();
    return;
  bb95:
    goto bb96;
  bb96:
    v579 = _str33;
    v580 = 0LL;
    v581 = (bool)str_equal((void*)v580, v2, v579);
    if (v581) goto bb97;
    else goto bb98;
  bb97:
    v583 = malloc(16);
    v584 = 32LL;
    v585 = (char*)v583 + 0;
    *(int64_t*)v585 = (int64_t)v584;
    v587 = (char*)v583 + 0;
    v588 = (char*)v0 + 0;
    v589 = *(int64_t*)v587;
    *(int64_t*)v588 = (int64_t)v589;
    v591 = (char*)v583 + 8;
    v592 = (char*)v0 + 8;
    v593 = *(int64_t*)v591;
    *(int64_t*)v592 = (int64_t)v593;
    p_call_stack_pop();
    return;
  bb98:
    goto bb99;
  bb99:
    v597 = _str34;
    v598 = 0LL;
    v599 = (bool)str_equal((void*)v598, v2, v597);
    if (v599) goto bb100;
    else goto bb101;
  bb100:
    v601 = malloc(16);
    v602 = 33LL;
    v603 = (char*)v601 + 0;
    *(int64_t*)v603 = (int64_t)v602;
    v605 = (char*)v601 + 0;
    v606 = (char*)v0 + 0;
    v607 = *(int64_t*)v605;
    *(int64_t*)v606 = (int64_t)v607;
    v609 = (char*)v601 + 8;
    v610 = (char*)v0 + 8;
    v611 = *(int64_t*)v609;
    *(int64_t*)v610 = (int64_t)v611;
    p_call_stack_pop();
    return;
  bb101:
    goto bb102;
  bb102:
    v615 = _str35;
    v616 = 0LL;
    v617 = (bool)str_equal((void*)v616, v2, v615);
    if (v617) goto bb103;
    else goto bb104;
  bb103:
    v619 = malloc(16);
    v620 = 34LL;
    v621 = (char*)v619 + 0;
    *(int64_t*)v621 = (int64_t)v620;
    v623 = (char*)v619 + 0;
    v624 = (char*)v0 + 0;
    v625 = *(int64_t*)v623;
    *(int64_t*)v624 = (int64_t)v625;
    v627 = (char*)v619 + 8;
    v628 = (char*)v0 + 8;
    v629 = *(int64_t*)v627;
    *(int64_t*)v628 = (int64_t)v629;
    p_call_stack_pop();
    return;
  bb104:
    goto bb105;
  bb105:
    v633 = _str36;
    v634 = 0LL;
    v635 = (bool)str_equal((void*)v634, v2, v633);
    if (v635) goto bb106;
    else goto bb107;
  bb106:
    v637 = malloc(16);
    v638 = 35LL;
    v639 = (char*)v637 + 0;
    *(int64_t*)v639 = (int64_t)v638;
    v641 = (char*)v637 + 0;
    v642 = (char*)v0 + 0;
    v643 = *(int64_t*)v641;
    *(int64_t*)v642 = (int64_t)v643;
    v645 = (char*)v637 + 8;
    v646 = (char*)v0 + 8;
    v647 = *(int64_t*)v645;
    *(int64_t*)v646 = (int64_t)v647;
    p_call_stack_pop();
    return;
  bb107:
    goto bb108;
  bb108:
    v651 = _str37;
    v652 = 0LL;
    v653 = (bool)str_equal((void*)v652, v2, v651);
    if (v653) goto bb109;
    else goto bb110;
  bb109:
    v655 = malloc(16);
    v656 = 36LL;
    v657 = (char*)v655 + 0;
    *(int64_t*)v657 = (int64_t)v656;
    v659 = (char*)v655 + 0;
    v660 = (char*)v0 + 0;
    v661 = *(int64_t*)v659;
    *(int64_t*)v660 = (int64_t)v661;
    v663 = (char*)v655 + 8;
    v664 = (char*)v0 + 8;
    v665 = *(int64_t*)v663;
    *(int64_t*)v664 = (int64_t)v665;
    p_call_stack_pop();
    return;
  bb110:
    goto bb111;
  bb111:
    v669 = _str38;
    v670 = 0LL;
    v671 = (bool)str_equal((void*)v670, v2, v669);
    if (v671) goto bb112;
    else goto bb113;
  bb112:
    v673 = malloc(16);
    v674 = 37LL;
    v675 = (char*)v673 + 0;
    *(int64_t*)v675 = (int64_t)v674;
    v677 = (char*)v673 + 0;
    v678 = (char*)v0 + 0;
    v679 = *(int64_t*)v677;
    *(int64_t*)v678 = (int64_t)v679;
    v681 = (char*)v673 + 8;
    v682 = (char*)v0 + 8;
    v683 = *(int64_t*)v681;
    *(int64_t*)v682 = (int64_t)v683;
    p_call_stack_pop();
    return;
  bb113:
    goto bb114;
  bb114:
    v687 = _str39;
    v688 = 0LL;
    v689 = (bool)str_equal((void*)v688, v2, v687);
    if (v689) goto bb115;
    else goto bb116;
  bb115:
    v691 = malloc(16);
    v692 = 38LL;
    v693 = (char*)v691 + 0;
    *(int64_t*)v693 = (int64_t)v692;
    v695 = (char*)v691 + 0;
    v696 = (char*)v0 + 0;
    v697 = *(int64_t*)v695;
    *(int64_t*)v696 = (int64_t)v697;
    v699 = (char*)v691 + 8;
    v700 = (char*)v0 + 8;
    v701 = *(int64_t*)v699;
    *(int64_t*)v700 = (int64_t)v701;
    p_call_stack_pop();
    return;
  bb116:
    goto bb117;
  bb117:
    v705 = _str40;
    v706 = 0LL;
    v707 = (bool)str_equal((void*)v706, v2, v705);
    if (v707) goto bb118;
    else goto bb119;
  bb118:
    v709 = malloc(16);
    v710 = 39LL;
    v711 = (char*)v709 + 0;
    *(int64_t*)v711 = (int64_t)v710;
    v713 = (char*)v709 + 0;
    v714 = (char*)v0 + 0;
    v715 = *(int64_t*)v713;
    *(int64_t*)v714 = (int64_t)v715;
    v717 = (char*)v709 + 8;
    v718 = (char*)v0 + 8;
    v719 = *(int64_t*)v717;
    *(int64_t*)v718 = (int64_t)v719;
    p_call_stack_pop();
    return;
  bb119:
    goto bb120;
  bb120:
    v723 = _str41;
    v724 = 0LL;
    v725 = (bool)str_equal((void*)v724, v2, v723);
    if (v725) goto bb121;
    else goto bb122;
  bb121:
    v727 = malloc(16);
    v728 = 40LL;
    v729 = (char*)v727 + 0;
    *(int64_t*)v729 = (int64_t)v728;
    v731 = (char*)v727 + 0;
    v732 = (char*)v0 + 0;
    v733 = *(int64_t*)v731;
    *(int64_t*)v732 = (int64_t)v733;
    v735 = (char*)v727 + 8;
    v736 = (char*)v0 + 8;
    v737 = *(int64_t*)v735;
    *(int64_t*)v736 = (int64_t)v737;
    p_call_stack_pop();
    return;
  bb122:
    goto bb123;
  bb123:
    v741 = _str42;
    v742 = 0LL;
    v743 = (bool)str_equal((void*)v742, v2, v741);
    if (v743) goto bb124;
    else goto bb125;
  bb124:
    v745 = malloc(16);
    v746 = 41LL;
    v747 = (char*)v745 + 0;
    *(int64_t*)v747 = (int64_t)v746;
    v749 = (char*)v745 + 0;
    v750 = (char*)v0 + 0;
    v751 = *(int64_t*)v749;
    *(int64_t*)v750 = (int64_t)v751;
    v753 = (char*)v745 + 8;
    v754 = (char*)v0 + 8;
    v755 = *(int64_t*)v753;
    *(int64_t*)v754 = (int64_t)v755;
    p_call_stack_pop();
    return;
  bb125:
    goto bb126;
  bb126:
    v759 = _str43;
    v760 = 0LL;
    v761 = (bool)str_equal((void*)v760, v2, v759);
    if (v761) goto bb127;
    else goto bb128;
  bb127:
    v763 = malloc(16);
    v764 = 42LL;
    v765 = (char*)v763 + 0;
    *(int64_t*)v765 = (int64_t)v764;
    v767 = (char*)v763 + 0;
    v768 = (char*)v0 + 0;
    v769 = *(int64_t*)v767;
    *(int64_t*)v768 = (int64_t)v769;
    v771 = (char*)v763 + 8;
    v772 = (char*)v0 + 8;
    v773 = *(int64_t*)v771;
    *(int64_t*)v772 = (int64_t)v773;
    p_call_stack_pop();
    return;
  bb128:
    goto bb129;
  bb129:
    v777 = _str44;
    v778 = 0LL;
    v779 = (bool)str_equal((void*)v778, v2, v777);
    if (v779) goto bb130;
    else goto bb131;
  bb130:
    v781 = malloc(16);
    v782 = 43LL;
    v783 = (char*)v781 + 0;
    *(int64_t*)v783 = (int64_t)v782;
    v785 = (char*)v781 + 0;
    v786 = (char*)v0 + 0;
    v787 = *(int64_t*)v785;
    *(int64_t*)v786 = (int64_t)v787;
    v789 = (char*)v781 + 8;
    v790 = (char*)v0 + 8;
    v791 = *(int64_t*)v789;
    *(int64_t*)v790 = (int64_t)v791;
    p_call_stack_pop();
    return;
  bb131:
    goto bb132;
  bb132:
    v795 = _str45;
    v796 = 0LL;
    v797 = (bool)str_equal((void*)v796, v2, v795);
    if (v797) goto bb133;
    else goto bb134;
  bb133:
    v799 = malloc(16);
    v800 = 44LL;
    v801 = (char*)v799 + 0;
    *(int64_t*)v801 = (int64_t)v800;
    v803 = (char*)v799 + 0;
    v804 = (char*)v0 + 0;
    v805 = *(int64_t*)v803;
    *(int64_t*)v804 = (int64_t)v805;
    v807 = (char*)v799 + 8;
    v808 = (char*)v0 + 8;
    v809 = *(int64_t*)v807;
    *(int64_t*)v808 = (int64_t)v809;
    p_call_stack_pop();
    return;
  bb134:
    goto bb135;
  bb135:
    v813 = _str46;
    v814 = 0LL;
    v815 = (bool)str_equal((void*)v814, v2, v813);
    if (v815) goto bb136;
    else goto bb137;
  bb136:
    v817 = malloc(16);
    v818 = 45LL;
    v819 = (char*)v817 + 0;
    *(int64_t*)v819 = (int64_t)v818;
    v821 = (char*)v817 + 0;
    v822 = (char*)v0 + 0;
    v823 = *(int64_t*)v821;
    *(int64_t*)v822 = (int64_t)v823;
    v825 = (char*)v817 + 8;
    v826 = (char*)v0 + 8;
    v827 = *(int64_t*)v825;
    *(int64_t*)v826 = (int64_t)v827;
    p_call_stack_pop();
    return;
  bb137:
    goto bb138;
  bb138:
    v831 = _str47;
    v832 = 0LL;
    v833 = (bool)str_equal((void*)v832, v2, v831);
    if (v833) goto bb139;
    else goto bb140;
  bb139:
    v835 = malloc(16);
    v836 = 46LL;
    v837 = (char*)v835 + 0;
    *(int64_t*)v837 = (int64_t)v836;
    v839 = (char*)v835 + 0;
    v840 = (char*)v0 + 0;
    v841 = *(int64_t*)v839;
    *(int64_t*)v840 = (int64_t)v841;
    v843 = (char*)v835 + 8;
    v844 = (char*)v0 + 8;
    v845 = *(int64_t*)v843;
    *(int64_t*)v844 = (int64_t)v845;
    p_call_stack_pop();
    return;
  bb140:
    goto bb141;
  bb141:
    v849 = _str48;
    v850 = 0LL;
    v851 = (bool)str_equal((void*)v850, v2, v849);
    if (v851) goto bb142;
    else goto bb143;
  bb142:
    v853 = malloc(16);
    v854 = 47LL;
    v855 = (char*)v853 + 0;
    *(int64_t*)v855 = (int64_t)v854;
    v857 = (char*)v853 + 0;
    v858 = (char*)v0 + 0;
    v859 = *(int64_t*)v857;
    *(int64_t*)v858 = (int64_t)v859;
    v861 = (char*)v853 + 8;
    v862 = (char*)v0 + 8;
    v863 = *(int64_t*)v861;
    *(int64_t*)v862 = (int64_t)v863;
    p_call_stack_pop();
    return;
  bb143:
    goto bb144;
  bb144:
    v867 = _str49;
    v868 = 0LL;
    v869 = (bool)str_equal((void*)v868, v2, v867);
    if (v869) goto bb145;
    else goto bb146;
  bb145:
    v871 = malloc(16);
    v872 = 48LL;
    v873 = (char*)v871 + 0;
    *(int64_t*)v873 = (int64_t)v872;
    v875 = (char*)v871 + 0;
    v876 = (char*)v0 + 0;
    v877 = *(int64_t*)v875;
    *(int64_t*)v876 = (int64_t)v877;
    v879 = (char*)v871 + 8;
    v880 = (char*)v0 + 8;
    v881 = *(int64_t*)v879;
    *(int64_t*)v880 = (int64_t)v881;
    p_call_stack_pop();
    return;
  bb146:
    goto bb147;
  bb147:
    v885 = _str50;
    v886 = 0LL;
    v887 = (bool)str_equal((void*)v886, v2, v885);
    if (v887) goto bb148;
    else goto bb149;
  bb148:
    v889 = malloc(16);
    v890 = 49LL;
    v891 = (char*)v889 + 0;
    *(int64_t*)v891 = (int64_t)v890;
    v893 = (char*)v889 + 0;
    v894 = (char*)v0 + 0;
    v895 = *(int64_t*)v893;
    *(int64_t*)v894 = (int64_t)v895;
    v897 = (char*)v889 + 8;
    v898 = (char*)v0 + 8;
    v899 = *(int64_t*)v897;
    *(int64_t*)v898 = (int64_t)v899;
    p_call_stack_pop();
    return;
  bb149:
    goto bb150;
  bb150:
    v903 = _str51;
    v904 = 0LL;
    v905 = (bool)str_equal((void*)v904, v2, v903);
    if (v905) goto bb151;
    else goto bb152;
  bb151:
    v907 = malloc(16);
    v908 = 50LL;
    v909 = (char*)v907 + 0;
    *(int64_t*)v909 = (int64_t)v908;
    v911 = (char*)v907 + 0;
    v912 = (char*)v0 + 0;
    v913 = *(int64_t*)v911;
    *(int64_t*)v912 = (int64_t)v913;
    v915 = (char*)v907 + 8;
    v916 = (char*)v0 + 8;
    v917 = *(int64_t*)v915;
    *(int64_t*)v916 = (int64_t)v917;
    p_call_stack_pop();
    return;
  bb152:
    goto bb153;
  bb153:
    v921 = _str52;
    v922 = 0LL;
    v923 = (bool)str_equal((void*)v922, v2, v921);
    if (v923) goto bb154;
    else goto bb155;
  bb154:
    v925 = malloc(16);
    v926 = 51LL;
    v927 = (char*)v925 + 0;
    *(int64_t*)v927 = (int64_t)v926;
    v929 = (char*)v925 + 0;
    v930 = (char*)v0 + 0;
    v931 = *(int64_t*)v929;
    *(int64_t*)v930 = (int64_t)v931;
    v933 = (char*)v925 + 8;
    v934 = (char*)v0 + 8;
    v935 = *(int64_t*)v933;
    *(int64_t*)v934 = (int64_t)v935;
    p_call_stack_pop();
    return;
  bb155:
    goto bb156;
  bb156:
    v939 = _str53;
    v940 = 0LL;
    v941 = (bool)str_equal((void*)v940, v2, v939);
    if (v941) goto bb157;
    else goto bb158;
  bb157:
    v943 = malloc(16);
    v944 = 52LL;
    v945 = (char*)v943 + 0;
    *(int64_t*)v945 = (int64_t)v944;
    v947 = (char*)v943 + 0;
    v948 = (char*)v0 + 0;
    v949 = *(int64_t*)v947;
    *(int64_t*)v948 = (int64_t)v949;
    v951 = (char*)v943 + 8;
    v952 = (char*)v0 + 8;
    v953 = *(int64_t*)v951;
    *(int64_t*)v952 = (int64_t)v953;
    p_call_stack_pop();
    return;
  bb158:
    goto bb159;
  bb159:
    v957 = _str54;
    v958 = 0LL;
    v959 = (bool)str_equal((void*)v958, v2, v957);
    if (v959) goto bb160;
    else goto bb161;
  bb160:
    v961 = malloc(16);
    v962 = 53LL;
    v963 = (char*)v961 + 0;
    *(int64_t*)v963 = (int64_t)v962;
    v965 = (char*)v961 + 0;
    v966 = (char*)v0 + 0;
    v967 = *(int64_t*)v965;
    *(int64_t*)v966 = (int64_t)v967;
    v969 = (char*)v961 + 8;
    v970 = (char*)v0 + 8;
    v971 = *(int64_t*)v969;
    *(int64_t*)v970 = (int64_t)v971;
    p_call_stack_pop();
    return;
  bb161:
    goto bb162;
  bb162:
    v975 = _str55;
    v976 = 0LL;
    v977 = (bool)str_equal((void*)v976, v2, v975);
    if (v977) goto bb163;
    else goto bb164;
  bb163:
    v979 = malloc(16);
    v980 = 54LL;
    v981 = (char*)v979 + 0;
    *(int64_t*)v981 = (int64_t)v980;
    v983 = (char*)v979 + 0;
    v984 = (char*)v0 + 0;
    v985 = *(int64_t*)v983;
    *(int64_t*)v984 = (int64_t)v985;
    v987 = (char*)v979 + 8;
    v988 = (char*)v0 + 8;
    v989 = *(int64_t*)v987;
    *(int64_t*)v988 = (int64_t)v989;
    p_call_stack_pop();
    return;
  bb164:
    goto bb165;
  bb165:
    v993 = _str56;
    v994 = 0LL;
    v995 = (bool)str_equal((void*)v994, v2, v993);
    if (v995) goto bb166;
    else goto bb167;
  bb166:
    v997 = malloc(16);
    v998 = 55LL;
    v999 = (char*)v997 + 0;
    *(int64_t*)v999 = (int64_t)v998;
    v1001 = (char*)v997 + 0;
    v1002 = (char*)v0 + 0;
    v1003 = *(int64_t*)v1001;
    *(int64_t*)v1002 = (int64_t)v1003;
    v1005 = (char*)v997 + 8;
    v1006 = (char*)v0 + 8;
    v1007 = *(int64_t*)v1005;
    *(int64_t*)v1006 = (int64_t)v1007;
    p_call_stack_pop();
    return;
  bb167:
    goto bb168;
  bb168:
    v1011 = _str57;
    v1012 = 0LL;
    v1013 = (bool)str_equal((void*)v1012, v2, v1011);
    if (v1013) goto bb169;
    else goto bb170;
  bb169:
    v1015 = malloc(16);
    v1016 = 56LL;
    v1017 = (char*)v1015 + 0;
    *(int64_t*)v1017 = (int64_t)v1016;
    v1019 = (char*)v1015 + 0;
    v1020 = (char*)v0 + 0;
    v1021 = *(int64_t*)v1019;
    *(int64_t*)v1020 = (int64_t)v1021;
    v1023 = (char*)v1015 + 8;
    v1024 = (char*)v0 + 8;
    v1025 = *(int64_t*)v1023;
    *(int64_t*)v1024 = (int64_t)v1025;
    p_call_stack_pop();
    return;
  bb170:
    goto bb171;
  bb171:
    v1029 = _str58;
    v1030 = 0LL;
    v1031 = (bool)str_equal((void*)v1030, v2, v1029);
    if (v1031) goto bb172;
    else goto bb173;
  bb172:
    v1033 = malloc(16);
    v1034 = 57LL;
    v1035 = (char*)v1033 + 0;
    *(int64_t*)v1035 = (int64_t)v1034;
    v1037 = (char*)v1033 + 0;
    v1038 = (char*)v0 + 0;
    v1039 = *(int64_t*)v1037;
    *(int64_t*)v1038 = (int64_t)v1039;
    v1041 = (char*)v1033 + 8;
    v1042 = (char*)v0 + 8;
    v1043 = *(int64_t*)v1041;
    *(int64_t*)v1042 = (int64_t)v1043;
    p_call_stack_pop();
    return;
  bb173:
    goto bb174;
  bb174:
    v1047 = _str59;
    v1048 = 0LL;
    v1049 = (bool)str_equal((void*)v1048, v2, v1047);
    if (v1049) goto bb175;
    else goto bb176;
  bb175:
    v1051 = malloc(16);
    v1052 = 58LL;
    v1053 = (char*)v1051 + 0;
    *(int64_t*)v1053 = (int64_t)v1052;
    v1055 = (char*)v1051 + 0;
    v1056 = (char*)v0 + 0;
    v1057 = *(int64_t*)v1055;
    *(int64_t*)v1056 = (int64_t)v1057;
    v1059 = (char*)v1051 + 8;
    v1060 = (char*)v0 + 8;
    v1061 = *(int64_t*)v1059;
    *(int64_t*)v1060 = (int64_t)v1061;
    p_call_stack_pop();
    return;
  bb176:
    goto bb177;
  bb177:
    v1065 = _str60;
    v1066 = 0LL;
    v1067 = (bool)str_equal((void*)v1066, v2, v1065);
    if (v1067) goto bb178;
    else goto bb179;
  bb178:
    v1069 = malloc(16);
    v1070 = 59LL;
    v1071 = (char*)v1069 + 0;
    *(int64_t*)v1071 = (int64_t)v1070;
    v1073 = (char*)v1069 + 0;
    v1074 = (char*)v0 + 0;
    v1075 = *(int64_t*)v1073;
    *(int64_t*)v1074 = (int64_t)v1075;
    v1077 = (char*)v1069 + 8;
    v1078 = (char*)v0 + 8;
    v1079 = *(int64_t*)v1077;
    *(int64_t*)v1078 = (int64_t)v1079;
    p_call_stack_pop();
    return;
  bb179:
    goto bb180;
  bb180:
    v1083 = _str61;
    v1084 = 0LL;
    v1085 = (bool)str_equal((void*)v1084, v2, v1083);
    if (v1085) goto bb181;
    else goto bb182;
  bb181:
    v1087 = malloc(16);
    v1088 = 60LL;
    v1089 = (char*)v1087 + 0;
    *(int64_t*)v1089 = (int64_t)v1088;
    v1091 = (char*)v1087 + 0;
    v1092 = (char*)v0 + 0;
    v1093 = *(int64_t*)v1091;
    *(int64_t*)v1092 = (int64_t)v1093;
    v1095 = (char*)v1087 + 8;
    v1096 = (char*)v0 + 8;
    v1097 = *(int64_t*)v1095;
    *(int64_t*)v1096 = (int64_t)v1097;
    p_call_stack_pop();
    return;
  bb182:
    goto bb183;
  bb183:
    v1101 = _str160;
    v1102 = 0LL;
    v1103 = (bool)str_equal((void*)v1102, v2, v1101);
    if (v1103) goto bb184;
    else goto bb185;
  bb184:
    v1105 = malloc(16);
    v1106 = 61LL;
    v1107 = (char*)v1105 + 0;
    *(int64_t*)v1107 = (int64_t)v1106;
    v1109 = (char*)v1105 + 0;
    v1110 = (char*)v0 + 0;
    v1111 = *(int64_t*)v1109;
    *(int64_t*)v1110 = (int64_t)v1111;
    v1113 = (char*)v1105 + 8;
    v1114 = (char*)v0 + 8;
    v1115 = *(int64_t*)v1113;
    *(int64_t*)v1114 = (int64_t)v1115;
    p_call_stack_pop();
    return;
  bb185:
    goto bb186;
  bb186:
    v1119 = _str161;
    v1120 = 0LL;
    v1121 = (bool)str_equal((void*)v1120, v2, v1119);
    if (v1121) goto bb187;
    else goto bb188;
  bb187:
    v1123 = malloc(16);
    v1124 = 61LL;
    v1125 = (char*)v1123 + 0;
    *(int64_t*)v1125 = (int64_t)v1124;
    v1127 = (char*)v1123 + 0;
    v1128 = (char*)v0 + 0;
    v1129 = *(int64_t*)v1127;
    *(int64_t*)v1128 = (int64_t)v1129;
    v1131 = (char*)v1123 + 8;
    v1132 = (char*)v0 + 8;
    v1133 = *(int64_t*)v1131;
    *(int64_t*)v1132 = (int64_t)v1133;
    p_call_stack_pop();
    return;
  bb188:
    goto bb189;
  bb189:
    v1137 = _str162;
    v1138 = 0LL;
    v1139 = (bool)str_equal((void*)v1138, v2, v1137);
    if (v1139) goto bb190;
    else goto bb191;
  bb190:
    v1141 = malloc(16);
    v1142 = 61LL;
    v1143 = (char*)v1141 + 0;
    *(int64_t*)v1143 = (int64_t)v1142;
    v1145 = (char*)v1141 + 0;
    v1146 = (char*)v0 + 0;
    v1147 = *(int64_t*)v1145;
    *(int64_t*)v1146 = (int64_t)v1147;
    v1149 = (char*)v1141 + 8;
    v1150 = (char*)v0 + 8;
    v1151 = *(int64_t*)v1149;
    *(int64_t*)v1150 = (int64_t)v1151;
    p_call_stack_pop();
    return;
  bb191:
    goto bb192;
  bb192:
    v1155 = _str163;
    v1156 = 0LL;
    v1157 = (bool)str_equal((void*)v1156, v2, v1155);
    if (v1157) goto bb193;
    else goto bb194;
  bb193:
    v1159 = malloc(16);
    v1160 = 62LL;
    v1161 = (char*)v1159 + 0;
    *(int64_t*)v1161 = (int64_t)v1160;
    v1163 = (char*)v1159 + 0;
    v1164 = (char*)v0 + 0;
    v1165 = *(int64_t*)v1163;
    *(int64_t*)v1164 = (int64_t)v1165;
    v1167 = (char*)v1159 + 8;
    v1168 = (char*)v0 + 8;
    v1169 = *(int64_t*)v1167;
    *(int64_t*)v1168 = (int64_t)v1169;
    p_call_stack_pop();
    return;
  bb194:
    goto bb195;
  bb195:
    v1173 = _str164;
    v1174 = 0LL;
    v1175 = (bool)str_equal((void*)v1174, v2, v1173);
    if (v1175) goto bb196;
    else goto bb197;
  bb196:
    v1177 = malloc(16);
    v1178 = 63LL;
    v1179 = (char*)v1177 + 0;
    *(int64_t*)v1179 = (int64_t)v1178;
    v1181 = (char*)v1177 + 0;
    v1182 = (char*)v0 + 0;
    v1183 = *(int64_t*)v1181;
    *(int64_t*)v1182 = (int64_t)v1183;
    v1185 = (char*)v1177 + 8;
    v1186 = (char*)v0 + 8;
    v1187 = *(int64_t*)v1185;
    *(int64_t*)v1186 = (int64_t)v1187;
    p_call_stack_pop();
    return;
  bb197:
    goto bb198;
  bb198:
    v1191 = _str165;
    v1192 = 0LL;
    v1193 = (bool)str_equal((void*)v1192, v2, v1191);
    if (v1193) goto bb199;
    else goto bb200;
  bb199:
    v1195 = malloc(16);
    v1196 = 64LL;
    v1197 = (char*)v1195 + 0;
    *(int64_t*)v1197 = (int64_t)v1196;
    v1199 = (char*)v1195 + 0;
    v1200 = (char*)v0 + 0;
    v1201 = *(int64_t*)v1199;
    *(int64_t*)v1200 = (int64_t)v1201;
    v1203 = (char*)v1195 + 8;
    v1204 = (char*)v0 + 8;
    v1205 = *(int64_t*)v1203;
    *(int64_t*)v1204 = (int64_t)v1205;
    p_call_stack_pop();
    return;
  bb200:
    goto bb201;
  bb201:
    v1209 = _str166;
    v1210 = 0LL;
    v1211 = (bool)str_equal((void*)v1210, v2, v1209);
    if (v1211) goto bb202;
    else goto bb203;
  bb202:
    v1213 = malloc(16);
    v1214 = 65LL;
    v1215 = (char*)v1213 + 0;
    *(int64_t*)v1215 = (int64_t)v1214;
    v1217 = (char*)v1213 + 0;
    v1218 = (char*)v0 + 0;
    v1219 = *(int64_t*)v1217;
    *(int64_t*)v1218 = (int64_t)v1219;
    v1221 = (char*)v1213 + 8;
    v1222 = (char*)v0 + 8;
    v1223 = *(int64_t*)v1221;
    *(int64_t*)v1222 = (int64_t)v1223;
    p_call_stack_pop();
    return;
  bb203:
    goto bb204;
  bb204:
    v1227 = _str167;
    v1228 = 0LL;
    v1229 = (bool)str_equal((void*)v1228, v2, v1227);
    if (v1229) goto bb205;
    else goto bb206;
  bb205:
    v1231 = malloc(16);
    v1232 = 66LL;
    v1233 = (char*)v1231 + 0;
    *(int64_t*)v1233 = (int64_t)v1232;
    v1235 = (char*)v1231 + 0;
    v1236 = (char*)v0 + 0;
    v1237 = *(int64_t*)v1235;
    *(int64_t*)v1236 = (int64_t)v1237;
    v1239 = (char*)v1231 + 8;
    v1240 = (char*)v0 + 8;
    v1241 = *(int64_t*)v1239;
    *(int64_t*)v1240 = (int64_t)v1241;
    p_call_stack_pop();
    return;
  bb206:
    goto bb207;
  bb207:
    v1245 = _str168;
    v1246 = 0LL;
    v1247 = (bool)str_equal((void*)v1246, v2, v1245);
    if (v1247) goto bb208;
    else goto bb209;
  bb208:
    v1249 = malloc(16);
    v1250 = 67LL;
    v1251 = (char*)v1249 + 0;
    *(int64_t*)v1251 = (int64_t)v1250;
    v1253 = (char*)v1249 + 0;
    v1254 = (char*)v0 + 0;
    v1255 = *(int64_t*)v1253;
    *(int64_t*)v1254 = (int64_t)v1255;
    v1257 = (char*)v1249 + 8;
    v1258 = (char*)v0 + 8;
    v1259 = *(int64_t*)v1257;
    *(int64_t*)v1258 = (int64_t)v1259;
    p_call_stack_pop();
    return;
  bb209:
    goto bb210;
  bb210:
    v1263 = _str169;
    v1264 = 0LL;
    v1265 = (bool)str_equal((void*)v1264, v2, v1263);
    if (v1265) goto bb211;
    else goto bb212;
  bb211:
    v1267 = malloc(16);
    v1268 = 68LL;
    v1269 = (char*)v1267 + 0;
    *(int64_t*)v1269 = (int64_t)v1268;
    v1271 = (char*)v1267 + 0;
    v1272 = (char*)v0 + 0;
    v1273 = *(int64_t*)v1271;
    *(int64_t*)v1272 = (int64_t)v1273;
    v1275 = (char*)v1267 + 8;
    v1276 = (char*)v0 + 8;
    v1277 = *(int64_t*)v1275;
    *(int64_t*)v1276 = (int64_t)v1277;
    p_call_stack_pop();
    return;
  bb212:
    goto bb213;
  bb213:
    v1281 = _str170;
    v1282 = 0LL;
    v1283 = (bool)str_equal((void*)v1282, v2, v1281);
    if (v1283) goto bb214;
    else goto bb215;
  bb214:
    v1285 = malloc(16);
    v1286 = 69LL;
    v1287 = (char*)v1285 + 0;
    *(int64_t*)v1287 = (int64_t)v1286;
    v1289 = (char*)v1285 + 0;
    v1290 = (char*)v0 + 0;
    v1291 = *(int64_t*)v1289;
    *(int64_t*)v1290 = (int64_t)v1291;
    v1293 = (char*)v1285 + 8;
    v1294 = (char*)v0 + 8;
    v1295 = *(int64_t*)v1293;
    *(int64_t*)v1294 = (int64_t)v1295;
    p_call_stack_pop();
    return;
  bb215:
    goto bb216;
  bb216:
    v1299 = _str171;
    v1300 = 0LL;
    v1301 = (bool)str_equal((void*)v1300, v2, v1299);
    if (v1301) goto bb217;
    else goto bb218;
  bb217:
    v1303 = malloc(16);
    v1304 = 70LL;
    v1305 = (char*)v1303 + 0;
    *(int64_t*)v1305 = (int64_t)v1304;
    v1307 = (char*)v1303 + 0;
    v1308 = (char*)v0 + 0;
    v1309 = *(int64_t*)v1307;
    *(int64_t*)v1308 = (int64_t)v1309;
    v1311 = (char*)v1303 + 8;
    v1312 = (char*)v0 + 8;
    v1313 = *(int64_t*)v1311;
    *(int64_t*)v1312 = (int64_t)v1313;
    p_call_stack_pop();
    return;
  bb218:
    goto bb219;
  bb219:
    v1317 = _str172;
    v1318 = 0LL;
    v1319 = (bool)str_equal((void*)v1318, v2, v1317);
    if (v1319) goto bb220;
    else goto bb221;
  bb220:
    v1321 = malloc(16);
    v1322 = 71LL;
    v1323 = (char*)v1321 + 0;
    *(int64_t*)v1323 = (int64_t)v1322;
    v1325 = (char*)v1321 + 0;
    v1326 = (char*)v0 + 0;
    v1327 = *(int64_t*)v1325;
    *(int64_t*)v1326 = (int64_t)v1327;
    v1329 = (char*)v1321 + 8;
    v1330 = (char*)v0 + 8;
    v1331 = *(int64_t*)v1329;
    *(int64_t*)v1330 = (int64_t)v1331;
    p_call_stack_pop();
    return;
  bb221:
    goto bb222;
  bb222:
    v1335 = _str173;
    v1336 = 0LL;
    v1337 = (bool)str_equal((void*)v1336, v2, v1335);
    if (v1337) goto bb223;
    else goto bb224;
  bb223:
    v1339 = malloc(16);
    v1340 = 72LL;
    v1341 = (char*)v1339 + 0;
    *(int64_t*)v1341 = (int64_t)v1340;
    v1343 = (char*)v1339 + 0;
    v1344 = (char*)v0 + 0;
    v1345 = *(int64_t*)v1343;
    *(int64_t*)v1344 = (int64_t)v1345;
    v1347 = (char*)v1339 + 8;
    v1348 = (char*)v0 + 8;
    v1349 = *(int64_t*)v1347;
    *(int64_t*)v1348 = (int64_t)v1349;
    p_call_stack_pop();
    return;
  bb224:
    goto bb225;
  bb225:
    v1353 = _str174;
    v1354 = 0LL;
    v1355 = (bool)str_equal((void*)v1354, v2, v1353);
    if (v1355) goto bb226;
    else goto bb227;
  bb226:
    v1357 = malloc(16);
    v1358 = 73LL;
    v1359 = (char*)v1357 + 0;
    *(int64_t*)v1359 = (int64_t)v1358;
    v1361 = (char*)v1357 + 0;
    v1362 = (char*)v0 + 0;
    v1363 = *(int64_t*)v1361;
    *(int64_t*)v1362 = (int64_t)v1363;
    v1365 = (char*)v1357 + 8;
    v1366 = (char*)v0 + 8;
    v1367 = *(int64_t*)v1365;
    *(int64_t*)v1366 = (int64_t)v1367;
    p_call_stack_pop();
    return;
  bb227:
    goto bb228;
  bb228:
    v1371 = _str175;
    v1372 = 0LL;
    v1373 = (bool)str_equal((void*)v1372, v2, v1371);
    if (v1373) goto bb229;
    else goto bb230;
  bb229:
    v1375 = malloc(16);
    v1376 = 74LL;
    v1377 = (char*)v1375 + 0;
    *(int64_t*)v1377 = (int64_t)v1376;
    v1379 = (char*)v1375 + 0;
    v1380 = (char*)v0 + 0;
    v1381 = *(int64_t*)v1379;
    *(int64_t*)v1380 = (int64_t)v1381;
    v1383 = (char*)v1375 + 8;
    v1384 = (char*)v0 + 8;
    v1385 = *(int64_t*)v1383;
    *(int64_t*)v1384 = (int64_t)v1385;
    p_call_stack_pop();
    return;
  bb230:
    goto bb231;
  bb231:
    v1389 = _str176;
    v1390 = 0LL;
    v1391 = (bool)str_equal((void*)v1390, v2, v1389);
    if (v1391) goto bb232;
    else goto bb233;
  bb232:
    v1393 = malloc(16);
    v1394 = 75LL;
    v1395 = (char*)v1393 + 0;
    *(int64_t*)v1395 = (int64_t)v1394;
    v1397 = (char*)v1393 + 0;
    v1398 = (char*)v0 + 0;
    v1399 = *(int64_t*)v1397;
    *(int64_t*)v1398 = (int64_t)v1399;
    v1401 = (char*)v1393 + 8;
    v1402 = (char*)v0 + 8;
    v1403 = *(int64_t*)v1401;
    *(int64_t*)v1402 = (int64_t)v1403;
    p_call_stack_pop();
    return;
  bb233:
    goto bb234;
  bb234:
    v1407 = malloc(16);
    v1408 = 80LL;
    v1409 = (char*)v1407 + 0;
    *(int64_t*)v1409 = (int64_t)v1408;
    v1411 = (char*)v1407 + 8;
    *(void**)v1411 = (void*)v2;
    v1413 = (char*)v1407 + 0;
    v1414 = (char*)v0 + 0;
    v1415 = *(int64_t*)v1413;
    *(int64_t*)v1414 = (int64_t)v1415;
    v1417 = (char*)v1407 + 8;
    v1418 = (char*)v0 + 8;
    v1419 = *(int64_t*)v1417;
    *(int64_t*)v1418 = (int64_t)v1419;
    p_call_stack_pop();
    return;
    return;
}

int64_t String_char_count(void* v0, int64_t v1) {
    p_call_stack_push("String_char_count");
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v3;
    v2 = 0LL;
    v3 = (int64_t)p_str_length((void*)v2, v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

int64_t str_rfind(void* v0, void* v1, void* v2) {
    p_call_stack_push("str_rfind");
    int64_t v5; /* placeholder for void */
    int64_t v3;
    int64_t v4;
    v3 = 0LL;
    v4 = (int64_t)String_rfind((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return v4;
    return 0;
}

void* str_trim_start(void* v0, void* v1) {
    p_call_stack_push("str_trim_start");
    int64_t v49;
    int64_t v18;
    int64_t v24; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v11;
    int64_t v2;
    void* v4;
    int64_t v10;
    int64_t v29;
    int64_t v30;
    int64_t v8; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v36;
    void* v16;
    int64_t v37;
    int64_t v46; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v41;
    int64_t v13; /* placeholder for void */
    void* v19;
    int64_t v21;
    int64_t v28; /* placeholder for void */
    bool v31;
    int64_t v34; /* placeholder for void */
    int64_t v38;
    int64_t v48;
    int64_t v6;
    int64_t v50;
    int64_t v51;
    int64_t v15;
    int64_t v40; /* placeholder for void */
    void* v7;
    void* v25;
    void* v45;
    void* v22;
    int64_t v33; /* placeholder for void */
    int64_t v3;
    int64_t v44; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v42;
    int64_t v23; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v14;
    int64_t v17; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    uint8_t v26;
    int64_t v32;
    int64_t v27; /* placeholder for void */
    bool v12;
    bool v43;
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
    v45 = _str137;
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

void token_new(void* v0, void* v1, void* v2, int64_t v3, int64_t v4) {
    p_call_stack_push("token_new");
    int64_t v37; /* placeholder for void */
    void* v26;
    int64_t v10; /* placeholder for void */
    int64_t v31;
    void* v34;
    void* v29;
    int64_t v16; /* placeholder for void */
    void* v8;
    void* v11;
    void* v21;
    int64_t v35;
    void* v20;
    void* v12;
    int64_t v23;
    int64_t v9;
    int64_t v24; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v17;
    int64_t v32; /* placeholder for void */
    void* v33;
    int64_t v36; /* placeholder for void */
    void* v15;
    int64_t v13;
    int64_t v27;
    void* v7;
    void* v25;
    int64_t v28; /* placeholder for void */
    void* v22;
    void* v30;
    void* v5;
    void* v6;
    void* v19;
    int64_t v18; /* placeholder for void */
    v5 = malloc(32);
    v6 = (char*)v5 + 0;
    v7 = (char*)v2 + 0;
    v8 = (char*)v6 + 0;
    v9 = *(int64_t*)v7;
    *(int64_t*)v8 = (int64_t)v9;
    v11 = (char*)v2 + 8;
    v12 = (char*)v6 + 8;
    v13 = *(int64_t*)v11;
    *(int64_t*)v12 = (int64_t)v13;
    v15 = (char*)v5 + 16;
    *(int64_t*)v15 = (int64_t)v3;
    v17 = (char*)v5 + 24;
    *(int64_t*)v17 = (int64_t)v4;
    v19 = (char*)v5 + 0;
    v20 = (char*)v0 + 0;
    v21 = (char*)v19 + 0;
    v22 = (char*)v20 + 0;
    v23 = *(int64_t*)v21;
    *(int64_t*)v22 = (int64_t)v23;
    v25 = (char*)v19 + 8;
    v26 = (char*)v20 + 8;
    v27 = *(int64_t*)v25;
    *(int64_t*)v26 = (int64_t)v27;
    v29 = (char*)v5 + 16;
    v30 = (char*)v0 + 16;
    v31 = *(int64_t*)v29;
    *(int64_t*)v30 = (int64_t)v31;
    v33 = (char*)v5 + 24;
    v34 = (char*)v0 + 24;
    v35 = *(int64_t*)v33;
    *(int64_t*)v34 = (int64_t)v35;
    p_call_stack_pop();
    return;
    return;
}

bool char_is_whitespace(void* v0, int64_t v1) {
    p_call_stack_push("char_is_whitespace");
    int64_t v13; /* placeholder for void */
    bool v9;
    bool v3;
    int64_t v2;
    int64_t v4;
    bool v5;
    int64_t v7;
    bool v6;
    bool v11;
    bool v12;
    int64_t v10;
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

void* int_to_str(void* v0, int64_t v1) {
    p_call_stack_push("int_to_str");
    void* v2;
    int64_t v3; /* placeholder for void */
    v2 = (void*)p_to_str((int64_t)v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

void Lexer_new(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_new");
    void* v36;
    int64_t v30; /* placeholder for void */
    void* v4;
    void* v7;
    int64_t v8; /* placeholder for void */
    void* v23;
    void* v10;
    int64_t v26; /* placeholder for void */
    void* v37;
    void* v19;
    int64_t v34; /* placeholder for void */
    void* v28;
    int64_t v11; /* placeholder for void */
    void* v24;
    void* v31;
    void* v35;
    void* v20;
    int64_t v38; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v21;
    int64_t v22; /* placeholder for void */
    int64_t v25;
    void* v3;
    void* v27;
    int64_t v6;
    void* v32;
    int64_t v39; /* placeholder for void */
    void* v17;
    void* v13;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    void* v16;
    int64_t v12;
    int64_t v9;
    int64_t v33;
    int64_t v29;
    v3 = malloc(40);
    v4 = (char*)v3 + 0;
    *(void**)v4 = (void*)v2;
    v6 = 0LL;
    v7 = (char*)v3 + 8;
    *(int64_t*)v7 = (int64_t)v6;
    v9 = 1LL;
    v10 = (char*)v3 + 16;
    *(int64_t*)v10 = (int64_t)v9;
    v12 = 1LL;
    v13 = (char*)v3 + 24;
    *(int64_t*)v13 = (int64_t)v12;
    v15 = 0LL;
    v16 = Vec_new((void*)v15);
    v17 = (char*)v3 + 32;
    *(void**)v17 = (void*)v16;
    v19 = (char*)v3 + 0;
    v20 = (char*)v0 + 0;
    v21 = *(void**)v19;
    *(void**)v20 = (void*)v21;
    v23 = (char*)v3 + 8;
    v24 = (char*)v0 + 8;
    v25 = *(int64_t*)v23;
    *(int64_t*)v24 = (int64_t)v25;
    v27 = (char*)v3 + 16;
    v28 = (char*)v0 + 16;
    v29 = *(int64_t*)v27;
    *(int64_t*)v28 = (int64_t)v29;
    v31 = (char*)v3 + 24;
    v32 = (char*)v0 + 24;
    v33 = *(int64_t*)v31;
    *(int64_t*)v32 = (int64_t)v33;
    v35 = (char*)v3 + 32;
    v36 = (char*)v0 + 32;
    v37 = *(void**)v35;
    *(void**)v36 = (void*)v37;
    p_call_stack_pop();
    return;
    return;
}

void Lexer_skip_line_comment(void* v0, void* v1) {
    p_call_stack_push("Lexer_skip_line_comment");
    void* v3;
    int64_t v10;
    bool v13;
    bool v9;
    int64_t v15; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    bool v14;
    int64_t v2; /* placeholder for void */
    void* v6;
    int64_t v4;
    int64_t v12;
    void* v5;
    int64_t v16;
    int64_t v17;
    int64_t v8;
    int64_t v19; /* placeholder for void */
    int64_t v11;
    int64_t v7;
    goto bb1;
  bb1:
    v3 = (char*)v1 + 8;
    v4 = *(int64_t*)v3;
    v5 = (char*)v1 + 0;
    v6 = *(void**)v5;
    v7 = 0LL;
    v8 = (int64_t)str_len((void*)v7, v6);
    v9 = v4 < v8;
    v10 = 0LL;
    v11 = (int64_t)Lexer_peek((void*)v10, v1);
    v12 = 10LL;
    v13 = v11 != v12;
    v14 = v9 && v13;
    if (v14) goto bb2;
    else goto bb3;
  bb2:
    v16 = 0LL;
    v17 = (int64_t)Lexer_advance((void*)v16, v1);
    goto bb1;
  bb3:
    p_call_stack_pop();
    return;
    return;
}

void* token_kind_debug(void* v0, void* v1) {
    p_call_stack_push("token_kind_debug");
    void* v586;
    void* v467;
    int64_t v281; /* placeholder for void */
    int64_t v517; /* placeholder for void */
    int64_t v974;
    int64_t v188; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v613; /* placeholder for void */
    int64_t v179;
    void* v58;
    int64_t v195; /* placeholder for void */
    void* v282;
    int64_t v907; /* placeholder for void */
    void* v981;
    bool v397;
    int64_t v412; /* placeholder for void */
    void* v380;
    bool v612;
    void* v175;
    int64_t v428; /* placeholder for void */
    int64_t v122;
    int64_t v622; /* placeholder for void */
    void* v86;
    int64_t v351; /* placeholder for void */
    int64_t v737;
    int64_t v858; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    bool v75;
    int64_t v172;
    void* v931;
    int64_t v470; /* placeholder for void */
    int64_t v933;
    void* v845;
    int64_t v937; /* placeholder for void */
    int64_t v501; /* placeholder for void */
    int64_t v722;
    void* v775;
    bool v383;
    void* v303;
    void* v310;
    int64_t v442; /* placeholder for void */
    int64_t v947;
    int64_t v524; /* placeholder for void */
    void* v546;
    int64_t v461; /* placeholder for void */
    int64_t v508; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    void* v700;
    bool v12;
    int64_t v515; /* placeholder for void */
    int64_t v536; /* placeholder for void */
    void* v553;
    int64_t v835;
    int64_t v849;
    int64_t v396;
    int64_t v921; /* placeholder for void */
    int64_t v965; /* placeholder for void */
    int64_t v307; /* placeholder for void */
    int64_t v923; /* placeholder for void */
    void* v149;
    void* v254;
    void* v56;
    void* v565;
    void* v492;
    int64_t v365; /* placeholder for void */
    bool v822;
    int64_t v520;
    int64_t v576;
    void* v677;
    int64_t v225; /* placeholder for void */
    int64_t v848;
    int64_t v569;
    int64_t v346;
    void* v462;
    int64_t v870;
    void* v338;
    void* v42;
    int64_t v475; /* placeholder for void */
    void* v593;
    int64_t v251; /* placeholder for void */
    bool v773;
    int64_t v935; /* placeholder for void */
    bool v278;
    int64_t v298;
    void* v443;
    int64_t v939;
    int64_t v157;
    int64_t v302; /* placeholder for void */
    void* v436;
    void* v126;
    int64_t v178;
    void* v203;
    int64_t v97; /* placeholder for void */
    void* v98;
    int64_t v662; /* placeholder for void */
    bool v689;
    bool v962;
    int64_t v263;
    bool v920;
    int64_t v11;
    int64_t v562;
    int64_t v673;
    bool v563;
    int64_t v592; /* placeholder for void */
    int64_t v601; /* placeholder for void */
    void* v9;
    void* v275;
    void* v453;
    int64_t v645;
    int64_t v680;
    void* v37;
    void* v471;
    int64_t v846; /* placeholder for void */
    bool v927;
    void* v140;
    int64_t v284;
    int64_t v688;
    int64_t v34; /* placeholder for void */
    int64_t v218; /* placeholder for void */
    void* v482;
    int64_t v674;
    int64_t v765;
    void* v219;
    int64_t v639;
    int64_t v123;
    int64_t v59;
    int64_t v169; /* placeholder for void */
    int64_t v283;
    int64_t v874; /* placeholder for void */
    int64_t v239; /* placeholder for void */
    int64_t v888; /* placeholder for void */
    void* v572;
    int64_t v200;
    int64_t v564; /* placeholder for void */
    void* v896;
    int64_t v578; /* placeholder for void */
    int64_t v825; /* placeholder for void */
    int64_t v863;
    int64_t v865; /* placeholder for void */
    int64_t v216; /* placeholder for void */
    int64_t v830; /* placeholder for void */
    bool v759;
    bool v376;
    int64_t v743;
    void* v777;
    bool v82;
    int64_t v744;
    bool v500;
    int64_t v332;
    int64_t v76; /* placeholder for void */
    int64_t v316; /* placeholder for void */
    void* v567;
    void* v366;
    int64_t v638;
    int64_t v690; /* placeholder for void */
    void* v812;
    int64_t v55; /* placeholder for void */
    void* v805;
    void* v525;
    void* v324;
    void* v296;
    bool v591;
    int64_t v143;
    bool v668;
    bool v976;
    int64_t v465;
    int64_t v381;
    int64_t v13; /* placeholder for void */
    int64_t v69; /* placeholder for void */
    int64_t v235;
    int64_t v954;
    int64_t v405; /* placeholder for void */
    int64_t v762; /* placeholder for void */
    void* v429;
    void* v693;
    void* v831;
    void* v114;
    void* v468;
    void* v770;
    int64_t v88;
    void* v105;
    void* v749;
    int64_t v814;
    void* v819;
    int64_t v902; /* placeholder for void */
    int64_t v102;
    void* v44;
    bool v787;
    bool v941;
    int64_t v534;
    int64_t v244; /* placeholder for void */
    int64_t v340;
    int64_t v664; /* placeholder for void */
    void* v217;
    int64_t v809; /* placeholder for void */
    bool v229;
    int64_t v391; /* placeholder for void */
    int64_t v288; /* placeholder for void */
    int64_t v881; /* placeholder for void */
    void* v491;
    int64_t v456; /* placeholder for void */
    int64_t v771;
    int64_t v99; /* placeholder for void */
    int64_t v220;
    int64_t v479;
    bool v717;
    void* v735;
    int64_t v344; /* placeholder for void */
    bool v173;
    int64_t v286; /* placeholder for void */
    int64_t v349; /* placeholder for void */
    int64_t v597;
    int64_t v960;
    void* v112;
    int64_t v451;
    void* v686;
    int64_t v573; /* placeholder for void */
    int64_t v305;
    void* v509;
    int64_t v872; /* placeholder for void */
    int64_t v255;
    void* v497;
    void* v649;
    void* v950;
    int64_t v139; /* placeholder for void */
    int64_t v982; /* placeholder for void */
    int64_t v445;
    void* v588;
    int64_t v506;
    int64_t v531; /* placeholder for void */
    int64_t v505;
    int64_t v389;
    int64_t v932;
    int64_t v134; /* placeholder for void */
    int64_t v433; /* placeholder for void */
    int64_t v547;
    bool v661;
    int64_t v741; /* placeholder for void */
    void* v315;
    void* v747;
    int64_t v78; /* placeholder for void */
    int64_t v277;
    void* v973;
    int64_t v566; /* placeholder for void */
    bool v348;
    int64_t v164;
    void* v957;
    int64_t v979; /* placeholder for void */
    int64_t v632;
    bool v682;
    void* v719;
    void* v922;
    int64_t v323; /* placeholder for void */
    void* v107;
    bool v808;
    int64_t v368;
    int64_t v914; /* placeholder for void */
    int64_t v459;
    int64_t v648; /* placeholder for void */
    void* v464;
    int64_t v109;
    void* v439;
    void* v385;
    int64_t v372; /* placeholder for void */
    int64_t v66;
    int64_t v232; /* placeholder for void */
    int64_t v897;
    int64_t v430;
    int64_t v400; /* placeholder for void */
    int64_t v706; /* placeholder for void */
    int64_t v968;
    int64_t v590;
    int64_t v946;
    int64_t v403;
    int64_t v104; /* placeholder for void */
    bool v752;
    int64_t v356; /* placeholder for void */
    bool v906;
    int64_t v419; /* placeholder for void */
    int64_t v683; /* placeholder for void */
    void* v756;
    int64_t v260; /* placeholder for void */
    int64_t v685; /* placeholder for void */
    int64_t v704; /* placeholder for void */
    int64_t v486;
    void* v868;
    int64_t v898;
    void* v978;
    int64_t v162; /* placeholder for void */
    void* v135;
    int64_t v444;
    int64_t v290;
    void* v635;
    void* v691;
    void* v784;
    int64_t v893; /* placeholder for void */
    bool v285;
    int64_t v183; /* placeholder for void */
    void* v14;
    bool v647;
    int64_t v905;
    void* v65;
    bool v362;
    int64_t v779;
    int64_t v813;
    int64_t v867; /* placeholder for void */
    void* v791;
    void* v665;
    bool v857;
    int64_t v767; /* placeholder for void */
    int64_t v53;
    int64_t v951; /* placeholder for void */
    bool v68;
    void* v478;
    int64_t v197; /* placeholder for void */
    void* v833;
    int64_t v113; /* placeholder for void */
    bool v549;
    bool v292;
    void* v959;
    void* v600;
    int64_t v772;
    void* v782;
    void* v252;
    void* v728;
    int64_t v604;
    int64_t v606; /* placeholder for void */
    void* v917;
    int64_t v151;
    void* v866;
    int64_t v751;
    void* v240;
    bool v152;
    void* v658;
    bool v605;
    bool v955;
    bool v264;
    void* v7;
    int64_t v911;
    int64_t v702;
    void* v493;
    void* v21;
    int64_t v386; /* placeholder for void */
    void* v705;
    int64_t v304;
    int64_t v583;
    bool v850;
    int64_t v186;
    bool v327;
    bool v556;
    int64_t v828;
    void* v392;
    bool v843;
    bool v411;
    int64_t v538; /* placeholder for void */
    void* v502;
    void* v656;
    void* v938;
    int64_t v221;
    int64_t v50; /* placeholder for void */
    void* v477;
    bool v208;
    int64_t v270;
    int64_t v526;
    bool v675;
    void* v331;
    int64_t v844; /* placeholder for void */
    int64_t v723;
    int64_t v926;
    int64_t v15; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v407; /* placeholder for void */
    int64_t v498;
    void* v672;
    int64_t v589;
    int64_t v701;
    int64_t v291;
    bool v474;
    int64_t v653;
    void* v684;
    int64_t v795; /* placeholder for void */
    int64_t v321; /* placeholder for void */
    int64_t v318;
    int64_t v136;
    int64_t v760; /* placeholder for void */
    int64_t v45;
    int64_t v842;
    void* v761;
    int64_t v736;
    bool v584;
    int64_t v409;
    int64_t v655; /* placeholder for void */
    int64_t v918;
    int64_t v869;
    int64_t v62; /* placeholder for void */
    int64_t v10;
    int64_t v513;
    int64_t v540;
    int64_t v4;
    bool v507;
    void* v427;
    int64_t v610;
    void* v854;
    int64_t v503; /* placeholder for void */
    void* v23;
    int64_t v799;
    void* v679;
    int64_t v687;
    void* v516;
    void* v924;
    bool v271;
    void* v359;
    int64_t v106; /* placeholder for void */
    int64_t v904;
    void* v826;
    int64_t v213;
    int64_t v575;
    void* v490;
    int64_t v545; /* placeholder for void */
    int64_t v624;
    int64_t v625;
    int64_t v95;
    int64_t v32;
    void* v133;
    void* v485;
    int64_t v181; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    bool v766;
    int64_t v272; /* placeholder for void */
    bool v145;
    void* v408;
    void* v495;
    int64_t v552; /* placeholder for void */
    int64_t v554;
    int64_t v568;
    int64_t v585; /* placeholder for void */
    int64_t v818; /* placeholder for void */
    int64_t v697; /* placeholder for void */
    void* v49;
    bool v432;
    int64_t v499;
    bool v514;
    int64_t v713; /* placeholder for void */
    void* v481;
    int64_t v725; /* placeholder for void */
    int64_t v739; /* placeholder for void */
    int64_t v821;
    void* v352;
    int64_t v886; /* placeholder for void */
    void* v308;
    int64_t v437;
    bool v54;
    void* v466;
    void* v494;
    int64_t v374;
    bool v222;
    void* v441;
    int64_t v695;
    void* v726;
    void* v79;
    bool v654;
    int64_t v417;
    bool v320;
    bool v250;
    int64_t v150;
    void* v196;
    int64_t v790; /* placeholder for void */
    void* v28;
    void* v184;
    void* v450;
    void* v226;
    void* v595;
    int64_t v300; /* placeholder for void */
    int64_t v860; /* placeholder for void */
    int64_t v211; /* placeholder for void */
    int64_t v127; /* placeholder for void */
    void* v544;
    int64_t v253; /* placeholder for void */
    void* v532;
    void* v373;
    int64_t v276;
    bool v598;
    int64_t v916; /* placeholder for void */
    bool v745;
    int64_t v853; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v108;
    int64_t v636; /* placeholder for void */
    bool v801;
    int64_t v246; /* placeholder for void */
    void* v539;
    int64_t v856;
    void* v971;
    int64_t v167; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v115;
    bool v521;
    int64_t v587; /* placeholder for void */
    int64_t v377; /* placeholder for void */
    void* v742;
    int64_t v18;
    int64_t v804; /* placeholder for void */
    void* v322;
    void* v371;
    int64_t v431;
    bool v215;
    int64_t v816; /* placeholder for void */
    int64_t v876;
    int64_t v529; /* placeholder for void */
    int64_t v944; /* placeholder for void */
    bool v341;
    bool v948;
    void* v91;
    void* v966;
    bool v40;
    bool v418;
    int64_t v83; /* placeholder for void */
    int64_t v527;
    int64_t v353;
    void* v607;
    int64_t v489; /* placeholder for void */
    void* v438;
    void* v245;
    int64_t v118; /* placeholder for void */
    int64_t v228;
    void* v2;
    int64_t v750;
    int64_t v820;
    int64_t v561;
    int64_t v841;
    bool v724;
    int64_t v165;
    bool v794;
    void* v894;
    void* v189;
    int64_t v206;
    int64_t v379; /* placeholder for void */
    void* v523;
    int64_t v834;
    void* v147;
    void* v205;
    void* v51;
    void* v70;
    void* v457;
    int64_t v611;
    int64_t v676; /* placeholder for void */
    int64_t v769; /* placeholder for void */
    bool v738;
    void* v630;
    int64_t v559; /* placeholder for void */
    void* v623;
    int64_t v193;
    void* v824;
    void* v901;
    int64_t v398; /* placeholder for void */
    bool v5;
    void* v35;
    void* v602;
    bool v89;
    void* v803;
    int64_t v895; /* placeholder for void */
    int64_t v52;
    void* v551;
    void* v231;
    int64_t v641; /* placeholder for void */
    void* v964;
    bool v131;
    void* v198;
    int64_t v354;
    int64_t v421; /* placeholder for void */
    int64_t v265; /* placeholder for void */
    int64_t v293; /* placeholder for void */
    int64_t v650; /* placeholder for void */
    int64_t v274; /* placeholder for void */
    void* v936;
    int64_t v129;
    void* v413;
    void* v142;
    void* v454;
    int64_t v660;
    void* v847;
    void* v422;
    bool v731;
    int64_t v81;
    int64_t v447; /* placeholder for void */
    double v449;
    bool v633;
    int64_t v788; /* placeholder for void */
    bool v815;
    void* v838;
    int64_t v975;
    void* v483;
    void* v889;
    int64_t v190; /* placeholder for void */
    void* v387;
    int64_t v727; /* placeholder for void */
    void* v637;
    int64_t v627; /* placeholder for void */
    bool v460;
    int64_t v393; /* placeholder for void */
    int64_t v309; /* placeholder for void */
    int64_t v715;
    int64_t v458;
    bool v117;
    int64_t v38;
    int64_t v48; /* placeholder for void */
    int64_t v667;
    void* v616;
    bool v166;
    void* v345;
    int64_t v314; /* placeholder for void */
    int64_t v185;
    int64_t v326;
    int64_t v793;
    void* v537;
    int64_t v692; /* placeholder for void */
    int64_t v940;
    int64_t v279; /* placeholder for void */
    void* v233;
    int64_t v171;
    int64_t v435;
    int64_t v718; /* placeholder for void */
    void* v798;
    int64_t v116;
    int64_t v925;
    int64_t v101;
    int64_t v230; /* placeholder for void */
    void* v943;
    void* v740;
    void* v609;
    void* v915;
    void* v336;
    void* v191;
    int64_t v342; /* placeholder for void */
    void* v261;
    int64_t v541;
    void* v301;
    int64_t v297;
    void* v317;
    bool v369;
    int64_t v963; /* placeholder for void */
    bool v934;
    int64_t v155; /* placeholder for void */
    void* v714;
    int64_t v204; /* placeholder for void */
    int64_t v599; /* placeholder for void */
    void* v394;
    int64_t v3;
    void* v163;
    void* v119;
    bool v488;
    void* v579;
    int64_t v94;
    int64_t v730;
    int64_t v758;
    void* v154;
    int64_t v223; /* placeholder for void */
    int64_t v720; /* placeholder for void */
    int64_t v757;
    bool v33;
    int64_t v46;
    bool v404;
    bool v710;
    int64_t v855;
    void* v873;
    int64_t v755; /* placeholder for void */
    int64_t v25;
    int64_t v248;
    int64_t v823; /* placeholder for void */
    int64_t v312;
    int64_t v214;
    bool v334;
    int64_t v953;
    bool v187;
    int64_t v414; /* placeholder for void */
    void* v733;
    int64_t v426; /* placeholder for void */
    int64_t v74;
    void* v266;
    int64_t v594; /* placeholder for void */
    int64_t v395;
    bool v640;
    bool v61;
    int64_t v571; /* placeholder for void */
    int64_t v582;
    int64_t v67;
    void* v910;
    int64_t v249;
    int64_t v416;
    int64_t v781; /* placeholder for void */
    void* v628;
    int64_t v694;
    bool v570;
    void* v817;
    bool v619;
    int64_t v17;
    int64_t v174; /* placeholder for void */
    int64_t v199;
    void* v663;
    int64_t v533;
    void* v511;
    int64_t v596;
    int64_t v258; /* placeholder for void */
    void* v754;
    int64_t v802; /* placeholder for void */
    int64_t v806;
    int64_t v375;
    void* v401;
    void* v182;
    bool v47;
    bool v864;
    int64_t v125; /* placeholder for void */
    void* v247;
    int64_t v646;
    int64_t v360;
    void* v399;
    int64_t v548;
    void* v415;
    int64_t v256;
    int64_t v339;
    int64_t v634; /* placeholder for void */
    bool v313;
    bool v878;
    void* v840;
    int64_t v267; /* placeholder for void */
    void* v887;
    int64_t v141; /* placeholder for void */
    int64_t v347;
    int64_t v666;
    int64_t v734; /* placeholder for void */
    int64_t v227;
    void* v861;
    int64_t v681;
    int64_t v827;
    int64_t v930; /* placeholder for void */
    int64_t v786;
    bool v899;
    int64_t v671; /* placeholder for void */
    void* v161;
    bool v535;
    int64_t v424;
    int64_t v522; /* placeholder for void */
    void* v707;
    int64_t v657; /* placeholder for void */
    void* v420;
    int64_t v57; /* placeholder for void */
    int64_t v956; /* placeholder for void */
    int64_t v39;
    int64_t v335; /* placeholder for void */
    int64_t v325;
    int64_t v22; /* placeholder for void */
    int64_t v158;
    int64_t v797; /* placeholder for void */
    void* v350;
    void* v128;
    int64_t v631;
    int64_t v234;
    void* v796;
    void* v455;
    void* v852;
    bool v110;
    void* v518;
    void* v84;
    int64_t v716;
    void* v621;
    void* v768;
    int64_t v839; /* placeholder for void */
    int64_t v811; /* placeholder for void */
    int64_t v472;
    int64_t v358; /* placeholder for void */
    int64_t v402;
    void* v448;
    void* v452;
    int64_t v43; /* placeholder for void */
    int64_t v510; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    void* v642;
    void* v63;
    void* v406;
    bool v528;
    int64_t v851; /* placeholder for void */
    int64_t v24;
    void* v644;
    int64_t v746; /* placeholder for void */
    int64_t v748; /* placeholder for void */
    int64_t v603;
    void* v880;
    void* v343;
    int64_t v980; /* placeholder for void */
    int64_t v669; /* placeholder for void */
    int64_t v241;
    int64_t v678; /* placeholder for void */
    int64_t v890;
    bool v103;
    void* v810;
    int64_t v487;
    int64_t v176; /* placeholder for void */
    void* v259;
    int64_t v311;
    void* v875;
    int64_t v367;
    int64_t v60;
    int64_t v242;
    int64_t v774; /* placeholder for void */
    bool v885;
    int64_t v132; /* placeholder for void */
    void* v721;
    int64_t v87;
    bool v836;
    int64_t v879; /* placeholder for void */
    void* v212;
    bool v243;
    bool v703;
    int64_t v972; /* placeholder for void */
    void* v93;
    int64_t v912;
    void* v581;
    void* v434;
    void* v364;
    int64_t v160; /* placeholder for void */
    int64_t v909; /* placeholder for void */
    int64_t v699; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    bool v124;
    void* v530;
    int64_t v919;
    int64_t v961;
    void* v440;
    void* v908;
    int64_t v832; /* placeholder for void */
    int64_t v800;
    int64_t v363; /* placeholder for void */
    int64_t v617;
    int64_t v144;
    void* v859;
    int64_t v333;
    int64_t v709;
    int64_t v550; /* placeholder for void */
    bool v913;
    bool v159;
    int64_t v382;
    int64_t v31;
    int64_t v543; /* placeholder for void */
    bool v201;
    int64_t v330; /* placeholder for void */
    int64_t v643; /* placeholder for void */
    void* v121;
    void* v698;
    int64_t v732; /* placeholder for void */
    void* v289;
    void* v287;
    int64_t v269;
    int64_t v295; /* placeholder for void */
    bool v696;
    int64_t v137;
    int64_t v862;
    int64_t v423;
    bool v871;
    int64_t v708;
    bool v236;
    int64_t v949; /* placeholder for void */
    bool v626;
    int64_t v262;
    void* v329;
    int64_t v473;
    bool v96;
    void* v560;
    int64_t v207;
    int64_t v711; /* placeholder for void */
    bool v829;
    int64_t v942; /* placeholder for void */
    void* v480;
    int64_t v484; /* placeholder for void */
    int64_t v884;
    void* v469;
    bool v969;
    int64_t v970; /* placeholder for void */
    int64_t v580; /* placeholder for void */
    void* v224;
    bool v180;
    int64_t v337; /* placeholder for void */
    bool v194;
    int64_t v80;
    int64_t v148; /* placeholder for void */
    int64_t v967;
    int64_t v20; /* placeholder for void */
    int64_t v629; /* placeholder for void */
    int64_t v192;
    int64_t v209; /* placeholder for void */
    int64_t v883;
    void* v763;
    bool v542;
    int64_t v36; /* placeholder for void */
    void* v945;
    int64_t v388;
    bool v577;
    void* v268;
    bool v257;
    void* v476;
    int64_t v652;
    int64_t v807;
    int64_t v776; /* placeholder for void */
    void* v789;
    int64_t v977; /* placeholder for void */
    int64_t v237; /* placeholder for void */
    bool v892;
    bool v355;
    bool v390;
    void* v280;
    int64_t v519;
    void* v463;
    int64_t v618;
    void* v177;
    int64_t v319;
    int64_t v778;
    void* v903;
    int64_t v783; /* placeholder for void */
    void* v168;
    int64_t v73;
    int64_t v85; /* placeholder for void */
    int64_t v608; /* placeholder for void */
    void* v357;
    void* v929;
    int64_t v837; /* placeholder for void */
    int64_t v146; /* placeholder for void */
    bool v299;
    int64_t v785;
    int64_t v615; /* placeholder for void */
    int64_t v496; /* placeholder for void */
    int64_t v202; /* placeholder for void */
    void* v273;
    bool v306;
    bool v425;
    int64_t v877;
    int64_t v384; /* placeholder for void */
    void* v30;
    void* v16;
    void* v378;
    int64_t v753; /* placeholder for void */
    void* v882;
    bool v446;
    int64_t v370; /* placeholder for void */
    void* v294;
    bool v26;
    int64_t v361;
    void* v614;
    int64_t v557; /* placeholder for void */
    int64_t v130;
    int64_t v620; /* placeholder for void */
    void* v156;
    void* v712;
    bool v780;
    int64_t v512;
    int64_t v555;
    int64_t v729;
    void* v72;
    int64_t v410;
    void* v574;
    void* v170;
    void* v504;
    bool v19;
    int64_t v764;
    int64_t v928; /* placeholder for void */
    void* v238;
    void* v952;
    void* v210;
    int64_t v792;
    int64_t v328; /* placeholder for void */
    int64_t v659;
    void* v651;
    void* v558;
    int64_t v900; /* placeholder for void */
    void* v77;
    int64_t v891;
    void* v670;
    int64_t v64; /* placeholder for void */
    bool v138;
    int64_t v958; /* placeholder for void */
    void* v100;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v981 = 0LL;
    p_call_stack_pop();
    return v981;
  bb2:
    v7 = _str177;
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
    v14 = _str178;
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
    v21 = _str179;
    p_call_stack_pop();
    return v21;
  bb7:
    v23 = (char*)v1 + 0;
    v24 = *(int64_t*)v23;
    v25 = 3LL;
    v26 = v24 == v25;
    if (v26) goto bb8;
    else goto bb9;
  bb8:
    v28 = _str180;
    p_call_stack_pop();
    return v28;
  bb9:
    v30 = (char*)v1 + 0;
    v31 = *(int64_t*)v30;
    v32 = 4LL;
    v33 = v31 == v32;
    if (v33) goto bb10;
    else goto bb11;
  bb10:
    v35 = _str181;
    p_call_stack_pop();
    return v35;
  bb11:
    v37 = (char*)v1 + 0;
    v38 = *(int64_t*)v37;
    v39 = 5LL;
    v40 = v38 == v39;
    if (v40) goto bb12;
    else goto bb13;
  bb12:
    v42 = _str182;
    p_call_stack_pop();
    return v42;
  bb13:
    v44 = (char*)v1 + 0;
    v45 = *(int64_t*)v44;
    v46 = 6LL;
    v47 = v45 == v46;
    if (v47) goto bb14;
    else goto bb15;
  bb14:
    v49 = _str183;
    p_call_stack_pop();
    return v49;
  bb15:
    v51 = (char*)v1 + 0;
    v52 = *(int64_t*)v51;
    v53 = 7LL;
    v54 = v52 == v53;
    if (v54) goto bb16;
    else goto bb17;
  bb16:
    v56 = _str184;
    p_call_stack_pop();
    return v56;
  bb17:
    v58 = (char*)v1 + 0;
    v59 = *(int64_t*)v58;
    v60 = 8LL;
    v61 = v59 == v60;
    if (v61) goto bb18;
    else goto bb19;
  bb18:
    v63 = _str185;
    p_call_stack_pop();
    return v63;
  bb19:
    v65 = (char*)v1 + 0;
    v66 = *(int64_t*)v65;
    v67 = 9LL;
    v68 = v66 == v67;
    if (v68) goto bb20;
    else goto bb21;
  bb20:
    v70 = _str186;
    p_call_stack_pop();
    return v70;
  bb21:
    v72 = (char*)v1 + 0;
    v73 = *(int64_t*)v72;
    v74 = 10LL;
    v75 = v73 == v74;
    if (v75) goto bb22;
    else goto bb23;
  bb22:
    v77 = _str187;
    p_call_stack_pop();
    return v77;
  bb23:
    v79 = (char*)v1 + 0;
    v80 = *(int64_t*)v79;
    v81 = 11LL;
    v82 = v80 == v81;
    if (v82) goto bb24;
    else goto bb25;
  bb24:
    v84 = _str188;
    p_call_stack_pop();
    return v84;
  bb25:
    v86 = (char*)v1 + 0;
    v87 = *(int64_t*)v86;
    v88 = 12LL;
    v89 = v87 == v88;
    if (v89) goto bb26;
    else goto bb27;
  bb26:
    v91 = _str189;
    p_call_stack_pop();
    return v91;
  bb27:
    v93 = (char*)v1 + 0;
    v94 = *(int64_t*)v93;
    v95 = 13LL;
    v96 = v94 == v95;
    if (v96) goto bb28;
    else goto bb29;
  bb28:
    v98 = _str190;
    p_call_stack_pop();
    return v98;
  bb29:
    v100 = (char*)v1 + 0;
    v101 = *(int64_t*)v100;
    v102 = 14LL;
    v103 = v101 == v102;
    if (v103) goto bb30;
    else goto bb31;
  bb30:
    v105 = _str191;
    p_call_stack_pop();
    return v105;
  bb31:
    v107 = (char*)v1 + 0;
    v108 = *(int64_t*)v107;
    v109 = 15LL;
    v110 = v108 == v109;
    if (v110) goto bb32;
    else goto bb33;
  bb32:
    v112 = _str192;
    p_call_stack_pop();
    return v112;
  bb33:
    v114 = (char*)v1 + 0;
    v115 = *(int64_t*)v114;
    v116 = 16LL;
    v117 = v115 == v116;
    if (v117) goto bb34;
    else goto bb35;
  bb34:
    v119 = _str193;
    p_call_stack_pop();
    return v119;
  bb35:
    v121 = (char*)v1 + 0;
    v122 = *(int64_t*)v121;
    v123 = 17LL;
    v124 = v122 == v123;
    if (v124) goto bb36;
    else goto bb37;
  bb36:
    v126 = _str194;
    p_call_stack_pop();
    return v126;
  bb37:
    v128 = (char*)v1 + 0;
    v129 = *(int64_t*)v128;
    v130 = 18LL;
    v131 = v129 == v130;
    if (v131) goto bb38;
    else goto bb39;
  bb38:
    v133 = _str195;
    p_call_stack_pop();
    return v133;
  bb39:
    v135 = (char*)v1 + 0;
    v136 = *(int64_t*)v135;
    v137 = 19LL;
    v138 = v136 == v137;
    if (v138) goto bb40;
    else goto bb41;
  bb40:
    v140 = _str196;
    p_call_stack_pop();
    return v140;
  bb41:
    v142 = (char*)v1 + 0;
    v143 = *(int64_t*)v142;
    v144 = 20LL;
    v145 = v143 == v144;
    if (v145) goto bb42;
    else goto bb43;
  bb42:
    v147 = _str197;
    p_call_stack_pop();
    return v147;
  bb43:
    v149 = (char*)v1 + 0;
    v150 = *(int64_t*)v149;
    v151 = 21LL;
    v152 = v150 == v151;
    if (v152) goto bb44;
    else goto bb45;
  bb44:
    v154 = _str198;
    p_call_stack_pop();
    return v154;
  bb45:
    v156 = (char*)v1 + 0;
    v157 = *(int64_t*)v156;
    v158 = 22LL;
    v159 = v157 == v158;
    if (v159) goto bb46;
    else goto bb47;
  bb46:
    v161 = _str199;
    p_call_stack_pop();
    return v161;
  bb47:
    v163 = (char*)v1 + 0;
    v164 = *(int64_t*)v163;
    v165 = 23LL;
    v166 = v164 == v165;
    if (v166) goto bb48;
    else goto bb49;
  bb48:
    v168 = _str200;
    p_call_stack_pop();
    return v168;
  bb49:
    v170 = (char*)v1 + 0;
    v171 = *(int64_t*)v170;
    v172 = 24LL;
    v173 = v171 == v172;
    if (v173) goto bb50;
    else goto bb51;
  bb50:
    v175 = _str201;
    p_call_stack_pop();
    return v175;
  bb51:
    v177 = (char*)v1 + 0;
    v178 = *(int64_t*)v177;
    v179 = 25LL;
    v180 = v178 == v179;
    if (v180) goto bb52;
    else goto bb53;
  bb52:
    v182 = _str202;
    p_call_stack_pop();
    return v182;
  bb53:
    v184 = (char*)v1 + 0;
    v185 = *(int64_t*)v184;
    v186 = 26LL;
    v187 = v185 == v186;
    if (v187) goto bb54;
    else goto bb55;
  bb54:
    v189 = _str203;
    p_call_stack_pop();
    return v189;
  bb55:
    v191 = (char*)v1 + 0;
    v192 = *(int64_t*)v191;
    v193 = 27LL;
    v194 = v192 == v193;
    if (v194) goto bb56;
    else goto bb57;
  bb56:
    v196 = _str204;
    p_call_stack_pop();
    return v196;
  bb57:
    v198 = (char*)v1 + 0;
    v199 = *(int64_t*)v198;
    v200 = 28LL;
    v201 = v199 == v200;
    if (v201) goto bb58;
    else goto bb59;
  bb58:
    v203 = _str205;
    p_call_stack_pop();
    return v203;
  bb59:
    v205 = (char*)v1 + 0;
    v206 = *(int64_t*)v205;
    v207 = 29LL;
    v208 = v206 == v207;
    if (v208) goto bb60;
    else goto bb61;
  bb60:
    v210 = _str206;
    p_call_stack_pop();
    return v210;
  bb61:
    v212 = (char*)v1 + 0;
    v213 = *(int64_t*)v212;
    v214 = 30LL;
    v215 = v213 == v214;
    if (v215) goto bb62;
    else goto bb63;
  bb62:
    v217 = _str207;
    p_call_stack_pop();
    return v217;
  bb63:
    v219 = (char*)v1 + 0;
    v220 = *(int64_t*)v219;
    v221 = 31LL;
    v222 = v220 == v221;
    if (v222) goto bb64;
    else goto bb65;
  bb64:
    v224 = _str208;
    p_call_stack_pop();
    return v224;
  bb65:
    v226 = (char*)v1 + 0;
    v227 = *(int64_t*)v226;
    v228 = 32LL;
    v229 = v227 == v228;
    if (v229) goto bb66;
    else goto bb67;
  bb66:
    v231 = _str209;
    p_call_stack_pop();
    return v231;
  bb67:
    v233 = (char*)v1 + 0;
    v234 = *(int64_t*)v233;
    v235 = 33LL;
    v236 = v234 == v235;
    if (v236) goto bb68;
    else goto bb69;
  bb68:
    v238 = _str210;
    p_call_stack_pop();
    return v238;
  bb69:
    v240 = (char*)v1 + 0;
    v241 = *(int64_t*)v240;
    v242 = 34LL;
    v243 = v241 == v242;
    if (v243) goto bb70;
    else goto bb71;
  bb70:
    v245 = _str211;
    p_call_stack_pop();
    return v245;
  bb71:
    v247 = (char*)v1 + 0;
    v248 = *(int64_t*)v247;
    v249 = 35LL;
    v250 = v248 == v249;
    if (v250) goto bb72;
    else goto bb73;
  bb72:
    v252 = _str212;
    p_call_stack_pop();
    return v252;
  bb73:
    v254 = (char*)v1 + 0;
    v255 = *(int64_t*)v254;
    v256 = 36LL;
    v257 = v255 == v256;
    if (v257) goto bb74;
    else goto bb75;
  bb74:
    v259 = _str213;
    p_call_stack_pop();
    return v259;
  bb75:
    v261 = (char*)v1 + 0;
    v262 = *(int64_t*)v261;
    v263 = 37LL;
    v264 = v262 == v263;
    if (v264) goto bb76;
    else goto bb77;
  bb76:
    v266 = _str214;
    p_call_stack_pop();
    return v266;
  bb77:
    v268 = (char*)v1 + 0;
    v269 = *(int64_t*)v268;
    v270 = 38LL;
    v271 = v269 == v270;
    if (v271) goto bb78;
    else goto bb79;
  bb78:
    v273 = _str215;
    p_call_stack_pop();
    return v273;
  bb79:
    v275 = (char*)v1 + 0;
    v276 = *(int64_t*)v275;
    v277 = 39LL;
    v278 = v276 == v277;
    if (v278) goto bb80;
    else goto bb81;
  bb80:
    v280 = _str216;
    p_call_stack_pop();
    return v280;
  bb81:
    v282 = (char*)v1 + 0;
    v283 = *(int64_t*)v282;
    v284 = 40LL;
    v285 = v283 == v284;
    if (v285) goto bb82;
    else goto bb83;
  bb82:
    v287 = _str217;
    p_call_stack_pop();
    return v287;
  bb83:
    v289 = (char*)v1 + 0;
    v290 = *(int64_t*)v289;
    v291 = 41LL;
    v292 = v290 == v291;
    if (v292) goto bb84;
    else goto bb85;
  bb84:
    v294 = _str218;
    p_call_stack_pop();
    return v294;
  bb85:
    v296 = (char*)v1 + 0;
    v297 = *(int64_t*)v296;
    v298 = 42LL;
    v299 = v297 == v298;
    if (v299) goto bb86;
    else goto bb87;
  bb86:
    v301 = _str219;
    p_call_stack_pop();
    return v301;
  bb87:
    v303 = (char*)v1 + 0;
    v304 = *(int64_t*)v303;
    v305 = 43LL;
    v306 = v304 == v305;
    if (v306) goto bb88;
    else goto bb89;
  bb88:
    v308 = _str220;
    p_call_stack_pop();
    return v308;
  bb89:
    v310 = (char*)v1 + 0;
    v311 = *(int64_t*)v310;
    v312 = 44LL;
    v313 = v311 == v312;
    if (v313) goto bb90;
    else goto bb91;
  bb90:
    v315 = _str221;
    p_call_stack_pop();
    return v315;
  bb91:
    v317 = (char*)v1 + 0;
    v318 = *(int64_t*)v317;
    v319 = 45LL;
    v320 = v318 == v319;
    if (v320) goto bb92;
    else goto bb93;
  bb92:
    v322 = _str222;
    p_call_stack_pop();
    return v322;
  bb93:
    v324 = (char*)v1 + 0;
    v325 = *(int64_t*)v324;
    v326 = 46LL;
    v327 = v325 == v326;
    if (v327) goto bb94;
    else goto bb95;
  bb94:
    v329 = _str223;
    p_call_stack_pop();
    return v329;
  bb95:
    v331 = (char*)v1 + 0;
    v332 = *(int64_t*)v331;
    v333 = 47LL;
    v334 = v332 == v333;
    if (v334) goto bb96;
    else goto bb97;
  bb96:
    v336 = _str224;
    p_call_stack_pop();
    return v336;
  bb97:
    v338 = (char*)v1 + 0;
    v339 = *(int64_t*)v338;
    v340 = 48LL;
    v341 = v339 == v340;
    if (v341) goto bb98;
    else goto bb99;
  bb98:
    v343 = _str225;
    p_call_stack_pop();
    return v343;
  bb99:
    v345 = (char*)v1 + 0;
    v346 = *(int64_t*)v345;
    v347 = 49LL;
    v348 = v346 == v347;
    if (v348) goto bb100;
    else goto bb101;
  bb100:
    v350 = _str226;
    p_call_stack_pop();
    return v350;
  bb101:
    v352 = (char*)v1 + 0;
    v353 = *(int64_t*)v352;
    v354 = 50LL;
    v355 = v353 == v354;
    if (v355) goto bb102;
    else goto bb103;
  bb102:
    v357 = _str227;
    p_call_stack_pop();
    return v357;
  bb103:
    v359 = (char*)v1 + 0;
    v360 = *(int64_t*)v359;
    v361 = 51LL;
    v362 = v360 == v361;
    if (v362) goto bb104;
    else goto bb105;
  bb104:
    v364 = _str228;
    p_call_stack_pop();
    return v364;
  bb105:
    v366 = (char*)v1 + 0;
    v367 = *(int64_t*)v366;
    v368 = 52LL;
    v369 = v367 == v368;
    if (v369) goto bb106;
    else goto bb107;
  bb106:
    v371 = _str229;
    p_call_stack_pop();
    return v371;
  bb107:
    v373 = (char*)v1 + 0;
    v374 = *(int64_t*)v373;
    v375 = 53LL;
    v376 = v374 == v375;
    if (v376) goto bb108;
    else goto bb109;
  bb108:
    v378 = _str230;
    p_call_stack_pop();
    return v378;
  bb109:
    v380 = (char*)v1 + 0;
    v381 = *(int64_t*)v380;
    v382 = 54LL;
    v383 = v381 == v382;
    if (v383) goto bb110;
    else goto bb111;
  bb110:
    v385 = _str231;
    p_call_stack_pop();
    return v385;
  bb111:
    v387 = (char*)v1 + 0;
    v388 = *(int64_t*)v387;
    v389 = 55LL;
    v390 = v388 == v389;
    if (v390) goto bb112;
    else goto bb113;
  bb112:
    v392 = _str232;
    p_call_stack_pop();
    return v392;
  bb113:
    v394 = (char*)v1 + 0;
    v395 = *(int64_t*)v394;
    v396 = 56LL;
    v397 = v395 == v396;
    if (v397) goto bb114;
    else goto bb115;
  bb114:
    v399 = _str233;
    p_call_stack_pop();
    return v399;
  bb115:
    v401 = (char*)v1 + 0;
    v402 = *(int64_t*)v401;
    v403 = 57LL;
    v404 = v402 == v403;
    if (v404) goto bb116;
    else goto bb117;
  bb116:
    v406 = _str234;
    p_call_stack_pop();
    return v406;
  bb117:
    v408 = (char*)v1 + 0;
    v409 = *(int64_t*)v408;
    v410 = 58LL;
    v411 = v409 == v410;
    if (v411) goto bb118;
    else goto bb119;
  bb118:
    v413 = _str235;
    p_call_stack_pop();
    return v413;
  bb119:
    v415 = (char*)v1 + 0;
    v416 = *(int64_t*)v415;
    v417 = 59LL;
    v418 = v416 == v417;
    if (v418) goto bb120;
    else goto bb121;
  bb120:
    v420 = _str236;
    p_call_stack_pop();
    return v420;
  bb121:
    v422 = (char*)v1 + 0;
    v423 = *(int64_t*)v422;
    v424 = 60LL;
    v425 = v423 == v424;
    if (v425) goto bb122;
    else goto bb123;
  bb122:
    v427 = _str237;
    p_call_stack_pop();
    return v427;
  bb123:
    v429 = (char*)v1 + 0;
    v430 = *(int64_t*)v429;
    v431 = 76LL;
    v432 = v430 == v431;
    if (v432) goto bb124;
    else goto bb125;
  bb124:
    v434 = (char*)v1 + 8;
    v435 = *(int64_t*)v434;
    v436 = _str238;
    v437 = 0LL;
    v438 = int_to_str((void*)v437, v435);
    v439 = p_str_concat(v436, v438);
    v440 = _str63;
    v441 = p_str_concat(v439, v440);
    p_call_stack_pop();
    return v441;
  bb125:
    v443 = (char*)v1 + 0;
    v444 = *(int64_t*)v443;
    v445 = 77LL;
    v446 = v444 == v445;
    if (v446) goto bb126;
    else goto bb127;
  bb126:
    v448 = (char*)v1 + 8;
    v449 = *(double*)v448;
    v450 = _str62;
    v451 = 0LL;
    v452 = float_debug((void*)v451, v449);
    v453 = p_str_concat(v450, v452);
    v454 = _str63;
    v455 = p_str_concat(v453, v454);
    p_call_stack_pop();
    return v455;
  bb127:
    v457 = (char*)v1 + 0;
    v458 = *(int64_t*)v457;
    v459 = 78LL;
    v460 = v458 == v459;
    if (v460) goto bb128;
    else goto bb129;
  bb128:
    v462 = (char*)v1 + 8;
    v463 = *(void**)v462;
    v464 = _str239;
    v465 = 0LL;
    v466 = str_escape((void*)v465, v463);
    v467 = p_str_concat(v464, v466);
    v468 = _str66;
    v469 = p_str_concat(v467, v468);
    p_call_stack_pop();
    return v469;
  bb129:
    v471 = (char*)v1 + 0;
    v472 = *(int64_t*)v471;
    v473 = 79LL;
    v474 = v472 == v473;
    if (v474) goto bb130;
    else goto bb131;
  bb130:
    v476 = (char*)v1 + 8;
    v477 = *(void**)v476;
    v478 = _str65;
    v479 = 0LL;
    v480 = str_escape((void*)v479, v477);
    v481 = p_str_concat(v478, v480);
    v482 = _str66;
    v483 = p_str_concat(v481, v482);
    p_call_stack_pop();
    return v483;
  bb131:
    v485 = (char*)v1 + 0;
    v486 = *(int64_t*)v485;
    v487 = 80LL;
    v488 = v486 == v487;
    if (v488) goto bb132;
    else goto bb133;
  bb132:
    v490 = (char*)v1 + 8;
    v491 = *(void**)v490;
    v492 = _str240;
    v493 = p_str_concat(v492, v491);
    v494 = _str66;
    v495 = p_str_concat(v493, v494);
    p_call_stack_pop();
    return v495;
  bb133:
    v497 = (char*)v1 + 0;
    v498 = *(int64_t*)v497;
    v499 = 134LL;
    v500 = v498 == v499;
    if (v500) goto bb134;
    else goto bb135;
  bb134:
    v502 = _str241;
    p_call_stack_pop();
    return v502;
  bb135:
    v504 = (char*)v1 + 0;
    v505 = *(int64_t*)v504;
    v506 = 61LL;
    v507 = v505 == v506;
    if (v507) goto bb136;
    else goto bb137;
  bb136:
    v509 = _str68;
    p_call_stack_pop();
    return v509;
  bb137:
    v511 = (char*)v1 + 0;
    v512 = *(int64_t*)v511;
    v513 = 62LL;
    v514 = v512 == v513;
    if (v514) goto bb138;
    else goto bb139;
  bb138:
    v516 = _str69;
    p_call_stack_pop();
    return v516;
  bb139:
    v518 = (char*)v1 + 0;
    v519 = *(int64_t*)v518;
    v520 = 63LL;
    v521 = v519 == v520;
    if (v521) goto bb140;
    else goto bb141;
  bb140:
    v523 = _str70;
    p_call_stack_pop();
    return v523;
  bb141:
    v525 = (char*)v1 + 0;
    v526 = *(int64_t*)v525;
    v527 = 64LL;
    v528 = v526 == v527;
    if (v528) goto bb142;
    else goto bb143;
  bb142:
    v530 = _str71;
    p_call_stack_pop();
    return v530;
  bb143:
    v532 = (char*)v1 + 0;
    v533 = *(int64_t*)v532;
    v534 = 65LL;
    v535 = v533 == v534;
    if (v535) goto bb144;
    else goto bb145;
  bb144:
    v537 = _str72;
    p_call_stack_pop();
    return v537;
  bb145:
    v539 = (char*)v1 + 0;
    v540 = *(int64_t*)v539;
    v541 = 66LL;
    v542 = v540 == v541;
    if (v542) goto bb146;
    else goto bb147;
  bb146:
    v544 = _str73;
    p_call_stack_pop();
    return v544;
  bb147:
    v546 = (char*)v1 + 0;
    v547 = *(int64_t*)v546;
    v548 = 67LL;
    v549 = v547 == v548;
    if (v549) goto bb148;
    else goto bb149;
  bb148:
    v551 = _str74;
    p_call_stack_pop();
    return v551;
  bb149:
    v553 = (char*)v1 + 0;
    v554 = *(int64_t*)v553;
    v555 = 68LL;
    v556 = v554 == v555;
    if (v556) goto bb150;
    else goto bb151;
  bb150:
    v558 = _str75;
    p_call_stack_pop();
    return v558;
  bb151:
    v560 = (char*)v1 + 0;
    v561 = *(int64_t*)v560;
    v562 = 69LL;
    v563 = v561 == v562;
    if (v563) goto bb152;
    else goto bb153;
  bb152:
    v565 = _str76;
    p_call_stack_pop();
    return v565;
  bb153:
    v567 = (char*)v1 + 0;
    v568 = *(int64_t*)v567;
    v569 = 70LL;
    v570 = v568 == v569;
    if (v570) goto bb154;
    else goto bb155;
  bb154:
    v572 = _str77;
    p_call_stack_pop();
    return v572;
  bb155:
    v574 = (char*)v1 + 0;
    v575 = *(int64_t*)v574;
    v576 = 71LL;
    v577 = v575 == v576;
    if (v577) goto bb156;
    else goto bb157;
  bb156:
    v579 = _str78;
    p_call_stack_pop();
    return v579;
  bb157:
    v581 = (char*)v1 + 0;
    v582 = *(int64_t*)v581;
    v583 = 72LL;
    v584 = v582 == v583;
    if (v584) goto bb158;
    else goto bb159;
  bb158:
    v586 = _str79;
    p_call_stack_pop();
    return v586;
  bb159:
    v588 = (char*)v1 + 0;
    v589 = *(int64_t*)v588;
    v590 = 73LL;
    v591 = v589 == v590;
    if (v591) goto bb160;
    else goto bb161;
  bb160:
    v593 = _str80;
    p_call_stack_pop();
    return v593;
  bb161:
    v595 = (char*)v1 + 0;
    v596 = *(int64_t*)v595;
    v597 = 74LL;
    v598 = v596 == v597;
    if (v598) goto bb162;
    else goto bb163;
  bb162:
    v600 = _str81;
    p_call_stack_pop();
    return v600;
  bb163:
    v602 = (char*)v1 + 0;
    v603 = *(int64_t*)v602;
    v604 = 75LL;
    v605 = v603 == v604;
    if (v605) goto bb164;
    else goto bb165;
  bb164:
    v607 = _str82;
    p_call_stack_pop();
    return v607;
  bb165:
    v609 = (char*)v1 + 0;
    v610 = *(int64_t*)v609;
    v611 = 81LL;
    v612 = v610 == v611;
    if (v612) goto bb166;
    else goto bb167;
  bb166:
    v614 = _str83;
    p_call_stack_pop();
    return v614;
  bb167:
    v616 = (char*)v1 + 0;
    v617 = *(int64_t*)v616;
    v618 = 82LL;
    v619 = v617 == v618;
    if (v619) goto bb168;
    else goto bb169;
  bb168:
    v621 = _str84;
    p_call_stack_pop();
    return v621;
  bb169:
    v623 = (char*)v1 + 0;
    v624 = *(int64_t*)v623;
    v625 = 83LL;
    v626 = v624 == v625;
    if (v626) goto bb170;
    else goto bb171;
  bb170:
    v628 = _str85;
    p_call_stack_pop();
    return v628;
  bb171:
    v630 = (char*)v1 + 0;
    v631 = *(int64_t*)v630;
    v632 = 84LL;
    v633 = v631 == v632;
    if (v633) goto bb172;
    else goto bb173;
  bb172:
    v635 = _str86;
    p_call_stack_pop();
    return v635;
  bb173:
    v637 = (char*)v1 + 0;
    v638 = *(int64_t*)v637;
    v639 = 85LL;
    v640 = v638 == v639;
    if (v640) goto bb174;
    else goto bb175;
  bb174:
    v642 = _str87;
    p_call_stack_pop();
    return v642;
  bb175:
    v644 = (char*)v1 + 0;
    v645 = *(int64_t*)v644;
    v646 = 86LL;
    v647 = v645 == v646;
    if (v647) goto bb176;
    else goto bb177;
  bb176:
    v649 = _str88;
    p_call_stack_pop();
    return v649;
  bb177:
    v651 = (char*)v1 + 0;
    v652 = *(int64_t*)v651;
    v653 = 87LL;
    v654 = v652 == v653;
    if (v654) goto bb178;
    else goto bb179;
  bb178:
    v656 = _str89;
    p_call_stack_pop();
    return v656;
  bb179:
    v658 = (char*)v1 + 0;
    v659 = *(int64_t*)v658;
    v660 = 88LL;
    v661 = v659 == v660;
    if (v661) goto bb180;
    else goto bb181;
  bb180:
    v663 = _str90;
    p_call_stack_pop();
    return v663;
  bb181:
    v665 = (char*)v1 + 0;
    v666 = *(int64_t*)v665;
    v667 = 89LL;
    v668 = v666 == v667;
    if (v668) goto bb182;
    else goto bb183;
  bb182:
    v670 = _str91;
    p_call_stack_pop();
    return v670;
  bb183:
    v672 = (char*)v1 + 0;
    v673 = *(int64_t*)v672;
    v674 = 90LL;
    v675 = v673 == v674;
    if (v675) goto bb184;
    else goto bb185;
  bb184:
    v677 = _str92;
    p_call_stack_pop();
    return v677;
  bb185:
    v679 = (char*)v1 + 0;
    v680 = *(int64_t*)v679;
    v681 = 91LL;
    v682 = v680 == v681;
    if (v682) goto bb186;
    else goto bb187;
  bb186:
    v684 = _str93;
    p_call_stack_pop();
    return v684;
  bb187:
    v686 = (char*)v1 + 0;
    v687 = *(int64_t*)v686;
    v688 = 92LL;
    v689 = v687 == v688;
    if (v689) goto bb188;
    else goto bb189;
  bb188:
    v691 = _str94;
    p_call_stack_pop();
    return v691;
  bb189:
    v693 = (char*)v1 + 0;
    v694 = *(int64_t*)v693;
    v695 = 93LL;
    v696 = v694 == v695;
    if (v696) goto bb190;
    else goto bb191;
  bb190:
    v698 = _str95;
    p_call_stack_pop();
    return v698;
  bb191:
    v700 = (char*)v1 + 0;
    v701 = *(int64_t*)v700;
    v702 = 94LL;
    v703 = v701 == v702;
    if (v703) goto bb192;
    else goto bb193;
  bb192:
    v705 = _str96;
    p_call_stack_pop();
    return v705;
  bb193:
    v707 = (char*)v1 + 0;
    v708 = *(int64_t*)v707;
    v709 = 95LL;
    v710 = v708 == v709;
    if (v710) goto bb194;
    else goto bb195;
  bb194:
    v712 = _str97;
    p_call_stack_pop();
    return v712;
  bb195:
    v714 = (char*)v1 + 0;
    v715 = *(int64_t*)v714;
    v716 = 96LL;
    v717 = v715 == v716;
    if (v717) goto bb196;
    else goto bb197;
  bb196:
    v719 = _str98;
    p_call_stack_pop();
    return v719;
  bb197:
    v721 = (char*)v1 + 0;
    v722 = *(int64_t*)v721;
    v723 = 97LL;
    v724 = v722 == v723;
    if (v724) goto bb198;
    else goto bb199;
  bb198:
    v726 = _str99;
    p_call_stack_pop();
    return v726;
  bb199:
    v728 = (char*)v1 + 0;
    v729 = *(int64_t*)v728;
    v730 = 98LL;
    v731 = v729 == v730;
    if (v731) goto bb200;
    else goto bb201;
  bb200:
    v733 = _str100;
    p_call_stack_pop();
    return v733;
  bb201:
    v735 = (char*)v1 + 0;
    v736 = *(int64_t*)v735;
    v737 = 99LL;
    v738 = v736 == v737;
    if (v738) goto bb202;
    else goto bb203;
  bb202:
    v740 = _str101;
    p_call_stack_pop();
    return v740;
  bb203:
    v742 = (char*)v1 + 0;
    v743 = *(int64_t*)v742;
    v744 = 100LL;
    v745 = v743 == v744;
    if (v745) goto bb204;
    else goto bb205;
  bb204:
    v747 = _str102;
    p_call_stack_pop();
    return v747;
  bb205:
    v749 = (char*)v1 + 0;
    v750 = *(int64_t*)v749;
    v751 = 101LL;
    v752 = v750 == v751;
    if (v752) goto bb206;
    else goto bb207;
  bb206:
    v754 = _str103;
    p_call_stack_pop();
    return v754;
  bb207:
    v756 = (char*)v1 + 0;
    v757 = *(int64_t*)v756;
    v758 = 102LL;
    v759 = v757 == v758;
    if (v759) goto bb208;
    else goto bb209;
  bb208:
    v761 = _str104;
    p_call_stack_pop();
    return v761;
  bb209:
    v763 = (char*)v1 + 0;
    v764 = *(int64_t*)v763;
    v765 = 103LL;
    v766 = v764 == v765;
    if (v766) goto bb210;
    else goto bb211;
  bb210:
    v768 = _str105;
    p_call_stack_pop();
    return v768;
  bb211:
    v770 = (char*)v1 + 0;
    v771 = *(int64_t*)v770;
    v772 = 104LL;
    v773 = v771 == v772;
    if (v773) goto bb212;
    else goto bb213;
  bb212:
    v775 = _str106;
    p_call_stack_pop();
    return v775;
  bb213:
    v777 = (char*)v1 + 0;
    v778 = *(int64_t*)v777;
    v779 = 105LL;
    v780 = v778 == v779;
    if (v780) goto bb214;
    else goto bb215;
  bb214:
    v782 = _str107;
    p_call_stack_pop();
    return v782;
  bb215:
    v784 = (char*)v1 + 0;
    v785 = *(int64_t*)v784;
    v786 = 106LL;
    v787 = v785 == v786;
    if (v787) goto bb216;
    else goto bb217;
  bb216:
    v789 = _str108;
    p_call_stack_pop();
    return v789;
  bb217:
    v791 = (char*)v1 + 0;
    v792 = *(int64_t*)v791;
    v793 = 107LL;
    v794 = v792 == v793;
    if (v794) goto bb218;
    else goto bb219;
  bb218:
    v796 = _str109;
    p_call_stack_pop();
    return v796;
  bb219:
    v798 = (char*)v1 + 0;
    v799 = *(int64_t*)v798;
    v800 = 108LL;
    v801 = v799 == v800;
    if (v801) goto bb220;
    else goto bb221;
  bb220:
    v803 = _str110;
    p_call_stack_pop();
    return v803;
  bb221:
    v805 = (char*)v1 + 0;
    v806 = *(int64_t*)v805;
    v807 = 109LL;
    v808 = v806 == v807;
    if (v808) goto bb222;
    else goto bb223;
  bb222:
    v810 = _str111;
    p_call_stack_pop();
    return v810;
  bb223:
    v812 = (char*)v1 + 0;
    v813 = *(int64_t*)v812;
    v814 = 110LL;
    v815 = v813 == v814;
    if (v815) goto bb224;
    else goto bb225;
  bb224:
    v817 = _str112;
    p_call_stack_pop();
    return v817;
  bb225:
    v819 = (char*)v1 + 0;
    v820 = *(int64_t*)v819;
    v821 = 111LL;
    v822 = v820 == v821;
    if (v822) goto bb226;
    else goto bb227;
  bb226:
    v824 = _str113;
    p_call_stack_pop();
    return v824;
  bb227:
    v826 = (char*)v1 + 0;
    v827 = *(int64_t*)v826;
    v828 = 112LL;
    v829 = v827 == v828;
    if (v829) goto bb228;
    else goto bb229;
  bb228:
    v831 = _str114;
    p_call_stack_pop();
    return v831;
  bb229:
    v833 = (char*)v1 + 0;
    v834 = *(int64_t*)v833;
    v835 = 113LL;
    v836 = v834 == v835;
    if (v836) goto bb230;
    else goto bb231;
  bb230:
    v838 = _str115;
    p_call_stack_pop();
    return v838;
  bb231:
    v840 = (char*)v1 + 0;
    v841 = *(int64_t*)v840;
    v842 = 114LL;
    v843 = v841 == v842;
    if (v843) goto bb232;
    else goto bb233;
  bb232:
    v845 = _str116;
    p_call_stack_pop();
    return v845;
  bb233:
    v847 = (char*)v1 + 0;
    v848 = *(int64_t*)v847;
    v849 = 115LL;
    v850 = v848 == v849;
    if (v850) goto bb234;
    else goto bb235;
  bb234:
    v852 = _str117;
    p_call_stack_pop();
    return v852;
  bb235:
    v854 = (char*)v1 + 0;
    v855 = *(int64_t*)v854;
    v856 = 116LL;
    v857 = v855 == v856;
    if (v857) goto bb236;
    else goto bb237;
  bb236:
    v859 = _str118;
    p_call_stack_pop();
    return v859;
  bb237:
    v861 = (char*)v1 + 0;
    v862 = *(int64_t*)v861;
    v863 = 117LL;
    v864 = v862 == v863;
    if (v864) goto bb238;
    else goto bb239;
  bb238:
    v866 = _str119;
    p_call_stack_pop();
    return v866;
  bb239:
    v868 = (char*)v1 + 0;
    v869 = *(int64_t*)v868;
    v870 = 118LL;
    v871 = v869 == v870;
    if (v871) goto bb240;
    else goto bb241;
  bb240:
    v873 = _str120;
    p_call_stack_pop();
    return v873;
  bb241:
    v875 = (char*)v1 + 0;
    v876 = *(int64_t*)v875;
    v877 = 119LL;
    v878 = v876 == v877;
    if (v878) goto bb242;
    else goto bb243;
  bb242:
    v880 = _str121;
    p_call_stack_pop();
    return v880;
  bb243:
    v882 = (char*)v1 + 0;
    v883 = *(int64_t*)v882;
    v884 = 120LL;
    v885 = v883 == v884;
    if (v885) goto bb244;
    else goto bb245;
  bb244:
    v887 = _str122;
    p_call_stack_pop();
    return v887;
  bb245:
    v889 = (char*)v1 + 0;
    v890 = *(int64_t*)v889;
    v891 = 121LL;
    v892 = v890 == v891;
    if (v892) goto bb246;
    else goto bb247;
  bb246:
    v894 = _str123;
    p_call_stack_pop();
    return v894;
  bb247:
    v896 = (char*)v1 + 0;
    v897 = *(int64_t*)v896;
    v898 = 122LL;
    v899 = v897 == v898;
    if (v899) goto bb248;
    else goto bb249;
  bb248:
    v901 = _str124;
    p_call_stack_pop();
    return v901;
  bb249:
    v903 = (char*)v1 + 0;
    v904 = *(int64_t*)v903;
    v905 = 123LL;
    v906 = v904 == v905;
    if (v906) goto bb250;
    else goto bb251;
  bb250:
    v908 = _str125;
    p_call_stack_pop();
    return v908;
  bb251:
    v910 = (char*)v1 + 0;
    v911 = *(int64_t*)v910;
    v912 = 124LL;
    v913 = v911 == v912;
    if (v913) goto bb252;
    else goto bb253;
  bb252:
    v915 = _str126;
    p_call_stack_pop();
    return v915;
  bb253:
    v917 = (char*)v1 + 0;
    v918 = *(int64_t*)v917;
    v919 = 125LL;
    v920 = v918 == v919;
    if (v920) goto bb254;
    else goto bb255;
  bb254:
    v922 = _str127;
    p_call_stack_pop();
    return v922;
  bb255:
    v924 = (char*)v1 + 0;
    v925 = *(int64_t*)v924;
    v926 = 126LL;
    v927 = v925 == v926;
    if (v927) goto bb256;
    else goto bb257;
  bb256:
    v929 = _str128;
    p_call_stack_pop();
    return v929;
  bb257:
    v931 = (char*)v1 + 0;
    v932 = *(int64_t*)v931;
    v933 = 127LL;
    v934 = v932 == v933;
    if (v934) goto bb258;
    else goto bb259;
  bb258:
    v936 = _str129;
    p_call_stack_pop();
    return v936;
  bb259:
    v938 = (char*)v1 + 0;
    v939 = *(int64_t*)v938;
    v940 = 128LL;
    v941 = v939 == v940;
    if (v941) goto bb260;
    else goto bb261;
  bb260:
    v943 = _str130;
    p_call_stack_pop();
    return v943;
  bb261:
    v945 = (char*)v1 + 0;
    v946 = *(int64_t*)v945;
    v947 = 129LL;
    v948 = v946 == v947;
    if (v948) goto bb262;
    else goto bb263;
  bb262:
    v950 = _str131;
    p_call_stack_pop();
    return v950;
  bb263:
    v952 = (char*)v1 + 0;
    v953 = *(int64_t*)v952;
    v954 = 130LL;
    v955 = v953 == v954;
    if (v955) goto bb264;
    else goto bb265;
  bb264:
    v957 = _str132;
    p_call_stack_pop();
    return v957;
  bb265:
    v959 = (char*)v1 + 0;
    v960 = *(int64_t*)v959;
    v961 = 131LL;
    v962 = v960 == v961;
    if (v962) goto bb266;
    else goto bb267;
  bb266:
    v964 = _str133;
    p_call_stack_pop();
    return v964;
  bb267:
    v966 = (char*)v1 + 0;
    v967 = *(int64_t*)v966;
    v968 = 132LL;
    v969 = v967 == v968;
    if (v969) goto bb268;
    else goto bb269;
  bb268:
    v971 = _str134;
    p_call_stack_pop();
    return v971;
  bb269:
    v973 = (char*)v1 + 0;
    v974 = *(int64_t*)v973;
    v975 = 133LL;
    v976 = v974 == v975;
    if (v976) goto bb270;
    else goto bb271;
  bb270:
    v978 = _str135;
    p_call_stack_pop();
    return v978;
  bb271:
    goto bb1;
    return 0;
}

void Lexer_read_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_string");
    int64_t v192; /* placeholder for void */
    int64_t v18;
    bool v36;
    void* v188;
    void* v122;
    void* v141;
    int64_t v155;
    int64_t v186;
    void* v13;
    int64_t v47; /* placeholder for void */
    int64_t v190;
    int64_t v16; /* placeholder for void */
    int64_t v154;
    void* v135;
    int64_t v191; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    void* v103;
    int64_t v6; /* placeholder for void */
    bool v27;
    int64_t v38;
    int64_t v21;
    int64_t v183; /* placeholder for void */
    void* v88;
    int64_t v55;
    int64_t v131; /* placeholder for void */
    void* v69;
    int64_t v172;
    int64_t v151; /* placeholder for void */
    int64_t v132; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    void* v91;
    void* v144;
    int64_t v138; /* placeholder for void */
    int64_t v123;
    int64_t v40;
    void* v106;
    void* v171;
    int64_t v108;
    void* v3;
    int64_t v92; /* placeholder for void */
    void* v9;
    int64_t v148; /* placeholder for void */
    void* v164;
    int64_t v67; /* placeholder for void */
    void* v51;
    int64_t v30;
    int64_t v25;
    int64_t v15; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v99;
    void* v137;
    int64_t v125; /* placeholder for void */
    void* v101;
    int64_t v145;
    int64_t v105; /* placeholder for void */
    int64_t v149;
    int64_t v72; /* placeholder for void */
    void* v119;
    void* v78;
    int64_t v159;
    int64_t v168; /* placeholder for void */
    void* v107;
    int64_t v26;
    bool v147;
    int64_t v53; /* placeholder for void */
    int64_t v85;
    int64_t v102;
    void* v114;
    int64_t v157;
    bool v46;
    int64_t v112;
    void* v48;
    int64_t v35;
    int64_t v45;
    void* v49;
    bool v56;
    int64_t v87; /* placeholder for void */
    void* v181;
    int64_t v179; /* placeholder for void */
    void* v133;
    int64_t v93; /* placeholder for void */
    int64_t v146;
    int64_t v139; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v50;
    int64_t v64;
    void* v20;
    bool v96;
    int64_t v44;
    int64_t v84;
    int64_t v129; /* placeholder for void */
    void* v180;
    void* v189;
    int64_t v134;
    void* v166;
    int64_t v63; /* placeholder for void */
    int64_t v65;
    int64_t v128; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v12;
    int64_t v170;
    void* v153;
    void* v177;
    void* v143;
    int64_t v121;
    void* v19;
    void* v68;
    void* v5;
    int64_t v160; /* placeholder for void */
    int64_t v34;
    int64_t v54;
    int64_t v187; /* placeholder for void */
    int64_t v24;
    void* v152;
    int64_t v43; /* placeholder for void */
    int64_t v94;
    int64_t v136;
    int64_t v163;
    int64_t v165; /* placeholder for void */
    void* v58;
    bool v86;
    int64_t v11;
    int64_t v52; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v59;
    int64_t v80;
    int64_t v120;
    bool v23;
    int64_t v75;
    int64_t v82; /* placeholder for void */
    int64_t v95;
    void* v113;
    int64_t v130; /* placeholder for void */
    int64_t v142;
    int64_t v60;
    bool v66;
    int64_t v150;
    void* v158;
    int64_t v115;
    void* v175;
    void* v176;
    int64_t v173; /* placeholder for void */
    int64_t v117;
    int64_t v140; /* placeholder for void */
    void* v32;
    bool v76;
    int64_t v126; /* placeholder for void */
    int64_t v41;
    void* v7;
    int64_t v73; /* placeholder for void */
    void* v42;
    void* v14;
    int64_t v31;
    int64_t v29; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    void* v61;
    int64_t v74;
    void* v89;
    int64_t v109;
    void* v110;
    void* v71;
    int64_t v70;
    void* v116;
    void* v156;
    int64_t v161; /* placeholder for void */
    int64_t v22;
    void* v17;
    void* v79;
    void* v98;
    void* v124;
    void* v162;
    void* v167;
    void* v174;
    bool v28;
    int64_t v90;
    int64_t v182;
    int64_t v4;
    int64_t v77; /* placeholder for void */
    void* v81;
    int64_t v169;
    int64_t v118; /* placeholder for void */
    int64_t v39;
    void* v184;
    int64_t v178;
    void* v185;
    int64_t v111;
    int64_t v127; /* placeholder for void */
    int64_t v100;
    int64_t v8;
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Lexer_advance((void*)v11, v2);
    v13 = _str137;
    v14 = malloc(8);
    *(void**)v14 = (void*)v13;
    goto bb1;
  bb1:
    v17 = (char*)v2 + 8;
    v18 = *(int64_t*)v17;
    v19 = (char*)v2 + 0;
    v20 = *(void**)v19;
    v21 = 0LL;
    v22 = (int64_t)str_len((void*)v21, v20);
    v23 = v18 < v22;
    v24 = 0LL;
    v25 = (int64_t)Lexer_peek((void*)v24, v2);
    v26 = 34LL;
    v27 = v25 != v26;
    v28 = v23 && v27;
    if (v28) goto bb2;
    else goto bb3;
  bb2:
    v30 = 0LL;
    v31 = (int64_t)Lexer_peek((void*)v30, v2);
    v32 = malloc(8);
    *(int64_t*)v32 = (int64_t)v31;
    v34 = *(int64_t*)v32;
    v35 = 92LL;
    v36 = v34 == v35;
    if (v36) goto bb4;
    else goto bb5;
  bb3:
    v141 = (char*)v2 + 8;
    v142 = *(int64_t*)v141;
    v143 = (char*)v2 + 0;
    v144 = *(void**)v143;
    v145 = 0LL;
    v146 = (int64_t)str_len((void*)v145, v144);
    v147 = v142 < v146;
    if (v147) goto bb25;
    else goto bb26;
  bb4:
    v38 = 0LL;
    v39 = (int64_t)Lexer_advance((void*)v38, v2);
    v40 = 0LL;
    v41 = (int64_t)Lexer_advance((void*)v40, v2);
    v42 = malloc(8);
    *(int64_t*)v42 = (int64_t)v41;
    v44 = *(int64_t*)v42;
    v45 = 110LL;
    v46 = v44 == v45;
    if (v46) goto bb7;
    else goto bb8;
  bb5:
    v133 = *(void**)v14;
    v134 = 0LL;
    v135 = Lexer_read_utf8((void*)v134, v2);
    v136 = 0LL;
    v137 = str_concat((void*)v136, v133, v135);
    *(void**)v14 = (void*)v137;
    goto bb6;
  bb6:
    goto bb1;
  bb7:
    v48 = *(void**)v14;
    v49 = _str139;
    v50 = 0LL;
    v51 = str_concat((void*)v50, v48, v49);
    *(void**)v14 = (void*)v51;
    goto bb9;
  bb8:
    v54 = *(int64_t*)v42;
    v55 = 116LL;
    v56 = v54 == v55;
    if (v56) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v58 = *(void**)v14;
    v59 = _str242;
    v60 = 0LL;
    v61 = str_concat((void*)v60, v58, v59);
    *(void**)v14 = (void*)v61;
    goto bb12;
  bb11:
    v64 = *(int64_t*)v42;
    v65 = 114LL;
    v66 = v64 == v65;
    if (v66) goto bb13;
    else goto bb14;
  bb12:
    goto bb9;
  bb13:
    v68 = *(void**)v14;
    v69 = _str243;
    v70 = 0LL;
    v71 = str_concat((void*)v70, v68, v69);
    *(void**)v14 = (void*)v71;
    goto bb15;
  bb14:
    v74 = *(int64_t*)v42;
    v75 = 92LL;
    v76 = v74 == v75;
    if (v76) goto bb16;
    else goto bb17;
  bb15:
    goto bb12;
  bb16:
    v78 = *(void**)v14;
    v79 = _str244;
    v80 = 0LL;
    v81 = str_concat((void*)v80, v78, v79);
    *(void**)v14 = (void*)v81;
    goto bb18;
  bb17:
    v84 = *(int64_t*)v42;
    v85 = 34LL;
    v86 = v84 == v85;
    if (v86) goto bb19;
    else goto bb20;
  bb18:
    goto bb15;
  bb19:
    v88 = *(void**)v14;
    v89 = _str64;
    v90 = 0LL;
    v91 = str_concat((void*)v90, v88, v89);
    *(void**)v14 = (void*)v91;
    goto bb21;
  bb20:
    v94 = *(int64_t*)v42;
    v95 = 48LL;
    v96 = v94 == v95;
    if (v96) goto bb22;
    else goto bb23;
  bb21:
    goto bb18;
  bb22:
    v98 = *(void**)v14;
    v99 = 0LL;
    v100 = 0LL;
    v101 = byte_to_str((void*)v100, v99);
    v102 = 0LL;
    v103 = str_concat((void*)v102, v98, v101);
    *(void**)v14 = (void*)v103;
    goto bb24;
  bb23:
    v106 = (char*)v2 + 32;
    v107 = *(void**)v106;
    v108 = *(int64_t*)v5;
    v109 = *(int64_t*)v9;
    v110 = _str245;
    v111 = *(int64_t*)v42;
    v112 = 0LL;
    v113 = byte_to_str((void*)v112, v111);
    v114 = p_str_concat(v110, v113);
    v115 = 0LL;
    v116 = lex_error((void*)v115, v108, v109, v114);
    v117 = 0LL;
    Vec_push((void*)v117, v107, (int64_t)v116);
    v119 = *(void**)v14;
    v120 = *(int64_t*)v42;
    v121 = 0LL;
    v122 = byte_to_str((void*)v121, v120);
    v123 = 0LL;
    v124 = str_concat((void*)v123, v119, v122);
    *(void**)v14 = (void*)v124;
    goto bb24;
  bb24:
    goto bb21;
  bb25:
    v149 = 0LL;
    v150 = (int64_t)Lexer_advance((void*)v149, v2);
    goto bb27;
  bb26:
    v152 = (char*)v2 + 32;
    v153 = *(void**)v152;
    v154 = *(int64_t*)v5;
    v155 = *(int64_t*)v9;
    v156 = _str246;
    v157 = 0LL;
    v158 = lex_error((void*)v157, v154, v155, v156);
    v159 = 0LL;
    Vec_push((void*)v159, v153, (int64_t)v158);
    goto bb27;
  bb27:
    v162 = malloc(16);
    v163 = 78LL;
    v164 = (char*)v162 + 0;
    *(int64_t*)v164 = (int64_t)v163;
    v166 = *(void**)v14;
    v167 = (char*)v162 + 8;
    *(void**)v167 = (void*)v166;
    v169 = *(int64_t*)v5;
    v170 = *(int64_t*)v9;
    v171 = malloc(32);
    v172 = 0LL;
    token_new(v171, (void*)v172, v162, v169, v170);
    v174 = (char*)v171 + 0;
    v175 = (char*)v0 + 0;
    v176 = (char*)v174 + 0;
    v177 = (char*)v175 + 0;
    v178 = *(int64_t*)v176;
    *(int64_t*)v177 = (int64_t)v178;
    v180 = (char*)v174 + 8;
    v181 = (char*)v175 + 8;
    v182 = *(int64_t*)v180;
    *(int64_t*)v181 = (int64_t)v182;
    v184 = (char*)v171 + 16;
    v185 = (char*)v0 + 16;
    v186 = *(int64_t*)v184;
    *(int64_t*)v185 = (int64_t)v186;
    v188 = (char*)v171 + 24;
    v189 = (char*)v0 + 24;
    v190 = *(int64_t*)v188;
    *(int64_t*)v189 = (int64_t)v190;
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_clone(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_clone");
    bool v41;
    int64_t v106; /* placeholder for void */
    void* v60;
    int64_t v44;
    int64_t v104; /* placeholder for void */
    int64_t v109;
    int64_t v78;
    int64_t v30;
    bool v38;
    bool v42;
    int64_t v46; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    void* v87;
    int64_t v108;
    int64_t v100;
    int64_t v12;
    int64_t v54; /* placeholder for void */
    int64_t v6;
    int64_t v36;
    int64_t v47;
    void* v96;
    int64_t v28; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v62;
    int64_t v77;
    int64_t v112;
    int64_t v2;
    int64_t v27;
    int64_t v91; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    void* v63;
    int64_t v113; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    void* v26;
    int64_t v43; /* placeholder for void */
    int64_t v76;
    int64_t v97;
    bool v75;
    int64_t v82;
    int64_t v67; /* placeholder for void */
    int64_t v74;
    int64_t v57;
    int64_t v111; /* placeholder for void */
    int64_t v65;
    int64_t v22;
    int64_t v88; /* placeholder for void */
    int64_t v84;
    int64_t v99; /* placeholder for void */
    int64_t v81; /* placeholder for void */
    int64_t v13;
    int64_t v34; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    bool v79;
    int64_t v89;
    int64_t v94; /* placeholder for void */
    int64_t v83;
    void* v29;
    int64_t v92;
    int64_t v70;
    int64_t v21; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    bool v53;
    int64_t v59;
    int64_t v40;
    int64_t v55;
    int64_t v101;
    int64_t v52;
    bool v80;
    int64_t v19;
    int64_t v103;
    int64_t v7;
    int64_t v51;
    int64_t v37;
    void* v14;
    int64_t v110; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v73;
    void* v90;
    void* v17;
    int64_t v56;
    void* v4;
    int64_t v72; /* placeholder for void */
    void* v10;
    int64_t v107;
    void* v23;
    void* v8;
    int64_t v3;
    void* v32;
    int64_t v18; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    void* v69;
    int64_t v85;
    int64_t v86;
    void* v93;
    int64_t v64; /* placeholder for void */
    int64_t v33;
    int64_t v102;
    int64_t v98; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v16;
    int64_t v39;
    int64_t v58;
    void* v20;
    int64_t v49; /* placeholder for void */
    void* v48;
    void* v66;
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

bool char_is_digit(void* v0, int64_t v1) {
    p_call_stack_push("char_is_digit");
    int64_t v2;
    bool v3;
    int64_t v4;
    bool v6;
    int64_t v7; /* placeholder for void */
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

void* Vec_flat_map(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_flat_map");
    int64_t v5; /* placeholder for void */
    int64_t v4;
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)vec_flat_map((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

int64_t Option_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Option_unwrap_or");
    int64_t v7; /* placeholder for void */
    int64_t v4;
    int64_t v15; /* placeholder for void */
    void* v11;
    int64_t v10; /* placeholder for void */
    int64_t v18;
    int64_t v19; /* placeholder for void */
    int64_t v5;
    void* v8;
    bool v14;
    int64_t v13;
    int64_t v12;
    int64_t v17; /* placeholder for void */
    void* v3;
    int64_t v9;
    bool v6;
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

void* Vec_map(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_map");
    int64_t v21;
    int64_t v20;
    int64_t v33;
    int64_t v13; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v24;
    int64_t v15;
    void* v8;
    int64_t v25;
    int64_t v31; /* placeholder for void */
    int64_t v30;
    int64_t v32;
    void* v37;
    void* v26;
    int64_t v38; /* placeholder for void */
    void* v4;
    int64_t v34;
    int64_t v10;
    void* v22;
    int64_t v18; /* placeholder for void */
    int64_t v36; /* placeholder for void */
    int64_t v29;
    int64_t v16;
    void* v28;
    int64_t v6; /* placeholder for void */
    bool v17;
    int64_t v14; /* placeholder for void */
    int64_t v7;
    int64_t v3;
    int64_t v19;
    void* v5;
    void* v12;
    int64_t v35; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v11;
    int64_t v9; /* placeholder for void */
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v25 = StringBuilder_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v25 = Vec_first((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v25 = HashMap_values((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v25 = char_to_upper((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v25 = Vec_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v25 = String_to_string((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v25 = str_parse_hex((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v25 = Lexer_peek((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v25 = token_kind_tag((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v25 = vec_enumerate((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v25 = Vec_capacity((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v25 = allocator_bytes_used((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v25 = Lexer_advance((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v25 = Vec_pop((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v25 = p_str_hash((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v25 = String_clone((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v25 = HashMap_keys((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v25 = str_parse_int((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v25 = HashMap_clone((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v25 = String_to_lowercase((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v25 = String_parse_int((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v25 = String_trim((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v25 = String_bytes((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v25 = Option_unwrap((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v25 = String_from((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v25 = String_to_uppercase((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v25 = char_to_lower((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v25 = HashMap_len((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v25 = String_len((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v25 = p_abs((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v25 = Vec_last((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v25 = ByteBuffer_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v25 = p_str_length((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v25 = str_len((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v25 = Result_unwrap((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v25 = String_char_count((void*)v2, (int64_t)v24);
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

int64_t String_to_uppercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_uppercase");
    int64_t v2;
    int64_t v20;
    int64_t v21;
    int64_t v23; /* placeholder for void */
    bool v36;
    int64_t v64;
    int64_t v69;
    int64_t v89; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v17;
    int64_t v29; /* placeholder for void */
    int64_t v39;
    void* v13;
    int64_t v70;
    int64_t v61;
    int64_t v16; /* placeholder for void */
    bool v63;
    int64_t v4; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v93; /* placeholder for void */
    void* v54;
    void* v51;
    void* v94;
    int64_t v14;
    int64_t v38;
    int64_t v18; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v34;
    int64_t v77;
    int64_t v25;
    bool v66;
    int64_t v80;
    void* v40;
    int64_t v52; /* placeholder for void */
    bool v67;
    int64_t v46; /* placeholder for void */
    int64_t v71;
    int64_t v56; /* placeholder for void */
    void* v27;
    int64_t v59; /* placeholder for void */
    void* v48;
    int64_t v73; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v53;
    int64_t v72; /* placeholder for void */
    int64_t v5;
    int64_t v74; /* placeholder for void */
    int64_t v81;
    int64_t v24;
    int64_t v35;
    int64_t v50;
    uint8_t v58;
    void* v28;
    int64_t v86;
    void* v88;
    void* v91;
    void* v22;
    int64_t v26;
    int64_t v41; /* placeholder for void */
    void* v76;
    void* v45;
    int64_t v92; /* placeholder for void */
    int64_t v95;
    int64_t v30;
    int64_t v33; /* placeholder for void */
    int64_t v43;
    int64_t v60; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v65;
    int64_t v82;
    int64_t v85;
    void* v57;
    void* v6;
    void* v10;
    int64_t v75; /* placeholder for void */
    int64_t v87;
    int64_t v55; /* placeholder for void */
    int64_t v9;
    int64_t v47;
    int64_t v62;
    int64_t v84; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    void* v7;
    int64_t v49; /* placeholder for void */
    int64_t v90;
    int64_t v78; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    bool v3;
    int64_t v42;
    void* v31;
    int64_t v12; /* placeholder for void */
    int64_t v98;
    int64_t v44;
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

void Result_map_err(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map_err");
    void* v15;
    void* v18;
    int64_t v6;
    int64_t v24; /* placeholder for void */
    void* v26;
    void* v31;
    int64_t v32;
    int64_t v37;
    int64_t v8; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v40;
    int64_t v42;
    void* v44;
    int64_t v10;
    void* v35;
    void* v4;
    void* v41;
    bool v29;
    int64_t v46;
    int64_t v19;
    void* v22;
    int64_t v28;
    int64_t v25; /* placeholder for void */
    void* v33;
    void* v17;
    int64_t v36; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    void* v38;
    int64_t v16; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v11;
    void* v9;
    int64_t v49; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v13;
    int64_t v5;
    int64_t v34;
    void* v45;
    int64_t v27;
    int64_t v48; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v23;
    bool v7;
    int64_t v12;
    void* v21;
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v37 = StringBuilder_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v37 = Vec_first((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v37 = HashMap_values((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v37 = char_to_upper((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v37 = Vec_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v37 = String_to_string((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v37 = str_parse_hex((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v37 = Lexer_peek((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v37 = token_kind_tag((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v37 = vec_enumerate((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v37 = Vec_capacity((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v37 = allocator_bytes_used((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v37 = Lexer_advance((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v37 = Vec_pop((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v37 = p_str_hash((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v37 = String_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v37 = HashMap_keys((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v37 = str_parse_int((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v37 = HashMap_clone((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v37 = String_to_lowercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v37 = String_parse_int((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v37 = String_trim((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v37 = String_bytes((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v37 = Option_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v37 = String_from((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v37 = String_to_uppercase((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v37 = char_to_lower((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v37 = HashMap_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v37 = String_len((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v37 = p_abs((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v37 = Vec_last((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v37 = ByteBuffer_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v37 = p_str_length((void*)v3, (int64_t)v32);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v37 = str_len((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v37 = Result_unwrap((void*)v3, (void*)v32);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v37 = String_char_count((void*)v3, (int64_t)v32);
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

int64_t StringBuilder_len(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_len");
    int64_t v3;
    void* v2;
    int64_t v4; /* placeholder for void */
    v2 = (char*)v1 + 8;
    v3 = *(int64_t*)v2;
    p_call_stack_pop();
    return v3;
    return 0;
}

bool StringBuilder_is_empty(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_is_empty");
    void* v2;
    int64_t v3;
    int64_t v4;
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

void ByteBuffer_new(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("ByteBuffer_new");
    void* v6;
    void* v12;
    int64_t v4;
    int64_t v18; /* placeholder for void */
    void* v3;
    int64_t v8;
    void* v9;
    int64_t v14; /* placeholder for void */
    void* v15;
    int64_t v7; /* placeholder for void */
    int64_t v17;
    void* v11;
    void* v5;
    void* v16;
    int64_t v10; /* placeholder for void */
    void* v13;
    int64_t v19; /* placeholder for void */
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

void* Vec_filter(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_filter");
    int64_t v30; /* placeholder for void */
    int64_t v28;
    void* v5;
    int64_t v6; /* placeholder for void */
    int64_t v29;
    int64_t v18; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v34;
    int64_t v9; /* placeholder for void */
    int64_t v3;
    void* v12;
    int64_t v24;
    void* v38;
    int64_t v39; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    bool v17;
    int64_t v13; /* placeholder for void */
    int64_t v35;
    int64_t v37; /* placeholder for void */
    int64_t v16;
    int64_t v36; /* placeholder for void */
    int64_t v7;
    void* v8;
    int64_t v19;
    int64_t v26; /* placeholder for void */
    int64_t v10;
    void* v4;
    int64_t v14; /* placeholder for void */
    int64_t v21;
    int64_t v31; /* placeholder for void */
    int64_t v15;
    bool v25;
    int64_t v11;
    void* v27;
    int64_t v20;
    void* v22;
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
        if (strcmp(_fn_name, "char_is_whitespace") == 0) {
            v25 = char_is_whitespace((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "lex_is_digit") == 0) {
            v25 = lex_is_digit((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "lex_is_hexdigit") == 0) {
            v25 = lex_is_hexdigit((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "lex_is_whitespace") == 0) {
            v25 = lex_is_whitespace((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_is_ok") == 0) {
            v25 = Result_is_ok((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_is_digit") == 0) {
            v25 = char_is_digit((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "char_is_upper") == 0) {
            v25 = char_is_upper((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "ByteBuffer_is_empty") == 0) {
            v25 = ByteBuffer_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "str_is_empty") == 0) {
            v25 = str_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "is_word_keyword") == 0) {
            v25 = is_word_keyword((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_is_alpha") == 0) {
            v25 = char_is_alpha((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Result_is_err") == 0) {
            v25 = Result_is_err((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "Option_is_none") == 0) {
            v25 = Option_is_none((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "char_is_lower") == 0) {
            v25 = char_is_lower((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Vec_is_empty") == 0) {
            v25 = Vec_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "lex_is_alpha") == 0) {
            v25 = lex_is_alpha((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "HashMap_is_empty") == 0) {
            v25 = HashMap_is_empty((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "StringBuilder_is_empty") == 0) {
            v25 = StringBuilder_is_empty((void*)v2, (void*)v24);
        } else if (strcmp(_fn_name, "String_is_empty") == 0) {
            v25 = String_is_empty((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "lex_is_alphanumeric") == 0) {
            v25 = lex_is_alphanumeric((void*)v2, (int64_t)v24);
        } else if (strcmp(_fn_name, "Option_is_some") == 0) {
            v25 = Option_is_some((void*)v2, (void*)v24);
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

void* Lexer_tokenize(void* v0, void* v1) {
    p_call_stack_push("Lexer_tokenize");
    void* v158;
    bool v94;
    void* v3;
    int64_t v72;
    void* v149;
    int64_t v8;
    int64_t v68; /* placeholder for void */
    void* v101;
    bool v60;
    int64_t v143; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    void* v63;
    int64_t v32;
    int64_t v141; /* placeholder for void */
    void* v62;
    int64_t v88; /* placeholder for void */
    bool v120;
    void* v7;
    void* v122;
    int64_t v70;
    int64_t v138; /* placeholder for void */
    void* v153;
    bool v59;
    int64_t v50; /* placeholder for void */
    int64_t v51; /* placeholder for void */
    int64_t v156;
    int64_t v40;
    int64_t v77;
    int64_t v35; /* placeholder for void */
    bool v13;
    int64_t v14; /* placeholder for void */
    int64_t v114; /* placeholder for void */
    void* v4;
    int64_t v20;
    int64_t v49;
    int64_t v131;
    int64_t v56;
    int64_t v124;
    int64_t v69;
    int64_t v24;
    void* v10;
    int64_t v23;
    int64_t v118;
    void* v144;
    bool v97;
    int64_t v107;
    int64_t v55;
    bool v82;
    int64_t v93;
    bool v81;
    void* v100;
    int64_t v105; /* placeholder for void */
    int64_t v117; /* placeholder for void */
    bool v47;
    int64_t v11;
    int64_t v99; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v78;
    int64_t v137; /* placeholder for void */
    bool v41;
    int64_t v102;
    int64_t v146;
    int64_t v67; /* placeholder for void */
    int64_t v127; /* placeholder for void */
    int64_t v116; /* placeholder for void */
    bool v46;
    int64_t v64;
    bool v33;
    int64_t v139; /* placeholder for void */
    bool v34;
    void* v85;
    void* v130;
    void* v111;
    bool v71;
    int64_t v92;
    int64_t v53;
    int64_t v2;
    int64_t v16;
    int64_t v103; /* placeholder for void */
    int64_t v104;
    int64_t v121; /* placeholder for void */
    int64_t v36;
    int64_t v128; /* placeholder for void */
    int64_t v119;
    int64_t v136; /* placeholder for void */
    int64_t v140; /* placeholder for void */
    void* v123;
    int64_t v5; /* placeholder for void */
    int64_t v15;
    bool v54;
    int64_t v73;
    int64_t v110; /* placeholder for void */
    int64_t v126;
    int64_t v142; /* placeholder for void */
    int64_t v148; /* placeholder for void */
    void* v86;
    void* v151;
    int64_t v79;
    int64_t v91; /* placeholder for void */
    int64_t v157; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    bool v28;
    int64_t v87;
    int64_t v155; /* placeholder for void */
    int64_t v80;
    bool v76;
    int64_t v31;
    void* v112;
    int64_t v39;
    void* v145;
    int64_t v106; /* placeholder for void */
    void* v17;
    bool v83;
    int64_t v108;
    int64_t v154;
    int64_t v150;
    int64_t v132; /* placeholder for void */
    int64_t v58;
    int64_t v22; /* placeholder for void */
    int64_t v74;
    int64_t v75;
    int64_t v90; /* placeholder for void */
    int64_t v26;
    int64_t v96;
    int64_t v95;
    int64_t v52;
    int64_t v89;
    bool v109;
    int64_t v44;
    int64_t v66;
    int64_t v125; /* placeholder for void */
    int64_t v159; /* placeholder for void */
    int64_t v57;
    int64_t v6; /* placeholder for void */
    int64_t v29;
    int64_t v135; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v43;
    int64_t v42;
    int64_t v134; /* placeholder for void */
    int64_t v30;
    void* v129;
    int64_t v19;
    int64_t v152;
    int64_t v133;
    int64_t v12;
    int64_t v61; /* placeholder for void */
    int64_t v45;
    int64_t v65; /* placeholder for void */
    bool v98;
    void* v9;
    int64_t v113;
    bool v21;
    void* v147;
    int64_t v27;
    int64_t v115;
    v2 = 0LL;
    v3 = Vec_new((void*)v2);
    v4 = malloc(8);
    *(void**)v4 = (void*)v3;
    goto bb1;
  bb1:
    v7 = (char*)v1 + 8;
    v8 = *(int64_t*)v7;
    v9 = (char*)v1 + 0;
    v10 = *(void**)v9;
    v11 = 0LL;
    v12 = (int64_t)str_len((void*)v11, v10);
    v13 = v8 < v12;
    if (v13) goto bb2;
    else goto bb3;
  bb2:
    v15 = 0LL;
    v16 = (int64_t)Lexer_peek((void*)v15, v1);
    v17 = malloc(8);
    *(int64_t*)v17 = (int64_t)v16;
    v19 = *(int64_t*)v17;
    v20 = 0LL;
    v21 = (bool)lex_is_whitespace((void*)v20, v19);
    if (v21) goto bb4;
    else goto bb5;
  bb3:
    v144 = *(void**)v4;
    v145 = malloc(16);
    v146 = 134LL;
    v147 = (char*)v145 + 0;
    *(int64_t*)v147 = (int64_t)v146;
    v149 = (char*)v1 + 16;
    v150 = *(int64_t*)v149;
    v151 = (char*)v1 + 24;
    v152 = *(int64_t*)v151;
    v153 = malloc(32);
    v154 = 0LL;
    token_new(v153, (void*)v154, v145, v150, v152);
    v156 = 0LL;
    Vec_push((void*)v156, v144, (int64_t)v153);
    v158 = *(void**)v4;
    p_call_stack_pop();
    return v158;
  bb4:
    v23 = 0LL;
    v24 = (int64_t)Lexer_advance((void*)v23, v1);
    goto bb6;
  bb5:
    v26 = *(int64_t*)v17;
    v27 = 47LL;
    v28 = v26 == v27;
    v29 = 1LL;
    v30 = 0LL;
    v31 = (int64_t)Lexer_peek_at((void*)v30, v1, v29);
    v32 = 47LL;
    v33 = v31 == v32;
    v34 = v28 && v33;
    if (v34) goto bb7;
    else goto bb8;
  bb6:
    goto bb1;
  bb7:
    v36 = 0LL;
    Lexer_skip_line_comment((void*)v36, v1);
    goto bb9;
  bb8:
    v39 = *(int64_t*)v17;
    v40 = 47LL;
    v41 = v39 == v40;
    v42 = 1LL;
    v43 = 0LL;
    v44 = (int64_t)Lexer_peek_at((void*)v43, v1, v42);
    v45 = 42LL;
    v46 = v44 == v45;
    v47 = v41 && v46;
    if (v47) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v49 = 0LL;
    Lexer_skip_block_comment((void*)v49, v1);
    goto bb12;
  bb11:
    v52 = *(int64_t*)v17;
    v53 = 102LL;
    v54 = v52 == v53;
    v55 = 1LL;
    v56 = 0LL;
    v57 = (int64_t)Lexer_peek_at((void*)v56, v1, v55);
    v58 = 34LL;
    v59 = v57 == v58;
    v60 = v54 && v59;
    if (v60) goto bb13;
    else goto bb14;
  bb12:
    goto bb9;
  bb13:
    v62 = *(void**)v4;
    v63 = malloc(32);
    v64 = 0LL;
    Lexer_read_fstring(v63, (void*)v64, v1);
    v66 = 0LL;
    Vec_push((void*)v66, v62, (int64_t)v63);
    goto bb15;
  bb14:
    v69 = *(int64_t*)v17;
    v70 = 114LL;
    v71 = v69 == v70;
    v72 = 1LL;
    v73 = 0LL;
    v74 = (int64_t)Lexer_peek_at((void*)v73, v1, v72);
    v75 = 34LL;
    v76 = v74 == v75;
    v77 = 1LL;
    v78 = 0LL;
    v79 = (int64_t)Lexer_peek_at((void*)v78, v1, v77);
    v80 = 35LL;
    v81 = v79 == v80;
    v82 = v76 || v81;
    v83 = v71 && v82;
    if (v83) goto bb16;
    else goto bb17;
  bb15:
    goto bb12;
  bb16:
    v85 = *(void**)v4;
    v86 = malloc(32);
    v87 = 0LL;
    Lexer_read_raw_string(v86, (void*)v87, v1);
    v89 = 0LL;
    Vec_push((void*)v89, v85, (int64_t)v86);
    goto bb18;
  bb17:
    v92 = *(int64_t*)v17;
    v93 = 0LL;
    v94 = (bool)lex_is_alpha((void*)v93, v92);
    v95 = *(int64_t*)v17;
    v96 = 95LL;
    v97 = v95 == v96;
    v98 = v94 || v97;
    if (v98) goto bb19;
    else goto bb20;
  bb18:
    goto bb15;
  bb19:
    v100 = *(void**)v4;
    v101 = malloc(32);
    v102 = 0LL;
    Lexer_read_identifier_or_keyword(v101, (void*)v102, v1);
    v104 = 0LL;
    Vec_push((void*)v104, v100, (int64_t)v101);
    goto bb21;
  bb20:
    v107 = *(int64_t*)v17;
    v108 = 0LL;
    v109 = (bool)lex_is_digit((void*)v108, v107);
    if (v109) goto bb22;
    else goto bb23;
  bb21:
    goto bb18;
  bb22:
    v111 = *(void**)v4;
    v112 = malloc(32);
    v113 = 0LL;
    Lexer_read_number(v112, (void*)v113, v1);
    v115 = 0LL;
    Vec_push((void*)v115, v111, (int64_t)v112);
    goto bb24;
  bb23:
    v118 = *(int64_t*)v17;
    v119 = 34LL;
    v120 = v118 == v119;
    if (v120) goto bb25;
    else goto bb26;
  bb24:
    goto bb21;
  bb25:
    v122 = *(void**)v4;
    v123 = malloc(32);
    v124 = 0LL;
    Lexer_read_string(v123, (void*)v124, v1);
    v126 = 0LL;
    Vec_push((void*)v126, v122, (int64_t)v123);
    goto bb27;
  bb26:
    v129 = *(void**)v4;
    v130 = malloc(32);
    v131 = 0LL;
    Lexer_read_operator(v130, (void*)v131, v1);
    v133 = 0LL;
    Vec_push((void*)v133, v129, (int64_t)v130);
    goto bb27;
  bb27:
    goto bb24;
    return 0;
}

void* Vec_skip(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_skip");
    int64_t v5; /* placeholder for void */
    int64_t v4;
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)vec_skip((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

int64_t String_byte_at(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_byte_at");
    bool v4;
    int64_t v8; /* placeholder for void */
    void* v18;
    int64_t v22; /* placeholder for void */
    int64_t v36;
    int64_t v15; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v24;
    int64_t v31;
    int64_t v20;
    int64_t v32;
    int64_t v37; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v10;
    uint8_t v28;
    int64_t v38; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    void* v27;
    void* v21;
    bool v33;
    int64_t v17;
    int64_t v23;
    int64_t v6;
    int64_t v29; /* placeholder for void */
    int64_t v3;
    int64_t v39;
    int64_t v30; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    int64_t v7;
    int64_t v5; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v13;
    int64_t v12; /* placeholder for void */
    int64_t v35;
    int64_t v14;
    int64_t v25; /* placeholder for void */
    bool v11;
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

int64_t Vec_first(void* v0, void* v1) {
    p_call_stack_push("Vec_first");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_first(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

bool Result_is_ok(void* v0, void* v1) {
    p_call_stack_push("Result_is_ok");
    bool v9;
    bool v14;
    int64_t v10; /* placeholder for void */
    int64_t v3;
    void* v16;
    int64_t v15; /* placeholder for void */
    void* v2;
    int64_t v4;
    bool v21;
    void* v7;
    int64_t v20; /* placeholder for void */
    int64_t v8;
    int64_t v17;
    int64_t v22; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v13;
    int64_t v12;
    bool v18;
    int64_t v6; /* placeholder for void */
    void* v11;
    bool v5;
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

void Vec_truncate(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_truncate");
    int64_t v4; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_vec_truncate(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

void Result_ok(void* v0, void* v1, void* v2) {
    p_call_stack_push("Result_ok");
    int64_t v24; /* placeholder for void */
    void* v32;
    int64_t v42;
    int64_t v9;
    void* v3;
    void* v36;
    int64_t v4;
    void* v40;
    int64_t v35; /* placeholder for void */
    bool v28;
    int64_t v26;
    int64_t v29; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    void* v41;
    int64_t v11;
    void* v14;
    int64_t v5;
    int64_t v33;
    void* v16;
    int64_t v27;
    int64_t v15; /* placeholder for void */
    int64_t v31;
    int64_t v22;
    void* v37;
    int64_t v44; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    bool v6;
    int64_t v46; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v10;
    void* v12;
    void* v17;
    void* v34;
    int64_t v7; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    void* v20;
    int64_t v38;
    void* v8;
    void* v25;
    void* v30;
    int64_t v45; /* placeholder for void */
    int64_t v18;
    void* v21;
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

int64_t HashMap_len(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_len");
    int64_t v9; /* placeholder for void */
    int64_t v3;
    int64_t v12; /* placeholder for void */
    int64_t v14;
    int64_t v6;
    int64_t v15; /* placeholder for void */
    void* v10;
    int64_t v5; /* placeholder for void */
    void* v7;
    void* v4;
    int64_t v2;
    int64_t v8; /* placeholder for void */
    int64_t v11;
    int64_t v13; /* placeholder for void */
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

bool String_ends_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_ends_with");
    void* v37;
    int64_t v41; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v44;
    void* v55;
    int64_t v75; /* placeholder for void */
    int64_t v43;
    int64_t v72; /* placeholder for void */
    uint8_t v71;
    bool v6;
    int64_t v78;
    int64_t v28;
    int64_t v84;
    int64_t v13;
    int64_t v35;
    int64_t v36;
    int64_t v77;
    int64_t v38; /* placeholder for void */
    int64_t v3;
    int64_t v16;
    int64_t v56; /* placeholder for void */
    bool v9;
    int64_t v42; /* placeholder for void */
    bool v31;
    int64_t v11; /* placeholder for void */
    int64_t v20;
    int64_t v21;
    int64_t v63;
    void* v58;
    void* v73;
    int64_t v85;
    bool v89;
    bool v7;
    bool v22;
    int64_t v69; /* placeholder for void */
    int64_t v62; /* placeholder for void */
    int64_t v49;
    int64_t v53;
    int64_t v10; /* placeholder for void */
    void* v67;
    int64_t v68; /* placeholder for void */
    int64_t v88; /* placeholder for void */
    void* v14;
    void* v51;
    int64_t v82; /* placeholder for void */
    bool v4;
    bool v29;
    int64_t v25; /* placeholder for void */
    int64_t v23; /* placeholder for void */
    int64_t v59; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v50;
    bool v45;
    int64_t v15; /* placeholder for void */
    int64_t v57;
    int64_t v65; /* placeholder for void */
    int64_t v66;
    int64_t v39;
    bool v81;
    int64_t v83; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    void* v64;
    int64_t v46; /* placeholder for void */
    int64_t v34;
    int64_t v12;
    void* v18;
    int64_t v26; /* placeholder for void */
    int64_t v48;
    int64_t v27;
    int64_t v54;
    void* v61;
    void* v70;
    void* v40;
    int64_t v76; /* placeholder for void */
    int64_t v17;
    int64_t v80; /* placeholder for void */
    int64_t v60;
    int64_t v5;
    uint8_t v74;
    int64_t v47;
    int64_t v86;
    int64_t v87; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    bool v79;
    bool v24;
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

int64_t String_slice(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_slice");
    void* v51;
    int64_t v19;
    int64_t v63; /* placeholder for void */
    int64_t v66;
    void* v12;
    int64_t v69; /* placeholder for void */
    int64_t v77;
    int64_t v92;
    int64_t v13; /* placeholder for void */
    int64_t v96; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v74;
    int64_t v65;
    void* v26;
    int64_t v54;
    void* v58;
    void* v24;
    void* v55;
    int64_t v49; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    int64_t v38;
    bool v32;
    int64_t v33; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v76;
    int64_t v62;
    void* v9;
    int64_t v23;
    bool v5;
    void* v8;
    int64_t v14; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    bool v40;
    int64_t v70;
    int64_t v80;
    int64_t v17; /* placeholder for void */
    void* v73;
    int64_t v82;
    void* v85;
    int64_t v30;
    int64_t v56; /* placeholder for void */
    int64_t v78;
    int64_t v7;
    int64_t v93; /* placeholder for void */
    int64_t v67;
    int64_t v10; /* placeholder for void */
    void* v88;
    void* v91;
    int64_t v39;
    int64_t v72;
    int64_t v22;
    int64_t v27; /* placeholder for void */
    int64_t v34;
    int64_t v45; /* placeholder for void */
    int64_t v50;
    int64_t v89; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v95;
    int64_t v31;
    int64_t v84;
    int64_t v46;
    void* v28;
    void* v68;
    int64_t v79;
    int64_t v61; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v71;
    int64_t v18; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v29; /* placeholder for void */
    bool v48;
    int64_t v47;
    void* v52;
    int64_t v11;
    int64_t v36; /* placeholder for void */
    int64_t v60; /* placeholder for void */
    int64_t v4;
    int64_t v81; /* placeholder for void */
    int64_t v87;
    int64_t v42;
    int64_t v6; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    void* v15;
    int64_t v16;
    int64_t v83;
    int64_t v53; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v59;
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

bool char_is_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_is_lower");
    bool v5;
    int64_t v7; /* placeholder for void */
    int64_t v2;
    bool v3;
    int64_t v4;
    bool v6;
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    p_call_stack_pop();
    return v6;
    return 0;
}

void Lexer_read_fstring(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_fstring");
    bool v193;
    void* v241;
    int64_t v188;
    bool v137;
    int64_t v218; /* placeholder for void */
    void* v53;
    int64_t v10; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v77;
    int64_t v175;
    int64_t v149; /* placeholder for void */
    int64_t v240; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v56;
    int64_t v123;
    int64_t v205;
    int64_t v122;
    int64_t v169; /* placeholder for void */
    int64_t v47;
    int64_t v249; /* placeholder for void */
    void* v21;
    int64_t v55; /* placeholder for void */
    int64_t v158;
    int64_t v261;
    int64_t v203; /* placeholder for void */
    void* v174;
    int64_t v191;
    int64_t v110;
    void* v265;
    void* v150;
    void* v108;
    int64_t v243;
    bool v48;
    void* v116;
    void* v260;
    void* v112;
    int64_t v79; /* placeholder for void */
    int64_t v190; /* placeholder for void */
    int64_t v57;
    int64_t v102;
    int64_t v207;
    int64_t v132; /* placeholder for void */
    int64_t v155;
    int64_t v37;
    int64_t v280; /* placeholder for void */
    int64_t v166;
    int64_t v151;
    int64_t v69; /* placeholder for void */
    void* v16;
    int64_t v42;
    void* v269;
    void* v80;
    void* v81;
    int64_t v167;
    int64_t v12;
    void* v152;
    int64_t v13;
    int64_t v235;
    int64_t v212;
    void* v215;
    void* v278;
    int64_t v271;
    void* v232;
    int64_t v154;
    int64_t v92;
    int64_t v177;
    bool v88;
    int64_t v165; /* placeholder for void */
    int64_t v181; /* placeholder for void */
    int64_t v262; /* placeholder for void */
    int64_t v41;
    int64_t v239;
    void* v63;
    int64_t v120; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    void* v221;
    int64_t v106; /* placeholder for void */
    void* v109;
    int64_t v143;
    int64_t v186; /* placeholder for void */
    int64_t v187;
    void* v213;
    int64_t v40;
    int64_t v4;
    void* v124;
    int64_t v216; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    bool v58;
    int64_t v163;
    void* v274;
    void* v126;
    int64_t v24;
    void* v22;
    void* v90;
    void* v91;
    void* v204;
    bool v29;
    int64_t v85; /* placeholder for void */
    void* v141;
    int64_t v101;
    int64_t v279;
    int64_t v84; /* placeholder for void */
    void* v144;
    int64_t v258;
    void* v9;
    void* v255;
    int64_t v117;
    int64_t v66;
    int64_t v281; /* placeholder for void */
    int64_t v195;
    int64_t v8;
    int64_t v31; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    void* v245;
    int64_t v259;
    int64_t v136;
    void* v51;
    int64_t v252;
    bool v78;
    void* v70;
    int64_t v171;
    int64_t v250; /* placeholder for void */
    int64_t v189;
    bool v25;
    bool v68;
    void* v200;
    int64_t v138; /* placeholder for void */
    int64_t v202; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v184;
    void* v7;
    int64_t v64; /* placeholder for void */
    void* v273;
    int64_t v28;
    int64_t v33;
    int64_t v11;
    void* v100;
    int64_t v129; /* placeholder for void */
    int64_t v130; /* placeholder for void */
    void* v198;
    int64_t v220; /* placeholder for void */
    int64_t v222;
    int64_t v275;
    int64_t v161; /* placeholder for void */
    int64_t v254; /* placeholder for void */
    void* v266;
    int64_t v32;
    int64_t v104;
    int64_t v67;
    int64_t v244;
    int64_t v107; /* placeholder for void */
    int64_t v229; /* placeholder for void */
    void* v153;
    void* v19;
    int64_t v20;
    bool v156;
    int64_t v226; /* placeholder for void */
    int64_t v162;
    void* v73;
    int64_t v111;
    int64_t v26;
    int64_t v23;
    void* v230;
    void* v115;
    int64_t v43;
    int64_t v134; /* placeholder for void */
    int64_t v248;
    int64_t v267;
    void* v5;
    int64_t v140;
    int64_t v209; /* placeholder for void */
    int64_t v86;
    bool v168;
    void* v256;
    void* v121;
    void* v180;
    int64_t v36;
    int64_t v139;
    int64_t v54; /* placeholder for void */
    bool v30;
    int64_t v199;
    int64_t v257; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v238;
    int64_t v6; /* placeholder for void */
    int64_t v76;
    void* v93;
    int64_t v194; /* placeholder for void */
    void* v206;
    void* v211;
    void* v270;
    void* v225;
    int64_t v95; /* placeholder for void */
    void* v251;
    int64_t v210; /* placeholder for void */
    void* v264;
    int64_t v214;
    int64_t v268; /* placeholder for void */
    void* v83;
    void* v147;
    int64_t v127; /* placeholder for void */
    void* v164;
    int64_t v65; /* placeholder for void */
    int64_t v172;
    bool v38;
    int64_t v59; /* placeholder for void */
    int64_t v183;
    int64_t v157;
    bool v159;
    int64_t v196;
    void* v263;
    void* v15;
    int64_t v135;
    void* v242;
    int64_t v27;
    int64_t v113;
    int64_t v18; /* placeholder for void */
    int64_t v179;
    void* v208;
    void* v142;
    int64_t v173; /* placeholder for void */
    int64_t v14;
    void* v118;
    int64_t v131; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    int64_t v231;
    void* v223;
    int64_t v82;
    int64_t v246;
    int64_t v46;
    int64_t v119;
    void* v105;
    int64_t v62;
    void* v50;
    int64_t v148; /* placeholder for void */
    int64_t v217; /* placeholder for void */
    int64_t v96;
    bool v160;
    void* v178;
    int64_t v219; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v176;
    void* v34;
    bool v185;
    int64_t v224;
    void* v71;
    int64_t v201; /* placeholder for void */
    void* v277;
    int64_t v125;
    void* v103;
    int64_t v228; /* placeholder for void */
    bool v236;
    int64_t v74; /* placeholder for void */
    bool v98;
    int64_t v97;
    int64_t v146;
    void* v3;
    void* v60;
    int64_t v94; /* placeholder for void */
    int64_t v72;
    int64_t v237; /* placeholder for void */
    int64_t v276; /* placeholder for void */
    int64_t v227; /* placeholder for void */
    void* v197;
    void* v247;
    void* v44;
    void* v233;
    int64_t v52;
    void* v253;
    void* v61;
    int64_t v192;
    int64_t v49; /* placeholder for void */
    int64_t v114;
    int64_t v170;
    int64_t v272; /* placeholder for void */
    int64_t v87;
    int64_t v234;
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Lexer_advance((void*)v11, v2);
    v13 = 0LL;
    v14 = (int64_t)Lexer_advance((void*)v13, v2);
    v15 = _str137;
    v16 = malloc(8);
    *(void**)v16 = (void*)v15;
    goto bb1;
  bb1:
    v19 = (char*)v2 + 8;
    v20 = *(int64_t*)v19;
    v21 = (char*)v2 + 0;
    v22 = *(void**)v21;
    v23 = 0LL;
    v24 = (int64_t)str_len((void*)v23, v22);
    v25 = v20 < v24;
    v26 = 0LL;
    v27 = (int64_t)Lexer_peek((void*)v26, v2);
    v28 = 34LL;
    v29 = v27 != v28;
    v30 = v25 && v29;
    if (v30) goto bb2;
    else goto bb3;
  bb2:
    v32 = 0LL;
    v33 = (int64_t)Lexer_peek((void*)v32, v2);
    v34 = malloc(8);
    *(int64_t*)v34 = (int64_t)v33;
    v36 = *(int64_t*)v34;
    v37 = 92LL;
    v38 = v36 == v37;
    if (v38) goto bb4;
    else goto bb5;
  bb3:
    v230 = (char*)v2 + 8;
    v231 = *(int64_t*)v230;
    v232 = (char*)v2 + 0;
    v233 = *(void**)v232;
    v234 = 0LL;
    v235 = (int64_t)str_len((void*)v234, v233);
    v236 = v231 < v235;
    if (v236) goto bb40;
    else goto bb41;
  bb4:
    v40 = 0LL;
    v41 = (int64_t)Lexer_advance((void*)v40, v2);
    v42 = 0LL;
    v43 = (int64_t)Lexer_advance((void*)v42, v2);
    v44 = malloc(8);
    *(int64_t*)v44 = (int64_t)v43;
    v46 = *(int64_t*)v44;
    v47 = 110LL;
    v48 = v46 == v47;
    if (v48) goto bb7;
    else goto bb8;
  bb5:
    v135 = *(int64_t*)v34;
    v136 = 123LL;
    v137 = v135 == v136;
    if (v137) goto bb25;
    else goto bb26;
  bb6:
    goto bb1;
  bb7:
    v50 = *(void**)v16;
    v51 = _str139;
    v52 = 0LL;
    v53 = str_concat((void*)v52, v50, v51);
    *(void**)v16 = (void*)v53;
    goto bb9;
  bb8:
    v56 = *(int64_t*)v44;
    v57 = 116LL;
    v58 = v56 == v57;
    if (v58) goto bb10;
    else goto bb11;
  bb9:
    goto bb6;
  bb10:
    v60 = *(void**)v16;
    v61 = _str242;
    v62 = 0LL;
    v63 = str_concat((void*)v62, v60, v61);
    *(void**)v16 = (void*)v63;
    goto bb12;
  bb11:
    v66 = *(int64_t*)v44;
    v67 = 114LL;
    v68 = v66 == v67;
    if (v68) goto bb13;
    else goto bb14;
  bb12:
    goto bb9;
  bb13:
    v70 = *(void**)v16;
    v71 = _str243;
    v72 = 0LL;
    v73 = str_concat((void*)v72, v70, v71);
    *(void**)v16 = (void*)v73;
    goto bb15;
  bb14:
    v76 = *(int64_t*)v44;
    v77 = 92LL;
    v78 = v76 == v77;
    if (v78) goto bb16;
    else goto bb17;
  bb15:
    goto bb12;
  bb16:
    v80 = *(void**)v16;
    v81 = _str244;
    v82 = 0LL;
    v83 = str_concat((void*)v82, v80, v81);
    *(void**)v16 = (void*)v83;
    goto bb18;
  bb17:
    v86 = *(int64_t*)v44;
    v87 = 34LL;
    v88 = v86 == v87;
    if (v88) goto bb19;
    else goto bb20;
  bb18:
    goto bb15;
  bb19:
    v90 = *(void**)v16;
    v91 = _str64;
    v92 = 0LL;
    v93 = str_concat((void*)v92, v90, v91);
    *(void**)v16 = (void*)v93;
    goto bb21;
  bb20:
    v96 = *(int64_t*)v44;
    v97 = 48LL;
    v98 = v96 == v97;
    if (v98) goto bb22;
    else goto bb23;
  bb21:
    goto bb18;
  bb22:
    v100 = *(void**)v16;
    v101 = 0LL;
    v102 = 0LL;
    v103 = byte_to_str((void*)v102, v101);
    v104 = 0LL;
    v105 = str_concat((void*)v104, v100, v103);
    *(void**)v16 = (void*)v105;
    goto bb24;
  bb23:
    v108 = (char*)v2 + 32;
    v109 = *(void**)v108;
    v110 = *(int64_t*)v5;
    v111 = *(int64_t*)v9;
    v112 = _str245;
    v113 = *(int64_t*)v44;
    v114 = 0LL;
    v115 = byte_to_str((void*)v114, v113);
    v116 = p_str_concat(v112, v115);
    v117 = 0LL;
    v118 = lex_error((void*)v117, v110, v111, v116);
    v119 = 0LL;
    Vec_push((void*)v119, v109, (int64_t)v118);
    v121 = *(void**)v16;
    v122 = *(int64_t*)v44;
    v123 = 0LL;
    v124 = byte_to_str((void*)v123, v122);
    v125 = 0LL;
    v126 = str_concat((void*)v125, v121, v124);
    *(void**)v16 = (void*)v126;
    goto bb24;
  bb24:
    goto bb21;
  bb25:
    v139 = 0LL;
    v140 = (int64_t)Lexer_advance((void*)v139, v2);
    v141 = *(void**)v16;
    v142 = _str247;
    v143 = 0LL;
    v144 = str_concat((void*)v143, v141, v142);
    *(void**)v16 = (void*)v144;
    v146 = 1LL;
    v147 = malloc(8);
    *(int64_t*)v147 = (int64_t)v146;
    goto bb28;
  bb26:
    v221 = *(void**)v16;
    v222 = 0LL;
    v223 = Lexer_read_utf8((void*)v222, v2);
    v224 = 0LL;
    v225 = str_concat((void*)v224, v221, v223);
    *(void**)v16 = (void*)v225;
    goto bb27;
  bb27:
    goto bb6;
  bb28:
    v150 = (char*)v2 + 8;
    v151 = *(int64_t*)v150;
    v152 = (char*)v2 + 0;
    v153 = *(void**)v152;
    v154 = 0LL;
    v155 = (int64_t)str_len((void*)v154, v153);
    v156 = v151 < v155;
    v157 = *(int64_t*)v147;
    v158 = 0LL;
    v159 = v157 > v158;
    v160 = v156 && v159;
    if (v160) goto bb29;
    else goto bb30;
  bb29:
    v162 = 0LL;
    v163 = (int64_t)Lexer_peek((void*)v162, v2);
    v164 = malloc(8);
    *(int64_t*)v164 = (int64_t)v163;
    v166 = *(int64_t*)v164;
    v167 = 123LL;
    v168 = v166 == v167;
    if (v168) goto bb31;
    else goto bb32;
  bb30:
    goto bb27;
  bb31:
    v170 = *(int64_t*)v147;
    v171 = 1LL;
    v172 = v170 + v171;
    *(int64_t*)v147 = (int64_t)v172;
    v174 = *(void**)v16;
    v175 = 0LL;
    v176 = (int64_t)Lexer_advance((void*)v175, v2);
    v177 = 0LL;
    v178 = byte_to_str((void*)v177, v176);
    v179 = 0LL;
    v180 = str_concat((void*)v179, v174, v178);
    *(void**)v16 = (void*)v180;
    goto bb33;
  bb32:
    v183 = *(int64_t*)v164;
    v184 = 125LL;
    v185 = v183 == v184;
    if (v185) goto bb34;
    else goto bb35;
  bb33:
    goto bb28;
  bb34:
    v187 = *(int64_t*)v147;
    v188 = 1LL;
    v189 = v187 - v188;
    *(int64_t*)v147 = (int64_t)v189;
    v191 = *(int64_t*)v147;
    v192 = 0LL;
    v193 = v191 == v192;
    if (v193) goto bb37;
    else goto bb38;
  bb35:
    v211 = *(void**)v16;
    v212 = 0LL;
    v213 = Lexer_read_utf8((void*)v212, v2);
    v214 = 0LL;
    v215 = str_concat((void*)v214, v211, v213);
    *(void**)v16 = (void*)v215;
    goto bb36;
  bb36:
    goto bb33;
  bb37:
    v195 = 0LL;
    v196 = (int64_t)Lexer_advance((void*)v195, v2);
    v197 = *(void**)v16;
    v198 = _str248;
    v199 = 0LL;
    v200 = str_concat((void*)v199, v197, v198);
    *(void**)v16 = (void*)v200;
    goto bb30;
  bb38:
    goto bb39;
  bb39:
    v204 = *(void**)v16;
    v205 = 0LL;
    v206 = Lexer_read_utf8((void*)v205, v2);
    v207 = 0LL;
    v208 = str_concat((void*)v207, v204, v206);
    *(void**)v16 = (void*)v208;
    goto bb36;
  bb40:
    v238 = 0LL;
    v239 = (int64_t)Lexer_advance((void*)v238, v2);
    goto bb42;
  bb41:
    v241 = (char*)v2 + 32;
    v242 = *(void**)v241;
    v243 = *(int64_t*)v5;
    v244 = *(int64_t*)v9;
    v245 = _str249;
    v246 = 0LL;
    v247 = lex_error((void*)v246, v243, v244, v245);
    v248 = 0LL;
    Vec_push((void*)v248, v242, (int64_t)v247);
    goto bb42;
  bb42:
    v251 = malloc(16);
    v252 = 79LL;
    v253 = (char*)v251 + 0;
    *(int64_t*)v253 = (int64_t)v252;
    v255 = *(void**)v16;
    v256 = (char*)v251 + 8;
    *(void**)v256 = (void*)v255;
    v258 = *(int64_t*)v5;
    v259 = *(int64_t*)v9;
    v260 = malloc(32);
    v261 = 0LL;
    token_new(v260, (void*)v261, v251, v258, v259);
    v263 = (char*)v260 + 0;
    v264 = (char*)v0 + 0;
    v265 = (char*)v263 + 0;
    v266 = (char*)v264 + 0;
    v267 = *(int64_t*)v265;
    *(int64_t*)v266 = (int64_t)v267;
    v269 = (char*)v263 + 8;
    v270 = (char*)v264 + 8;
    v271 = *(int64_t*)v269;
    *(int64_t*)v270 = (int64_t)v271;
    v273 = (char*)v260 + 16;
    v274 = (char*)v0 + 16;
    v275 = *(int64_t*)v273;
    *(int64_t*)v274 = (int64_t)v275;
    v277 = (char*)v260 + 24;
    v278 = (char*)v0 + 24;
    v279 = *(int64_t*)v277;
    *(int64_t*)v278 = (int64_t)v279;
    p_call_stack_pop();
    return;
    return;
}

bool lex_is_hexdigit(void* v0, int64_t v1) {
    p_call_stack_push("lex_is_hexdigit");
    int64_t v9;
    int64_t v15;
    bool v18;
    bool v3;
    int64_t v19; /* placeholder for void */
    bool v11;
    bool v6;
    bool v16;
    bool v14;
    int64_t v2;
    bool v17;
    int64_t v4;
    bool v12;
    int64_t v13;
    int64_t v7;
    bool v5;
    bool v8;
    bool v10;
    v2 = 48LL;
    v3 = v1 >= v2;
    v4 = 57LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    v7 = 65LL;
    v8 = v1 >= v7;
    v9 = 70LL;
    v10 = v1 <= v9;
    v11 = v8 && v10;
    v12 = v6 || v11;
    v13 = 97LL;
    v14 = v1 >= v13;
    v15 = 102LL;
    v16 = v1 <= v15;
    v17 = v14 && v16;
    v18 = v12 || v17;
    p_call_stack_pop();
    return v18;
    return 0;
}

int64_t vec_zip(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_zip");
    int64_t v23; /* placeholder for void */
    int64_t v30;
    bool v32;
    int64_t v45; /* placeholder for void */
    int64_t v43;
    int64_t v6; /* placeholder for void */
    void* v27;
    int64_t v18;
    int64_t v31;
    int64_t v42;
    int64_t v35;
    int64_t v49; /* placeholder for void */
    void* v13;
    void* v16;
    int64_t v17; /* placeholder for void */
    void* v5;
    int64_t v11;
    int64_t v12;
    int64_t v19;
    int64_t v33; /* placeholder for void */
    int64_t v51;
    int64_t v25; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v26;
    int64_t v21; /* placeholder for void */
    void* v9;
    int64_t v39; /* placeholder for void */
    int64_t v41;
    int64_t v50; /* placeholder for void */
    int64_t v34;
    int64_t v38;
    int64_t v46;
    int64_t v52; /* placeholder for void */
    int64_t v15;
    bool v20;
    int64_t v22;
    int64_t v44;
    int64_t v40;
    int64_t v10; /* placeholder for void */
    int64_t v4;
    int64_t v37;
    int64_t v7;
    int64_t v28; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v47;
    int64_t v8;
    int64_t v29; /* placeholder for void */
    int64_t v48;
    int64_t v3;
    int64_t v36;
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

int64_t HashMap_values(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_values");
    int64_t v19;
    int64_t v30;
    void* v16;
    int64_t v37; /* placeholder for void */
    int64_t v40;
    int64_t v43;
    int64_t v44; /* placeholder for void */
    int64_t v11;
    int64_t v28; /* placeholder for void */
    int64_t v2;
    int64_t v5; /* placeholder for void */
    int64_t v20;
    int64_t v38;
    int64_t v25;
    int64_t v39;
    int64_t v24;
    int64_t v6;
    void* v13;
    int64_t v34;
    int64_t v7;
    int64_t v35;
    void* v27;
    int64_t v41; /* placeholder for void */
    int64_t v29;
    int64_t v36;
    int64_t v3;
    int64_t v12;
    int64_t v14; /* placeholder for void */
    int64_t v15;
    int64_t v33; /* placeholder for void */
    int64_t v23;
    int64_t v17; /* placeholder for void */
    int64_t v8;
    int64_t v26;
    int64_t v31;
    bool v21;
    int64_t v22; /* placeholder for void */
    void* v9;
    int64_t v42; /* placeholder for void */
    void* v4;
    int64_t v18; /* placeholder for void */
    void* v32;
    int64_t v10; /* placeholder for void */
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

void p_str_copy_n(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("p_str_copy_n");
    int64_t v29; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v30;
    int64_t v34;
    int64_t v18;
    int64_t v33; /* placeholder for void */
    int64_t v38;
    int64_t v39;
    int64_t v17; /* placeholder for void */
    void* v22;
    void* v26;
    int64_t v8;
    void* v24;
    int64_t v28; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v12;
    int64_t v35;
    int64_t v25; /* placeholder for void */
    uint8_t v23;
    int64_t v27;
    void* v6;
    void* v9;
    bool v13;
    int64_t v31;
    int64_t v11; /* placeholder for void */
    int64_t v15;
    void* v19;
    int64_t v40;
    int64_t v41; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v36;
    void* v4;
    void* v16;
    int64_t v43; /* placeholder for void */
    int64_t v32;
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

bool String_is_empty(void* v0, int64_t v1) {
    p_call_stack_push("String_is_empty");
    void* v14;
    int64_t v6; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    uint8_t v15;
    int64_t v17; /* placeholder for void */
    bool v20;
    int64_t v21; /* placeholder for void */
    int64_t v2;
    bool v3;
    int64_t v10;
    int64_t v7; /* placeholder for void */
    bool v5;
    void* v11;
    int64_t v12; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v18;
    int64_t v19;
    int64_t v16; /* placeholder for void */
    void* v8;
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

int64_t char_to_lower(void* v0, int64_t v1) {
    p_call_stack_push("char_to_lower");
    int64_t v7; /* placeholder for void */
    int64_t v8;
    int64_t v9;
    int64_t v2;
    bool v3;
    bool v6;
    bool v5;
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v11; /* placeholder for void */
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

int64_t vec_take(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_take");
    int64_t v38; /* placeholder for void */
    int64_t v8;
    void* v11;
    bool v27;
    void* v5;
    int64_t v16; /* placeholder for void */
    int64_t v35;
    int64_t v12; /* placeholder for void */
    int64_t v4;
    int64_t v10; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v25;
    int64_t v40;
    int64_t v14;
    int64_t v37;
    int64_t v6; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v22;
    int64_t v32;
    int64_t v17;
    int64_t v21;
    int64_t v24; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v9;
    int64_t v33;
    int64_t v34; /* placeholder for void */
    int64_t v7;
    bool v15;
    int64_t v26;
    int64_t v3;
    int64_t v23; /* placeholder for void */
    int64_t v29;
    int64_t v31;
    int64_t v36;
    int64_t v13;
    int64_t v19; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    int64_t v30;
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

void* str_trim_end(void* v0, void* v1) {
    p_call_stack_push("str_trim_end");
    int64_t v13; /* placeholder for void */
    int64_t v3;
    void* v18;
    void* v21;
    int64_t v11;
    int64_t v14;
    int64_t v36; /* placeholder for void */
    int64_t v38;
    void* v4;
    int64_t v40;
    int64_t v50;
    bool v12;
    int64_t v51;
    void* v24;
    int64_t v6;
    int64_t v17;
    int64_t v22; /* placeholder for void */
    int64_t v31;
    bool v33;
    bool v45;
    int64_t v29; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v43;
    int64_t v15;
    void* v47;
    int64_t v44;
    int64_t v49; /* placeholder for void */
    int64_t v53;
    int64_t v54; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    int64_t v16;
    int64_t v25; /* placeholder for void */
    void* v27;
    int64_t v19; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    uint8_t v28;
    int64_t v34;
    int64_t v23;
    int64_t v39;
    int64_t v46; /* placeholder for void */
    int64_t v32;
    int64_t v42; /* placeholder for void */
    void* v7;
    int64_t v52;
    int64_t v41; /* placeholder for void */
    int64_t v26; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v10;
    int64_t v20;
    int64_t v5; /* placeholder for void */
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
    v47 = _str137;
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

int64_t String_push_str(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_push_str");
    int64_t v11;
    int64_t v33;
    int64_t v18; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    void* v17;
    int64_t v38; /* placeholder for void */
    void* v5;
    void* v40;
    int64_t v25;
    int64_t v44;
    int64_t v45; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v3;
    int64_t v35; /* placeholder for void */
    int64_t v36;
    int64_t v42; /* placeholder for void */
    void* v9;
    int64_t v30;
    int64_t v4;
    int64_t v31;
    int64_t v28; /* placeholder for void */
    int64_t v8;
    int64_t v41;
    int64_t v19;
    void* v16;
    int64_t v32;
    int64_t v13;
    int64_t v20;
    int64_t v7;
    int64_t v29;
    void* v37;
    int64_t v6; /* placeholder for void */
    int64_t v24;
    int64_t v12;
    int64_t v27;
    int64_t v26;
    int64_t v15;
    int64_t v23;
    void* v34;
    int64_t v21;
    int64_t v39; /* placeholder for void */
    int64_t v14;
    int64_t v43; /* placeholder for void */
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

void* HashMap_to_str(void* v0, int64_t v1) {
    p_call_stack_push("HashMap_to_str");
    int64_t v93; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    bool v84;
    void* v118;
    void* v117;
    void* v132;
    int64_t v124; /* placeholder for void */
    void* v134;
    int64_t v101; /* placeholder for void */
    int64_t v40;
    int64_t v52;
    int64_t v75; /* placeholder for void */
    int64_t v98;
    void* v100;
    void* v123;
    int64_t v51; /* placeholder for void */
    int64_t v19;
    int64_t v83;
    bool v29;
    int64_t v7;
    int64_t v34;
    int64_t v45;
    int64_t v68;
    int64_t v119; /* placeholder for void */
    void* v91;
    void* v9;
    bool v88;
    void* v105;
    int64_t v66;
    int64_t v87;
    int64_t v78; /* placeholder for void */
    int64_t v41;
    int64_t v102;
    bool v50;
    void* v112;
    int64_t v11;
    int64_t v26;
    int64_t v135; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    int64_t v15;
    int64_t v14; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v53;
    int64_t v23;
    int64_t v36; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v47;
    void* v109;
    int64_t v81; /* placeholder for void */
    int64_t v44; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v94; /* placeholder for void */
    void* v43;
    int64_t v60;
    int64_t v31;
    int64_t v111; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v37;
    int64_t v97;
    int64_t v104;
    int64_t v99;
    int64_t v56; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v103;
    int64_t v86;
    int64_t v79;
    int64_t v58;
    int64_t v16;
    int64_t v57;
    int64_t v13; /* placeholder for void */
    void* v12;
    bool v17;
    void* v20;
    int64_t v22; /* placeholder for void */
    int64_t v64; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v107;
    int64_t v39;
    int64_t v62;
    int64_t v73;
    int64_t v38;
    void* v77;
    int64_t v89; /* placeholder for void */
    void* v35;
    int64_t v82;
    void* v122;
    int64_t v108;
    int64_t v49;
    int64_t v71;
    int64_t v70; /* placeholder for void */
    void* v90;
    int64_t v72;
    void* v116;
    int64_t v28;
    int64_t v6;
    int64_t v2;
    int64_t v5; /* placeholder for void */
    int64_t v33;
    int64_t v63; /* placeholder for void */
    void* v113;
    int64_t v61;
    int64_t v3;
    int64_t v133; /* placeholder for void */
    int64_t v55;
    int64_t v127;
    int64_t v67;
    int64_t v126;
    int64_t v125;
    void* v110;
    int64_t v25;
    int64_t v46;
    void* v76;
    void* v120;
    int64_t v121;
    void* v130;
    int64_t v115; /* placeholder for void */
    int64_t v32;
    int64_t v69; /* placeholder for void */
    int64_t v24;
    void* v114;
    int64_t v27;
    int64_t v59;
    int64_t v85; /* placeholder for void */
    void* v131;
    int64_t v54;
    int64_t v8;
    int64_t v80; /* placeholder for void */
    void* v92;
    int64_t v42;
    void* v4;
    int64_t v21; /* placeholder for void */
    int64_t v48;
    int64_t v96;
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
    v76 = _str247;
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
    v131 = _str248;
    v132 = p_str_concat(v130, v131);
    *(void**)v77 = (void*)v132;
    v134 = *(void**)v77;
    p_call_stack_pop();
    return v134;
  bb13:
    v90 = *(void**)v77;
    v91 = _str144;
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
    v117 = _str147;
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

int64_t String_replace(void* v0, int64_t v1, int64_t v2, int64_t v3) {
    p_call_stack_push("String_replace");
    void* v179;
    int64_t v13; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    void* v52;
    int64_t v81;
    int64_t v147;
    int64_t v192;
    int64_t v21; /* placeholder for void */
    int64_t v157; /* placeholder for void */
    int64_t v170;
    int64_t v64;
    int64_t v19;
    int64_t v156; /* placeholder for void */
    void* v163;
    bool v121;
    int64_t v122; /* placeholder for void */
    void* v49;
    int64_t v141; /* placeholder for void */
    int64_t v143;
    int64_t v196;
    int64_t v20; /* placeholder for void */
    int64_t v59;
    void* v61;
    bool v68;
    int64_t v91;
    int64_t v28;
    void* v69;
    int64_t v185;
    int64_t v18; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v72;
    int64_t v180;
    int64_t v22;
    uint8_t v113;
    int64_t v120;
    int64_t v186;
    int64_t v105;
    int64_t v6; /* placeholder for void */
    int64_t v30; /* placeholder for void */
    int64_t v33;
    int64_t v42; /* placeholder for void */
    int64_t v71;
    int64_t v128;
    int64_t v152; /* placeholder for void */
    void* v159;
    int64_t v187; /* placeholder for void */
    int64_t v98; /* placeholder for void */
    bool v77;
    int64_t v169; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    int64_t v53;
    int64_t v65;
    int64_t v130; /* placeholder for void */
    int64_t v140;
    int64_t v178; /* placeholder for void */
    int64_t v29;
    int64_t v184;
    void* v9;
    void* v34;
    int64_t v62; /* placeholder for void */
    bool v25;
    int64_t v36;
    int64_t v191;
    int64_t v183;
    int64_t v119;
    int64_t v40;
    int64_t v133; /* placeholder for void */
    void* v109;
    void* v166;
    int64_t v44;
    int64_t v79;
    int64_t v149; /* placeholder for void */
    int64_t v95;
    int64_t v47; /* placeholder for void */
    int64_t v162;
    bool v26;
    int64_t v56;
    bool v23;
    int64_t v117; /* placeholder for void */
    int64_t v107; /* placeholder for void */
    void* v46;
    int64_t v41;
    int64_t v118; /* placeholder for void */
    int64_t v153; /* placeholder for void */
    void* v57;
    int64_t v17; /* placeholder for void */
    int64_t v24;
    int64_t v51; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v127;
    int64_t v139;
    int64_t v164; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v15;
    int64_t v10; /* placeholder for void */
    int64_t v126; /* placeholder for void */
    void* v151;
    int64_t v7;
    int64_t v82;
    void* v173;
    int64_t v60;
    int64_t v181; /* placeholder for void */
    int64_t v32;
    int64_t v193; /* placeholder for void */
    int64_t v80;
    int64_t v104; /* placeholder for void */
    int64_t v102;
    int64_t v63; /* placeholder for void */
    void* v103;
    int64_t v158;
    int64_t v197; /* placeholder for void */
    int64_t v138;
    int64_t v124; /* placeholder for void */
    bool v123;
    void* v160;
    int64_t v137; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v111; /* placeholder for void */
    int64_t v135;
    int64_t v150;
    int64_t v172;
    int64_t v11;
    int64_t v175;
    int64_t v114; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v142; /* placeholder for void */
    int64_t v134;
    int64_t v58; /* placeholder for void */
    int64_t v188;
    int64_t v190;
    void* v176;
    int64_t v94; /* placeholder for void */
    int64_t v48;
    int64_t v37;
    int64_t v96;
    int64_t v27; /* placeholder for void */
    bool v38;
    bool v5;
    int64_t v125; /* placeholder for void */
    bool v132;
    int64_t v54; /* placeholder for void */
    void* v100;
    int64_t v76;
    int64_t v194; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    void* v115;
    void* v148;
    int64_t v161; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v108;
    int64_t v146; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v99;
    int64_t v78; /* placeholder for void */
    int64_t v129;
    void* v45;
    int64_t v136;
    bool v83;
    int64_t v90;
    int64_t v168; /* placeholder for void */
    int64_t v4;
    int64_t v88; /* placeholder for void */
    int64_t v31; /* placeholder for void */
    int64_t v75;
    int64_t v92;
    void* v106;
    int64_t v67; /* placeholder for void */
    uint8_t v116;
    int64_t v144;
    void* v12;
    void* v112;
    void* v145;
    void* v154;
    int64_t v171;
    int64_t v131; /* placeholder for void */
    void* v93;
    int64_t v89;
    int64_t v189; /* placeholder for void */
    bool v66;
    int64_t v195; /* placeholder for void */
    int64_t v16;
    int64_t v177; /* placeholder for void */
    int64_t v86; /* placeholder for void */
    bool v85;
    uint8_t v155;
    int64_t v14; /* placeholder for void */
    int64_t v87; /* placeholder for void */
    int64_t v174; /* placeholder for void */
    void* v8;
    int64_t v167;
    void* v97;
    int64_t v165; /* placeholder for void */
    int64_t v43; /* placeholder for void */
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

int64_t String_parse_int(void* v0, int64_t v1) {
    p_call_stack_push("String_parse_int");
    int64_t v63; /* placeholder for void */
    int64_t v36;
    int64_t v13; /* placeholder for void */
    int64_t v5;
    uint8_t v65;
    int64_t v83; /* placeholder for void */
    bool v3;
    int64_t v28;
    void* v47;
    int64_t v29;
    int64_t v109; /* placeholder for void */
    int64_t v73;
    int64_t v4; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    void* v107;
    int64_t v37;
    int64_t v105;
    int64_t v55;
    int64_t v111; /* placeholder for void */
    int64_t v45;
    int64_t v115; /* placeholder for void */
    uint8_t v108;
    int64_t v26;
    int64_t v44; /* placeholder for void */
    void* v64;
    int64_t v80; /* placeholder for void */
    int64_t v74;
    int64_t v71; /* placeholder for void */
    int64_t v103;
    void* v14;
    bool v38;
    int64_t v89;
    int64_t v68;
    int64_t v101;
    int64_t v58;
    int64_t v25;
    int64_t v50; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v95;
    int64_t v6; /* placeholder for void */
    uint8_t v79;
    int64_t v69;
    int64_t v96;
    int64_t v18; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v51;
    void* v22;
    int64_t v16;
    bool v87;
    int64_t v110; /* placeholder for void */
    bool v70;
    int64_t v75; /* placeholder for void */
    int64_t v114;
    int64_t v99;
    int64_t v40; /* placeholder for void */
    uint8_t v21;
    int64_t v61; /* placeholder for void */
    bool v53;
    int64_t v76;
    int64_t v113;
    int64_t v88;
    int64_t v2;
    int64_t v15; /* placeholder for void */
    int64_t v62;
    void* v12;
    bool v39;
    int64_t v67; /* placeholder for void */
    int64_t v60;
    bool v91;
    int64_t v41;
    int64_t v84; /* placeholder for void */
    int64_t v102;
    int64_t v42;
    int64_t v106; /* placeholder for void */
    bool v34;
    bool v90;
    bool v27;
    bool v30;
    bool v31;
    int64_t v72;
    int64_t v23; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v100; /* placeholder for void */
    int64_t v85;
    int64_t v24; /* placeholder for void */
    int64_t v33;
    int64_t v19; /* placeholder for void */
    bool v35;
    int64_t v98;
    int64_t v56;
    void* v9;
    uint8_t v48;
    int64_t v57; /* placeholder for void */
    int64_t v52;
    int64_t v43;
    int64_t v59;
    int64_t v94;
    int64_t v66; /* placeholder for void */
    int64_t v11;
    int64_t v93;
    int64_t v112;
    int64_t v8;
    void* v17;
    int64_t v7; /* placeholder for void */
    void* v78;
    int64_t v81; /* placeholder for void */
    int64_t v86;
    int64_t v10; /* placeholder for void */
    int64_t v97;
    int64_t v32;
    void* v20;
    int64_t v54; /* placeholder for void */
    int64_t v77; /* placeholder for void */
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

int64_t Result_unwrap(void* v0, void* v1) {
    p_call_stack_push("Result_unwrap");
    void* v17;
    void* v7;
    bool v5;
    int64_t v9; /* placeholder for void */
    void* v15;
    int64_t v21;
    bool v13;
    int64_t v11;
    void* v2;
    int64_t v8;
    void* v10;
    int64_t v16;
    int64_t v18; /* placeholder for void */
    int64_t v4;
    int64_t v22; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v12;
    int64_t v19; /* placeholder for void */
    int64_t v3;
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
    v17 = _str250;
    p_panic(v17);
    goto bb1;
  bb5:
    goto bb1;
    return 0;
}

void Result_map(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_map");
    int64_t v29;
    void* v36;
    int64_t v42;
    void* v11;
    int64_t v43; /* placeholder for void */
    int64_t v12;
    int64_t v5;
    int64_t v14; /* placeholder for void */
    void* v23;
    int64_t v17; /* placeholder for void */
    int64_t v15;
    int64_t v24;
    int64_t v26; /* placeholder for void */
    void* v27;
    int64_t v20;
    int64_t v28;
    void* v40;
    int64_t v21; /* placeholder for void */
    void* v13;
    void* v45;
    int64_t v46;
    int64_t v49; /* placeholder for void */
    int64_t v10;
    bool v7;
    void* v41;
    void* v19;
    void* v4;
    int64_t v33;
    void* v18;
    void* v22;
    int64_t v25; /* placeholder for void */
    void* v38;
    bool v30;
    void* v16;
    int64_t v47; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v44;
    void* v32;
    void* v34;
    int64_t v35;
    int64_t v37; /* placeholder for void */
    int64_t v8; /* placeholder for void */
    void* v9;
    int64_t v48; /* placeholder for void */
    int64_t v6;
    int64_t v31; /* placeholder for void */
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v15 = Lexer_peek((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v15 = token_kind_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v15 = Lexer_advance((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
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

void assert_msg(void* v0, bool v1, void* v2) {
    p_call_stack_push("assert_msg");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v5; /* placeholder for void */
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

bool ByteBuffer_is_empty(void* v0, void* v1) {
    p_call_stack_push("ByteBuffer_is_empty");
    int64_t v3;
    void* v2;
    bool v5;
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

void Vec_for_each(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_for_each");
    int64_t v20;
    int64_t v22;
    int64_t v27; /* placeholder for void */
    int64_t v17;
    int64_t v3;
    int64_t v14; /* placeholder for void */
    int64_t v24;
    int64_t v25; /* placeholder for void */
    void* v18;
    int64_t v26; /* placeholder for void */
    int64_t v16;
    int64_t v19; /* placeholder for void */
    int64_t v23;
    int64_t v6;
    int64_t v7;
    bool v13;
    void* v8;
    int64_t v12;
    void* v4;
    int64_t v21; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v5; /* placeholder for void */
    int64_t v11;
    int64_t v15;
    int64_t v9; /* placeholder for void */
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
        if (strcmp(_fn_name, "HashMap_grow") == 0) {
            HashMap_grow((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_clear") == 0) {
            Vec_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Vec_sort") == 0) {
            Vec_sort((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Lexer_skip_line_comment") == 0) {
            Lexer_skip_line_comment((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_new") == 0) {
            StringBuilder_new((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "Lexer_skip_block_comment") == 0) {
            Lexer_skip_block_comment((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "StringBuilder_clear") == 0) {
            StringBuilder_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "println") == 0) {
            println((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "assert") == 0) {
            assert((void*)v2, (bool)v20);
        } else if (strcmp(_fn_name, "Vec_reverse") == 0) {
            Vec_reverse((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "ByteBuffer_clear") == 0) {
            ByteBuffer_clear((void*)v2, (void*)v20);
        } else if (strcmp(_fn_name, "vec_dedup") == 0) {
            vec_dedup((void*)v2, (int64_t)v20);
        } else if (strcmp(_fn_name, "Vec_dedup") == 0) {
            Vec_dedup((void*)v2, (void*)v20);
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

int64_t Option_unwrap_or_else(void* v0, void* v1, void* v2) {
    p_call_stack_push("Option_unwrap_or_else");
    int64_t v10; /* placeholder for void */
    int64_t v19;
    void* v11;
    int64_t v5;
    int64_t v15; /* placeholder for void */
    int64_t v16;
    bool v6;
    int64_t v20; /* placeholder for void */
    int64_t v4;
    int64_t v18; /* placeholder for void */
    void* v3;
    int64_t v17; /* placeholder for void */
    int64_t v12;
    int64_t v13;
    int64_t v9;
    int64_t v7; /* placeholder for void */
    bool v14;
    void* v8;
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
        if (strcmp(_fn_name, "main") == 0) {
            v16 = main_entry((void*)v2);
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

void Vec_extend(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_extend");
    int64_t v3; /* placeholder for void */
    int64_t v4; /* placeholder for void */
    p_vec_extend(v1, v2);
    p_call_stack_pop();
    return;
    return;
}

int64_t HashMap_new(void* v0) {
    p_call_stack_push("HashMap_new");
    int64_t v28;
    void* v21;
    int64_t v26; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    void* v11;
    int64_t v10;
    int64_t v4; /* placeholder for void */
    int64_t v37;
    int64_t v36; /* placeholder for void */
    int64_t v13;
    int64_t v8;
    int64_t v5;
    int64_t v1;
    int64_t v20;
    int64_t v23; /* placeholder for void */
    int64_t v31;
    int64_t v17; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    void* v6;
    void* v30;
    int64_t v34;
    void* v3;
    int64_t v19;
    void* v33;
    int64_t v38; /* placeholder for void */
    int64_t v14;
    int64_t v9;
    int64_t v18;
    void* v24;
    int64_t v29; /* placeholder for void */
    int64_t v7; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    void* v16;
    int64_t v15;
    int64_t v22; /* placeholder for void */
    void* v2;
    int64_t v25;
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

void ByteBuffer_from_string(void* v0, void* v1, void* v2) {
    p_call_stack_push("ByteBuffer_from_string");
    int64_t v49; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v37;
    int64_t v44; /* placeholder for void */
    int64_t v20; /* placeholder for void */
    void* v47;
    int64_t v53; /* placeholder for void */
    int64_t v54; /* placeholder for void */
    int64_t v21;
    void* v46;
    void* v26;
    int64_t v38;
    void* v11;
    void* v4;
    int64_t v22;
    int64_t v16; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    void* v23;
    int64_t v28;
    int64_t v43; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v42;
    int64_t v17;
    int64_t v18;
    int64_t v13;
    int64_t v31; /* placeholder for void */
    void* v48;
    int64_t v10;
    int64_t v30; /* placeholder for void */
    int64_t v5;
    int64_t v9;
    int64_t v40;
    void* v29;
    bool v19;
    void* v51;
    void* v45;
    void* v32;
    int64_t v35; /* placeholder for void */
    void* v50;
    void* v7;
    void* v36;
    int64_t v52;
    int64_t v25;
    int64_t v3;
    int64_t v8; /* placeholder for void */
    int64_t v34; /* placeholder for void */
    void* v14;
    uint8_t v33;
    int64_t v41;
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

int64_t str_len(void* v0, void* v1) {
    p_call_stack_push("str_len");
    int64_t v3;
    int64_t v4; /* placeholder for void */
    int64_t v2;
    v2 = 0LL;
    v3 = (int64_t)String_len((void*)v2, (int64_t)v1);
    p_call_stack_pop();
    return v3;
    return 0;
}

bool String_starts_with(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_starts_with");
    uint8_t v57;
    int64_t v62; /* placeholder for void */
    bool v22;
    int64_t v69; /* placeholder for void */
    void* v50;
    int64_t v15; /* placeholder for void */
    int64_t v32;
    int64_t v5;
    int64_t v58; /* placeholder for void */
    bool v33;
    int64_t v19; /* placeholder for void */
    int64_t v40;
    int64_t v71;
    bool v24;
    int64_t v29; /* placeholder for void */
    int64_t v42; /* placeholder for void */
    bool v75;
    int64_t v49;
    int64_t v36;
    int64_t v8; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v43;
    void* v56;
    int64_t v26; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    int64_t v35;
    uint8_t v60;
    int64_t v12;
    int64_t v72;
    int64_t v39;
    int64_t v23; /* placeholder for void */
    int64_t v31;
    void* v41;
    bool v67;
    int64_t v38; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v20;
    bool v7;
    void* v47;
    int64_t v76; /* placeholder for void */
    int64_t v64;
    void* v44;
    int64_t v27;
    int64_t v52;
    bool v6;
    int64_t v21;
    int64_t v46;
    int64_t v17;
    int64_t v70;
    int64_t v34; /* placeholder for void */
    void* v14;
    int64_t v51; /* placeholder for void */
    void* v53;
    void* v28;
    int64_t v66; /* placeholder for void */
    int64_t v68; /* placeholder for void */
    int64_t v16;
    int64_t v48; /* placeholder for void */
    bool v65;
    int64_t v54; /* placeholder for void */
    int64_t v3;
    int64_t v30; /* placeholder for void */
    int64_t v63;
    void* v59;
    int64_t v13;
    int64_t v45; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    void* v37;
    bool v9;
    int64_t v11; /* placeholder for void */
    void* v18;
    bool v4;
    int64_t v10; /* placeholder for void */
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

int64_t String_from(void* v0, int64_t v1) {
    p_call_stack_push("String_from");
    int64_t v11; /* placeholder for void */
    int64_t v18; /* placeholder for void */
    void* v22;
    int64_t v34;
    int64_t v14;
    void* v10;
    void* v13;
    void* v43;
    int64_t v46; /* placeholder for void */
    int64_t v47;
    int64_t v29; /* placeholder for void */
    void* v37;
    int64_t v12; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v20;
    void* v28;
    int64_t v33; /* placeholder for void */
    int64_t v35;
    int64_t v4; /* placeholder for void */
    int64_t v38; /* placeholder for void */
    void* v6;
    int64_t v39;
    void* v40;
    int64_t v8; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v9;
    int64_t v26;
    void* v27;
    int64_t v42; /* placeholder for void */
    int64_t v19; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v36;
    int64_t v44;
    int64_t v5;
    void* v7;
    int64_t v24;
    bool v3;
    int64_t v23; /* placeholder for void */
    int64_t v25;
    int64_t v32;
    int64_t v21;
    int64_t v48; /* placeholder for void */
    int64_t v17;
    int64_t v31;
    int64_t v2;
    int64_t v30;
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

bool char_is_alpha(void* v0, int64_t v1) {
    p_call_stack_push("char_is_alpha");
    bool v12;
    int64_t v13; /* placeholder for void */
    int64_t v2;
    bool v5;
    bool v11;
    bool v3;
    bool v8;
    int64_t v4;
    int64_t v7;
    int64_t v9;
    bool v10;
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

int64_t Result_unwrap_or(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Result_unwrap_or");
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v17;
    bool v14;
    int64_t v21; /* placeholder for void */
    int64_t v13;
    int64_t v19; /* placeholder for void */
    int64_t v9;
    int64_t v4;
    int64_t v12;
    int64_t v15; /* placeholder for void */
    int64_t v5;
    void* v8;
    void* v16;
    bool v6;
    int64_t v20;
    int64_t v18; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    void* v11;
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

int64_t token_kind_tag(void* v0, void* v1) {
    p_call_stack_push("token_kind_tag");
    void* v303;
    int64_t v160; /* placeholder for void */
    bool v799;
    int64_t v942; /* placeholder for void */
    int64_t v456; /* placeholder for void */
    int64_t v494;
    void* v324;
    bool v68;
    int64_t v767; /* placeholder for void */
    int64_t v399;
    int64_t v98;
    void* v408;
    int64_t v737; /* placeholder for void */
    int64_t v361;
    int64_t v186;
    void* v198;
    int64_t v697; /* placeholder for void */
    bool v313;
    int64_t v713;
    bool v546;
    int64_t v634; /* placeholder for void */
    int64_t v784;
    int64_t v830; /* placeholder for void */
    int64_t v507;
    void* v656;
    int64_t v630;
    int64_t v265; /* placeholder for void */
    int64_t v858; /* placeholder for void */
    bool v869;
    int64_t v486;
    int64_t v874;
    bool v348;
    void* v894;
    bool v946;
    int64_t v227;
    int64_t v917;
    bool v404;
    int64_t v405; /* placeholder for void */
    int64_t v174; /* placeholder for void */
    int64_t v437;
    bool v764;
    int64_t v788; /* placeholder for void */
    int64_t v921; /* placeholder for void */
    int64_t v228;
    int64_t v702; /* placeholder for void */
    bool v939;
    void* v471;
    int64_t v470; /* placeholder for void */
    bool v173;
    void* v422;
    void* v534;
    void* v9;
    void* v506;
    int64_t v714;
    void* v373;
    int64_t v776;
    int64_t v441;
    int64_t v591;
    int64_t v463; /* placeholder for void */
    int64_t v476;
    void* v810;
    void* v289;
    int64_t v444;
    int64_t v18;
    int64_t v116;
    int64_t v857;
    bool v603;
    void* v226;
    int64_t v805;
    int64_t v452;
    void* v275;
    void* v705;
    int64_t v853;
    void* v824;
    int64_t v479;
    int64_t v759;
    int64_t v209; /* placeholder for void */
    int64_t v279; /* placeholder for void */
    void* v586;
    void* v859;
    int64_t v957; /* placeholder for void */
    int64_t v217;
    bool v355;
    void* v719;
    int64_t v907; /* placeholder for void */
    int64_t v556; /* placeholder for void */
    int64_t v955;
    int64_t v781; /* placeholder for void */
    int64_t v120; /* placeholder for void */
    int64_t v692;
    int64_t v544;
    int64_t v924;
    bool v89;
    int64_t v64; /* placeholder for void */
    int64_t v218; /* placeholder for void */
    int64_t v360;
    int64_t v519; /* placeholder for void */
    int64_t v605;
    int64_t v585; /* placeholder for void */
    int64_t v424;
    int64_t v580;
    int64_t v195; /* placeholder for void */
    int64_t v136;
    int64_t v385;
    int64_t v148; /* placeholder for void */
    int64_t v281; /* placeholder for void */
    int64_t v512; /* placeholder for void */
    int64_t v625; /* placeholder for void */
    int64_t v844; /* placeholder for void */
    int64_t v531; /* placeholder for void */
    int64_t v137;
    int64_t v342; /* placeholder for void */
    bool v722;
    int64_t v821; /* placeholder for void */
    int64_t v709; /* placeholder for void */
    int64_t v528;
    void* v184;
    int64_t v139; /* placeholder for void */
    int64_t v627; /* placeholder for void */
    int64_t v371;
    int64_t v123;
    int64_t v69; /* placeholder for void */
    int64_t v532;
    int64_t v344; /* placeholder for void */
    int64_t v604; /* placeholder for void */
    int64_t v402;
    int64_t v459;
    int64_t v316; /* placeholder for void */
    int64_t v861;
    void* v887;
    void* v567;
    int64_t v914; /* placeholder for void */
    int64_t v295; /* placeholder for void */
    int64_t v197; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v312;
    int64_t v388;
    int64_t v843;
    void* v156;
    int64_t v772; /* placeholder for void */
    int64_t v169; /* placeholder for void */
    int64_t v431;
    int64_t v498; /* placeholder for void */
    int64_t v78; /* placeholder for void */
    int64_t v297;
    int64_t v738;
    int64_t v315;
    bool v925;
    int64_t v258; /* placeholder for void */
    int64_t v748;
    void* v191;
    int64_t v956; /* placeholder for void */
    int64_t v204; /* placeholder for void */
    void* v51;
    int64_t v179;
    void* v478;
    bool v502;
    int64_t v947; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v24;
    void* v649;
    bool v708;
    int64_t v414; /* placeholder for void */
    int64_t v28;
    int64_t v262;
    int64_t v522;
    bool v610;
    int64_t v661;
    int64_t v181; /* placeholder for void */
    bool v509;
    int64_t v127; /* placeholder for void */
    int64_t v535;
    int64_t v793; /* placeholder for void */
    bool v673;
    int64_t v514;
    bool v834;
    int64_t v505; /* placeholder for void */
    int64_t v472;
    void* v663;
    int64_t v717;
    void* v943;
    int64_t v62; /* placeholder for void */
    void* v747;
    int64_t v877; /* placeholder for void */
    int64_t v521;
    int64_t v363; /* placeholder for void */
    int64_t v916;
    bool v953;
    int64_t v109;
    bool v806;
    int64_t v739; /* placeholder for void */
    int64_t v629;
    int64_t v305;
    int64_t v207;
    bool v187;
    int64_t v190; /* placeholder for void */
    bool v327;
    bool v341;
    int64_t v392;
    int64_t v367;
    int64_t v518;
    bool v523;
    int64_t v744; /* placeholder for void */
    bool v897;
    bool v320;
    void* v128;
    int64_t v32;
    bool v743;
    int64_t v101;
    int64_t v389;
    int64_t v428; /* placeholder for void */
    int64_t v119;
    int64_t v298;
    int64_t v477; /* placeholder for void */
    int64_t v294;
    int64_t v833;
    bool v481;
    int64_t v346;
    void* v600;
    void* v593;
    int64_t v724;
    bool v383;
    int64_t v178;
    bool v537;
    int64_t v451;
    void* v628;
    int64_t v307; /* placeholder for void */
    int64_t v87;
    int64_t v727;
    int64_t v288; /* placeholder for void */
    int64_t v511;
    void* v282;
    void* v775;
    bool v813;
    bool v729;
    int64_t v893; /* placeholder for void */
    int64_t v280;
    int64_t v952;
    int64_t v430;
    bool v432;
    int64_t v945;
    int64_t v48; /* placeholder for void */
    int64_t v309; /* placeholder for void */
    int64_t v597; /* placeholder for void */
    int64_t v885;
    void* v240;
    int64_t v872; /* placeholder for void */
    int64_t v17;
    int64_t v251; /* placeholder for void */
    int64_t v674; /* placeholder for void */
    int64_t v328; /* placeholder for void */
    void* v72;
    bool v827;
    int64_t v898; /* placeholder for void */
    int64_t v94;
    int64_t v440; /* placeholder for void */
    void* v607;
    int64_t v825;
    void* v219;
    int64_t v412; /* placeholder for void */
    bool v516;
    int64_t v774; /* placeholder for void */
    int64_t v356; /* placeholder for void */
    int64_t v10;
    bool v201;
    bool v687;
    bool v792;
    bool v264;
    int64_t v164;
    int64_t v90; /* placeholder for void */
    void* v513;
    void* v79;
    int64_t v126;
    int64_t v662; /* placeholder for void */
    void* v499;
    int64_t v249;
    int64_t v438;
    int64_t v56;
    int64_t v335; /* placeholder for void */
    int64_t v112;
    void* v401;
    int64_t v762;
    bool v397;
    void* v254;
    int64_t v529;
    int64_t v53;
    void* v135;
    int64_t v147;
    int64_t v685;
    int64_t v933; /* placeholder for void */
    int64_t v590; /* placeholder for void */
    int64_t v237; /* placeholder for void */
    int64_t v434;
    bool v918;
    int64_t v85; /* placeholder for void */
    void* v352;
    int64_t v49;
    int64_t v59;
    int64_t v809; /* placeholder for void */
    int64_t v220;
    int64_t v259;
    int64_t v550;
    int64_t v67;
    int64_t v619;
    int64_t v475; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v172;
    int64_t v386; /* placeholder for void */
    bool v645;
    int64_t v766;
    int64_t v807; /* placeholder for void */
    int64_t v108;
    int64_t v571;
    void* v163;
    int64_t v63;
    void* v37;
    bool v12;
    int64_t v349; /* placeholder for void */
    int64_t v681; /* placeholder for void */
    int64_t v445;
    int64_t v651;
    int64_t v829;
    bool v488;
    int64_t v105;
    int64_t v223; /* placeholder for void */
    int64_t v524; /* placeholder for void */
    int64_t v646; /* placeholder for void */
    bool v652;
    int64_t v851; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v263;
    int64_t v99; /* placeholder for void */
    int64_t v581;
    int64_t v672;
    void* v733;
    int64_t v758; /* placeholder for void */
    int64_t v318;
    bool v390;
    int64_t v398; /* placeholder for void */
    int64_t v615;
    int64_t v618; /* placeholder for void */
    int64_t v592; /* placeholder for void */
    int64_t v865; /* placeholder for void */
    bool v334;
    int64_t v723; /* placeholder for void */
    int64_t v860;
    int64_t v892;
    int64_t v560; /* placeholder for void */
    int64_t v395;
    int64_t v542; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    bool v638;
    int64_t v165;
    int64_t v88;
    int64_t v13; /* placeholder for void */
    void* v100;
    int64_t v301;
    int64_t v329;
    int64_t v769;
    void* v621;
    int64_t v732; /* placeholder for void */
    int64_t v115;
    int64_t v210;
    int64_t v330; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v891; /* placeholder for void */
    bool v573;
    int64_t v21;
    int64_t v951;
    int64_t v632; /* placeholder for void */
    void* v485;
    int64_t v302; /* placeholder for void */
    int64_t v176; /* placeholder for void */
    int64_t v407; /* placeholder for void */
    int64_t v242;
    int64_t v563;
    int64_t v706;
    int64_t v304;
    int64_t v578; /* placeholder for void */
    int64_t v679;
    int64_t v448;
    int64_t v202; /* placeholder for void */
    bool v159;
    bool v299;
    bool v701;
    int64_t v503; /* placeholder for void */
    int64_t v905; /* placeholder for void */
    void* v557;
    int64_t v193;
    int64_t v211; /* placeholder for void */
    int64_t v598;
    int64_t v678;
    int64_t v735;
    void* v915;
    int64_t v940; /* placeholder for void */
    bool v848;
    int64_t v102;
    bool v694;
    int64_t v868;
    bool v624;
    void* v768;
    int64_t v480;
    int64_t v574; /* placeholder for void */
    int64_t v269;
    int64_t v416;
    int64_t v770;
    void* v394;
    bool v841;
    bool v666;
    int64_t v790;
    bool v862;
    int64_t v321; /* placeholder for void */
    int64_t v449; /* placeholder for void */
    bool v75;
    int64_t v920;
    int64_t v364;
    bool v376;
    int64_t v658;
    int64_t v144;
    int64_t v333;
    int64_t v675;
    int64_t v913;
    int64_t v937;
    int64_t v216; /* placeholder for void */
    int64_t v783;
    int64_t v287;
    bool v596;
    int64_t v847;
    bool v96;
    int64_t v113; /* placeholder for void */
    int64_t v290;
    void* v443;
    int64_t v354;
    bool v425;
    int64_t v954; /* placeholder for void */
    int64_t v185;
    int64_t v146; /* placeholder for void */
    int64_t v378;
    int64_t v409;
    int64_t v122;
    int64_t v837; /* placeholder for void */
    int64_t v347;
    int64_t v657;
    bool v757;
    int64_t v640;
    bool v530;
    int64_t v849; /* placeholder for void */
    int64_t v42;
    int64_t v245;
    int64_t v565; /* placeholder for void */
    int64_t v620; /* placeholder for void */
    bool v474;
    int64_t v856; /* placeholder for void */
    int64_t v277;
    int64_t v435; /* placeholder for void */
    void* v726;
    int64_t v358; /* placeholder for void */
    bool v904;
    int64_t v256;
    bool v180;
    int64_t v133;
    int64_t v214;
    int64_t v266;
    bool v215;
    bool v145;
    int64_t v707;
    int64_t v804;
    void* v831;
    int64_t v29; /* placeholder for void */
    int64_t v668;
    int64_t v526; /* placeholder for void */
    int64_t v912; /* placeholder for void */
    bool v750;
    int64_t v690; /* placeholder for void */
    int64_t v396;
    bool v124;
    bool v278;
    int64_t v73;
    int64_t v284;
    void* v366;
    int64_t v340;
    int64_t v400; /* placeholder for void */
    int64_t v633;
    void* v817;
    int64_t v66;
    int64_t v70;
    bool v306;
    int64_t v958;
    void* v740;
    void* v380;
    int64_t v538; /* placeholder for void */
    double v549;
    void* v261;
    int64_t v391; /* placeholder for void */
    int64_t v473;
    int64_t v540;
    int64_t v454; /* placeholder for void */
    bool v680;
    int64_t v665;
    int64_t v157;
    void* v436;
    void* v712;
    int64_t v370; /* placeholder for void */
    int64_t v746; /* placeholder for void */
    void* v170;
    int64_t v365; /* placeholder for void */
    bool v911;
    int64_t v588;
    int64_t v664;
    void* v929;
    int64_t v794;
    int64_t v608;
    int64_t v80;
    void* v23;
    int64_t v500;
    bool v659;
    int64_t v129;
    int64_t v372; /* placeholder for void */
    int64_t v882;
    int64_t v959; /* placeholder for void */
    void* v177;
    int64_t v536;
    int64_t v760; /* placeholder for void */
    int64_t v162; /* placeholder for void */
    int64_t v493;
    int64_t v584;
    void* v415;
    int64_t v241;
    int64_t v801;
    bool v117;
    void* v331;
    int64_t v420;
    int64_t v468; /* placeholder for void */
    int64_t v381;
    int64_t v547; /* placeholder for void */
    void* v901;
    void* v570;
    void* v121;
    int64_t v286; /* placeholder for void */
    int64_t v554;
    int64_t v31;
    int64_t v896;
    int64_t v721;
    int64_t v886; /* placeholder for void */
    int64_t v734;
    int64_t v350;
    int64_t v643;
    int64_t v11;
    int64_t v594;
    int64_t v686;
    int64_t v379; /* placeholder for void */
    int64_t v427;
    int64_t v8; /* placeholder for void */
    bool v439;
    int64_t v469;
    void* v149;
    int64_t v60;
    int64_t v57; /* placeholder for void */
    void* v677;
    void* v838;
    int64_t v154;
    int64_t v155; /* placeholder for void */
    int64_t v601;
    void* v450;
    int64_t v466;
    void* v93;
    void* v464;
    int64_t v587;
    int64_t v599; /* placeholder for void */
    int64_t v655; /* placeholder for void */
    bool v883;
    int64_t v234;
    int64_t v300; /* placeholder for void */
    int64_t v95;
    int64_t v545;
    int64_t v938;
    int64_t v336;
    int64_t v595;
    void* v691;
    int64_t v150;
    int64_t v842; /* placeholder for void */
    void* v520;
    int64_t v203;
    bool v890;
    bool v771;
    int64_t v200;
    bool v47;
    bool v257;
    int64_t v183; /* placeholder for void */
    int64_t v568;
    int64_t v559;
    int64_t v255;
    int64_t v206;
    bool v5;
    bool v362;
    int64_t v835; /* placeholder for void */
    bool v855;
    int64_t v43; /* placeholder for void */
    int64_t v125; /* placeholder for void */
    void* v429;
    int64_t v270;
    void* v268;
    int64_t v711; /* placeholder for void */
    int64_t v274; /* placeholder for void */
    int64_t v725; /* placeholder for void */
    int64_t v25;
    int64_t v720;
    int64_t v293; /* placeholder for void */
    void* v908;
    int64_t v413;
    int64_t v171;
    int64_t v765; /* placeholder for void */
    int64_t v927;
    bool v418;
    int64_t v224;
    bool v932;
    bool v26;
    int64_t v823; /* placeholder for void */
    int64_t v934;
    void* v317;
    int64_t v871;
    int64_t v622;
    void* v310;
    int64_t v949; /* placeholder for void */
    int64_t v38;
    int64_t v695; /* placeholder for void */
    bool v229;
    bool v40;
    int64_t v504;
    int64_t v867;
    int64_t v716; /* placeholder for void */
    int64_t v710;
    int64_t v14;
    void* v543;
    int64_t v235;
    int64_t v863; /* placeholder for void */
    int64_t v948;
    int64_t v613; /* placeholder for void */
    int64_t v458;
    int64_t v836;
    int64_t v704; /* placeholder for void */
    int64_t v780;
    int64_t v553;
    int64_t v699;
    int64_t v636;
    void* v527;
    int64_t v577;
    int64_t v246; /* placeholder for void */
    void* v457;
    bool v582;
    int64_t v175;
    int64_t v143;
    int64_t v337; /* placeholder for void */
    int64_t v654;
    int64_t v616;
    bool v369;
    int64_t v763;
    int64_t v840;
    void* v86;
    bool v285;
    int64_t v487;
    int64_t v864;
    int64_t v816; /* placeholder for void */
    int64_t v3;
    bool v292;
    int64_t v81;
    int64_t v221;
    int64_t v797;
    int64_t v45;
    bool v411;
    void* v16;
    int64_t v272; /* placeholder for void */
    int64_t v319;
    int64_t v703;
    void* v359;
    int64_t v322;
    void* v670;
    bool v715;
    int64_t v944;
    int64_t v153; /* placeholder for void */
    int64_t v795; /* placeholder for void */
    int64_t v919; /* placeholder for void */
    int64_t v71; /* placeholder for void */
    int64_t v870; /* placeholder for void */
    int64_t v786; /* placeholder for void */
    int64_t v510; /* placeholder for void */
    int64_t v39;
    int64_t v314; /* placeholder for void */
    int64_t v718; /* placeholder for void */
    int64_t v118; /* placeholder for void */
    void* v338;
    int64_t v676; /* placeholder for void */
    void* v558;
    int64_t v753; /* placeholder for void */
    int64_t v787;
    int64_t v426; /* placeholder for void */
    int64_t v482; /* placeholder for void */
    int64_t v490;
    int64_t v777;
    int64_t v283;
    bool v778;
    bool v243;
    int64_t v483;
    int64_t v276;
    int64_t v410;
    int64_t v583; /* placeholder for void */
    void* v845;
    void* v754;
    int64_t v508;
    int64_t v798;
    void* v803;
    int64_t v639; /* placeholder for void */
    int64_t v741;
    int64_t v130;
    int64_t v182;
    int64_t v84;
    int64_t v611; /* placeholder for void */
    int64_t v339;
    bool v460;
    bool v446;
    int64_t v941;
    int64_t v232; /* placeholder for void */
    int64_t v648; /* placeholder for void */
    int64_t v515;
    int64_t v465;
    int64_t v906;
    int64_t v910;
    int64_t v111; /* placeholder for void */
    int64_t v802; /* placeholder for void */
    int64_t v689;
    int64_t v231;
    int64_t v899;
    int64_t v846;
    int64_t v4;
    int64_t v323; /* placeholder for void */
    int64_t v612;
    int64_t v525;
    int64_t v484; /* placeholder for void */
    int64_t v641; /* placeholder for void */
    int64_t v671;
    int64_t v791;
    int64_t v756;
    int64_t v815;
    void* v950;
    void* v635;
    void* v539;
    int64_t v745;
    int64_t v36; /* placeholder for void */
    void* v684;
    bool v876;
    int64_t v377; /* placeholder for void */
    int64_t v253; /* placeholder for void */
    void* v552;
    bool v19;
    int64_t v382;
    int64_t v850;
    int64_t v433; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v572;
    void* v880;
    int64_t v884; /* placeholder for void */
    int64_t v626;
    int64_t v423;
    int64_t v496; /* placeholder for void */
    int64_t v773;
    int64_t v393; /* placeholder for void */
    void* v575;
    int64_t v818;
    int64_t v332;
    bool v631;
    bool v453;
    int64_t v417;
    int64_t v808;
    int64_t v819;
    int64_t v902;
    int64_t v779; /* placeholder for void */
    int64_t v828; /* placeholder for void */
    int64_t v248;
    int64_t v800; /* placeholder for void */
    int64_t v7;
    bool v736;
    int64_t v895;
    void* v345;
    void* v561;
    int64_t v213;
    bool v222;
    int64_t v151;
    void* v614;
    int64_t v260; /* placeholder for void */
    int64_t v351; /* placeholder for void */
    int64_t v930;
    void* v114;
    int64_t v189;
    int64_t v419; /* placeholder for void */
    int64_t v497;
    int64_t v50; /* placeholder for void */
    int64_t v238;
    int64_t v132; /* placeholder for void */
    void* v142;
    int64_t v244; /* placeholder for void */
    bool v271;
    int64_t v343;
    int64_t v326;
    int64_t v230; /* placeholder for void */
    int64_t v91;
    bool v785;
    int64_t v134; /* placeholder for void */
    int64_t v168;
    void* v205;
    int64_t v653; /* placeholder for void */
    bool v33;
    bool v131;
    int64_t v669; /* placeholder for void */
    int64_t v52;
    bool v166;
    int64_t v462;
    int64_t v551; /* placeholder for void */
    int64_t v875;
    void* v296;
    bool v250;
    int64_t v637;
    int64_t v900; /* placeholder for void */
    void* v247;
    int64_t v403;
    int64_t v650;
    int64_t v76; /* placeholder for void */
    int64_t v889;
    int64_t v308;
    int64_t v931;
    bool v467;
    int64_t v74;
    int64_t v602;
    int64_t v141; /* placeholder for void */
    void* v576;
    bool v617;
    int64_t v682;
    int64_t v935; /* placeholder for void */
    bool v82;
    void* v873;
    int64_t v406;
    int64_t v728;
    int64_t v693;
    void* v44;
    void* v387;
    int64_t v688; /* placeholder for void */
    int64_t v421; /* placeholder for void */
    void* v761;
    int64_t v839;
    bool v194;
    int64_t v489; /* placeholder for void */
    bool v236;
    bool v555;
    void* v922;
    int64_t v660; /* placeholder for void */
    int64_t v812;
    int64_t v832;
    void* v58;
    void* v866;
    int64_t v442; /* placeholder for void */
    int64_t v609;
    int64_t v34; /* placeholder for void */
    int64_t v35;
    int64_t v888;
    int64_t v83; /* placeholder for void */
    int64_t v881;
    int64_t v569; /* placeholder for void */
    int64_t v752;
    int64_t v909;
    int64_t v291;
    bool v61;
    int64_t v447; /* placeholder for void */
    int64_t v517; /* placeholder for void */
    int64_t v903;
    int64_t v926; /* placeholder for void */
    void* v548;
    int64_t v683; /* placeholder for void */
    int64_t v854;
    void* v698;
    int64_t v461; /* placeholder for void */
    bool v138;
    int64_t v667; /* placeholder for void */
    int64_t v491; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v140;
    bool v103;
    void* v789;
    bool v208;
    void* v65;
    int64_t v826;
    bool v495;
    int64_t v501;
    int64_t v455;
    bool v110;
    int64_t v879; /* placeholder for void */
    bool v152;
    int64_t v158;
    int64_t v225; /* placeholder for void */
    int64_t v375;
    int64_t v196;
    int64_t v541;
    void* v579;
    int64_t v730; /* placeholder for void */
    int64_t v751; /* placeholder for void */
    int64_t v252;
    int64_t v644;
    void* v212;
    int64_t v822;
    bool v589;
    int64_t v273;
    void* v233;
    int64_t v731;
    int64_t v533; /* placeholder for void */
    int64_t v696;
    int64_t v606; /* placeholder for void */
    int64_t v814; /* placeholder for void */
    int64_t v562;
    int64_t v97; /* placeholder for void */
    int64_t v239; /* placeholder for void */
    int64_t v742;
    void* v107;
    bool v564;
    int64_t v27; /* placeholder for void */
    int64_t v325;
    void* v796;
    void* v936;
    int64_t v878;
    int64_t v384; /* placeholder for void */
    void* v642;
    int64_t v46;
    int64_t v353;
    void* v566;
    int64_t v161;
    int64_t v928; /* placeholder for void */
    int64_t v267; /* placeholder for void */
    int64_t v700;
    int64_t v755;
    int64_t v188; /* placeholder for void */
    void* v30;
    int64_t v167; /* placeholder for void */
    int64_t v199;
    void* v852;
    bool v820;
    void* v492;
    bool v54;
    int64_t v368;
    int64_t v623;
    int64_t v20; /* placeholder for void */
    int64_t v749;
    int64_t v647;
    int64_t v923;
    void* v2;
    void* v782;
    int64_t v357;
    int64_t v311;
    int64_t v374;
    int64_t v811;
    int64_t v77;
    int64_t v192;
    v2 = (char*)v1 + 0;
    v3 = *(int64_t*)v2;
    v4 = 0LL;
    v5 = v3 == v4;
    if (v5) goto bb2;
    else goto bb3;
  bb1:
    v958 = 0LL;
    p_call_stack_pop();
    return v958;
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
    v23 = (char*)v1 + 0;
    v24 = *(int64_t*)v23;
    v25 = 3LL;
    v26 = v24 == v25;
    if (v26) goto bb8;
    else goto bb9;
  bb8:
    v28 = 3LL;
    p_call_stack_pop();
    return v28;
  bb9:
    v30 = (char*)v1 + 0;
    v31 = *(int64_t*)v30;
    v32 = 4LL;
    v33 = v31 == v32;
    if (v33) goto bb10;
    else goto bb11;
  bb10:
    v35 = 4LL;
    p_call_stack_pop();
    return v35;
  bb11:
    v37 = (char*)v1 + 0;
    v38 = *(int64_t*)v37;
    v39 = 5LL;
    v40 = v38 == v39;
    if (v40) goto bb12;
    else goto bb13;
  bb12:
    v42 = 5LL;
    p_call_stack_pop();
    return v42;
  bb13:
    v44 = (char*)v1 + 0;
    v45 = *(int64_t*)v44;
    v46 = 6LL;
    v47 = v45 == v46;
    if (v47) goto bb14;
    else goto bb15;
  bb14:
    v49 = 6LL;
    p_call_stack_pop();
    return v49;
  bb15:
    v51 = (char*)v1 + 0;
    v52 = *(int64_t*)v51;
    v53 = 7LL;
    v54 = v52 == v53;
    if (v54) goto bb16;
    else goto bb17;
  bb16:
    v56 = 7LL;
    p_call_stack_pop();
    return v56;
  bb17:
    v58 = (char*)v1 + 0;
    v59 = *(int64_t*)v58;
    v60 = 8LL;
    v61 = v59 == v60;
    if (v61) goto bb18;
    else goto bb19;
  bb18:
    v63 = 8LL;
    p_call_stack_pop();
    return v63;
  bb19:
    v65 = (char*)v1 + 0;
    v66 = *(int64_t*)v65;
    v67 = 9LL;
    v68 = v66 == v67;
    if (v68) goto bb20;
    else goto bb21;
  bb20:
    v70 = 9LL;
    p_call_stack_pop();
    return v70;
  bb21:
    v72 = (char*)v1 + 0;
    v73 = *(int64_t*)v72;
    v74 = 10LL;
    v75 = v73 == v74;
    if (v75) goto bb22;
    else goto bb23;
  bb22:
    v77 = 10LL;
    p_call_stack_pop();
    return v77;
  bb23:
    v79 = (char*)v1 + 0;
    v80 = *(int64_t*)v79;
    v81 = 11LL;
    v82 = v80 == v81;
    if (v82) goto bb24;
    else goto bb25;
  bb24:
    v84 = 11LL;
    p_call_stack_pop();
    return v84;
  bb25:
    v86 = (char*)v1 + 0;
    v87 = *(int64_t*)v86;
    v88 = 12LL;
    v89 = v87 == v88;
    if (v89) goto bb26;
    else goto bb27;
  bb26:
    v91 = 12LL;
    p_call_stack_pop();
    return v91;
  bb27:
    v93 = (char*)v1 + 0;
    v94 = *(int64_t*)v93;
    v95 = 13LL;
    v96 = v94 == v95;
    if (v96) goto bb28;
    else goto bb29;
  bb28:
    v98 = 13LL;
    p_call_stack_pop();
    return v98;
  bb29:
    v100 = (char*)v1 + 0;
    v101 = *(int64_t*)v100;
    v102 = 14LL;
    v103 = v101 == v102;
    if (v103) goto bb30;
    else goto bb31;
  bb30:
    v105 = 14LL;
    p_call_stack_pop();
    return v105;
  bb31:
    v107 = (char*)v1 + 0;
    v108 = *(int64_t*)v107;
    v109 = 15LL;
    v110 = v108 == v109;
    if (v110) goto bb32;
    else goto bb33;
  bb32:
    v112 = 15LL;
    p_call_stack_pop();
    return v112;
  bb33:
    v114 = (char*)v1 + 0;
    v115 = *(int64_t*)v114;
    v116 = 16LL;
    v117 = v115 == v116;
    if (v117) goto bb34;
    else goto bb35;
  bb34:
    v119 = 16LL;
    p_call_stack_pop();
    return v119;
  bb35:
    v121 = (char*)v1 + 0;
    v122 = *(int64_t*)v121;
    v123 = 17LL;
    v124 = v122 == v123;
    if (v124) goto bb36;
    else goto bb37;
  bb36:
    v126 = 17LL;
    p_call_stack_pop();
    return v126;
  bb37:
    v128 = (char*)v1 + 0;
    v129 = *(int64_t*)v128;
    v130 = 18LL;
    v131 = v129 == v130;
    if (v131) goto bb38;
    else goto bb39;
  bb38:
    v133 = 18LL;
    p_call_stack_pop();
    return v133;
  bb39:
    v135 = (char*)v1 + 0;
    v136 = *(int64_t*)v135;
    v137 = 19LL;
    v138 = v136 == v137;
    if (v138) goto bb40;
    else goto bb41;
  bb40:
    v140 = 19LL;
    p_call_stack_pop();
    return v140;
  bb41:
    v142 = (char*)v1 + 0;
    v143 = *(int64_t*)v142;
    v144 = 20LL;
    v145 = v143 == v144;
    if (v145) goto bb42;
    else goto bb43;
  bb42:
    v147 = 20LL;
    p_call_stack_pop();
    return v147;
  bb43:
    v149 = (char*)v1 + 0;
    v150 = *(int64_t*)v149;
    v151 = 21LL;
    v152 = v150 == v151;
    if (v152) goto bb44;
    else goto bb45;
  bb44:
    v154 = 21LL;
    p_call_stack_pop();
    return v154;
  bb45:
    v156 = (char*)v1 + 0;
    v157 = *(int64_t*)v156;
    v158 = 22LL;
    v159 = v157 == v158;
    if (v159) goto bb46;
    else goto bb47;
  bb46:
    v161 = 22LL;
    p_call_stack_pop();
    return v161;
  bb47:
    v163 = (char*)v1 + 0;
    v164 = *(int64_t*)v163;
    v165 = 23LL;
    v166 = v164 == v165;
    if (v166) goto bb48;
    else goto bb49;
  bb48:
    v168 = 23LL;
    p_call_stack_pop();
    return v168;
  bb49:
    v170 = (char*)v1 + 0;
    v171 = *(int64_t*)v170;
    v172 = 24LL;
    v173 = v171 == v172;
    if (v173) goto bb50;
    else goto bb51;
  bb50:
    v175 = 24LL;
    p_call_stack_pop();
    return v175;
  bb51:
    v177 = (char*)v1 + 0;
    v178 = *(int64_t*)v177;
    v179 = 25LL;
    v180 = v178 == v179;
    if (v180) goto bb52;
    else goto bb53;
  bb52:
    v182 = 25LL;
    p_call_stack_pop();
    return v182;
  bb53:
    v184 = (char*)v1 + 0;
    v185 = *(int64_t*)v184;
    v186 = 26LL;
    v187 = v185 == v186;
    if (v187) goto bb54;
    else goto bb55;
  bb54:
    v189 = 26LL;
    p_call_stack_pop();
    return v189;
  bb55:
    v191 = (char*)v1 + 0;
    v192 = *(int64_t*)v191;
    v193 = 27LL;
    v194 = v192 == v193;
    if (v194) goto bb56;
    else goto bb57;
  bb56:
    v196 = 27LL;
    p_call_stack_pop();
    return v196;
  bb57:
    v198 = (char*)v1 + 0;
    v199 = *(int64_t*)v198;
    v200 = 28LL;
    v201 = v199 == v200;
    if (v201) goto bb58;
    else goto bb59;
  bb58:
    v203 = 28LL;
    p_call_stack_pop();
    return v203;
  bb59:
    v205 = (char*)v1 + 0;
    v206 = *(int64_t*)v205;
    v207 = 29LL;
    v208 = v206 == v207;
    if (v208) goto bb60;
    else goto bb61;
  bb60:
    v210 = 29LL;
    p_call_stack_pop();
    return v210;
  bb61:
    v212 = (char*)v1 + 0;
    v213 = *(int64_t*)v212;
    v214 = 30LL;
    v215 = v213 == v214;
    if (v215) goto bb62;
    else goto bb63;
  bb62:
    v217 = 30LL;
    p_call_stack_pop();
    return v217;
  bb63:
    v219 = (char*)v1 + 0;
    v220 = *(int64_t*)v219;
    v221 = 31LL;
    v222 = v220 == v221;
    if (v222) goto bb64;
    else goto bb65;
  bb64:
    v224 = 31LL;
    p_call_stack_pop();
    return v224;
  bb65:
    v226 = (char*)v1 + 0;
    v227 = *(int64_t*)v226;
    v228 = 32LL;
    v229 = v227 == v228;
    if (v229) goto bb66;
    else goto bb67;
  bb66:
    v231 = 32LL;
    p_call_stack_pop();
    return v231;
  bb67:
    v233 = (char*)v1 + 0;
    v234 = *(int64_t*)v233;
    v235 = 33LL;
    v236 = v234 == v235;
    if (v236) goto bb68;
    else goto bb69;
  bb68:
    v238 = 33LL;
    p_call_stack_pop();
    return v238;
  bb69:
    v240 = (char*)v1 + 0;
    v241 = *(int64_t*)v240;
    v242 = 34LL;
    v243 = v241 == v242;
    if (v243) goto bb70;
    else goto bb71;
  bb70:
    v245 = 34LL;
    p_call_stack_pop();
    return v245;
  bb71:
    v247 = (char*)v1 + 0;
    v248 = *(int64_t*)v247;
    v249 = 35LL;
    v250 = v248 == v249;
    if (v250) goto bb72;
    else goto bb73;
  bb72:
    v252 = 35LL;
    p_call_stack_pop();
    return v252;
  bb73:
    v254 = (char*)v1 + 0;
    v255 = *(int64_t*)v254;
    v256 = 36LL;
    v257 = v255 == v256;
    if (v257) goto bb74;
    else goto bb75;
  bb74:
    v259 = 36LL;
    p_call_stack_pop();
    return v259;
  bb75:
    v261 = (char*)v1 + 0;
    v262 = *(int64_t*)v261;
    v263 = 37LL;
    v264 = v262 == v263;
    if (v264) goto bb76;
    else goto bb77;
  bb76:
    v266 = 37LL;
    p_call_stack_pop();
    return v266;
  bb77:
    v268 = (char*)v1 + 0;
    v269 = *(int64_t*)v268;
    v270 = 38LL;
    v271 = v269 == v270;
    if (v271) goto bb78;
    else goto bb79;
  bb78:
    v273 = 38LL;
    p_call_stack_pop();
    return v273;
  bb79:
    v275 = (char*)v1 + 0;
    v276 = *(int64_t*)v275;
    v277 = 39LL;
    v278 = v276 == v277;
    if (v278) goto bb80;
    else goto bb81;
  bb80:
    v280 = 39LL;
    p_call_stack_pop();
    return v280;
  bb81:
    v282 = (char*)v1 + 0;
    v283 = *(int64_t*)v282;
    v284 = 40LL;
    v285 = v283 == v284;
    if (v285) goto bb82;
    else goto bb83;
  bb82:
    v287 = 40LL;
    p_call_stack_pop();
    return v287;
  bb83:
    v289 = (char*)v1 + 0;
    v290 = *(int64_t*)v289;
    v291 = 41LL;
    v292 = v290 == v291;
    if (v292) goto bb84;
    else goto bb85;
  bb84:
    v294 = 41LL;
    p_call_stack_pop();
    return v294;
  bb85:
    v296 = (char*)v1 + 0;
    v297 = *(int64_t*)v296;
    v298 = 42LL;
    v299 = v297 == v298;
    if (v299) goto bb86;
    else goto bb87;
  bb86:
    v301 = 42LL;
    p_call_stack_pop();
    return v301;
  bb87:
    v303 = (char*)v1 + 0;
    v304 = *(int64_t*)v303;
    v305 = 43LL;
    v306 = v304 == v305;
    if (v306) goto bb88;
    else goto bb89;
  bb88:
    v308 = 43LL;
    p_call_stack_pop();
    return v308;
  bb89:
    v310 = (char*)v1 + 0;
    v311 = *(int64_t*)v310;
    v312 = 44LL;
    v313 = v311 == v312;
    if (v313) goto bb90;
    else goto bb91;
  bb90:
    v315 = 44LL;
    p_call_stack_pop();
    return v315;
  bb91:
    v317 = (char*)v1 + 0;
    v318 = *(int64_t*)v317;
    v319 = 45LL;
    v320 = v318 == v319;
    if (v320) goto bb92;
    else goto bb93;
  bb92:
    v322 = 45LL;
    p_call_stack_pop();
    return v322;
  bb93:
    v324 = (char*)v1 + 0;
    v325 = *(int64_t*)v324;
    v326 = 46LL;
    v327 = v325 == v326;
    if (v327) goto bb94;
    else goto bb95;
  bb94:
    v329 = 46LL;
    p_call_stack_pop();
    return v329;
  bb95:
    v331 = (char*)v1 + 0;
    v332 = *(int64_t*)v331;
    v333 = 47LL;
    v334 = v332 == v333;
    if (v334) goto bb96;
    else goto bb97;
  bb96:
    v336 = 47LL;
    p_call_stack_pop();
    return v336;
  bb97:
    v338 = (char*)v1 + 0;
    v339 = *(int64_t*)v338;
    v340 = 48LL;
    v341 = v339 == v340;
    if (v341) goto bb98;
    else goto bb99;
  bb98:
    v343 = 48LL;
    p_call_stack_pop();
    return v343;
  bb99:
    v345 = (char*)v1 + 0;
    v346 = *(int64_t*)v345;
    v347 = 49LL;
    v348 = v346 == v347;
    if (v348) goto bb100;
    else goto bb101;
  bb100:
    v350 = 49LL;
    p_call_stack_pop();
    return v350;
  bb101:
    v352 = (char*)v1 + 0;
    v353 = *(int64_t*)v352;
    v354 = 50LL;
    v355 = v353 == v354;
    if (v355) goto bb102;
    else goto bb103;
  bb102:
    v357 = 50LL;
    p_call_stack_pop();
    return v357;
  bb103:
    v359 = (char*)v1 + 0;
    v360 = *(int64_t*)v359;
    v361 = 51LL;
    v362 = v360 == v361;
    if (v362) goto bb104;
    else goto bb105;
  bb104:
    v364 = 51LL;
    p_call_stack_pop();
    return v364;
  bb105:
    v366 = (char*)v1 + 0;
    v367 = *(int64_t*)v366;
    v368 = 52LL;
    v369 = v367 == v368;
    if (v369) goto bb106;
    else goto bb107;
  bb106:
    v371 = 52LL;
    p_call_stack_pop();
    return v371;
  bb107:
    v373 = (char*)v1 + 0;
    v374 = *(int64_t*)v373;
    v375 = 53LL;
    v376 = v374 == v375;
    if (v376) goto bb108;
    else goto bb109;
  bb108:
    v378 = 53LL;
    p_call_stack_pop();
    return v378;
  bb109:
    v380 = (char*)v1 + 0;
    v381 = *(int64_t*)v380;
    v382 = 54LL;
    v383 = v381 == v382;
    if (v383) goto bb110;
    else goto bb111;
  bb110:
    v385 = 54LL;
    p_call_stack_pop();
    return v385;
  bb111:
    v387 = (char*)v1 + 0;
    v388 = *(int64_t*)v387;
    v389 = 55LL;
    v390 = v388 == v389;
    if (v390) goto bb112;
    else goto bb113;
  bb112:
    v392 = 55LL;
    p_call_stack_pop();
    return v392;
  bb113:
    v394 = (char*)v1 + 0;
    v395 = *(int64_t*)v394;
    v396 = 56LL;
    v397 = v395 == v396;
    if (v397) goto bb114;
    else goto bb115;
  bb114:
    v399 = 56LL;
    p_call_stack_pop();
    return v399;
  bb115:
    v401 = (char*)v1 + 0;
    v402 = *(int64_t*)v401;
    v403 = 57LL;
    v404 = v402 == v403;
    if (v404) goto bb116;
    else goto bb117;
  bb116:
    v406 = 57LL;
    p_call_stack_pop();
    return v406;
  bb117:
    v408 = (char*)v1 + 0;
    v409 = *(int64_t*)v408;
    v410 = 58LL;
    v411 = v409 == v410;
    if (v411) goto bb118;
    else goto bb119;
  bb118:
    v413 = 58LL;
    p_call_stack_pop();
    return v413;
  bb119:
    v415 = (char*)v1 + 0;
    v416 = *(int64_t*)v415;
    v417 = 59LL;
    v418 = v416 == v417;
    if (v418) goto bb120;
    else goto bb121;
  bb120:
    v420 = 59LL;
    p_call_stack_pop();
    return v420;
  bb121:
    v422 = (char*)v1 + 0;
    v423 = *(int64_t*)v422;
    v424 = 60LL;
    v425 = v423 == v424;
    if (v425) goto bb122;
    else goto bb123;
  bb122:
    v427 = 60LL;
    p_call_stack_pop();
    return v427;
  bb123:
    v429 = (char*)v1 + 0;
    v430 = *(int64_t*)v429;
    v431 = 61LL;
    v432 = v430 == v431;
    if (v432) goto bb124;
    else goto bb125;
  bb124:
    v434 = 61LL;
    p_call_stack_pop();
    return v434;
  bb125:
    v436 = (char*)v1 + 0;
    v437 = *(int64_t*)v436;
    v438 = 62LL;
    v439 = v437 == v438;
    if (v439) goto bb126;
    else goto bb127;
  bb126:
    v441 = 62LL;
    p_call_stack_pop();
    return v441;
  bb127:
    v443 = (char*)v1 + 0;
    v444 = *(int64_t*)v443;
    v445 = 63LL;
    v446 = v444 == v445;
    if (v446) goto bb128;
    else goto bb129;
  bb128:
    v448 = 63LL;
    p_call_stack_pop();
    return v448;
  bb129:
    v450 = (char*)v1 + 0;
    v451 = *(int64_t*)v450;
    v452 = 64LL;
    v453 = v451 == v452;
    if (v453) goto bb130;
    else goto bb131;
  bb130:
    v455 = 64LL;
    p_call_stack_pop();
    return v455;
  bb131:
    v457 = (char*)v1 + 0;
    v458 = *(int64_t*)v457;
    v459 = 65LL;
    v460 = v458 == v459;
    if (v460) goto bb132;
    else goto bb133;
  bb132:
    v462 = 65LL;
    p_call_stack_pop();
    return v462;
  bb133:
    v464 = (char*)v1 + 0;
    v465 = *(int64_t*)v464;
    v466 = 66LL;
    v467 = v465 == v466;
    if (v467) goto bb134;
    else goto bb135;
  bb134:
    v469 = 66LL;
    p_call_stack_pop();
    return v469;
  bb135:
    v471 = (char*)v1 + 0;
    v472 = *(int64_t*)v471;
    v473 = 67LL;
    v474 = v472 == v473;
    if (v474) goto bb136;
    else goto bb137;
  bb136:
    v476 = 67LL;
    p_call_stack_pop();
    return v476;
  bb137:
    v478 = (char*)v1 + 0;
    v479 = *(int64_t*)v478;
    v480 = 68LL;
    v481 = v479 == v480;
    if (v481) goto bb138;
    else goto bb139;
  bb138:
    v483 = 68LL;
    p_call_stack_pop();
    return v483;
  bb139:
    v485 = (char*)v1 + 0;
    v486 = *(int64_t*)v485;
    v487 = 69LL;
    v488 = v486 == v487;
    if (v488) goto bb140;
    else goto bb141;
  bb140:
    v490 = 69LL;
    p_call_stack_pop();
    return v490;
  bb141:
    v492 = (char*)v1 + 0;
    v493 = *(int64_t*)v492;
    v494 = 70LL;
    v495 = v493 == v494;
    if (v495) goto bb142;
    else goto bb143;
  bb142:
    v497 = 70LL;
    p_call_stack_pop();
    return v497;
  bb143:
    v499 = (char*)v1 + 0;
    v500 = *(int64_t*)v499;
    v501 = 71LL;
    v502 = v500 == v501;
    if (v502) goto bb144;
    else goto bb145;
  bb144:
    v504 = 71LL;
    p_call_stack_pop();
    return v504;
  bb145:
    v506 = (char*)v1 + 0;
    v507 = *(int64_t*)v506;
    v508 = 72LL;
    v509 = v507 == v508;
    if (v509) goto bb146;
    else goto bb147;
  bb146:
    v511 = 72LL;
    p_call_stack_pop();
    return v511;
  bb147:
    v513 = (char*)v1 + 0;
    v514 = *(int64_t*)v513;
    v515 = 73LL;
    v516 = v514 == v515;
    if (v516) goto bb148;
    else goto bb149;
  bb148:
    v518 = 73LL;
    p_call_stack_pop();
    return v518;
  bb149:
    v520 = (char*)v1 + 0;
    v521 = *(int64_t*)v520;
    v522 = 74LL;
    v523 = v521 == v522;
    if (v523) goto bb150;
    else goto bb151;
  bb150:
    v525 = 74LL;
    p_call_stack_pop();
    return v525;
  bb151:
    v527 = (char*)v1 + 0;
    v528 = *(int64_t*)v527;
    v529 = 75LL;
    v530 = v528 == v529;
    if (v530) goto bb152;
    else goto bb153;
  bb152:
    v532 = 75LL;
    p_call_stack_pop();
    return v532;
  bb153:
    v534 = (char*)v1 + 0;
    v535 = *(int64_t*)v534;
    v536 = 76LL;
    v537 = v535 == v536;
    if (v537) goto bb154;
    else goto bb155;
  bb154:
    v539 = (char*)v1 + 8;
    v540 = *(int64_t*)v539;
    v541 = 76LL;
    p_call_stack_pop();
    return v541;
  bb155:
    v543 = (char*)v1 + 0;
    v544 = *(int64_t*)v543;
    v545 = 77LL;
    v546 = v544 == v545;
    if (v546) goto bb156;
    else goto bb157;
  bb156:
    v548 = (char*)v1 + 8;
    v549 = *(double*)v548;
    v550 = 77LL;
    p_call_stack_pop();
    return v550;
  bb157:
    v552 = (char*)v1 + 0;
    v553 = *(int64_t*)v552;
    v554 = 78LL;
    v555 = v553 == v554;
    if (v555) goto bb158;
    else goto bb159;
  bb158:
    v557 = (char*)v1 + 8;
    v558 = *(void**)v557;
    v559 = 78LL;
    p_call_stack_pop();
    return v559;
  bb159:
    v561 = (char*)v1 + 0;
    v562 = *(int64_t*)v561;
    v563 = 79LL;
    v564 = v562 == v563;
    if (v564) goto bb160;
    else goto bb161;
  bb160:
    v566 = (char*)v1 + 8;
    v567 = *(void**)v566;
    v568 = 79LL;
    p_call_stack_pop();
    return v568;
  bb161:
    v570 = (char*)v1 + 0;
    v571 = *(int64_t*)v570;
    v572 = 80LL;
    v573 = v571 == v572;
    if (v573) goto bb162;
    else goto bb163;
  bb162:
    v575 = (char*)v1 + 8;
    v576 = *(void**)v575;
    v577 = 80LL;
    p_call_stack_pop();
    return v577;
  bb163:
    v579 = (char*)v1 + 0;
    v580 = *(int64_t*)v579;
    v581 = 81LL;
    v582 = v580 == v581;
    if (v582) goto bb164;
    else goto bb165;
  bb164:
    v584 = 81LL;
    p_call_stack_pop();
    return v584;
  bb165:
    v586 = (char*)v1 + 0;
    v587 = *(int64_t*)v586;
    v588 = 82LL;
    v589 = v587 == v588;
    if (v589) goto bb166;
    else goto bb167;
  bb166:
    v591 = 82LL;
    p_call_stack_pop();
    return v591;
  bb167:
    v593 = (char*)v1 + 0;
    v594 = *(int64_t*)v593;
    v595 = 83LL;
    v596 = v594 == v595;
    if (v596) goto bb168;
    else goto bb169;
  bb168:
    v598 = 83LL;
    p_call_stack_pop();
    return v598;
  bb169:
    v600 = (char*)v1 + 0;
    v601 = *(int64_t*)v600;
    v602 = 84LL;
    v603 = v601 == v602;
    if (v603) goto bb170;
    else goto bb171;
  bb170:
    v605 = 84LL;
    p_call_stack_pop();
    return v605;
  bb171:
    v607 = (char*)v1 + 0;
    v608 = *(int64_t*)v607;
    v609 = 85LL;
    v610 = v608 == v609;
    if (v610) goto bb172;
    else goto bb173;
  bb172:
    v612 = 85LL;
    p_call_stack_pop();
    return v612;
  bb173:
    v614 = (char*)v1 + 0;
    v615 = *(int64_t*)v614;
    v616 = 86LL;
    v617 = v615 == v616;
    if (v617) goto bb174;
    else goto bb175;
  bb174:
    v619 = 86LL;
    p_call_stack_pop();
    return v619;
  bb175:
    v621 = (char*)v1 + 0;
    v622 = *(int64_t*)v621;
    v623 = 87LL;
    v624 = v622 == v623;
    if (v624) goto bb176;
    else goto bb177;
  bb176:
    v626 = 87LL;
    p_call_stack_pop();
    return v626;
  bb177:
    v628 = (char*)v1 + 0;
    v629 = *(int64_t*)v628;
    v630 = 88LL;
    v631 = v629 == v630;
    if (v631) goto bb178;
    else goto bb179;
  bb178:
    v633 = 88LL;
    p_call_stack_pop();
    return v633;
  bb179:
    v635 = (char*)v1 + 0;
    v636 = *(int64_t*)v635;
    v637 = 89LL;
    v638 = v636 == v637;
    if (v638) goto bb180;
    else goto bb181;
  bb180:
    v640 = 89LL;
    p_call_stack_pop();
    return v640;
  bb181:
    v642 = (char*)v1 + 0;
    v643 = *(int64_t*)v642;
    v644 = 90LL;
    v645 = v643 == v644;
    if (v645) goto bb182;
    else goto bb183;
  bb182:
    v647 = 90LL;
    p_call_stack_pop();
    return v647;
  bb183:
    v649 = (char*)v1 + 0;
    v650 = *(int64_t*)v649;
    v651 = 91LL;
    v652 = v650 == v651;
    if (v652) goto bb184;
    else goto bb185;
  bb184:
    v654 = 91LL;
    p_call_stack_pop();
    return v654;
  bb185:
    v656 = (char*)v1 + 0;
    v657 = *(int64_t*)v656;
    v658 = 92LL;
    v659 = v657 == v658;
    if (v659) goto bb186;
    else goto bb187;
  bb186:
    v661 = 92LL;
    p_call_stack_pop();
    return v661;
  bb187:
    v663 = (char*)v1 + 0;
    v664 = *(int64_t*)v663;
    v665 = 93LL;
    v666 = v664 == v665;
    if (v666) goto bb188;
    else goto bb189;
  bb188:
    v668 = 93LL;
    p_call_stack_pop();
    return v668;
  bb189:
    v670 = (char*)v1 + 0;
    v671 = *(int64_t*)v670;
    v672 = 94LL;
    v673 = v671 == v672;
    if (v673) goto bb190;
    else goto bb191;
  bb190:
    v675 = 94LL;
    p_call_stack_pop();
    return v675;
  bb191:
    v677 = (char*)v1 + 0;
    v678 = *(int64_t*)v677;
    v679 = 95LL;
    v680 = v678 == v679;
    if (v680) goto bb192;
    else goto bb193;
  bb192:
    v682 = 95LL;
    p_call_stack_pop();
    return v682;
  bb193:
    v684 = (char*)v1 + 0;
    v685 = *(int64_t*)v684;
    v686 = 96LL;
    v687 = v685 == v686;
    if (v687) goto bb194;
    else goto bb195;
  bb194:
    v689 = 96LL;
    p_call_stack_pop();
    return v689;
  bb195:
    v691 = (char*)v1 + 0;
    v692 = *(int64_t*)v691;
    v693 = 97LL;
    v694 = v692 == v693;
    if (v694) goto bb196;
    else goto bb197;
  bb196:
    v696 = 97LL;
    p_call_stack_pop();
    return v696;
  bb197:
    v698 = (char*)v1 + 0;
    v699 = *(int64_t*)v698;
    v700 = 98LL;
    v701 = v699 == v700;
    if (v701) goto bb198;
    else goto bb199;
  bb198:
    v703 = 98LL;
    p_call_stack_pop();
    return v703;
  bb199:
    v705 = (char*)v1 + 0;
    v706 = *(int64_t*)v705;
    v707 = 99LL;
    v708 = v706 == v707;
    if (v708) goto bb200;
    else goto bb201;
  bb200:
    v710 = 99LL;
    p_call_stack_pop();
    return v710;
  bb201:
    v712 = (char*)v1 + 0;
    v713 = *(int64_t*)v712;
    v714 = 100LL;
    v715 = v713 == v714;
    if (v715) goto bb202;
    else goto bb203;
  bb202:
    v717 = 100LL;
    p_call_stack_pop();
    return v717;
  bb203:
    v719 = (char*)v1 + 0;
    v720 = *(int64_t*)v719;
    v721 = 101LL;
    v722 = v720 == v721;
    if (v722) goto bb204;
    else goto bb205;
  bb204:
    v724 = 101LL;
    p_call_stack_pop();
    return v724;
  bb205:
    v726 = (char*)v1 + 0;
    v727 = *(int64_t*)v726;
    v728 = 102LL;
    v729 = v727 == v728;
    if (v729) goto bb206;
    else goto bb207;
  bb206:
    v731 = 102LL;
    p_call_stack_pop();
    return v731;
  bb207:
    v733 = (char*)v1 + 0;
    v734 = *(int64_t*)v733;
    v735 = 103LL;
    v736 = v734 == v735;
    if (v736) goto bb208;
    else goto bb209;
  bb208:
    v738 = 103LL;
    p_call_stack_pop();
    return v738;
  bb209:
    v740 = (char*)v1 + 0;
    v741 = *(int64_t*)v740;
    v742 = 104LL;
    v743 = v741 == v742;
    if (v743) goto bb210;
    else goto bb211;
  bb210:
    v745 = 104LL;
    p_call_stack_pop();
    return v745;
  bb211:
    v747 = (char*)v1 + 0;
    v748 = *(int64_t*)v747;
    v749 = 105LL;
    v750 = v748 == v749;
    if (v750) goto bb212;
    else goto bb213;
  bb212:
    v752 = 105LL;
    p_call_stack_pop();
    return v752;
  bb213:
    v754 = (char*)v1 + 0;
    v755 = *(int64_t*)v754;
    v756 = 106LL;
    v757 = v755 == v756;
    if (v757) goto bb214;
    else goto bb215;
  bb214:
    v759 = 106LL;
    p_call_stack_pop();
    return v759;
  bb215:
    v761 = (char*)v1 + 0;
    v762 = *(int64_t*)v761;
    v763 = 107LL;
    v764 = v762 == v763;
    if (v764) goto bb216;
    else goto bb217;
  bb216:
    v766 = 107LL;
    p_call_stack_pop();
    return v766;
  bb217:
    v768 = (char*)v1 + 0;
    v769 = *(int64_t*)v768;
    v770 = 108LL;
    v771 = v769 == v770;
    if (v771) goto bb218;
    else goto bb219;
  bb218:
    v773 = 108LL;
    p_call_stack_pop();
    return v773;
  bb219:
    v775 = (char*)v1 + 0;
    v776 = *(int64_t*)v775;
    v777 = 109LL;
    v778 = v776 == v777;
    if (v778) goto bb220;
    else goto bb221;
  bb220:
    v780 = 109LL;
    p_call_stack_pop();
    return v780;
  bb221:
    v782 = (char*)v1 + 0;
    v783 = *(int64_t*)v782;
    v784 = 110LL;
    v785 = v783 == v784;
    if (v785) goto bb222;
    else goto bb223;
  bb222:
    v787 = 110LL;
    p_call_stack_pop();
    return v787;
  bb223:
    v789 = (char*)v1 + 0;
    v790 = *(int64_t*)v789;
    v791 = 111LL;
    v792 = v790 == v791;
    if (v792) goto bb224;
    else goto bb225;
  bb224:
    v794 = 111LL;
    p_call_stack_pop();
    return v794;
  bb225:
    v796 = (char*)v1 + 0;
    v797 = *(int64_t*)v796;
    v798 = 112LL;
    v799 = v797 == v798;
    if (v799) goto bb226;
    else goto bb227;
  bb226:
    v801 = 112LL;
    p_call_stack_pop();
    return v801;
  bb227:
    v803 = (char*)v1 + 0;
    v804 = *(int64_t*)v803;
    v805 = 113LL;
    v806 = v804 == v805;
    if (v806) goto bb228;
    else goto bb229;
  bb228:
    v808 = 113LL;
    p_call_stack_pop();
    return v808;
  bb229:
    v810 = (char*)v1 + 0;
    v811 = *(int64_t*)v810;
    v812 = 114LL;
    v813 = v811 == v812;
    if (v813) goto bb230;
    else goto bb231;
  bb230:
    v815 = 114LL;
    p_call_stack_pop();
    return v815;
  bb231:
    v817 = (char*)v1 + 0;
    v818 = *(int64_t*)v817;
    v819 = 115LL;
    v820 = v818 == v819;
    if (v820) goto bb232;
    else goto bb233;
  bb232:
    v822 = 115LL;
    p_call_stack_pop();
    return v822;
  bb233:
    v824 = (char*)v1 + 0;
    v825 = *(int64_t*)v824;
    v826 = 116LL;
    v827 = v825 == v826;
    if (v827) goto bb234;
    else goto bb235;
  bb234:
    v829 = 116LL;
    p_call_stack_pop();
    return v829;
  bb235:
    v831 = (char*)v1 + 0;
    v832 = *(int64_t*)v831;
    v833 = 117LL;
    v834 = v832 == v833;
    if (v834) goto bb236;
    else goto bb237;
  bb236:
    v836 = 117LL;
    p_call_stack_pop();
    return v836;
  bb237:
    v838 = (char*)v1 + 0;
    v839 = *(int64_t*)v838;
    v840 = 118LL;
    v841 = v839 == v840;
    if (v841) goto bb238;
    else goto bb239;
  bb238:
    v843 = 118LL;
    p_call_stack_pop();
    return v843;
  bb239:
    v845 = (char*)v1 + 0;
    v846 = *(int64_t*)v845;
    v847 = 119LL;
    v848 = v846 == v847;
    if (v848) goto bb240;
    else goto bb241;
  bb240:
    v850 = 119LL;
    p_call_stack_pop();
    return v850;
  bb241:
    v852 = (char*)v1 + 0;
    v853 = *(int64_t*)v852;
    v854 = 120LL;
    v855 = v853 == v854;
    if (v855) goto bb242;
    else goto bb243;
  bb242:
    v857 = 120LL;
    p_call_stack_pop();
    return v857;
  bb243:
    v859 = (char*)v1 + 0;
    v860 = *(int64_t*)v859;
    v861 = 121LL;
    v862 = v860 == v861;
    if (v862) goto bb244;
    else goto bb245;
  bb244:
    v864 = 121LL;
    p_call_stack_pop();
    return v864;
  bb245:
    v866 = (char*)v1 + 0;
    v867 = *(int64_t*)v866;
    v868 = 122LL;
    v869 = v867 == v868;
    if (v869) goto bb246;
    else goto bb247;
  bb246:
    v871 = 122LL;
    p_call_stack_pop();
    return v871;
  bb247:
    v873 = (char*)v1 + 0;
    v874 = *(int64_t*)v873;
    v875 = 123LL;
    v876 = v874 == v875;
    if (v876) goto bb248;
    else goto bb249;
  bb248:
    v878 = 123LL;
    p_call_stack_pop();
    return v878;
  bb249:
    v880 = (char*)v1 + 0;
    v881 = *(int64_t*)v880;
    v882 = 124LL;
    v883 = v881 == v882;
    if (v883) goto bb250;
    else goto bb251;
  bb250:
    v885 = 124LL;
    p_call_stack_pop();
    return v885;
  bb251:
    v887 = (char*)v1 + 0;
    v888 = *(int64_t*)v887;
    v889 = 125LL;
    v890 = v888 == v889;
    if (v890) goto bb252;
    else goto bb253;
  bb252:
    v892 = 125LL;
    p_call_stack_pop();
    return v892;
  bb253:
    v894 = (char*)v1 + 0;
    v895 = *(int64_t*)v894;
    v896 = 126LL;
    v897 = v895 == v896;
    if (v897) goto bb254;
    else goto bb255;
  bb254:
    v899 = 126LL;
    p_call_stack_pop();
    return v899;
  bb255:
    v901 = (char*)v1 + 0;
    v902 = *(int64_t*)v901;
    v903 = 127LL;
    v904 = v902 == v903;
    if (v904) goto bb256;
    else goto bb257;
  bb256:
    v906 = 127LL;
    p_call_stack_pop();
    return v906;
  bb257:
    v908 = (char*)v1 + 0;
    v909 = *(int64_t*)v908;
    v910 = 128LL;
    v911 = v909 == v910;
    if (v911) goto bb258;
    else goto bb259;
  bb258:
    v913 = 128LL;
    p_call_stack_pop();
    return v913;
  bb259:
    v915 = (char*)v1 + 0;
    v916 = *(int64_t*)v915;
    v917 = 129LL;
    v918 = v916 == v917;
    if (v918) goto bb260;
    else goto bb261;
  bb260:
    v920 = 129LL;
    p_call_stack_pop();
    return v920;
  bb261:
    v922 = (char*)v1 + 0;
    v923 = *(int64_t*)v922;
    v924 = 130LL;
    v925 = v923 == v924;
    if (v925) goto bb262;
    else goto bb263;
  bb262:
    v927 = 130LL;
    p_call_stack_pop();
    return v927;
  bb263:
    v929 = (char*)v1 + 0;
    v930 = *(int64_t*)v929;
    v931 = 131LL;
    v932 = v930 == v931;
    if (v932) goto bb264;
    else goto bb265;
  bb264:
    v934 = 131LL;
    p_call_stack_pop();
    return v934;
  bb265:
    v936 = (char*)v1 + 0;
    v937 = *(int64_t*)v936;
    v938 = 132LL;
    v939 = v937 == v938;
    if (v939) goto bb266;
    else goto bb267;
  bb266:
    v941 = 132LL;
    p_call_stack_pop();
    return v941;
  bb267:
    v943 = (char*)v1 + 0;
    v944 = *(int64_t*)v943;
    v945 = 133LL;
    v946 = v944 == v945;
    if (v946) goto bb268;
    else goto bb269;
  bb268:
    v948 = 133LL;
    p_call_stack_pop();
    return v948;
  bb269:
    v950 = (char*)v1 + 0;
    v951 = *(int64_t*)v950;
    v952 = 134LL;
    v953 = v951 == v952;
    if (v953) goto bb270;
    else goto bb271;
  bb270:
    v955 = 134LL;
    p_call_stack_pop();
    return v955;
  bb271:
    goto bb1;
    return 0;
}

int64_t char_to_upper(void* v0, int64_t v1) {
    p_call_stack_push("char_to_upper");
    int64_t v4;
    int64_t v2;
    int64_t v8;
    int64_t v9;
    int64_t v11; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    bool v6;
    bool v5;
    int64_t v7; /* placeholder for void */
    bool v3;
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

void Result_and_then(void* v0, void* v1, void* v2, void* v3) {
    p_call_stack_push("Result_and_then");
    void* v16;
    void* v22;
    int64_t v29;
    int64_t v35;
    void* v40;
    void* v13;
    int64_t v21; /* placeholder for void */
    void* v11;
    void* v32;
    int64_t v42;
    int64_t v20;
    int64_t v26; /* placeholder for void */
    void* v34;
    int64_t v46;
    int64_t v6;
    int64_t v5;
    int64_t v15;
    bool v7;
    int64_t v10;
    int64_t v8; /* placeholder for void */
    void* v27;
    void* v18;
    int64_t v28;
    int64_t v37; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    void* v23;
    int64_t v43; /* placeholder for void */
    int64_t v12;
    void* v44;
    void* v45;
    int64_t v47; /* placeholder for void */
    void* v9;
    bool v30;
    int64_t v49; /* placeholder for void */
    int64_t v25; /* placeholder for void */
    void* v38;
    int64_t v24;
    void* v19;
    void* v41;
    void* v36;
    int64_t v50; /* placeholder for void */
    int64_t v48; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v4;
    int64_t v17; /* placeholder for void */
    int64_t v33;
    int64_t v31; /* placeholder for void */
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
        if (strcmp(_fn_name, "StringBuilder_len") == 0) {
            v15 = StringBuilder_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_first") == 0) {
            v15 = Vec_first((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_values") == 0) {
            v15 = HashMap_values((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_upper") == 0) {
            v15 = char_to_upper((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_len") == 0) {
            v15 = Vec_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_to_string") == 0) {
            v15 = String_to_string((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_hex") == 0) {
            v15 = str_parse_hex((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_peek") == 0) {
            v15 = Lexer_peek((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "token_kind_tag") == 0) {
            v15 = token_kind_tag((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "vec_enumerate") == 0) {
            v15 = vec_enumerate((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_capacity") == 0) {
            v15 = Vec_capacity((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "allocator_bytes_used") == 0) {
            v15 = allocator_bytes_used((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Lexer_advance") == 0) {
            v15 = Lexer_advance((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Vec_pop") == 0) {
            v15 = Vec_pop((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_hash") == 0) {
            v15 = p_str_hash((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_clone") == 0) {
            v15 = String_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_keys") == 0) {
            v15 = HashMap_keys((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_parse_int") == 0) {
            v15 = str_parse_int((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "HashMap_clone") == 0) {
            v15 = HashMap_clone((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_lowercase") == 0) {
            v15 = String_to_lowercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_parse_int") == 0) {
            v15 = String_parse_int((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_trim") == 0) {
            v15 = String_trim((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_bytes") == 0) {
            v15 = String_bytes((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Option_unwrap") == 0) {
            v15 = Option_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_from") == 0) {
            v15 = String_from((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_to_uppercase") == 0) {
            v15 = String_to_uppercase((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "char_to_lower") == 0) {
            v15 = char_to_lower((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "HashMap_len") == 0) {
            v15 = HashMap_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "String_len") == 0) {
            v15 = String_len((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "abs") == 0) {
            v15 = p_abs((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "Vec_last") == 0) {
            v15 = Vec_last((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "ByteBuffer_len") == 0) {
            v15 = ByteBuffer_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "p_str_length") == 0) {
            v15 = p_str_length((void*)v3, (int64_t)v10);
        } else if (strcmp(_fn_name, "str_len") == 0) {
            v15 = str_len((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "Result_unwrap") == 0) {
            v15 = Result_unwrap((void*)v3, (void*)v10);
        } else if (strcmp(_fn_name, "String_char_count") == 0) {
            v15 = String_char_count((void*)v3, (int64_t)v10);
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

void* StringBuilder_to_str(void* v0, void* v1) {
    p_call_stack_push("StringBuilder_to_str");
    int64_t v4; /* placeholder for void */
    void* v2;
    void* v3;
    v2 = (char*)v1 + 0;
    v3 = *(void**)v2;
    p_call_stack_pop();
    return v3;
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

void StringBuilder_append_str(void* v0, void* v1, void* v2) {
    p_call_stack_push("StringBuilder_append_str");
    int64_t v7; /* placeholder for void */
    void* v3;
    int64_t v10;
    int64_t v13;
    int64_t v12;
    int64_t v11;
    int64_t v14; /* placeholder for void */
    void* v5;
    void* v9;
    void* v4;
    int64_t v15; /* placeholder for void */
    void* v6;
    void* v8;
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

int64_t vec_chunk(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_chunk");
    int64_t v23;
    int64_t v14; /* placeholder for void */
    int64_t v7;
    int64_t v18; /* placeholder for void */
    int64_t v27;
    void* v5;
    int64_t v8;
    int64_t v10; /* placeholder for void */
    int64_t v4;
    int64_t v26; /* placeholder for void */
    int64_t v3;
    int64_t v19;
    int64_t v25; /* placeholder for void */
    void* v9;
    int64_t v22; /* placeholder for void */
    int64_t v15;
    int64_t v13; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    bool v17;
    int64_t v24;
    int64_t v6; /* placeholder for void */
    int64_t v16;
    int64_t v20;
    int64_t v21;
    void* v12;
    int64_t v11;
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

double str_parse_float(void* v0, void* v1) {
    p_call_stack_push("str_parse_float");
    int64_t v3; /* placeholder for void */
    double v2;
    v2 = (double)p_str_to_float(v1);
    p_call_stack_pop();
    return v2;
    return 0;
}

int64_t String_to_lowercase(void* v0, int64_t v1) {
    p_call_stack_push("String_to_lowercase");
    int64_t v20;
    int64_t v9;
    int64_t v79; /* placeholder for void */
    int64_t v47;
    int64_t v5;
    int64_t v21;
    void* v57;
    int64_t v68; /* placeholder for void */
    int64_t v46; /* placeholder for void */
    int64_t v32; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v27;
    bool v67;
    int64_t v50;
    void* v54;
    int64_t v87;
    int64_t v4; /* placeholder for void */
    void* v88;
    int64_t v53;
    int64_t v29; /* placeholder for void */
    int64_t v62;
    int64_t v43;
    int64_t v90;
    int64_t v93; /* placeholder for void */
    int64_t v95;
    int64_t v41; /* placeholder for void */
    int64_t v89; /* placeholder for void */
    int64_t v33; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    void* v6;
    int64_t v56; /* placeholder for void */
    bool v63;
    void* v40;
    int64_t v80;
    int64_t v23; /* placeholder for void */
    int64_t v61;
    bool v3;
    int64_t v12; /* placeholder for void */
    int64_t v35;
    int64_t v38;
    int64_t v59; /* placeholder for void */
    void* v10;
    int64_t v15; /* placeholder for void */
    int64_t v64;
    int64_t v72; /* placeholder for void */
    int64_t v55; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v30;
    void* v22;
    int64_t v96; /* placeholder for void */
    int64_t v34;
    int64_t v42;
    void* v94;
    int64_t v44;
    int64_t v82;
    void* v7;
    int64_t v24;
    int64_t v19; /* placeholder for void */
    int64_t v73; /* placeholder for void */
    int64_t v17;
    int64_t v70;
    int64_t v25;
    void* v28;
    int64_t v8; /* placeholder for void */
    void* v91;
    void* v13;
    int64_t v83; /* placeholder for void */
    int64_t v2;
    int64_t v86;
    int64_t v97; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v26;
    int64_t v18; /* placeholder for void */
    uint8_t v58;
    bool v66;
    void* v76;
    void* v51;
    int64_t v60; /* placeholder for void */
    int64_t v69;
    int64_t v81;
    int64_t v37; /* placeholder for void */
    int64_t v85;
    int64_t v98;
    void* v31;
    bool v36;
    int64_t v78; /* placeholder for void */
    int64_t v71;
    int64_t v39;
    int64_t v74; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v14;
    int64_t v77;
    int64_t v65;
    int64_t v92; /* placeholder for void */
    void* v45;
    void* v48;
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

bool lex_is_whitespace(void* v0, int64_t v1) {
    p_call_stack_push("lex_is_whitespace");
    bool v18;
    bool v17;
    int64_t v4;
    int64_t v13;
    int64_t v19; /* placeholder for void */
    bool v6;
    int64_t v7;
    bool v5;
    bool v11;
    bool v15;
    bool v3;
    int64_t v2;
    bool v9;
    bool v12;
    int64_t v16;
    int64_t v10;
    bool v14;
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
    v13 = 12LL;
    v14 = v1 == v13;
    v15 = v12 || v14;
    v16 = 11LL;
    v17 = v1 == v16;
    v18 = v15 || v17;
    p_call_stack_pop();
    return v18;
    return 0;
}

int64_t p_abs(void* v0, int64_t v1) {
    p_call_stack_push("abs");
    int64_t v4; /* placeholder for void */
    bool v3;
    int64_t v6; /* placeholder for void */
    int64_t v2;
    int64_t v5;
    int64_t v7; /* placeholder for void */
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

void* p_str_from_ptr(void* v0, int64_t v1) {
    p_call_stack_push("p_str_from_ptr");
    int64_t v27; /* placeholder for void */
    int64_t v45; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v70; /* placeholder for void */
    int64_t v59;
    int64_t v12; /* placeholder for void */
    void* v62;
    int64_t v72;
    int64_t v85;
    int64_t v34;
    int64_t v32; /* placeholder for void */
    bool v36;
    int64_t v67; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v97;
    int64_t v86; /* placeholder for void */
    int64_t v82; /* placeholder for void */
    int64_t v81;
    int64_t v25;
    int64_t v33; /* placeholder for void */
    int64_t v47; /* placeholder for void */
    int64_t v98;
    uint8_t v30;
    int64_t v19; /* placeholder for void */
    int64_t v99; /* placeholder for void */
    int64_t v102; /* placeholder for void */
    int64_t v9;
    void* v103;
    int64_t v111; /* placeholder for void */
    uint8_t v49;
    bool v3;
    void* v69;
    void* v13;
    void* v60;
    int64_t v43;
    int64_t v64;
    int64_t v73;
    void* v78;
    int64_t v63; /* placeholder for void */
    int64_t v83;
    int64_t v87;
    int64_t v91;
    void* v31;
    int64_t v14;
    int64_t v35;
    void* v6;
    int64_t v2;
    int64_t v4; /* placeholder for void */
    int64_t v54;
    int64_t v24; /* placeholder for void */
    int64_t v42;
    int64_t v58; /* placeholder for void */
    void* v76;
    int64_t v37; /* placeholder for void */
    int64_t v44;
    int64_t v15; /* placeholder for void */
    int64_t v84;
    int64_t v38;
    int64_t v18; /* placeholder for void */
    int64_t v105; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    int64_t v5;
    int64_t v40;
    void* v48;
    int64_t v8; /* placeholder for void */
    void* v26;
    void* v65;
    int64_t v88;
    void* v80;
    int64_t v89;
    int64_t v100;
    int64_t v109; /* placeholder for void */
    int64_t v90; /* placeholder for void */
    int64_t v107;
    bool v74;
    int64_t v53;
    int64_t v96;
    void* v7;
    int64_t v46;
    int64_t v51; /* placeholder for void */
    int64_t v104;
    int64_t v61; /* placeholder for void */
    int64_t v101; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v95; /* placeholder for void */
    void* v110;
    int64_t v16; /* placeholder for void */
    int64_t v28; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    void* v29;
    void* v23;
    int64_t v41; /* placeholder for void */
    int64_t v22;
    void* v20;
    void* v57;
    void* v56;
    int64_t v55;
    int64_t v39;
    int64_t v71; /* placeholder for void */
    int64_t v11; /* placeholder for void */
    int64_t v75; /* placeholder for void */
    void* v10;
    int64_t v68;
    void* v17;
    uint8_t v77;
    int64_t v79; /* placeholder for void */
    void* v94;
    void* v93;
    void* v108;
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

void* str_split_char(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("str_split_char");
    int64_t v45;
    int64_t v46; /* placeholder for void */
    int64_t v11;
    void* v5;
    void* v4;
    int64_t v7;
    void* v30;
    int64_t v35; /* placeholder for void */
    bool v38;
    int64_t v57; /* placeholder for void */
    int64_t v21; /* placeholder for void */
    void* v24;
    int64_t v52; /* placeholder for void */
    int64_t v44;
    int64_t v17; /* placeholder for void */
    int64_t v47;
    int64_t v49;
    int64_t v63;
    void* v8;
    int64_t v13; /* placeholder for void */
    int64_t v26;
    int64_t v6; /* placeholder for void */
    int64_t v16; /* placeholder for void */
    int64_t v18;
    int64_t v3;
    int64_t v42;
    int64_t v29;
    int64_t v36; /* placeholder for void */
    int64_t v10;
    int64_t v55;
    int64_t v54;
    int64_t v41;
    int64_t v53;
    void* v15;
    void* v33;
    int64_t v43;
    uint8_t v34;
    bool v20;
    int64_t v59;
    int64_t v60;
    void* v58;
    int64_t v50; /* placeholder for void */
    void* v40;
    int64_t v31; /* placeholder for void */
    void* v12;
    int64_t v62;
    int64_t v66; /* placeholder for void */
    void* v27;
    int64_t v48;
    int64_t v61;
    int64_t v25; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    int64_t v37;
    int64_t v51; /* placeholder for void */
    int64_t v19;
    int64_t v32; /* placeholder for void */
    int64_t v23;
    int64_t v28; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v14;
    int64_t v22;
    void* v65;
    int64_t v64; /* placeholder for void */
    int64_t v9; /* placeholder for void */
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

bool token_kind_eq(void* v0, void* v1, void* v2) {
    p_call_stack_push("token_kind_eq");
    bool v7;
    int64_t v3;
    int64_t v6;
    int64_t v4;
    int64_t v5;
    int64_t v8; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)token_kind_tag((void*)v3, v1);
    v5 = 0LL;
    v6 = (int64_t)token_kind_tag((void*)v5, v2);
    v7 = v4 == v6;
    p_call_stack_pop();
    return v7;
    return 0;
}

void allocator_free(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("allocator_free");
    int64_t v4; /* placeholder for void */
    int64_t v3; /* placeholder for void */
    p_allocator_free(v1, v2);
    p_call_stack_pop();
    return;
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

void Lexer_read_operator(void* v0, void* v1, void* v2) {
    p_call_stack_push("Lexer_read_operator");
    int64_t v512;
    void* v513;
    void* v3;
    int64_t v445;
    int64_t v82; /* placeholder for void */
    bool v47;
    int64_t v297; /* placeholder for void */
    int64_t v41; /* placeholder for void */
    int64_t v567;
    int64_t v135;
    void* v503;
    int64_t v416;
    int64_t v70;
    int64_t v163; /* placeholder for void */
    int64_t v66; /* placeholder for void */
    int64_t v728; /* placeholder for void */
    int64_t v690; /* placeholder for void */
    void* v404;
    int64_t v407; /* placeholder for void */
    int64_t v495; /* placeholder for void */
    int64_t v564; /* placeholder for void */
    int64_t v76;
    int64_t v600;
    int64_t v683; /* placeholder for void */
    void* v603;
    void* v38;
    int64_t v520; /* placeholder for void */
    int64_t v214; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v322;
    int64_t v701; /* placeholder for void */
    int64_t v340;
    bool v631;
    void* v659;
    void* v574;
    bool v563;
    int64_t v688; /* placeholder for void */
    void* v653;
    int64_t v18; /* placeholder for void */
    int64_t v25;
    void* v250;
    int64_t v644;
    int64_t v130; /* placeholder for void */
    int64_t v193;
    int64_t v231; /* placeholder for void */
    int64_t v198;
    int64_t v720; /* placeholder for void */
    int64_t v128; /* placeholder for void */
    int64_t v528;
    int64_t v239;
    int64_t v78; /* placeholder for void */
    int64_t v287;
    int64_t v565;
    int64_t v227;
    int64_t v562;
    int64_t v590; /* placeholder for void */
    int64_t v616; /* placeholder for void */
    int64_t v695; /* placeholder for void */
    int64_t v573;
    int64_t v707;
    bool v392;
    void* v546;
    int64_t v194;
    void* v540;
    int64_t v592;
    void* v479;
    int64_t v99; /* placeholder for void */
    int64_t v723;
    int64_t v91; /* placeholder for void */
    int64_t v205;
    int64_t v354; /* placeholder for void */
    bool v379;
    int64_t v278;
    int64_t v451; /* placeholder for void */
    int64_t v609;
    int64_t v170; /* placeholder for void */
    bool v534;
    int64_t v365;
    bool v651;
    int64_t v527;
    int64_t v247;
    int64_t v654;
    int64_t v704; /* placeholder for void */
    int64_t v506; /* placeholder for void */
    void* v372;
    int64_t v98; /* placeholder for void */
    bool v621;
    int64_t v415;
    int64_t v469;
    void* v538;
    int64_t v257; /* placeholder for void */
    void* v424;
    int64_t v180;
    bool v296;
    int64_t v369;
    int64_t v67;
    int64_t v462; /* placeholder for void */
    int64_t v610;
    int64_t v395;
    int64_t v719;
    int64_t v393; /* placeholder for void */
    int64_t v608; /* placeholder for void */
    int64_t v259; /* placeholder for void */
    int64_t v364;
    bool v413;
    bool v157;
    int64_t v715;
    int64_t v518;
    int64_t v419; /* placeholder for void */
    int64_t v222;
    void* v256;
    void* v483;
    int64_t v221;
    void* v334;
    int64_t v491;
    int64_t v21;
    int64_t v698; /* placeholder for void */
    bool v341;
    int64_t v368;
    int64_t v409; /* placeholder for void */
    int64_t v192; /* placeholder for void */
    int64_t v467; /* placeholder for void */
    int64_t v702; /* placeholder for void */
    int64_t v8;
    int64_t v246;
    int64_t v344;
    int64_t v363;
    int64_t v376;
    bool v461;
    void* v666;
    bool v85;
    int64_t v188; /* placeholder for void */
    int64_t v274;
    int64_t v425; /* placeholder for void */
    int64_t v88;
    int64_t v583;
    void* v587;
    int64_t v536;
    int64_t v652; /* placeholder for void */
    int64_t v43; /* placeholder for void */
    int64_t v68;
    int64_t v433; /* placeholder for void */
    int64_t v360;
    int64_t v255;
    void* v345;
    void* v555;
    int64_t v560; /* placeholder for void */
    int64_t v681; /* placeholder for void */
    int64_t v692; /* placeholder for void */
    int64_t v301;
    int64_t v132;
    int64_t v414; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v145; /* placeholder for void */
    int64_t v710; /* placeholder for void */
    int64_t v80; /* placeholder for void */
    void* v102;
    int64_t v184; /* placeholder for void */
    int64_t v435;
    int64_t v458; /* placeholder for void */
    int64_t v649;
    int64_t v686; /* placeholder for void */
    void* v726;
    void* v711;
    void* v58;
    int64_t v173;
    int64_t v626; /* placeholder for void */
    int64_t v177;
    int64_t v201; /* placeholder for void */
    void* v351;
    void* v396;
    void* v204;
    int64_t v406; /* placeholder for void */
    bool v219;
    bool v499;
    int64_t v333;
    int64_t v662;
    void* v671;
    int64_t v269;
    int64_t v556;
    int64_t v140;
    void* v633;
    int64_t v665;
    int64_t v637; /* placeholder for void */
    void* v430;
    bool v23;
    int64_t v53; /* placeholder for void */
    int64_t v208; /* placeholder for void */
    int64_t v164;
    int64_t v298;
    int64_t v189; /* placeholder for void */
    int64_t v377;
    int64_t v243;
    int64_t v390;
    int64_t v599;
    int64_t v554; /* placeholder for void */
    int64_t v450; /* placeholder for void */
    int64_t v440;
    int64_t v275;
    int64_t v136;
    int64_t v410; /* placeholder for void */
    int64_t v203;
    int64_t v497;
    void* v557;
    bool v90;
    int64_t v92;
    int64_t v233; /* placeholder for void */
    int64_t v16;
    int64_t v107;
    int64_t v44; /* placeholder for void */
    void* v96;
    int64_t v294;
    int64_t v449; /* placeholder for void */
    void* v572;
    int64_t v602; /* placeholder for void */
    int64_t v594; /* placeholder for void */
    void* v674;
    void* v676;
    int64_t v525; /* placeholder for void */
    void* v655;
    int64_t v699; /* placeholder for void */
    void* v15;
    int64_t v367; /* placeholder for void */
    int64_t v190; /* placeholder for void */
    int64_t v559; /* placeholder for void */
    int64_t v685; /* placeholder for void */
    void* v454;
    int64_t v86; /* placeholder for void */
    int64_t v235;
    void* v19;
    int64_t v387; /* placeholder for void */
    int64_t v453;
    int64_t v465;
    int64_t v675;
    int64_t v286;
    int64_t v242;
    int64_t v639;
    int64_t v706;
    void* v485;
    int64_t v213; /* placeholder for void */
    int64_t v352;
    int64_t v689; /* placeholder for void */
    int64_t v24; /* placeholder for void */
    int64_t v264; /* placeholder for void */
    int64_t v330;
    bool v323;
    int64_t v281;
    int64_t v417;
    void* v714;
    int64_t v571;
    int64_t v318; /* placeholder for void */
    int64_t v211;
    int64_t v384;
    void* v13;
    void* v166;
    int64_t v408; /* placeholder for void */
    bool v418;
    void* v613;
    int64_t v169; /* placeholder for void */
    bool v641;
    int64_t v319; /* placeholder for void */
    int64_t v216; /* placeholder for void */
    int64_t v650;
    void* v721;
    int64_t v371;
    int64_t v443; /* placeholder for void */
    int64_t v558; /* placeholder for void */
    int64_t v320; /* placeholder for void */
    int64_t v54;
    int64_t v160;
    bool v466;
    void* v75;
    int64_t v324; /* placeholder for void */
    int64_t v79; /* placeholder for void */
    int64_t v359;
    int64_t v152; /* placeholder for void */
    int64_t v588; /* placeholder for void */
    int64_t v516; /* placeholder for void */
    int64_t v597; /* placeholder for void */
    int64_t v617; /* placeholder for void */
    void* v722;
    int64_t v220; /* placeholder for void */
    int64_t v106; /* placeholder for void */
    int64_t v22;
    int64_t v36; /* placeholder for void */
    void* v353;
    int64_t v143;
    int64_t v11;
    int64_t v151; /* placeholder for void */
    bool v175;
    int64_t v59; /* placeholder for void */
    bool v65;
    int64_t v423;
    int64_t v83;
    int64_t v27;
    void* v282;
    void* v5;
    int64_t v51;
    int64_t v312; /* placeholder for void */
    void* v168;
    int64_t v104; /* placeholder for void */
    int64_t v426; /* placeholder for void */
    int64_t v457; /* placeholder for void */
    int64_t v460;
    int64_t v42; /* placeholder for void */
    void* v306;
    int64_t v48; /* placeholder for void */
    int64_t v474; /* placeholder for void */
    void* v56;
    bool v568;
    int64_t v117;
    int64_t v582; /* placeholder for void */
    int64_t v155;
    void* v605;
    int64_t v405; /* placeholder for void */
    void* v446;
    int64_t v31;
    int64_t v388; /* placeholder for void */
    int64_t v262;
    void* v148;
    int64_t v33;
    int64_t v241;
    int64_t v551;
    int64_t v656; /* placeholder for void */
    void* v708;
    bool v509;
    int64_t v579;
    bool v224;
    void* v585;
    int64_t v709;
    void* v94;
    int64_t v283; /* placeholder for void */
    int64_t v226;
    int64_t v421;
    int64_t v577; /* placeholder for void */
    int64_t v570;
    int64_t v153; /* placeholder for void */
    bool v611;
    bool v52;
    int64_t v127; /* placeholder for void */
    int64_t v272; /* placeholder for void */
    void* v254;
    int64_t v378;
    int64_t v93;
    void* v302;
    int64_t v463;
    int64_t v679; /* placeholder for void */
    int64_t v394;
    int64_t v677; /* placeholder for void */
    int64_t v30;
    int64_t v307;
    void* v300;
    int64_t v268; /* placeholder for void */
    int64_t v356; /* placeholder for void */
    int64_t v290; /* placeholder for void */
    int64_t v61; /* placeholder for void */
    int64_t v217;
    int64_t v427; /* placeholder for void */
    int64_t v134; /* placeholder for void */
    int64_t v348; /* placeholder for void */
    void* v591;
    int64_t v614;
    int64_t v284; /* placeholder for void */
    int64_t v45;
    int64_t v337; /* placeholder for void */
    int64_t v304; /* placeholder for void */
    int64_t v209; /* placeholder for void */
    int64_t v670;
    void* v523;
    int64_t v584;
    int64_t v397;
    void* v248;
    int64_t v103; /* placeholder for void */
    void* v100;
    int64_t v225; /* placeholder for void */
    int64_t v468;
    int64_t v386; /* placeholder for void */
    bool v519;
    int64_t v569; /* placeholder for void */
    int64_t v444;
    bool v529;
    int64_t v475;
    int64_t v647; /* placeholder for void */
    void* v615;
    int64_t v700; /* placeholder for void */
    void* v428;
    int64_t v113;
    void* v150;
    int64_t v434; /* placeholder for void */
    int64_t v533;
    int64_t v484;
    int64_t v299;
    int64_t v642; /* placeholder for void */
    void* v521;
    int64_t v158; /* placeholder for void */
    bool v114;
    int64_t v197;
    int64_t v159;
    int64_t v101;
    int64_t v131;
    int64_t v154; /* placeholder for void */
    void* v477;
    int64_t v176; /* placeholder for void */
    int64_t v530; /* placeholder for void */
    int64_t v729; /* placeholder for void */
    int64_t v182; /* placeholder for void */
    int64_t v74; /* placeholder for void */
    int64_t v291;
    void* v332;
    int64_t v343;
    int64_t v273;
    int64_t v346;
    int64_t v411;
    bool v244;
    void* v212;
    int64_t v266; /* placeholder for void */
    int64_t v505; /* placeholder for void */
    bool v289;
    int64_t v149;
    bool v442;
    int64_t v311; /* placeholder for void */
    int64_t v89;
    int64_t v46;
    bool v138;
    int64_t v285; /* placeholder for void */
    int64_t v622; /* placeholder for void */
    int64_t v441;
    void* v385;
    int64_t v240;
    int64_t v596; /* placeholder for void */
    bool v473;
    int64_t v724; /* placeholder for void */
    int64_t v108;
    int64_t v471;
    int64_t v576; /* placeholder for void */
    int64_t v267; /* placeholder for void */
    int64_t v292;
    int64_t v73; /* placeholder for void */
    int64_t v277; /* placeholder for void */
    int64_t v236;
    int64_t v476;
    int64_t v486; /* placeholder for void */
    int64_t v531;
    int64_t v632; /* placeholder for void */
    int64_t v439;
    int64_t v172;
    int64_t v498;
    int64_t v487; /* placeholder for void */
    int64_t v165;
    int64_t v400; /* placeholder for void */
    int64_t v62;
    void* v230;
    int64_t v161;
    int64_t v432; /* placeholder for void */
    int64_t v4;
    int64_t v72; /* placeholder for void */
    void* v118;
    int64_t v493; /* placeholder for void */
    bool v581;
    int64_t v640;
    int64_t v258; /* placeholder for void */
    int64_t v245; /* placeholder for void */
    int64_t v116;
    int64_t v336; /* placeholder for void */
    int64_t v687; /* placeholder for void */
    int64_t v535; /* placeholder for void */
    int64_t v362; /* placeholder for void */
    int64_t v339;
    int64_t v547; /* placeholder for void */
    void* v725;
    void* v667;
    void* v422;
    void* v625;
    void* v717;
    int64_t v489; /* placeholder for void */
    void* v313;
    void* v705;
    int64_t v338;
    int64_t v350; /* placeholder for void */
    int64_t v420;
    int64_t v566;
    bool v200;
    int64_t v586;
    int64_t v316; /* placeholder for void */
    int64_t v399; /* placeholder for void */
    int64_t v619;
    int64_t v464;
    int64_t v507;
    int64_t v171; /* placeholder for void */
    int64_t v279;
    int64_t v329; /* placeholder for void */
    void* v398;
    int64_t v293;
    void* v77;
    void* v490;
    int64_t v326;
    int64_t v604;
    int64_t v550; /* placeholder for void */
    int64_t v496; /* placeholder for void */
    int64_t v522;
    int64_t v456; /* placeholder for void */
    bool v553;
    int64_t v223;
    int64_t v459;
    void* v643;
    void* v228;
    void* v718;
    int64_t v147; /* placeholder for void */
    int64_t v342; /* placeholder for void */
    int64_t v156;
    int64_t v305; /* placeholder for void */
    int64_t v199;
    int64_t v335; /* placeholder for void */
    void* v142;
    int64_t v548; /* placeholder for void */
    int64_t v310; /* placeholder for void */
    int64_t v37; /* placeholder for void */
    void* v124;
    void* v263;
    void* v179;
    int64_t v412;
    int64_t v12;
    bool v109;
    int64_t v327;
    int64_t v358; /* placeholder for void */
    void* v452;
    void* v623;
    bool v195;
    int64_t v355; /* placeholder for void */
    int64_t v251; /* placeholder for void */
    int64_t v260; /* placeholder for void */
    int64_t v207; /* placeholder for void */
    int64_t v595; /* placeholder for void */
    int64_t v658; /* placeholder for void */
    int64_t v391;
    int64_t v57;
    int64_t v29; /* placeholder for void */
    void* v120;
    int64_t v481; /* placeholder for void */
    int64_t v502;
    int64_t v664;
    void* v668;
    int64_t v691; /* placeholder for void */
    void* v347;
    int64_t v694; /* placeholder for void */
    int64_t v196; /* placeholder for void */
    int64_t v634;
    void* v17;
    int64_t v234;
    int64_t v373; /* placeholder for void */
    int64_t v508;
    int64_t v646; /* placeholder for void */
    void* v663;
    int64_t v580;
    int64_t v575; /* placeholder for void */
    bool v366;
    int64_t v26;
    int64_t v624;
    int64_t v60; /* placeholder for void */
    void* v383;
    int64_t v317; /* placeholder for void */
    int64_t v380; /* placeholder for void */
    int64_t v515; /* placeholder for void */
    int64_t v455; /* placeholder for void */
    int64_t v295;
    int64_t v314;
    void* v448;
    bool v328;
    int64_t v697; /* placeholder for void */
    int64_t v627; /* placeholder for void */
    int64_t v252; /* placeholder for void */
    int64_t v39;
    void* v181;
    void* v187;
    int64_t v357; /* placeholder for void */
    void* v32;
    int64_t v589; /* placeholder for void */
    void* v645;
    int64_t v87;
    int64_t v137;
    void* v185;
    int64_t v526; /* placeholder for void */
    int64_t v115; /* placeholder for void */
    int64_t v696; /* placeholder for void */
    void* v7;
    bool v271;
    int64_t v10; /* placeholder for void */
    int64_t v552;
    int64_t v612; /* placeholder for void */
    int64_t v598; /* placeholder for void */
    int64_t v657; /* placeholder for void */
    int64_t v543; /* placeholder for void */
    bool v162;
    int64_t v186;
    void* v669;
    void* v69;
    int64_t v606; /* placeholder for void */
    int64_t v97; /* placeholder for void */
    int64_t v303; /* placeholder for void */
    int64_t v349; /* placeholder for void */
    int64_t v618; /* placeholder for void */
    int64_t v122; /* placeholder for void */
    int64_t v174;
    int64_t v620;
    int64_t v470;
    void* v315;
    bool v601;
    int64_t v63;
    int64_t v84;
    int64_t v539;
    void* v126;
    bool v237;
    int64_t v628; /* placeholder for void */
    int64_t v309; /* placeholder for void */
    int64_t v638; /* placeholder for void */
    int64_t v510; /* placeholder for void */
    int64_t v431; /* placeholder for void */
    int64_t v238; /* placeholder for void */
    int64_t v630;
    bool v28;
    void* v544;
    int64_t v202;
    int64_t v215; /* placeholder for void */
    int64_t v125;
    int64_t v578;
    int64_t v648; /* placeholder for void */
    int64_t v549; /* placeholder for void */
    int64_t v478;
    int64_t v146; /* placeholder for void */
    int64_t v716; /* placeholder for void */
    int64_t v447;
    int64_t v229;
    void* v40;
    int64_t v20; /* placeholder for void */
    int64_t v6; /* placeholder for void */
    int64_t v123; /* placeholder for void */
    void* v144;
    int64_t v265; /* placeholder for void */
    int64_t v191; /* placeholder for void */
    int64_t v288;
    int64_t v375; /* placeholder for void */
    int64_t v249;
    int64_t v532;
    int64_t v218;
    int64_t v81; /* placeholder for void */
    void* v210;
    int64_t v438; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v661;
    int64_t v374; /* placeholder for void */
    int64_t v139; /* placeholder for void */
    int64_t v693; /* placeholder for void */
    int64_t v95;
    void* v660;
    int64_t v480; /* placeholder for void */
    int64_t v494; /* placeholder for void */
    int64_t v561;
    int64_t v253; /* placeholder for void */
    int64_t v482; /* placeholder for void */
    int64_t v524; /* placeholder for void */
    void* v71;
    int64_t v401; /* placeholder for void */
    int64_t v325;
    int64_t v321;
    int64_t v64;
    int64_t v167;
    void* v370;
    void* v511;
    int64_t v436;
    int64_t v537;
    int64_t v678; /* placeholder for void */
    int64_t v55;
    int64_t v680; /* placeholder for void */
    int64_t v389;
    void* v712;
    void* v308;
    int64_t v472;
    int64_t v517;
    int64_t v672;
    int64_t v111;
    void* v9;
    int64_t v141;
    int64_t v119;
    bool v276;
    int64_t v183; /* placeholder for void */
    int64_t v514; /* placeholder for void */
    void* v713;
    int64_t v105; /* placeholder for void */
    int64_t v112;
    void* v593;
    void* v635;
    void* v206;
    int64_t v50;
    int64_t v110; /* placeholder for void */
    void* v261;
    int64_t v682; /* placeholder for void */
    int64_t v232; /* placeholder for void */
    int64_t v403;
    void* v492;
    int64_t v500; /* placeholder for void */
    int64_t v381;
    void* v402;
    int64_t v703; /* placeholder for void */
    int64_t v49;
    int64_t v121; /* placeholder for void */
    int64_t v545;
    int64_t v382;
    int64_t v270;
    bool v361;
    int64_t v504; /* placeholder for void */
    void* v34;
    int64_t v488; /* placeholder for void */
    int64_t v542; /* placeholder for void */
    int64_t v629;
    int64_t v331;
    int64_t v541; /* placeholder for void */
    int64_t v178;
    int64_t v607; /* placeholder for void */
    int64_t v727;
    int64_t v636; /* placeholder for void */
    void* v501;
    int64_t v673; /* placeholder for void */
    int64_t v429;
    bool v133;
    int64_t v684; /* placeholder for void */
    void* v280;
    bool v437;
    v3 = (char*)v2 + 16;
    v4 = *(int64_t*)v3;
    v5 = malloc(8);
    *(int64_t*)v5 = (int64_t)v4;
    v7 = (char*)v2 + 24;
    v8 = *(int64_t*)v7;
    v9 = malloc(8);
    *(int64_t*)v9 = (int64_t)v8;
    v11 = 0LL;
    v12 = (int64_t)Lexer_advance((void*)v11, v2);
    v13 = malloc(8);
    *(int64_t*)v13 = (int64_t)v12;
    v15 = malloc(16);
    v16 = 134LL;
    v17 = (char*)v15 + 0;
    *(int64_t*)v17 = (int64_t)v16;
    v19 = malloc(8);
    *(void**)v19 = (void*)v15;
    v21 = *(int64_t*)v13;
    v22 = 43LL;
    v23 = v21 == v22;
    if (v23) goto bb1;
    else goto bb2;
  bb1:
    v25 = 0LL;
    v26 = (int64_t)Lexer_peek((void*)v25, v2);
    v27 = 61LL;
    v28 = v26 == v27;
    if (v28) goto bb4;
    else goto bb5;
  bb2:
    v45 = *(int64_t*)v13;
    v46 = 45LL;
    v47 = v45 == v46;
    if (v47) goto bb7;
    else goto bb8;
  bb3:
    v705 = *(void**)v19;
    v706 = *(int64_t*)v5;
    v707 = *(int64_t*)v9;
    v708 = malloc(32);
    v709 = 0LL;
    token_new(v708, (void*)v709, v705, v706, v707);
    v711 = (char*)v708 + 0;
    v712 = (char*)v0 + 0;
    v713 = (char*)v711 + 0;
    v714 = (char*)v712 + 0;
    v715 = *(int64_t*)v713;
    *(int64_t*)v714 = (int64_t)v715;
    v717 = (char*)v711 + 8;
    v718 = (char*)v712 + 8;
    v719 = *(int64_t*)v717;
    *(int64_t*)v718 = (int64_t)v719;
    v721 = (char*)v708 + 16;
    v722 = (char*)v0 + 16;
    v723 = *(int64_t*)v721;
    *(int64_t*)v722 = (int64_t)v723;
    v725 = (char*)v708 + 24;
    v726 = (char*)v0 + 24;
    v727 = *(int64_t*)v725;
    *(int64_t*)v726 = (int64_t)v727;
    p_call_stack_pop();
    return;
  bb4:
    v30 = 0LL;
    v31 = (int64_t)Lexer_advance((void*)v30, v2);
    v32 = malloc(16);
    v33 = 106LL;
    v34 = (char*)v32 + 0;
    *(int64_t*)v34 = (int64_t)v33;
    *(void**)v19 = (void*)v32;
    goto bb6;
  bb5:
    v38 = malloc(16);
    v39 = 81LL;
    v40 = (char*)v38 + 0;
    *(int64_t*)v40 = (int64_t)v39;
    *(void**)v19 = (void*)v38;
    goto bb6;
  bb6:
    goto bb3;
  bb7:
    v49 = 0LL;
    v50 = (int64_t)Lexer_peek((void*)v49, v2);
    v51 = 62LL;
    v52 = v50 == v51;
    if (v52) goto bb10;
    else goto bb11;
  bb8:
    v83 = *(int64_t*)v13;
    v84 = 42LL;
    v85 = v83 == v84;
    if (v85) goto bb16;
    else goto bb17;
  bb9:
    goto bb3;
  bb10:
    v54 = 0LL;
    v55 = (int64_t)Lexer_advance((void*)v54, v2);
    v56 = malloc(16);
    v57 = 116LL;
    v58 = (char*)v56 + 0;
    *(int64_t*)v58 = (int64_t)v57;
    *(void**)v19 = (void*)v56;
    goto bb12;
  bb11:
    v62 = 0LL;
    v63 = (int64_t)Lexer_peek((void*)v62, v2);
    v64 = 61LL;
    v65 = v63 == v64;
    if (v65) goto bb13;
    else goto bb14;
  bb12:
    goto bb9;
  bb13:
    v67 = 0LL;
    v68 = (int64_t)Lexer_advance((void*)v67, v2);
    v69 = malloc(16);
    v70 = 107LL;
    v71 = (char*)v69 + 0;
    *(int64_t*)v71 = (int64_t)v70;
    *(void**)v19 = (void*)v69;
    goto bb15;
  bb14:
    v75 = malloc(16);
    v76 = 82LL;
    v77 = (char*)v75 + 0;
    *(int64_t*)v77 = (int64_t)v76;
    *(void**)v19 = (void*)v75;
    goto bb15;
  bb15:
    goto bb12;
  bb16:
    v87 = 0LL;
    v88 = (int64_t)Lexer_peek((void*)v87, v2);
    v89 = 61LL;
    v90 = v88 == v89;
    if (v90) goto bb19;
    else goto bb20;
  bb17:
    v107 = *(int64_t*)v13;
    v108 = 47LL;
    v109 = v107 == v108;
    if (v109) goto bb22;
    else goto bb23;
  bb18:
    goto bb9;
  bb19:
    v92 = 0LL;
    v93 = (int64_t)Lexer_advance((void*)v92, v2);
    v94 = malloc(16);
    v95 = 108LL;
    v96 = (char*)v94 + 0;
    *(int64_t*)v96 = (int64_t)v95;
    *(void**)v19 = (void*)v94;
    goto bb21;
  bb20:
    v100 = malloc(16);
    v101 = 83LL;
    v102 = (char*)v100 + 0;
    *(int64_t*)v102 = (int64_t)v101;
    *(void**)v19 = (void*)v100;
    goto bb21;
  bb21:
    goto bb18;
  bb22:
    v111 = 0LL;
    v112 = (int64_t)Lexer_peek((void*)v111, v2);
    v113 = 61LL;
    v114 = v112 == v113;
    if (v114) goto bb25;
    else goto bb26;
  bb23:
    v131 = *(int64_t*)v13;
    v132 = 37LL;
    v133 = v131 == v132;
    if (v133) goto bb28;
    else goto bb29;
  bb24:
    goto bb18;
  bb25:
    v116 = 0LL;
    v117 = (int64_t)Lexer_advance((void*)v116, v2);
    v118 = malloc(16);
    v119 = 109LL;
    v120 = (char*)v118 + 0;
    *(int64_t*)v120 = (int64_t)v119;
    *(void**)v19 = (void*)v118;
    goto bb27;
  bb26:
    v124 = malloc(16);
    v125 = 84LL;
    v126 = (char*)v124 + 0;
    *(int64_t*)v126 = (int64_t)v125;
    *(void**)v19 = (void*)v124;
    goto bb27;
  bb27:
    goto bb24;
  bb28:
    v135 = 0LL;
    v136 = (int64_t)Lexer_peek((void*)v135, v2);
    v137 = 61LL;
    v138 = v136 == v137;
    if (v138) goto bb31;
    else goto bb32;
  bb29:
    v155 = *(int64_t*)v13;
    v156 = 61LL;
    v157 = v155 == v156;
    if (v157) goto bb34;
    else goto bb35;
  bb30:
    goto bb24;
  bb31:
    v140 = 0LL;
    v141 = (int64_t)Lexer_advance((void*)v140, v2);
    v142 = malloc(16);
    v143 = 110LL;
    v144 = (char*)v142 + 0;
    *(int64_t*)v144 = (int64_t)v143;
    *(void**)v19 = (void*)v142;
    goto bb33;
  bb32:
    v148 = malloc(16);
    v149 = 85LL;
    v150 = (char*)v148 + 0;
    *(int64_t*)v150 = (int64_t)v149;
    *(void**)v19 = (void*)v148;
    goto bb33;
  bb33:
    goto bb30;
  bb34:
    v159 = 0LL;
    v160 = (int64_t)Lexer_peek((void*)v159, v2);
    v161 = 61LL;
    v162 = v160 == v161;
    if (v162) goto bb37;
    else goto bb38;
  bb35:
    v193 = *(int64_t*)v13;
    v194 = 33LL;
    v195 = v193 == v194;
    if (v195) goto bb43;
    else goto bb44;
  bb36:
    goto bb30;
  bb37:
    v164 = 0LL;
    v165 = (int64_t)Lexer_advance((void*)v164, v2);
    v166 = malloc(16);
    v167 = 87LL;
    v168 = (char*)v166 + 0;
    *(int64_t*)v168 = (int64_t)v167;
    *(void**)v19 = (void*)v166;
    goto bb39;
  bb38:
    v172 = 0LL;
    v173 = (int64_t)Lexer_peek((void*)v172, v2);
    v174 = 62LL;
    v175 = v173 == v174;
    if (v175) goto bb40;
    else goto bb41;
  bb39:
    goto bb36;
  bb40:
    v177 = 0LL;
    v178 = (int64_t)Lexer_advance((void*)v177, v2);
    v179 = malloc(16);
    v180 = 117LL;
    v181 = (char*)v179 + 0;
    *(int64_t*)v181 = (int64_t)v180;
    *(void**)v19 = (void*)v179;
    goto bb42;
  bb41:
    v185 = malloc(16);
    v186 = 86LL;
    v187 = (char*)v185 + 0;
    *(int64_t*)v187 = (int64_t)v186;
    *(void**)v19 = (void*)v185;
    goto bb42;
  bb42:
    goto bb39;
  bb43:
    v197 = 0LL;
    v198 = (int64_t)Lexer_peek((void*)v197, v2);
    v199 = 61LL;
    v200 = v198 == v199;
    if (v200) goto bb46;
    else goto bb47;
  bb44:
    v217 = *(int64_t*)v13;
    v218 = 60LL;
    v219 = v217 == v218;
    if (v219) goto bb49;
    else goto bb50;
  bb45:
    goto bb36;
  bb46:
    v202 = 0LL;
    v203 = (int64_t)Lexer_advance((void*)v202, v2);
    v204 = malloc(16);
    v205 = 88LL;
    v206 = (char*)v204 + 0;
    *(int64_t*)v206 = (int64_t)v205;
    *(void**)v19 = (void*)v204;
    goto bb48;
  bb47:
    v210 = malloc(16);
    v211 = 93LL;
    v212 = (char*)v210 + 0;
    *(int64_t*)v212 = (int64_t)v211;
    *(void**)v19 = (void*)v210;
    goto bb48;
  bb48:
    goto bb45;
  bb49:
    v221 = 0LL;
    v222 = (int64_t)Lexer_peek((void*)v221, v2);
    v223 = 61LL;
    v224 = v222 == v223;
    if (v224) goto bb52;
    else goto bb53;
  bb50:
    v269 = *(int64_t*)v13;
    v270 = 62LL;
    v271 = v269 == v270;
    if (v271) goto bb61;
    else goto bb62;
  bb51:
    goto bb45;
  bb52:
    v226 = 0LL;
    v227 = (int64_t)Lexer_advance((void*)v226, v2);
    v228 = malloc(16);
    v229 = 91LL;
    v230 = (char*)v228 + 0;
    *(int64_t*)v230 = (int64_t)v229;
    *(void**)v19 = (void*)v228;
    goto bb54;
  bb53:
    v234 = 0LL;
    v235 = (int64_t)Lexer_peek((void*)v234, v2);
    v236 = 60LL;
    v237 = v235 == v236;
    if (v237) goto bb55;
    else goto bb56;
  bb54:
    goto bb51;
  bb55:
    v239 = 0LL;
    v240 = (int64_t)Lexer_advance((void*)v239, v2);
    v241 = 0LL;
    v242 = (int64_t)Lexer_peek((void*)v241, v2);
    v243 = 61LL;
    v244 = v242 == v243;
    if (v244) goto bb58;
    else goto bb59;
  bb56:
    v261 = malloc(16);
    v262 = 89LL;
    v263 = (char*)v261 + 0;
    *(int64_t*)v263 = (int64_t)v262;
    *(void**)v19 = (void*)v261;
    goto bb57;
  bb57:
    goto bb54;
  bb58:
    v246 = 0LL;
    v247 = (int64_t)Lexer_advance((void*)v246, v2);
    v248 = malloc(16);
    v249 = 114LL;
    v250 = (char*)v248 + 0;
    *(int64_t*)v250 = (int64_t)v249;
    *(void**)v19 = (void*)v248;
    goto bb60;
  bb59:
    v254 = malloc(16);
    v255 = 101LL;
    v256 = (char*)v254 + 0;
    *(int64_t*)v256 = (int64_t)v255;
    *(void**)v19 = (void*)v254;
    goto bb60;
  bb60:
    goto bb57;
  bb61:
    v273 = 0LL;
    v274 = (int64_t)Lexer_peek((void*)v273, v2);
    v275 = 61LL;
    v276 = v274 == v275;
    if (v276) goto bb64;
    else goto bb65;
  bb62:
    v321 = *(int64_t*)v13;
    v322 = 38LL;
    v323 = v321 == v322;
    if (v323) goto bb73;
    else goto bb74;
  bb63:
    goto bb51;
  bb64:
    v278 = 0LL;
    v279 = (int64_t)Lexer_advance((void*)v278, v2);
    v280 = malloc(16);
    v281 = 92LL;
    v282 = (char*)v280 + 0;
    *(int64_t*)v282 = (int64_t)v281;
    *(void**)v19 = (void*)v280;
    goto bb66;
  bb65:
    v286 = 0LL;
    v287 = (int64_t)Lexer_peek((void*)v286, v2);
    v288 = 62LL;
    v289 = v287 == v288;
    if (v289) goto bb67;
    else goto bb68;
  bb66:
    goto bb63;
  bb67:
    v291 = 0LL;
    v292 = (int64_t)Lexer_advance((void*)v291, v2);
    v293 = 0LL;
    v294 = (int64_t)Lexer_peek((void*)v293, v2);
    v295 = 61LL;
    v296 = v294 == v295;
    if (v296) goto bb70;
    else goto bb71;
  bb68:
    v313 = malloc(16);
    v314 = 90LL;
    v315 = (char*)v313 + 0;
    *(int64_t*)v315 = (int64_t)v314;
    *(void**)v19 = (void*)v313;
    goto bb69;
  bb69:
    goto bb66;
  bb70:
    v298 = 0LL;
    v299 = (int64_t)Lexer_advance((void*)v298, v2);
    v300 = malloc(16);
    v301 = 115LL;
    v302 = (char*)v300 + 0;
    *(int64_t*)v302 = (int64_t)v301;
    *(void**)v19 = (void*)v300;
    goto bb72;
  bb71:
    v306 = malloc(16);
    v307 = 102LL;
    v308 = (char*)v306 + 0;
    *(int64_t*)v308 = (int64_t)v307;
    *(void**)v19 = (void*)v306;
    goto bb72;
  bb72:
    goto bb69;
  bb73:
    v325 = 0LL;
    v326 = (int64_t)Lexer_peek((void*)v325, v2);
    v327 = 38LL;
    v328 = v326 == v327;
    if (v328) goto bb76;
    else goto bb77;
  bb74:
    v359 = *(int64_t*)v13;
    v360 = 124LL;
    v361 = v359 == v360;
    if (v361) goto bb82;
    else goto bb83;
  bb75:
    goto bb63;
  bb76:
    v330 = 0LL;
    v331 = (int64_t)Lexer_advance((void*)v330, v2);
    v332 = malloc(16);
    v333 = 94LL;
    v334 = (char*)v332 + 0;
    *(int64_t*)v334 = (int64_t)v333;
    *(void**)v19 = (void*)v332;
    goto bb78;
  bb77:
    v338 = 0LL;
    v339 = (int64_t)Lexer_peek((void*)v338, v2);
    v340 = 61LL;
    v341 = v339 == v340;
    if (v341) goto bb79;
    else goto bb80;
  bb78:
    goto bb75;
  bb79:
    v343 = 0LL;
    v344 = (int64_t)Lexer_advance((void*)v343, v2);
    v345 = malloc(16);
    v346 = 111LL;
    v347 = (char*)v345 + 0;
    *(int64_t*)v347 = (int64_t)v346;
    *(void**)v19 = (void*)v345;
    goto bb81;
  bb80:
    v351 = malloc(16);
    v352 = 95LL;
    v353 = (char*)v351 + 0;
    *(int64_t*)v353 = (int64_t)v352;
    *(void**)v19 = (void*)v351;
    goto bb81;
  bb81:
    goto bb78;
  bb82:
    v363 = 0LL;
    v364 = (int64_t)Lexer_peek((void*)v363, v2);
    v365 = 124LL;
    v366 = v364 == v365;
    if (v366) goto bb85;
    else goto bb86;
  bb83:
    v411 = *(int64_t*)v13;
    v412 = 94LL;
    v413 = v411 == v412;
    if (v413) goto bb94;
    else goto bb95;
  bb84:
    goto bb75;
  bb85:
    v368 = 0LL;
    v369 = (int64_t)Lexer_advance((void*)v368, v2);
    v370 = malloc(16);
    v371 = 96LL;
    v372 = (char*)v370 + 0;
    *(int64_t*)v372 = (int64_t)v371;
    *(void**)v19 = (void*)v370;
    goto bb87;
  bb86:
    v376 = 0LL;
    v377 = (int64_t)Lexer_peek((void*)v376, v2);
    v378 = 62LL;
    v379 = v377 == v378;
    if (v379) goto bb88;
    else goto bb89;
  bb87:
    goto bb84;
  bb88:
    v381 = 0LL;
    v382 = (int64_t)Lexer_advance((void*)v381, v2);
    v383 = malloc(16);
    v384 = 98LL;
    v385 = (char*)v383 + 0;
    *(int64_t*)v385 = (int64_t)v384;
    *(void**)v19 = (void*)v383;
    goto bb90;
  bb89:
    v389 = 0LL;
    v390 = (int64_t)Lexer_peek((void*)v389, v2);
    v391 = 61LL;
    v392 = v390 == v391;
    if (v392) goto bb91;
    else goto bb92;
  bb90:
    goto bb87;
  bb91:
    v394 = 0LL;
    v395 = (int64_t)Lexer_advance((void*)v394, v2);
    v396 = malloc(16);
    v397 = 112LL;
    v398 = (char*)v396 + 0;
    *(int64_t*)v398 = (int64_t)v397;
    *(void**)v19 = (void*)v396;
    goto bb93;
  bb92:
    v402 = malloc(16);
    v403 = 97LL;
    v404 = (char*)v402 + 0;
    *(int64_t*)v404 = (int64_t)v403;
    *(void**)v19 = (void*)v402;
    goto bb93;
  bb93:
    goto bb90;
  bb94:
    v415 = 0LL;
    v416 = (int64_t)Lexer_peek((void*)v415, v2);
    v417 = 61LL;
    v418 = v416 == v417;
    if (v418) goto bb97;
    else goto bb98;
  bb95:
    v435 = *(int64_t*)v13;
    v436 = 126LL;
    v437 = v435 == v436;
    if (v437) goto bb100;
    else goto bb101;
  bb96:
    goto bb84;
  bb97:
    v420 = 0LL;
    v421 = (int64_t)Lexer_advance((void*)v420, v2);
    v422 = malloc(16);
    v423 = 113LL;
    v424 = (char*)v422 + 0;
    *(int64_t*)v424 = (int64_t)v423;
    *(void**)v19 = (void*)v422;
    goto bb99;
  bb98:
    v428 = malloc(16);
    v429 = 99LL;
    v430 = (char*)v428 + 0;
    *(int64_t*)v430 = (int64_t)v429;
    *(void**)v19 = (void*)v428;
    goto bb99;
  bb99:
    goto bb96;
  bb100:
    v439 = 0LL;
    v440 = (int64_t)Lexer_peek((void*)v439, v2);
    v441 = 62LL;
    v442 = v440 == v441;
    if (v442) goto bb103;
    else goto bb104;
  bb101:
    v459 = *(int64_t*)v13;
    v460 = 46LL;
    v461 = v459 == v460;
    if (v461) goto bb106;
    else goto bb107;
  bb102:
    goto bb96;
  bb103:
    v444 = 0LL;
    v445 = (int64_t)Lexer_advance((void*)v444, v2);
    v446 = malloc(16);
    v447 = 105LL;
    v448 = (char*)v446 + 0;
    *(int64_t*)v448 = (int64_t)v447;
    *(void**)v19 = (void*)v446;
    goto bb105;
  bb104:
    v452 = malloc(16);
    v453 = 100LL;
    v454 = (char*)v452 + 0;
    *(int64_t*)v454 = (int64_t)v453;
    *(void**)v19 = (void*)v452;
    goto bb105;
  bb105:
    goto bb102;
  bb106:
    v463 = 0LL;
    v464 = (int64_t)Lexer_peek((void*)v463, v2);
    v465 = 46LL;
    v466 = v464 == v465;
    if (v466) goto bb109;
    else goto bb110;
  bb107:
    v497 = *(int64_t*)v13;
    v498 = 35LL;
    v499 = v497 == v498;
    if (v499) goto bb115;
    else goto bb116;
  bb108:
    goto bb102;
  bb109:
    v468 = 0LL;
    v469 = (int64_t)Lexer_advance((void*)v468, v2);
    v470 = 0LL;
    v471 = (int64_t)Lexer_peek((void*)v470, v2);
    v472 = 61LL;
    v473 = v471 == v472;
    if (v473) goto bb112;
    else goto bb113;
  bb110:
    v490 = malloc(16);
    v491 = 118LL;
    v492 = (char*)v490 + 0;
    *(int64_t*)v492 = (int64_t)v491;
    *(void**)v19 = (void*)v490;
    goto bb111;
  bb111:
    goto bb108;
  bb112:
    v475 = 0LL;
    v476 = (int64_t)Lexer_advance((void*)v475, v2);
    v477 = malloc(16);
    v478 = 104LL;
    v479 = (char*)v477 + 0;
    *(int64_t*)v479 = (int64_t)v478;
    *(void**)v19 = (void*)v477;
    goto bb114;
  bb113:
    v483 = malloc(16);
    v484 = 103LL;
    v485 = (char*)v483 + 0;
    *(int64_t*)v485 = (int64_t)v484;
    *(void**)v19 = (void*)v483;
    goto bb114;
  bb114:
    goto bb111;
  bb115:
    v501 = malloc(16);
    v502 = 126LL;
    v503 = (char*)v501 + 0;
    *(int64_t*)v503 = (int64_t)v502;
    *(void**)v19 = (void*)v501;
    goto bb117;
  bb116:
    v507 = *(int64_t*)v13;
    v508 = 64LL;
    v509 = v507 == v508;
    if (v509) goto bb118;
    else goto bb119;
  bb117:
    goto bb108;
  bb118:
    v511 = malloc(16);
    v512 = 127LL;
    v513 = (char*)v511 + 0;
    *(int64_t*)v513 = (int64_t)v512;
    *(void**)v19 = (void*)v511;
    goto bb120;
  bb119:
    v517 = *(int64_t*)v13;
    v518 = 44LL;
    v519 = v517 == v518;
    if (v519) goto bb121;
    else goto bb122;
  bb120:
    goto bb117;
  bb121:
    v521 = malloc(16);
    v522 = 119LL;
    v523 = (char*)v521 + 0;
    *(int64_t*)v523 = (int64_t)v522;
    *(void**)v19 = (void*)v521;
    goto bb123;
  bb122:
    v527 = *(int64_t*)v13;
    v528 = 58LL;
    v529 = v527 == v528;
    if (v529) goto bb124;
    else goto bb125;
  bb123:
    goto bb120;
  bb124:
    v531 = 0LL;
    v532 = (int64_t)Lexer_peek((void*)v531, v2);
    v533 = 58LL;
    v534 = v532 == v533;
    if (v534) goto bb127;
    else goto bb128;
  bb125:
    v551 = *(int64_t*)v13;
    v552 = 59LL;
    v553 = v551 == v552;
    if (v553) goto bb130;
    else goto bb131;
  bb126:
    goto bb123;
  bb127:
    v536 = 0LL;
    v537 = (int64_t)Lexer_advance((void*)v536, v2);
    v538 = malloc(16);
    v539 = 122LL;
    v540 = (char*)v538 + 0;
    *(int64_t*)v540 = (int64_t)v539;
    *(void**)v19 = (void*)v538;
    goto bb129;
  bb128:
    v544 = malloc(16);
    v545 = 120LL;
    v546 = (char*)v544 + 0;
    *(int64_t*)v546 = (int64_t)v545;
    *(void**)v19 = (void*)v544;
    goto bb129;
  bb129:
    goto bb126;
  bb130:
    v555 = malloc(16);
    v556 = 121LL;
    v557 = (char*)v555 + 0;
    *(int64_t*)v557 = (int64_t)v556;
    *(void**)v19 = (void*)v555;
    goto bb132;
  bb131:
    v561 = *(int64_t*)v13;
    v562 = 63LL;
    v563 = v561 == v562;
    if (v563) goto bb133;
    else goto bb134;
  bb132:
    goto bb126;
  bb133:
    v565 = 0LL;
    v566 = (int64_t)Lexer_peek((void*)v565, v2);
    v567 = 63LL;
    v568 = v566 == v567;
    if (v568) goto bb136;
    else goto bb137;
  bb134:
    v599 = *(int64_t*)v13;
    v600 = 40LL;
    v601 = v599 == v600;
    if (v601) goto bb142;
    else goto bb143;
  bb135:
    goto bb132;
  bb136:
    v570 = 0LL;
    v571 = (int64_t)Lexer_advance((void*)v570, v2);
    v572 = malloc(16);
    v573 = 124LL;
    v574 = (char*)v572 + 0;
    *(int64_t*)v574 = (int64_t)v573;
    *(void**)v19 = (void*)v572;
    goto bb138;
  bb137:
    v578 = 0LL;
    v579 = (int64_t)Lexer_peek((void*)v578, v2);
    v580 = 46LL;
    v581 = v579 == v580;
    if (v581) goto bb139;
    else goto bb140;
  bb138:
    goto bb135;
  bb139:
    v583 = 0LL;
    v584 = (int64_t)Lexer_advance((void*)v583, v2);
    v585 = malloc(16);
    v586 = 125LL;
    v587 = (char*)v585 + 0;
    *(int64_t*)v587 = (int64_t)v586;
    *(void**)v19 = (void*)v585;
    goto bb141;
  bb140:
    v591 = malloc(16);
    v592 = 123LL;
    v593 = (char*)v591 + 0;
    *(int64_t*)v593 = (int64_t)v592;
    *(void**)v19 = (void*)v591;
    goto bb141;
  bb141:
    goto bb138;
  bb142:
    v603 = malloc(16);
    v604 = 130LL;
    v605 = (char*)v603 + 0;
    *(int64_t*)v605 = (int64_t)v604;
    *(void**)v19 = (void*)v603;
    goto bb144;
  bb143:
    v609 = *(int64_t*)v13;
    v610 = 41LL;
    v611 = v609 == v610;
    if (v611) goto bb145;
    else goto bb146;
  bb144:
    goto bb135;
  bb145:
    v613 = malloc(16);
    v614 = 131LL;
    v615 = (char*)v613 + 0;
    *(int64_t*)v615 = (int64_t)v614;
    *(void**)v19 = (void*)v613;
    goto bb147;
  bb146:
    v619 = *(int64_t*)v13;
    v620 = 123LL;
    v621 = v619 == v620;
    if (v621) goto bb148;
    else goto bb149;
  bb147:
    goto bb144;
  bb148:
    v623 = malloc(16);
    v624 = 132LL;
    v625 = (char*)v623 + 0;
    *(int64_t*)v625 = (int64_t)v624;
    *(void**)v19 = (void*)v623;
    goto bb150;
  bb149:
    v629 = *(int64_t*)v13;
    v630 = 125LL;
    v631 = v629 == v630;
    if (v631) goto bb151;
    else goto bb152;
  bb150:
    goto bb147;
  bb151:
    v633 = malloc(16);
    v634 = 133LL;
    v635 = (char*)v633 + 0;
    *(int64_t*)v635 = (int64_t)v634;
    *(void**)v19 = (void*)v633;
    goto bb153;
  bb152:
    v639 = *(int64_t*)v13;
    v640 = 91LL;
    v641 = v639 == v640;
    if (v641) goto bb154;
    else goto bb155;
  bb153:
    goto bb150;
  bb154:
    v643 = malloc(16);
    v644 = 128LL;
    v645 = (char*)v643 + 0;
    *(int64_t*)v645 = (int64_t)v644;
    *(void**)v19 = (void*)v643;
    goto bb156;
  bb155:
    v649 = *(int64_t*)v13;
    v650 = 93LL;
    v651 = v649 == v650;
    if (v651) goto bb157;
    else goto bb158;
  bb156:
    goto bb153;
  bb157:
    v653 = malloc(16);
    v654 = 129LL;
    v655 = (char*)v653 + 0;
    *(int64_t*)v655 = (int64_t)v654;
    *(void**)v19 = (void*)v653;
    goto bb159;
  bb158:
    v659 = (char*)v2 + 32;
    v660 = *(void**)v659;
    v661 = *(int64_t*)v5;
    v662 = *(int64_t*)v9;
    v663 = _str251;
    v664 = *(int64_t*)v13;
    v665 = 0LL;
    v666 = byte_to_str((void*)v665, v664);
    v667 = p_str_concat(v663, v666);
    v668 = _str252;
    v669 = p_str_concat(v667, v668);
    v670 = 0LL;
    v671 = lex_error((void*)v670, v661, v662, v669);
    v672 = 0LL;
    Vec_push((void*)v672, v660, (int64_t)v671);
    v674 = malloc(16);
    v675 = 134LL;
    v676 = (char*)v674 + 0;
    *(int64_t*)v676 = (int64_t)v675;
    *(void**)v19 = (void*)v674;
    goto bb159;
  bb159:
    goto bb156;
    return;
}

void* Vec_take(void* v0, void* v1, int64_t v2) {
    p_call_stack_push("Vec_take");
    int64_t v3;
    int64_t v4;
    int64_t v5; /* placeholder for void */
    v3 = 0LL;
    v4 = (int64_t)vec_take((void*)v3, (int64_t)v1, v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

void* Vec_zip(void* v0, void* v1, void* v2) {
    p_call_stack_push("Vec_zip");
    int64_t v4;
    int64_t v5; /* placeholder for void */
    int64_t v3;
    v3 = 0LL;
    v4 = (int64_t)vec_zip((void*)v3, (int64_t)v1, (int64_t)v2);
    p_call_stack_pop();
    return (void*)v4;
    return 0;
}

int64_t String_split(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("String_split");
    int64_t v178;
    int64_t v172; /* placeholder for void */
    int64_t v57;
    int64_t v70; /* placeholder for void */
    int64_t v144; /* placeholder for void */
    int64_t v47;
    void* v63;
    int64_t v121; /* placeholder for void */
    int64_t v83; /* placeholder for void */
    bool v53;
    int64_t v37; /* placeholder for void */
    int64_t v91; /* placeholder for void */
    int64_t v113;
    int64_t v147;
    void* v160;
    int64_t v36; /* placeholder for void */
    int64_t v17;
    int64_t v68;
    void* v69;
    int64_t v46; /* placeholder for void */
    int64_t v41;
    int64_t v99; /* placeholder for void */
    void* v78;
    int64_t v101;
    int64_t v85;
    uint8_t v79;
    int64_t v88; /* placeholder for void */
    void* v32;
    int64_t v100;
    int64_t v105;
    int64_t v125; /* placeholder for void */
    int64_t v95;
    bool v10;
    int64_t v102;
    int64_t v39;
    void* v45;
    int64_t v43; /* placeholder for void */
    int64_t v135;
    int64_t v145; /* placeholder for void */
    bool v11;
    int64_t v149;
    void* v150;
    int64_t v6; /* placeholder for void */
    void* v48;
    int64_t v153;
    bool v154;
    int64_t v158;
    int64_t v137; /* placeholder for void */
    void* v171;
    int64_t v152;
    int64_t v24;
    void* v5;
    int64_t v128; /* placeholder for void */
    int64_t v92; /* placeholder for void */
    int64_t v49; /* placeholder for void */
    int64_t v133; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    void* v35;
    void* v59;
    void* v177;
    void* v109;
    int64_t v164;
    int64_t v117;
    int64_t v90; /* placeholder for void */
    bool v98;
    int64_t v131;
    int64_t v138;
    int64_t v119;
    void* v18;
    int64_t v25; /* placeholder for void */
    int64_t v7;
    void* v108;
    int64_t v143;
    int64_t v165;
    int64_t v62;
    int64_t v71;
    int64_t v122;
    bool v44;
    int64_t v80; /* placeholder for void */
    void* v81;
    void* v28;
    int64_t v73; /* placeholder for void */
    int64_t v86;
    int64_t v142;
    int64_t v76; /* placeholder for void */
    int64_t v12; /* placeholder for void */
    int64_t v104; /* placeholder for void */
    int64_t v132;
    int64_t v65;
    int64_t v163;
    void* v123;
    int64_t v58;
    int64_t v130;
    int64_t v173;
    int64_t v111;
    int64_t v156;
    int64_t v30;
    int64_t v185; /* placeholder for void */
    int64_t v187;
    bool v42;
    int64_t v188; /* placeholder for void */
    void* v72;
    int64_t v115;
    int64_t v148;
    int64_t v182;
    void* v120;
    int64_t v140; /* placeholder for void */
    int64_t v151; /* placeholder for void */
    int64_t v161; /* placeholder for void */
    int64_t v21;
    void* v66;
    int64_t v155; /* placeholder for void */
    void* v159;
    int64_t v118;
    int64_t v141;
    int64_t v167; /* placeholder for void */
    int64_t v168;
    int64_t v179; /* placeholder for void */
    int64_t v112;
    int64_t v146; /* placeholder for void */
    int64_t v162;
    bool v22;
    int64_t v64; /* placeholder for void */
    int64_t v74;
    int64_t v55;
    int64_t v54; /* placeholder for void */
    int64_t v13;
    int64_t v23; /* placeholder for void */
    int64_t v94;
    int64_t v139; /* placeholder for void */
    int64_t v176; /* placeholder for void */
    int64_t v186; /* placeholder for void */
    int64_t v183;
    int64_t v20;
    int64_t v33; /* placeholder for void */
    int64_t v110; /* placeholder for void */
    uint8_t v82;
    int64_t v124; /* placeholder for void */
    int64_t v129; /* placeholder for void */
    int64_t v136;
    bool v89;
    int64_t v38;
    int64_t v166;
    int64_t v114;
    int64_t v170;
    int64_t v181;
    int64_t v96; /* placeholder for void */
    void* v103;
    int64_t v19; /* placeholder for void */
    int64_t v106;
    int64_t v134;
    int64_t v31;
    int64_t v52;
    int64_t v60; /* placeholder for void */
    int64_t v84; /* placeholder for void */
    int64_t v9;
    int64_t v56;
    void* v75;
    int64_t v175; /* placeholder for void */
    int64_t v61;
    bool v87;
    int64_t v40;
    int64_t v107;
    void* v126;
    int64_t v93;
    int64_t v184; /* placeholder for void */
    void* v174;
    int64_t v77; /* placeholder for void */
    int64_t v169;
    int64_t v180; /* placeholder for void */
    int64_t v50; /* placeholder for void */
    int64_t v3;
    int64_t v27;
    int64_t v97; /* placeholder for void */
    int64_t v67; /* placeholder for void */
    int64_t v127;
    int64_t v4;
    int64_t v29; /* placeholder for void */
    int64_t v15; /* placeholder for void */
    int64_t v116; /* placeholder for void */
    int64_t v34;
    int64_t v157;
    int64_t v26; /* placeholder for void */
    bool v8;
    int64_t v16;
    int64_t v51;
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

int64_t p_str_compare(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("p_str_compare");
    uint8_t v74;
    int64_t v31; /* placeholder for void */
    uint8_t v37;
    int64_t v24; /* placeholder for void */
    int64_t v41;
    int64_t v42;
    int64_t v6; /* placeholder for void */
    int64_t v52; /* placeholder for void */
    int64_t v5;
    int64_t v20; /* placeholder for void */
    int64_t v60;
    void* v70;
    bool v3;
    int64_t v79;
    int64_t v22; /* placeholder for void */
    int64_t v53;
    int64_t v75; /* placeholder for void */
    int64_t v13; /* placeholder for void */
    int64_t v56; /* placeholder for void */
    bool v51;
    bool v46;
    int64_t v64;
    int64_t v4; /* placeholder for void */
    int64_t v39; /* placeholder for void */
    int64_t v78;
    bool v9;
    int64_t v18;
    int64_t v48; /* placeholder for void */
    int64_t v17; /* placeholder for void */
    bool v47;
    int64_t v30; /* placeholder for void */
    int64_t v76; /* placeholder for void */
    void* v29;
    int64_t v49;
    int64_t v68;
    int64_t v45;
    int64_t v62;
    int64_t v63;
    void* v26;
    void* v38;
    int64_t v12;
    int64_t v66;
    int64_t v19; /* placeholder for void */
    int64_t v77;
    int64_t v58;
    int64_t v28;
    bool v16;
    int64_t v55;
    int64_t v7; /* placeholder for void */
    int64_t v54;
    int64_t v8;
    int64_t v59;
    void* v36;
    int64_t v80; /* placeholder for void */
    int64_t v57; /* placeholder for void */
    int64_t v72; /* placeholder for void */
    void* v23;
    int64_t v27; /* placeholder for void */
    uint8_t v71;
    bool v43;
    int64_t v25;
    int64_t v61; /* placeholder for void */
    int64_t v11;
    void* v34;
    int64_t v67; /* placeholder for void */
    uint8_t v33;
    void* v21;
    int64_t v44;
    int64_t v15;
    void* v32;
    int64_t v69; /* placeholder for void */
    int64_t v65; /* placeholder for void */
    int64_t v14; /* placeholder for void */
    int64_t v35; /* placeholder for void */
    int64_t v10; /* placeholder for void */
    int64_t v40; /* placeholder for void */
    void* v73;
    int64_t v50;
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

bool Option_is_some(void* v0, void* v1) {
    p_call_stack_push("Option_is_some");
    int64_t v17; /* placeholder for void */
    void* v11;
    int64_t v3;
    int64_t v18; /* placeholder for void */
    void* v2;
    int64_t v4;
    int64_t v6; /* placeholder for void */
    bool v16;
    bool v19;
    bool v5;
    bool v14;
    int64_t v20; /* placeholder for void */
    bool v9;
    void* v7;
    int64_t v12;
    int64_t v8;
    int64_t v13;
    int64_t v15; /* placeholder for void */
    int64_t v10; /* placeholder for void */
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

void* Vec_clone(void* v0, void* v1) {
    p_call_stack_push("Vec_clone");
    int64_t v2;
    int64_t v3; /* placeholder for void */
    v2 = (int64_t)p_vec_clone(v1);
    p_call_stack_pop();
    return (void*)v2;
    return 0;
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

bool lex_is_alpha(void* v0, int64_t v1) {
    p_call_stack_push("lex_is_alpha");
    bool v10;
    int64_t v7;
    int64_t v13; /* placeholder for void */
    int64_t v9;
    bool v6;
    bool v5;
    int64_t v2;
    bool v3;
    bool v8;
    bool v11;
    int64_t v4;
    bool v12;
    v2 = 97LL;
    v3 = v1 >= v2;
    v4 = 122LL;
    v5 = v1 <= v4;
    v6 = v3 && v5;
    v7 = 65LL;
    v8 = v1 >= v7;
    v9 = 90LL;
    v10 = v1 <= v9;
    v11 = v8 && v10;
    v12 = v6 || v11;
    p_call_stack_pop();
    return v12;
    return 0;
}

bool vec_contains_val(void* v0, int64_t v1, int64_t v2) {
    p_call_stack_push("vec_contains_val");
    int64_t v26; /* placeholder for void */
    int64_t v12;
    int64_t v21; /* placeholder for void */
    int64_t v27; /* placeholder for void */
    int64_t v7;
    bool v13;
    void* v8;
    int64_t v19; /* placeholder for void */
    int64_t v22; /* placeholder for void */
    int64_t v23;
    int64_t v4;
    bool v18;
    int64_t v25;
    int64_t v10; /* placeholder for void */
    bool v20;
    int64_t v3;
    int64_t v15;
    int64_t v14; /* placeholder for void */
    int64_t v9; /* placeholder for void */
    int64_t v11;
    void* v5;
    int64_t v29; /* placeholder for void */
    int64_t v17;
    int64_t v16;
    int64_t v6; /* placeholder for void */
    int64_t v24;
    bool v28;
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

// ─── C entry point ───
int main(int argc, char** argv) {
    __p_argc = argc; __p_argv = argv;
    // Register functions for p_thread_spawn lookup
    p_thread_register("ByteBuffer_read_byte", (int64_t(*)())ByteBuffer_read_byte);
    p_thread_register("vec_dedup", (int64_t(*)())vec_dedup);
    p_thread_register("parse_radix", (int64_t(*)())parse_radix);
    p_thread_register("String_clone", (int64_t(*)())String_clone);
    p_thread_register("HashMap_keys", (int64_t(*)())HashMap_keys);
    p_thread_register("is_word_keyword", (int64_t(*)())is_word_keyword);
    p_thread_register("str_split", (int64_t(*)())str_split);
    p_thread_register("Option_unwrap", (int64_t(*)())Option_unwrap);
    p_thread_register("str_to_upper", (int64_t(*)())str_to_upper);
    p_thread_register("Vec_all", (int64_t(*)())Vec_all);
    p_thread_register("Lexer_skip_block_comment", (int64_t(*)())Lexer_skip_block_comment);
    p_thread_register("str_parse_int_range", (int64_t(*)())str_parse_int_range);
    p_thread_register("ByteBuffer_write_bytes", (int64_t(*)())ByteBuffer_write_bytes);
    p_thread_register("vec_window", (int64_t(*)())vec_window);
    p_thread_register("p_str_equal", (int64_t(*)())p_str_equal);
    p_thread_register("Vec_len", (int64_t(*)())Vec_len);
    p_thread_register("p_str_hash", (int64_t(*)())p_str_hash);
    p_thread_register("Vec_contains_val", (int64_t(*)())Vec_contains_val);
    p_thread_register("String_trim", (int64_t(*)())String_trim);
    p_thread_register("allocator_bytes_used", (int64_t(*)())allocator_bytes_used);
    p_thread_register("String_contains", (int64_t(*)())String_contains);
    p_thread_register("HashMap_grow", (int64_t(*)())HashMap_grow);
    p_thread_register("token_kind_to_str", (int64_t(*)())token_kind_to_str);
    p_thread_register("Lexer_read_raw_string", (int64_t(*)())Lexer_read_raw_string);
    p_thread_register("Option_map", (int64_t(*)())Option_map);
    p_thread_register("vec_sort_by", (int64_t(*)())vec_sort_by);
    p_thread_register("println", (int64_t(*)())println);
    p_thread_register("Result_expect", (int64_t(*)())Result_expect);
    p_thread_register("Vec_window", (int64_t(*)())Vec_window);
    p_thread_register("StringBuilder_append_char", (int64_t(*)())StringBuilder_append_char);
    p_thread_register("ByteBuffer_len", (int64_t(*)())ByteBuffer_len);
    p_thread_register("ByteBuffer_to_str", (int64_t(*)())ByteBuffer_to_str);
    p_thread_register("str_compare", (int64_t(*)())str_compare);
    p_thread_register("Vec_remove", (int64_t(*)())Vec_remove);
    p_thread_register("HashMap_remove", (int64_t(*)())HashMap_remove);
    p_thread_register("Lexer_peek_at", (int64_t(*)())Lexer_peek_at);
    p_thread_register("HashMap_find_slot", (int64_t(*)())HashMap_find_slot);
    p_thread_register("str_from_bytes", (int64_t(*)())str_from_bytes);
    p_thread_register("str_to_lower", (int64_t(*)())str_to_lower);
    p_thread_register("Vec_is_empty", (int64_t(*)())Vec_is_empty);
    p_thread_register("assert", (int64_t(*)())assert);
    p_thread_register("HashMap_get_or", (int64_t(*)())HashMap_get_or);
    p_thread_register("Lexer_read_identifier_or_keyword", (int64_t(*)())Lexer_read_identifier_or_keyword);
    p_thread_register("str_ends_with", (int64_t(*)())str_ends_with);
    p_thread_register("Lexer_advance", (int64_t(*)())Lexer_advance);
    p_thread_register("ByteBuffer_write_byte", (int64_t(*)())ByteBuffer_write_byte);
    p_thread_register("pad_right", (int64_t(*)())pad_right);
    p_thread_register("StringBuilder_clear", (int64_t(*)())StringBuilder_clear);
    p_thread_register("ByteBuffer_set_byte", (int64_t(*)())ByteBuffer_set_byte);
    p_thread_register("pad_left", (int64_t(*)())pad_left);
    p_thread_register("str_find", (int64_t(*)())str_find);
    p_thread_register("String_char_at", (int64_t(*)())String_char_at);
    p_thread_register("char_to_str", (int64_t(*)())char_to_str);
    p_thread_register("Vec_pop", (int64_t(*)())Vec_pop);
    p_thread_register("String_repeat", (int64_t(*)())String_repeat);
    p_thread_register("Vec_sort", (int64_t(*)())Vec_sort);
    p_thread_register("str_parse_hex", (int64_t(*)())str_parse_hex);
    p_thread_register("HashMap_is_empty", (int64_t(*)())HashMap_is_empty);
    p_thread_register("HashMap_clear", (int64_t(*)())HashMap_clear);
    p_thread_register("HashMap_get", (int64_t(*)())HashMap_get);
    p_thread_register("Vec_contains", (int64_t(*)())Vec_contains);
    p_thread_register("byte_to_str", (int64_t(*)())byte_to_str);
    p_thread_register("Result_unwrap_or_else", (int64_t(*)())Result_unwrap_or_else);
    p_thread_register("String_find", (int64_t(*)())String_find);
    p_thread_register("lex_is_digit", (int64_t(*)())lex_is_digit);
    p_thread_register("String_bytes", (int64_t(*)())String_bytes);
    p_thread_register("Vec_to_str", (int64_t(*)())Vec_to_str);
    p_thread_register("Vec_last", (int64_t(*)())Vec_last);
    p_thread_register("String_to_string", (int64_t(*)())String_to_string);
    p_thread_register("str_byte_at", (int64_t(*)())str_byte_at);
    p_thread_register("Lexer_peek", (int64_t(*)())Lexer_peek);
    p_thread_register("Lexer_read_utf8", (int64_t(*)())Lexer_read_utf8);
    p_thread_register("char_is_upper", (int64_t(*)())char_is_upper);
    p_thread_register("Vec_fold", (int64_t(*)())Vec_fold);
    p_thread_register("Vec_clear", (int64_t(*)())Vec_clear);
    p_thread_register("str_equal", (int64_t(*)())str_equal);
    p_thread_register("lex_error", (int64_t(*)())lex_error);
    p_thread_register("float_to_str", (int64_t(*)())float_to_str);
    p_thread_register("Lexer_read_number", (int64_t(*)())Lexer_read_number);
    p_thread_register("Vec_any", (int64_t(*)())Vec_any);
    p_thread_register("Vec_dedup", (int64_t(*)())Vec_dedup);
    p_thread_register("repeat_char", (int64_t(*)())repeat_char);
    p_thread_register("Vec_capacity", (int64_t(*)())Vec_capacity);
    p_thread_register("float_debug", (int64_t(*)())float_debug);
    p_thread_register("Vec_get", (int64_t(*)())Vec_get);
    p_thread_register("Vec_sort_by", (int64_t(*)())Vec_sort_by);
    p_thread_register("Vec_find", (int64_t(*)())Vec_find);
    p_thread_register("Vec_enumerate", (int64_t(*)())Vec_enumerate);
    p_thread_register("Option_is_none", (int64_t(*)())Option_is_none);
    p_thread_register("Vec_insert", (int64_t(*)())Vec_insert);
    p_thread_register("Option_and_then", (int64_t(*)())Option_and_then);
    p_thread_register("Option_or_else", (int64_t(*)())Option_or_else);
    p_thread_register("String_rfind", (int64_t(*)())String_rfind);
    p_thread_register("Option_expect", (int64_t(*)())Option_expect);
    p_thread_register("Vec_set", (int64_t(*)())Vec_set);
    p_thread_register("Vec_reverse", (int64_t(*)())Vec_reverse);
    p_thread_register("Result_is_err", (int64_t(*)())Result_is_err);
    p_thread_register("Result_err", (int64_t(*)())Result_err);
    p_thread_register("StringBuilder_new", (int64_t(*)())StringBuilder_new);
    p_thread_register("lex_is_alphanumeric", (int64_t(*)())lex_is_alphanumeric);
    p_thread_register("StringBuilder_append_int", (int64_t(*)())StringBuilder_append_int);
    p_thread_register("vec_skip", (int64_t(*)())vec_skip);
    p_thread_register("ByteBuffer_clear", (int64_t(*)())ByteBuffer_clear);
    p_thread_register("Vec_position", (int64_t(*)())Vec_position);
    p_thread_register("max", (int64_t(*)())p_max);
    p_thread_register("HashMap_insert", (int64_t(*)())HashMap_insert);
    p_thread_register("clamp", (int64_t(*)())clamp);
    p_thread_register("String_len", (int64_t(*)())String_len);
    p_thread_register("create_allocator", (int64_t(*)())create_allocator);
    p_thread_register("str_is_empty", (int64_t(*)())str_is_empty);
    p_thread_register("min", (int64_t(*)())p_min);
    p_thread_register("allocator_alloc", (int64_t(*)())allocator_alloc);
    p_thread_register("vec_enumerate", (int64_t(*)())vec_enumerate);
    p_thread_register("str_parse_int", (int64_t(*)())str_parse_int);
    p_thread_register("p_str_length", (int64_t(*)())p_str_length);
    p_thread_register("Vec_chunk", (int64_t(*)())Vec_chunk);
    p_thread_register("str_escape", (int64_t(*)())str_escape);
    p_thread_register("vec_flat_map", (int64_t(*)())vec_flat_map);
    p_thread_register("HashMap_contains", (int64_t(*)())HashMap_contains);
    p_thread_register("str_slice", (int64_t(*)())str_slice);
    p_thread_register("Vec_push", (int64_t(*)())Vec_push);
    p_thread_register("str_bytes", (int64_t(*)())str_bytes);
    p_thread_register("str_contains", (int64_t(*)())str_contains);
    p_thread_register("keyword_or_ident", (int64_t(*)())keyword_or_ident);
    p_thread_register("String_char_count", (int64_t(*)())String_char_count);
    p_thread_register("str_rfind", (int64_t(*)())str_rfind);
    p_thread_register("str_trim_start", (int64_t(*)())str_trim_start);
    p_thread_register("token_new", (int64_t(*)())token_new);
    p_thread_register("char_is_whitespace", (int64_t(*)())char_is_whitespace);
    p_thread_register("int_to_str", (int64_t(*)())int_to_str);
    p_thread_register("Lexer_new", (int64_t(*)())Lexer_new);
    p_thread_register("Lexer_skip_line_comment", (int64_t(*)())Lexer_skip_line_comment);
    p_thread_register("token_kind_debug", (int64_t(*)())token_kind_debug);
    p_thread_register("Lexer_read_string", (int64_t(*)())Lexer_read_string);
    p_thread_register("HashMap_clone", (int64_t(*)())HashMap_clone);
    p_thread_register("char_is_digit", (int64_t(*)())char_is_digit);
    p_thread_register("Vec_flat_map", (int64_t(*)())Vec_flat_map);
    p_thread_register("Option_unwrap_or", (int64_t(*)())Option_unwrap_or);
    p_thread_register("Vec_map", (int64_t(*)())Vec_map);
    p_thread_register("String_to_uppercase", (int64_t(*)())String_to_uppercase);
    p_thread_register("Result_map_err", (int64_t(*)())Result_map_err);
    p_thread_register("StringBuilder_len", (int64_t(*)())StringBuilder_len);
    p_thread_register("StringBuilder_is_empty", (int64_t(*)())StringBuilder_is_empty);
    p_thread_register("ByteBuffer_new", (int64_t(*)())ByteBuffer_new);
    p_thread_register("Vec_filter", (int64_t(*)())Vec_filter);
    p_thread_register("Lexer_tokenize", (int64_t(*)())Lexer_tokenize);
    p_thread_register("Vec_skip", (int64_t(*)())Vec_skip);
    p_thread_register("String_byte_at", (int64_t(*)())String_byte_at);
    p_thread_register("Vec_first", (int64_t(*)())Vec_first);
    p_thread_register("Result_is_ok", (int64_t(*)())Result_is_ok);
    p_thread_register("Vec_truncate", (int64_t(*)())Vec_truncate);
    p_thread_register("Result_ok", (int64_t(*)())Result_ok);
    p_thread_register("HashMap_len", (int64_t(*)())HashMap_len);
    p_thread_register("String_ends_with", (int64_t(*)())String_ends_with);
    p_thread_register("String_slice", (int64_t(*)())String_slice);
    p_thread_register("char_is_lower", (int64_t(*)())char_is_lower);
    p_thread_register("Lexer_read_fstring", (int64_t(*)())Lexer_read_fstring);
    p_thread_register("lex_is_hexdigit", (int64_t(*)())lex_is_hexdigit);
    p_thread_register("vec_zip", (int64_t(*)())vec_zip);
    p_thread_register("HashMap_values", (int64_t(*)())HashMap_values);
    p_thread_register("p_str_copy_n", (int64_t(*)())p_str_copy_n);
    p_thread_register("String_is_empty", (int64_t(*)())String_is_empty);
    p_thread_register("char_to_lower", (int64_t(*)())char_to_lower);
    p_thread_register("vec_take", (int64_t(*)())vec_take);
    p_thread_register("str_replace", (int64_t(*)())str_replace);
    p_thread_register("str_trim_end", (int64_t(*)())str_trim_end);
    p_thread_register("str_repeat", (int64_t(*)())str_repeat);
    p_thread_register("String_push_str", (int64_t(*)())String_push_str);
    p_thread_register("HashMap_to_str", (int64_t(*)())HashMap_to_str);
    p_thread_register("String_replace", (int64_t(*)())String_replace);
    p_thread_register("String_parse_int", (int64_t(*)())String_parse_int);
    p_thread_register("Result_unwrap", (int64_t(*)())Result_unwrap);
    p_thread_register("Result_map", (int64_t(*)())Result_map);
    p_thread_register("assert_msg", (int64_t(*)())assert_msg);
    p_thread_register("ByteBuffer_is_empty", (int64_t(*)())ByteBuffer_is_empty);
    p_thread_register("Vec_for_each", (int64_t(*)())Vec_for_each);
    p_thread_register("Option_unwrap_or_else", (int64_t(*)())Option_unwrap_or_else);
    p_thread_register("str_trim", (int64_t(*)())str_trim);
    p_thread_register("Vec_extend", (int64_t(*)())Vec_extend);
    p_thread_register("HashMap_new", (int64_t(*)())HashMap_new);
    p_thread_register("ByteBuffer_from_string", (int64_t(*)())ByteBuffer_from_string);
    p_thread_register("str_len", (int64_t(*)())str_len);
    p_thread_register("String_starts_with", (int64_t(*)())String_starts_with);
    p_thread_register("String_from", (int64_t(*)())String_from);
    p_thread_register("char_is_alpha", (int64_t(*)())char_is_alpha);
    p_thread_register("Result_unwrap_or", (int64_t(*)())Result_unwrap_or);
    p_thread_register("token_kind_tag", (int64_t(*)())token_kind_tag);
    p_thread_register("char_to_upper", (int64_t(*)())char_to_upper);
    p_thread_register("Result_and_then", (int64_t(*)())Result_and_then);
    p_thread_register("StringBuilder_to_str", (int64_t(*)())StringBuilder_to_str);
    p_thread_register("str_starts_with", (int64_t(*)())str_starts_with);
    p_thread_register("StringBuilder_append_str", (int64_t(*)())StringBuilder_append_str);
    p_thread_register("vec_chunk", (int64_t(*)())vec_chunk);
    p_thread_register("str_parse_float", (int64_t(*)())str_parse_float);
    p_thread_register("String_to_lowercase", (int64_t(*)())String_to_lowercase);
    p_thread_register("lex_is_whitespace", (int64_t(*)())lex_is_whitespace);
    p_thread_register("abs", (int64_t(*)())p_abs);
    p_thread_register("p_str_from_ptr", (int64_t(*)())p_str_from_ptr);
    p_thread_register("str_split_char", (int64_t(*)())str_split_char);
    p_thread_register("token_kind_eq", (int64_t(*)())token_kind_eq);
    p_thread_register("allocator_free", (int64_t(*)())allocator_free);
    p_thread_register("str_concat", (int64_t(*)())str_concat);
    p_thread_register("Lexer_read_operator", (int64_t(*)())Lexer_read_operator);
    p_thread_register("Vec_take", (int64_t(*)())Vec_take);
    p_thread_register("Vec_zip", (int64_t(*)())Vec_zip);
    p_thread_register("String_split", (int64_t(*)())String_split);
    p_thread_register("p_str_compare", (int64_t(*)())p_str_compare);
    p_thread_register("Option_is_some", (int64_t(*)())Option_is_some);
    p_thread_register("Vec_clone", (int64_t(*)())Vec_clone);
    p_thread_register("Vec_new", (int64_t(*)())Vec_new);
    p_thread_register("lex_is_alpha", (int64_t(*)())lex_is_alpha);
    p_thread_register("vec_contains_val", (int64_t(*)())vec_contains_val);
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
