/* Link stubs for interpreter-only builtins referenced (but never called)
   by compiled Fides code — lets probe binaries link under zig cc. */
long long p_is_dir(long long a) { (void)a; return 0; }
long long p_mkdir(long long a) { (void)a; return 0; }
long long p_dir_list(long long a) { (void)a; return 0; }
long long p_rename(long long a, long long b) { (void)a; (void)b; return 0; }
long long p_rmdir(long long a) { (void)a; return 0; }
long long p_getpid(void) { return 0; }
long long p_process_run(long long a) { (void)a; return 0; }
long long p_process_stdout(long long a) { (void)a; return 0; }
long long p_process_stderr(long long a) { (void)a; return 0; }
long long p_process_code(long long a) { (void)a; return 0; }
long long p_process_close(long long a) { (void)a; return 0; }
