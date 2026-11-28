# Target Roadmap — Every Major Platform, No Toolchain

> Goal: `fid build --target <t>` produces a working program for every
> mainstream platform from any host, with no C compiler, no LLVM, no linker
> and no libc download, and each target is tier 1 (its suite runs in CI on
> every commit and blocks on failure, docs/reference/platforms.md).
>
> Created Nov 24 2026. Scope: targets that half exist (A-D) and more
> operating systems (E-H). Not in scope here: 32-bit targets (the compiler
> assumes 8-byte words throughout), big-endian architectures, and C
> libraries for other targets (see the Nov 24 discussion; they need their own
> roadmap).
>
> State it starts from (docs/reference/platforms.md, cross-compilation.md):
> no-toolchain output for Windows x64 (pe_ir), Linux x86-64/ARM64/RISC-V 64
> (static ELF, compiler/nat_ir.fd + nat_enc.fd, stdlib/linux_runtime/),
> macOS x64/ARM64 Mach-O (tier 3), WASM (`fid wasm`, tier 3, 316 of 344
> suite tests), Cortex-M Thumb-2 (nat_thumb.fd), x86 bare metal (`--bare`,
> `--boot`) and x86-64 UEFI (`--uefi`, pe_ir). `aarch64-windows` and
> `x86_64-freebsd` only through C (`zig cc`).
>
> Markers: `[ ]` open, `[~]` partly done, `[x]` done. Sizes: S hours,
> M days, L weeks, XL months. Each goal names what checks it.

## A. macOS native to tier 1 (XT1-XT13)

- [ ] XT1. Run the whole bootstrap suite natively on macOS ARM64 (M).
      `NATIVE=1 bash scripts/cross-test.sh aarch64-macos` on the
      macos-15 runner (macos-native.yml); record the pass count and every
      failure with its cause.
- [ ] XT2. The same on macOS x64 (M). The macos-15-intel runner;
      `x86_64-macos`.
- [ ] XT3. Fix the darwin runtime gaps the suite finds (L).
      stdlib/linux_runtime/darwin.fd: files, directories, processes,
      environment, time, sockets, `select`/kqueue for async.
- [ ] XT4. Threads and green tasks on Darwin (M). pthread-free threads
      through `bsdthread_create` or libSystem; the task and channel tests
      (test_spawn*, test_chan*, test_task*, test_async*) pass.
- [ ] XT5. Thread-local storage on Darwin (M): the TLS slot the runtime
      uses (pe_scope_key / word 0 of the TLS table on Linux) works on
      macOS ARM64 and x64.
- [ ] XT6. Signals and panics with traces (S-M): stack overflow from the
      guard page, SIGSEGV/SIGBUS to "Invalid memory access", traces with
      function names (`pe_fn_name_at`).
- [ ] XT7. Code signing that Gatekeeper and `codesign --verify` accept
      (M): the ad-hoc signature nat_enc.fd writes, checked on the runner,
      plus hardened-runtime flags as an option.
- [ ] XT8. Universal binaries tier 1 (S): `--target universal` fat files
      (both slices) run on both runners.
- [ ] XT9. Linking system frameworks (M): `extern "C"` against
      libSystem, CoreFoundation and Security by name, binds through dyld
      (the GOT path), with `.tbd`-free lookup; a test calling one of each.
- [ ] XT10. Debug info on macOS (M): DWARF in the Mach-O (or a
      `.dSYM`) that `lldb` reads: breakpoints by line, locals.
- [ ] XT11. Native self-host on macOS (M): `fid emit compiler/main.fd`
      on a Mac builds a `fid` that rebuilds itself byte for byte, as the
      Windows native fixpoint of Nov 24.
- [ ] XT12. CI blocks on the macOS native suite (S): macos-native.yml
      required on every PR; platforms.md moves macOS native to tier 1.
- [ ] XT13. Installer and release archive for macOS (S-M): release.yml
      builds and signs `fid` for both architectures.

## B. Windows ARM64 native (XT14-XT27)

- [ ] XT14. PE/COFF headers for ARM64 (M): machine type 0xAA64, the
      section layout, base relocations for ARM64 (IMAGE_REL_BASED_DIR64),
      in nat_enc.fd's PE writer (`nat_pe_*`).
- [ ] XT15. The Windows ARM64 calling convention in nat_ir (M):
      AAPCS64 with Microsoft's differences (x18 reserved, variadic calls
      pass floats in integer registers, the frame-record rules).
