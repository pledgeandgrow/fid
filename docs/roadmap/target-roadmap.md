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
- [ ] XT22. `--target aarch64-windows` on the no-toolchain path (S):
      main.fd sends it to the machine layer, not to C; cross-compilation.md
      updated.
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

- [ ] XT28. The last 28 suite tests on `fid wasm` (M):
      scripts/wasm-test.sh to 344 of 344; each failure fixed or given a
      reason in the test's backends line.
- [ ] XT29. Green tasks that interleave on WASM (L): today tasks run to
      their end when entered (docs/reference/interp-tasks.md); stack
      switching through asyncify-style transformation or the stack-switching
      proposal, so channels between tasks work.
- [ ] XT30. Exceptions and panics with traces on WASM (M): a panic
      prints the Fides function names, not a wasm trap number.
- [ ] XT31. Memory growth and the allocator (M): `memory.grow` on
      demand, the allocator freeing back, a test allocating past 4 GB on
      wasm64 later.
- [ ] XT32. WASI preview 1 coverage (M): every file, clock, random,
      args and environment function the stdlib uses, run under wasmtime and
      Node.
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
- [ ] XT39. Size (S-M): dead code removed (reach.fd), a hello world
      under 20 KB, measured in CI.
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
- [ ] XT44. ELF branding (S): the FreeBSD ABI note / `EI_OSABI` so the
      kernel runs the static executable.
- [ ] XT45. Files, directories, processes, environment, time (M) on the
      FreeBSD runtime.
- [ ] XT46. Threads through `thr_new` and futex-like waits through
      `_umtx_op` (M).
- [ ] XT47. Sockets and kqueue for async IO (M).
- [ ] XT48. Signals, stack overflow and panic traces (S-M).
- [ ] XT49. ARM64 FreeBSD (M): the same runtime for `aarch64-freebsd`.
- [ ] XT50. `--target x86_64-freebsd` on the no-toolchain path (S): no
      `zig cc` needed.
- [ ] XT51. CI in a FreeBSD VM (M): the suite under a FreeBSD image
      (qemu or a hosted runner).
- [ ] XT52. Tier 1 (S): platforms.md, cross-compilation.md updated.

## E. OpenBSD and NetBSD (XT53-XT63)

- [ ] XT53. Dynamic libc on OpenBSD (L): OpenBSD only accepts system
      calls from libc (pinsyscalls), so the runtime calls libc functions
      through a dynamically linked `libc.so`: an ELF with PT_INTERP,
      DT_NEEDED and the relocations (the CI2 dynamic-link path).
- [ ] XT54. OpenBSD ELF notes (S): the OpenBSD identification note and
      the `PT_OPENBSD_*` headers the kernel requires (W^X, randomize).
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

- [ ] XT64. An Android target (S): `--target aarch64-android` with the
      API level as an option (`-android34`), PIE by default.
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

- [ ] XT77. An iOS target (S): `--target aarch64-ios` and
      `aarch64-ios-simulator` (a different platform id in the Mach-O load
      commands).
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
