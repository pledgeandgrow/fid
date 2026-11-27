/* scripts/stackwrap.c — appended to generated C after renaming main to
   fides_main: runs the program on a thread with a 256MB stack. The
   compiler's typechecker recurses deeper than the 8MB default main-stack
   holds (macOS seed, tier1 Nov 27: "Panic: Stack overflow" in
   TypeChecker_check_expr_inner). Same wrapper build-selfhost.sh adds. */
typedef struct { int argc; char** argv; int ret; } MainArgs;
#ifdef _WIN32
#include <windows.h>
static DWORD WINAPI thread_main(LPVOID p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &a, 0, NULL);
    if (!h) return 1;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return a.ret;
}
#else
#include <pthread.h>
static void* thread_main(void* p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    pthread_attr_t attr; pthread_t t;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, (size_t)256*1024*1024);
    if (pthread_create(&t, &attr, thread_main, &a) != 0) return 1;
    pthread_join(t, NULL);
    return a.ret;
}
#endif