- [ ] XT16. Exception and unwind data for ARM64 (L): `.pdata`/`.xdata`
      unwind codes for every function, so panics, stack overflow and
      debuggers unwind (the x64 path writes x64 unwind codes).
- [ ] XT17. Imports on ARM64 (M): the import table and thunks through
      the IAT with ARM64 code (`adrp`/`ldr` + `br`), kernel32 and ws2_32.
- [ ] XT18. stdlib/linux_runtime/win.fd on ARM64 (L): the `--nat`
      Windows runtime built for `aarch64-windows`: files, threads, IOCP,
      sockets, time.
- [ ] XT19. Structured exception handling and the vectored handler (M):
      access violations, stack overflow and division faults become panics,
      as the x64 handler does.
- [ ] XT20. Thread-local storage on Windows ARM64 (S-M): the TEB through
      x18.
- [ ] XT21. Floats and SIMD (M): NEON for `@simd` and float registers,
      the callee-saved v8-v15 rules on Windows.
- [~] XT22. `--target aarch64-windows` on the no-toolchain path (S):
      main.fd sends it to the machine layer, not to C; cross-compilation.md
      updated.
      Nov 27 (fid built from the Nov 27 evening tree, zig, 6 of 8 units at
      -O2): `--target aarch64-windows` (also `windows-arm64`,
      `arm64-windows`, with or without --nat, and emit-ir) goes to the
      machine layer and is refused with rc 1, naming XT14 and writing no
      file; never an x86-64 PE, never the C route
      (tests/nattools/xt_ios_target_check.sh ok; cross-compilation.md and
      platforms.md updated). Left: it builds nothing until the PE writer
      has ARM64 (XT14-XT18); the check then wants machine 0xAA64.
- [ ] XT23. A CI runner for Windows ARM64 (M): GitHub's windows-11-arm
      runner or an emulated one; the suite on it.
- [ ] XT24. PDB debug info for ARM64 (M): compiler/pdb.fd's records for
      ARM64 registers, so WinDbg and Visual Studio debug the output.
- [ ] XT25. C callbacks on Windows ARM64 (S): BG166's entry adapters for
      the Windows ARM64 convention.
- [ ] XT26. The native `fid` for Windows ARM64 (M): `fid` built with
      `--target aarch64-windows` runs `run`, `check`, `emit`.
- [ ] XT27. Tier 1 (S): the suite blocking in CI; platforms.md and the
      installer list Windows ARM64.

## C. WASM native to tier 1, WASI preview 2, the browser (XT28-XT42)

