#!/bin/bash
# wrap-for-cc.sh — apply build-selfhost.sh's 256MB-stack wrapper to a raw
# dump-c output, so the C can be compiled independently of the (contended)
# build pipeline. Usage: wrap-for-cc.sh <in.c> <out.c>
set -eu
in="$1"; out="$2"
sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$in" > "$out"
cat >> "$out" <<'EOF'

// === Stack wrapper (added by scripts/wrap-for-cc.sh, as build-selfhost.sh) ===
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
EOF
echo "wrapped: $out"
