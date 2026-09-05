// Wrapper to run fides-c2 with a larger stack size
#include <windows.h>
#include <stdio.h>
#include <string.h>

extern int real_main(int argc, char** argv);

typedef struct {
    int argc;
    char** argv;
    int ret;
} MainArgs;

DWORD WINAPI thread_main(LPVOID param) {
    MainArgs* args = (MainArgs*)param;
    args->ret = real_main(args->argc, args->argv);
    return 0;
}

int main(int argc, char** argv) {
    // If first arg is "--wrapped", run directly
    if (argc > 1 && strcmp(argv[1], "--wrapped") == 0) {
        // Shift args
        for (int i = 1; i < argc - 1; i++) argv[i] = argv[i+1];
        argv[argc-1] = NULL;
        argc--;
        return real_main(argc, argv);
    }
    // Otherwise, run on a thread with 256MB stack
    MainArgs args = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &args, 0, NULL);
    if (!h) {
        fprintf(stderr, "Failed to create thread\n");
        return 1;
    }
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return args.ret;
}