- [~] XT28. The last 28 suite tests on `fid wasm` (M):
      scripts/wasm-test.sh to 344 of 344; each failure fixed or given a
      reason in the test's backends line.
      Partly Nov 28: `ALL=1` now selects 1,390 tests (the 344 is stale).
      With a zig-built fid of the Nov 27 tree: 1,287 passed, 7 failed,
      1 did not build, 95 skipped (lane-skips.txt wasm32-wasi-native, or
      `fid run` gives no reference). Fixed for it: float `%` (pert_c_fmod),
      the WASI no-op overridable only by the program's own definition,
      pert_c_nat_fma and 16-bit volatile loads/stores in wasi.fd, a 64 MB
      stack in wasm-run.mjs, `//@ backends`/`//@ env` and the Expected-block
      fallback in wasm-test.sh and tools/wasm_test.fd. Of the 8 left, 3 are
      the reference, not wasm (wasm prints what `fid ccompile` prints):
      test_pe88_prune_reach (the interpreter omits "dropped 5", which the
      test's Expected block has), test_zeroize_vec (the interpreter prints
      "interp: skipped"), test_std_x509 (`fid run` times out; wasm == C,
      and both differ from the test's line 38). Real wasm failures:
      test_deref_minimal (address 0 is valid memory: a null write is not
      caught), test_bg249_task_stack_grow and test_tasks (320 KB task
      stacks run out of memory; test_tasks passes on C),
      test_fix_fnval_c_ptr (a C function as a value gets table index 0),
      test_std_motion (builds, 171,510 bytes, exits 0 and prints nothing;
      in the gate run its build failed with an empty log).
- [ ] XT29. Green tasks that interleave on WASM (L): today tasks run to
      their end when entered (docs/reference/interp-tasks.md); stack
      switching through asyncify-style transformation or the stack-switching
      proposal, so channels between tasks work.
- [ ] XT30. Exceptions and panics with traces on WASM (M): a panic
      prints the Fides function names, not a wasm trap number.
- [x] XT31. Memory growth and the allocator (M): `memory.grow` on
      demand, the allocator freeing back, a test allocating past 4 GB on
      wasm64 later.
      Done Nov 27: stdlib/linux_runtime/wasi.fd keeps a list of free
      regions (best fit; freed pages reused), refuses more than 4 GB with
      ENOMEM, and every `-> str` builtin of the Linux runtime gives its
      string to its owner (base.fd `p_str_give`; 33 return sites in
      base.fd/sys.fd leaked a string each call on WASM and the machine
      layer). tests/wasm/memory-check.sh 5/5 with the fid of the Nov 27
      evening tree, also with FID_NAT_OFF=wpromote,wnoop: 200 MB held in
      three rounds peaks under 360 MB (was 1,828), 3 million strings and
      30,000 file reads under 32 MB (was 332), a refused memory.grow
      prints "out of memory (...)". Peak memory is printed by
      FID_WASM_MEMSTAT=1 (scripts/wasm-run.mjs). Later, as written:
      wasm64. Not part of this goal: test_bg249_task_stack_grow (20,000
      tasks with 320 KB stacks) runs out near 1.8 GB; task stacks on WASI
      can be smaller.
- [x] XT32. WASI preview 1 coverage (M): every file, clock, random,
      args and environment function the stdlib uses, run under wasmtime and
      Node.
      Done Nov 28: wasmtime 50.0.3 installed here; tests/wasm/wasi-check.sh
      10/10 (each of the 5 checks under Node and wasmtime). wasmtime found
      one bug Node could not: a directory was opened with the write rights,
      wasmtime refuses that with EISDIR, and every listing (fs_list_dir,
      fs_walk, fs_glob) came back empty (Node on Windows has no
      fd_readdir; scripts/wasm-run.mjs lists folders itself). Fixed in
      stdlib/linux_runtime/wasi.fd pe_wasi_openat: a directory asks for no
      fd_datasync/fd_write/fd_allocate/fd_filestat_set_size.
      memory-check.sh 5/5 after the fix.
      Nov 27: docs/reference/wasi.md lists each preview 1 call the runtime
      makes and what the stdlib reaches through it;
      tests/wasm/wasi-check.sh (files, directories, clocks and random,
      args and environment, file pages) 5/5 under Node with the fid of the
      Nov 27 evening tree, each run leaving its folder empty. Left:
      wasmtime is not on this machine, so the wasmtime half has not run
      (the script runs it when wasmtime is on PATH).
- [ ] XT33. WASI preview 2 / the component model (L): a `--target
      wasm32-wasip2` that emits a component with WIT imports for files,
      sockets and HTTP.
- [ ] XT34. Sockets and HTTP on WASI 0.2 (L): stdlib/net.fd and http.fd
      over `wasi:sockets` and `wasi:http`.
- [ ] XT35. A browser target (L): `--target wasm32-browser` with a JS
      loader written by `fid` (no bundler): console output, the DOM through
      `extern "js"` imports.
- [ ] XT36. Calling JS from Fides and Fides from JS (M): exported
      functions with typed arguments (numbers, strings through memory),
      imports declared in Fides.
- [ ] XT37. Threads on WASM (L): shared memory and atomics (wasm
      threads), `spawn` on Web Workers / wasi-threads.
- [ ] XT38. SIMD on WASM (M): `@simd` loops to wasm SIMD128.
- [~] XT39. Size (S-M): dead code removed (reach.fd), a hello world
      under 20 KB, measured in CI.
      Nov 27: tests/wasm/size-check.sh measures it (sections, largest
      functions, LIMIT 20480). With the fid of the Nov 27 evening tree
      hello is 95,034 bytes, 143 functions (92,692 without the name
      section, NONAMES=1); it was 178,274 bytes, 215 functions, with the
      same runtime before compiler/wasm.fd's changes (allocas promoted to
      locals, FID_NAT_OFF=wpromote; WASI calls a program never reaches
      written as no-ops, FID_NAT_OFF=wnoop: 151,127 with both off). Left:
      the 20 KB (the runtime's reachable functions are most of it), and
      the CI step.
- [ ] XT40. Debug info (M): DWARF custom sections (or source maps) that
      Chrome DevTools reads.
- [ ] XT41. A CI lane per runtime (S): wasmtime, Node and a headless
      browser run the suite.
- [ ] XT42. Tier 1 (S): platforms.md moves `fid wasm` to tier 1; the C
      route for wasm32-wasi becomes optional.

## D. FreeBSD native (XT43-XT52)

- [ ] XT43. A FreeBSD runtime file (M): stdlib/linux_runtime/freebsd.fd
      with FreeBSD's system call numbers and calling convention (carry flag
      for errors, not negative returns).
