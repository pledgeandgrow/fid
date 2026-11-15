// bare-run: run a `fid emit --bare` binary on Linux x86-64, without QEMU.
//
//   gcc -O2 -o bare-run scripts/bare-run.c
//   ./bare-run program.bin [load-address]
//
// The binary is mapped at its load address (0x100000 by default: the
// bootable image of release G1; 0x10000 for the old flat binary) with the
// bare heap at 0x300000, and entered at its first byte in user mode (the
// image's first bytes jump past the boot headers to the 64-bit code; the
// boot path's mode switch, IDT and PIC set-up are not run). The
// instructions a user process may not run fault into a SIGSEGV handler that
// plays the hardware a bare program talks to:
//   out to COM1 (0x3F8)      the byte goes to stdout
//   out to 0xF4              QEMU's isa-debug-exit: exit with the byte
//                            (main's result, or 1 after a panic)
//   in from COM1+5 (LSR)     "transmitter empty" (other ports read 0)
//   cli, sti, other in/out   skipped
//   hlt                      the program is done: exit with rax
// Anything else privileged (lgdt, mov cr3, ...) stops with a message: those
// need QEMU (scripts/bare-qemu-test.sh).
#define _GNU_SOURCE
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ucontext.h>
#include <unistd.h>

static int dlab; /* COM1 line control: divisor latch access */

static void put(int c) {
    unsigned char b = (unsigned char)c;
    if (write(1, &b, 1) < 0) _exit(3);
}

static void on_fault(int sig, siginfo_t *si, void *ctx) {
    (void)sig; (void)si;
    ucontext_t *uc = (ucontext_t *)ctx;
    greg_t *g = uc->uc_mcontext.gregs;
    unsigned char *ip = (unsigned char *)g[REG_RIP];
    unsigned port = (unsigned)(g[REG_RDX] & 0xFFFF);
    int len = 0;
    int opsize16 = 0;
    if (ip[0] == 0x66) { opsize16 = 1; ip++; len = 1; }
    switch (ip[0]) {
    case 0xEE: /* out dx, al */
        /* with LCR.DLAB set (p_serial_init), 0x3F8 is the divisor latch */
        if (port == 0x3FB) dlab = (int)(g[REG_RAX] & 0x80);
        if (port == 0x3F8 && !dlab) put((int)(g[REG_RAX] & 0xFF));
        if (port == 0xF4) _exit((int)(g[REG_RAX] & 0xFF));
        len += 1; break;
    case 0xEF: len += 1; break;                 /* out dx, ax/eax */
    case 0xE6: case 0xE7:                       /* out imm8, al/eax */
        if (ip[1] == 0xF4) _exit((int)(g[REG_RAX] & 0xFF));
        len += 2; break;
    case 0xEC: /* in al, dx */
        g[REG_RAX] = (g[REG_RAX] & ~(greg_t)0xFF) | (port == 0x3FD ? 0x60 : 0);
        len += 1; break;
    case 0xED: /* in ax/eax, dx */
        g[REG_RAX] = opsize16 ? (g[REG_RAX] & ~(greg_t)0xFFFF) : 0;
        len += 1; break;
    case 0xE4: case 0xE5:
        g[REG_RAX] &= ~(greg_t)0xFF; len += 2; break;
    case 0xFA: case 0xFB: len += 1; break;      /* cli, sti */
    case 0xF4:                                  /* hlt: done */
        _exit((int)(g[REG_RAX] & 0xFF));
    default:
        fprintf(stderr, "bare-run: fault at %p (bytes %02x %02x %02x %02x): needs QEMU\n",
                (void *)g[REG_RIP], ip[0], ip[1], ip[2], ip[3]);
        _exit(2);
    }
    g[REG_RIP] += len;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: bare-run <program.bin> [load-address]\n"); return 2; }
    uintptr_t load = argc > 2 ? (uintptr_t)strtoull(argv[2], 0, 0) : 0x100000;
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 2; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    size_t span = ((size_t)size + (1u << 16) + 0xFFF) & ~(size_t)0xFFF;
    if (load < 0x300000 && load + span > 0x300000) {
        fprintf(stderr, "bare-run: the image (%ld bytes at %#lx) runs into the heap at 0x300000\n",
                size, (unsigned long)load);
        return 2;
    }
    void *img = mmap((void *)load, span, PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (img == MAP_FAILED || (uintptr_t)img != load) {
        fprintf(stderr, "bare-run: cannot map %#lx (vm.mmap_min_addr?)\n", (unsigned long)load);
        return 2;
    }
    if (fread(img, 1, (size_t)size, f) != (size_t)size) { perror("read"); return 2; }
    fclose(f);
    void *heap = mmap((void *)0x300000, 64u << 20, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (heap == MAP_FAILED) { fprintf(stderr, "bare-run: cannot map the heap at 0x300000\n"); return 2; }
    size_t stack_size = 8u << 20;
    char *stack = mmap(0, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_fault;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    void *top = stack + stack_size - 64;
    __asm__ volatile("mov %0, %%rsp\n\tjmp *%1" : : "r"(top), "r"((void *)load) : "memory");
    return 0;
}