- [x] XT44. ELF branding (S): the FreeBSD ABI note / `EI_OSABI` so the
      kernel runs the static executable.
      Done Nov 28, on FreeBSD 15.1-RELEASE kernels under QEMU (amd64 with
      WHPX, arm64 with TCG; the official bootonly images): the
      `FID_IMAGE_ONLY=1` hello of `x86_64-freebsd` and of `aarch64-freebsd`
      is taken as a FreeBSD binary (`brandelf`: brand 'FreeBSD' (9);
      `elfdump -n`: FreeBSD 1400097 NT_FREEBSD_ABI_TAG; `file`: "for
      FreeBSD 14.0 (1400097)"); execve succeeds and the program runs under
      the FreeBSD ABI: ktrace shows its first call, Linux mmap, taken as
      FreeBSD's call of that number (link on amd64, semop on arm64), then
      SIGSEGV. An OpenBSD-branded image on the same kernel: "Exec format
      error". Printing waits on the FreeBSD runtime (XT43).
      Nov 27: compiler/nat_enc.fd brands `x86_64-freebsd` and
      `aarch64-freebsd` images: EI_OSABI 9, a PT_NOTE inside the first
      PT_LOAD holding the "FreeBSD" note (type 1, NT_FREEBSD_ABI_TAG, desc
      1400097), and a .note.tag SHT_NOTE section at it; Linux images stay
      System V with no note. tests/nattools/xt_elf_notes_check.sh all
      passed with the fid of the Nov 27 evening tree, and llvm-readelf
      reads "UNIX - FreeBSD" and the note. Left: no FreeBSD here, so no
      FreeBSD kernel has run one; there is no FreeBSD runtime yet (XT43),
      so the image is written only with FID_IMAGE_ONLY=1, over the Linux
      runtime.
- [ ] XT45. Files, directories, processes, environment, time (M) on the
      FreeBSD runtime.
- [ ] XT46. Threads through `thr_new` and futex-like waits through
      `_umtx_op` (M).
- [ ] XT47. Sockets and kqueue for async IO (M).
- [ ] XT48. Signals, stack overflow and panic traces (S-M).
- [ ] XT49. ARM64 FreeBSD (M): the same runtime for `aarch64-freebsd`.
- [~] XT50. `--target x86_64-freebsd` on the no-toolchain path (S): no
      `zig cc` needed.
      Nov 27: `emit --target x86_64-freebsd` (and `aarch64-freebsd`) goes
      to the machine layer, not to C; with no zig on PATH it is refused
      naming XT43 (no FreeBSD runtime yet) and pointing to `ccompile` and
      FID_IMAGE_ONLY=1, which writes the branded image with no zig
      (xt_elf_notes_check.sh). Left: a program that runs, which needs the
      FreeBSD runtime (XT43).
      Nov 28: the C route ran on FreeBSD 15.1: `ccompile --target
      x86_64-freebsd` of hello, arithmetic, strings, test_vec_builtins,
      test_struct_eq, test_vec_f64 and test_bg129_match_patterns, run on
      the amd64 kernel: rc 0 and stdout byte for byte `fid run`'s; hello
      through `--target aarch64-freebsd` on the arm64 kernel the same. The
      machine-layer image execs there (XT44) and faults at its first
      system call, as expected without XT43.
- [ ] XT51. CI in a FreeBSD VM (M): the suite under a FreeBSD image
      (qemu or a hosted runner).
- [ ] XT52. Tier 1 (S): platforms.md, cross-compilation.md updated.

## E. OpenBSD and NetBSD (XT53-XT63)

- [ ] XT53. Dynamic libc on OpenBSD (L): OpenBSD only accepts system
      calls from libc (pinsyscalls), so the runtime calls libc functions
      through a dynamically linked `libc.so`: an ELF with PT_INTERP,
      DT_NEEDED and the relocations (the CI2 dynamic-link path).
- [~] XT54. OpenBSD ELF notes (S): the OpenBSD identification note and
      the `PT_OPENBSD_*` headers the kernel requires (W^X, randomize).
      Nov 27: `x86_64-openbsd` / `aarch64-openbsd` images have the
      "OpenBSD" note (type 1, desc 0, .note.openbsd.ident) and
      PT_OPENBSD_NOBTCFI, and PT_OPENBSD_WXNEEDED only when the program
      needs writable code (xt_elf_notes_check.sh all passed with the fid
      of the Nov 27 evening tree; llvm-readelf reads the note). Left: no
      OpenBSD kernel has loaded one, and only FID_IMAGE_ONLY=1 writes it,
      since OpenBSD takes system calls only from libc (XT53).
      Nov 28, on OpenBSD 8.0 (GENERIC#82 installed with the autoinstaller,
      and the RAMDISK_CD miniroot; QEMU, WHPX): every image is REFUSED at
      exec with EINVAL ("Invalid argument"; ktrace: exec failed). The note
      works: with the PT_NOTE removed the kernel says ENOEXEC instead, as
      it does for a FreeBSD image. The cause is the missing PT_PHDR: an
      ET_DYN without one is refused (exec_elf.c, "need phdr and load
      sections for PIE"). A copy with PT_PHDR patched in (over GNU_STACK)
      execs and is killed by pinsyscalls on its first call ("pinsyscalls
      addr ... code 9 ... error 78", SIGABRT): that part is XT53. Also
      found: the writable-code build (FID_NAT_OFF=wxsplit: one RWX LOAD
      with PT_OPENBSD_WXNEEDED), patched the same way, execs on a
      wxallowed mount and faults at its entry point before any call
      (SEGV_ACCERR): the kernel maps a W|X segment without X, so a static
      image with an RWX LOAD can never run there, WXNEEDED or not.
      Also found: `ccompile --target x86_64-openbsd` linked -lws2_32
      (main.fd's C link had no OpenBSD branch: the Windows flags) and
      failed "unable to find dynamic system library 'ws2_32'"; so did
      `ccompile --target aarch64-android`.
      Fixed Nov 28: nat_enc.fd nat_elf_img writes PT_PHDR (first, R, over
      the program headers) in every OpenBSD image; xt_elf_notes_check.sh
      checks it on both architectures and the RWX build, and that Linux
      images still have none (all passed with a fid built from the
      fixed tree). On OpenBSD 8.0 the new image now execs and is stopped
      by pinsyscalls at its first call (SIGABRT, rc 134): that is XT53.
      main.fd: an OpenBSD branch for the C link (-lm -lpthread, zig's
      -lunwind) and the ELF -z options; Android through zig is refused
      with a message (zig ships no bionic: `emit --target`, or CC = the
      NDK's clang). codegen_c.fd: no `<alloca.h>` on OpenBSD (alloca is in
      stdlib.h there). With these the C route works on OpenBSD 8.0: hello
      built with `zig cc -target x86_64-openbsd` (alloca.h patched out by
      hand, as codegen_c.fd now writes it) prints `fid run`'s output byte
      for byte, rc 0, on / and on /usr, ld.so binding libc.so.104.0 and
      libpthread.so.28.1. Left: the static image needs libc's pinned
      system calls (XT53).
- [ ] XT55. An OpenBSD runtime file over libc (M): files, processes,
      time, environment.
- [ ] XT56. Threads and waits on OpenBSD (M): `__tfork` / pthread from
      libc, `futex`.
- [ ] XT57. `pledge` and `unveil` from capabilities (M): the program's
      effects (EV17) become a `pledge` promise string at start; a test that
      a forbidden call is killed.
- [ ] XT58. Sockets and kqueue on OpenBSD (M).
- [ ] XT59. A NetBSD runtime file (M): NetBSD system calls (direct calls
      are allowed), its ELF note.
- [ ] XT60. Threads on NetBSD (M): `_lwp_create`, `_lwp_park`.
- [ ] XT61. Sockets and kqueue on NetBSD (S-M).
- [ ] XT62. ARM64 for both (M): `aarch64-openbsd`, `aarch64-netbsd`.
- [ ] XT63. CI VMs for both, then tier 2 (M): the suite in qemu images;
      tier 2 in platforms.md (tier 1 needs blocking CI).

## F. Android (XT64-XT76)

- [~] XT64. An Android target (S): `--target aarch64-android` with the
      API level as an option (`-android34`), PIE by default.
      Nov 27: `aarch64-android` (API level 34 unless the target names
      one: `aarch64-android29`, the NDK's `aarch64-linux-android31`;
      `android-arm64` too) builds a static PIE (ET_DYN, no PT_INTERP)
      through the machine layer with the Linux runtime and an "Android"
      note (.note.android.ident, type 1) carrying the API level; an API
      level below the minimum and a build that is not position-independent
      are refused (xt_elf_notes_check.sh all passed with the fid of the
      Nov 27 evening tree; `x86_64-android` builds as well). Left: run on
      a device or emulator; dynamic bionic is XT65.
      Nov 28, run: on the Android emulator (API 35 x86_64 system image,
      WHPX), 140 suite tests (every 10th runnable one of tests/bootstrap)
      built with `emit --target x86_64-android`: 134 built and ran, 126
      match `fid run`, and all 134 print the same output as the same test
      built for x86_64-linux and run in WSL (the other 8: `fid run` timed
      out on 6, and printed differently on test_lib_fs_view and
      test_stdlib_mysql_db). The 6 that did not build: 4 fail on every
      target (compile errors), and 2 call C functions, which the machine
      layer links for glibc only (XT65). The same 140 built for
      `aarch64-android` and run under qemu-aarch64 (WSL Ubuntu 24.04):
      134 of 134 match the emulator's output and exit code. Left:
      aarch64-android on Android itself (the emulator has no ARM images
      on an x86-64 host; a device, or an ARM64 host's emulator).
- [ ] XT65. Dynamic linking against bionic (L): DT_NEEDED libc.so,
      libm.so, libdl.so, liblog.so; the Android linker's requirements
      (no text relocations, `DT_FLAGS_1` PIE, packed relocations optional).
- [ ] XT66. An Android runtime file (M): files, time, threads through
      bionic's pthread, sockets, `__android_log_write` for output.
- [ ] XT67. TLS on Android (M): bionic's TLS layout (the `TPIDR_EL0`
      slots bionic reserves).
- [ ] XT68. Command-line executables (S): run with `adb shell` on an
      emulator; the suite through adb.
- [ ] XT69. Shared libraries (M): `fid build --shared` writes a `.so`
      with exported C functions (needed for apps).
- [ ] XT70. JNI entry points (M): `@jni` (or `extern "jni"`) exports
      `Java_<pkg>_<cls>_<method>` with JNIEnv handling; stdlib/jni.fd
      bindings checked.
- [ ] XT71. NativeActivity apps (L): `android_main` through
      stdlib/android_ndk.fd, input and a surface, no Java written by the
      user.
- [ ] XT72. APK packaging without the SDK build tools (L): `fid apk`
      writes the manifest (binary XML), the zip, aligns it and signs it
      with APK Signature Scheme v2.
- [ ] XT73. x86-64 Android for the emulator (M): `x86_64-android`.
- [ ] XT74. Debugging on Android (M): DWARF that `lldb` through
      `adb` reads.
- [ ] XT75. CI with the Android emulator (M): an emulator in CI runs the
      suite through adb.
- [ ] XT76. Tier 2 (S): platforms.md (PL6) updated.

## G. iOS (XT77-XT88)

- [~] XT77. An iOS target (S): `--target aarch64-ios` and
      `aarch64-ios-simulator` (a different platform id in the Mach-O load
      commands).
      Nov 27: both go to the machine layer's Mach-O writer with the darwin
      runtime; LC_BUILD_VERSION names PLATFORM_IOS (2) or
      PLATFORM_IOSSIMULATOR (7), minos 15.0 unless the target names one
      (`aarch64-ios17.2`), sdk 17.0 (compiler/nat_enc.fd
      nat_macho_set_platform). The device image is the aarch64-macos image
      but for LC_BUILD_VERSION and the signature; aarch64-macos still says
      macOS 11.0; emit-ir keeps the platform; a non-aarch64 device target,
      a bad version and `ccompile --target aarch64-ios` are refused
      (tests/nattools/xt_ios_target_check.sh ok, llvm-objdump reads
      "platform ios"; universal_check ok with the same fid). Left: no Mac
      or iPhone here, so nothing has run; signing and dyld for iOS are
      XT78.
- [ ] XT78. Mach-O for iOS (M): `LC_BUILD_VERSION` with platform iOS,
      minimum version, no `LC_MAIN` restrictions; dyld binds to libSystem.
- [ ] XT79. An iOS runtime (M): darwin.fd over libSystem calls only
      (iOS forbids raw system calls in App Store apps), the sandbox's paths.
- [ ] XT80. Running on the simulator (M): `xcrun simctl spawn` runs a
      command-line test binary; the suite on the simulator in CI.
- [ ] XT81. Code signing for devices (L): a real signature (not ad-hoc)
      with a certificate and an entitlements blob, written by `fid`
      (CodeDirectory with SHA-256, the CMS signature); checked with
      `codesign --verify`.
- [ ] XT82. App bundles (M): `fid app` writes `.app` with Info.plist,
      the executable and resources; `.ipa` packaging for distribution.
- [ ] XT83. Calling UIKit (L): stdlib/objc.fd's message sending for an
      app delegate and a window, with a minimal example app.
- [ ] XT84. Static libraries and frameworks for Xcode projects (M): a
      `.a` or `.xcframework` exporting C functions, so Swift apps call
      Fides.
- [ ] XT85. Threads, TLS and GCD (M): pthread from libSystem;
      `dispatch_async` bindings.
- [ ] XT86. Debug info for lldb on iOS (M).
- [ ] XT87. CI on a macOS runner with the simulator (S).
- [ ] XT88. Tier 2 (S): platforms.md (PL6) updated; App Store rules
      documented (no JIT, no raw system calls).

## H. UEFI and freestanding for ARM64 and RISC-V (XT89-XT100)

- [ ] XT89. UEFI images for ARM64 (M): PE32+ with machine 0xAA64,
      subsystem EFI_APPLICATION, from the machine layer (x86-64 UEFI goes
      through pe_ir's `pe_ir_generate_uefi`).
- [ ] XT90. UEFI images for RISC-V 64 (M): machine 0x5064.
- [ ] XT91. A UEFI runtime for both (M): console output, memory through
      boot services, `ExitBootServices`; the same API as x86-64 UEFI.
- [ ] XT92. UEFI tests under qemu (S-M): tests/uefi/run.sh with
      `qemu-system-aarch64` + AAVMF and `qemu-system-riscv64` + EDK2.
- [ ] XT93. Freestanding ARM64 (`--bare --target aarch64-none`) (M):
      a flat or ELF image with a start routine, no OS, a linker-script-free
      memory layout option.
- [ ] XT94. Freestanding RISC-V 64 (M): `riscv64-none` with an SBI
      console.
- [ ] XT95. Exception vectors (M): ARM64 vector table and RISC-V trap
      handler through `@interrupt`, as x86's IDT setup.
- [ ] XT96. MMU setup helpers (M): page tables for ARM64 and Sv39/Sv48
      for RISC-V, as the x86 paging tests (test_bare_paging).
- [ ] XT97. Device access (S-M): MMIO with volatile loads and stores,
      UART drivers for PL011 and 16550 (stdlib/uart.fd).
- [ ] XT98. Raspberry Pi 4/5 (ARM64) bare-metal example (M): boots on
      real hardware and under qemu's raspi machine.
- [ ] XT99. A RISC-V board example (M): VisionFive 2 or qemu virt.
- [ ] XT100. CI for both (S): bare-metal.yml boots the ARM64 and
      RISC-V tests under qemu, as it does for x86.

## Order

1. A and C first: macOS and the browser/WASI cover the most users.
2. B and D: Windows ARM64 and FreeBSD complete Zig's first tier.
3. H: small, reuses what the x86 bare-metal path has.
4. F and G: mobile is the largest effort (packaging, signing, app models).
5. E: OpenBSD/NetBSD last (smallest user base; OpenBSD needs the
   dynamic-libc path first).

Rough total: A ~3-4 weeks, B ~5-6 weeks, C ~6-8 weeks, D ~2-3 weeks,
E ~4-5 weeks, F ~6-8 weeks, G ~6-8 weeks, H ~4 weeks, about 9-12 months
of serial work. All of it is after the 1.0 language freeze; XT12 (macOS
tier 1) is the one worth having before 1.0.
