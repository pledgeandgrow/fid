# Fides OS — General-Purpose Use: Programming, Gaming, Desktop

> Can the Stratified Kernel be used for everyday computing? Yes, but not on
> day one. Here's what works, what needs building, and the timeline.

## Programming / Development

### What Already Works

Fides already has most development tools built and working:

| Tool | Linux equivalent | Fides status |
|------|-----------------|-------------|
| Compiler | gcc, rustc | Built (fidesc) |
| Debugger | gdb, lldb | Built |
| Profiler | perf | Built |
| LSP server | clangd, rust-analyzer | Built |
| Formatter | rustfmt, gofmt | Built |
| Linter | clippy, eslint | Built |
| Package manager | apt, cargo | Built (ark) |
| Documentation | doxygen, rustdoc | Built (scribe) |
| Code coverage | gcov, llvm-cov | Built (veil) |
| Test runner | cargo test | Built (303 test files) |

### What Development Looks Like

```bash
$ fides shell                          # Fides shell
$ fides edit my_app.fd                 # Editor with LSP
$ fides run my_app.fd                  # Run in interpreter
$ fides debug my_app.fd                # Debug
$ fides profile my_app.fd              # Profile
$ fides build my_app.fd --target x86-64  # Native executable
$ fides build my_app.fd --target arm64   # Cross-compile
$ ark add fides-os/gui                 # Install package
$ fides test                           # Run tests
$ scribe my_app.fd --format html       # Generate docs
```

### What's Different from Linux Development

**You can only develop in Fides.** No gcc, no rustc, no Python. But this is
also an advantage:
- Compiler, debugger, profiler, LSP are all integrated — no glue
- Effect annotations catch bugs at compile time
- Comptime evaluation lets you test code at build time
- The same compiler builds your app and the OS kernel

### Porting Existing Languages

| Language | Approach | Effort |
|----------|----------|--------|
| C/C++ | LLVM Fides target | Medium |
| Python | Port CPython as stratum | Medium |
| Rust | LLVM Fides target | Hard |
| JavaScript | Port QuickJS/V8 | Medium |

Ported languages run in isolated compatibility strata with inferred
capabilities. They don't get effect verification or memory safety.

---

## Gaming

### What Games Need

| Requirement | Linux | Fides OS |
|-------------|-------|---------|
| Graphics | OpenGL, Vulkan (Mesa) | GPU stratum (SPIR-V/PTX codegen) |
| Audio | PulseAudio, PipeWire | Audio stratum (@realtime region) |
| Input | evdev, libinput | Input stratum (keyboard, mouse, gamepad) |
| Window system | X11, Wayland | Window manager stratum |
| Game engine | Unity, Godot, Unreal | Fides game stdlib (26 modules) |
| Networking | Berkeley sockets | Network stratum (@tainted) |

### How Games Work on Fides OS

```fides
@no-panic
fn game_main() -> i64 {
    region game_memory with budget(1073741824) {}  // 1GB pre-allocated
    region audio with budget(67108864) @realtime {} // 64MB, realtime
    region render with budget(536870912) @gpu {}    // 512MB GPU

    @realtime @deterministic
    fn game_loop() -> void {
        while running {
            val input = input_stratum.poll()      // Direct, no evdev
            game_state.update(input)               // Pure computation
            render_stratum.submit(game_state.frame())  // Direct GPU
            audio_stratum.play(game_state.audio())     // Direct audio
            @wait_vsync()
        }
    }
}
```

### Performance Comparison

| Metric | Linux + Vulkan | Fides OS |
|--------|---------------|---------|
| Frame submission | vkQueueSubmit -> ioctl (20-50 us) | Direct GPU (50-100 ns) |
| Input latency | evdev -> compositor -> app (5-15 ms) | Input stratum -> app (0.5-1 ms) |
| Audio latency | PulseAudio -> ALSA (10-30 ms) | Audio stratum -> HW (1-3 ms) |
| Memory allocation | malloc/free (frame drops) | Pre-allocated (zero) |
| Context switches | 100-1000/s | 0 (dedicated core) |
| Useful computation/frame | 12-14 ms (2-4 ms overhead) | 16 ms (0.6 ms overhead) |

**Result:** 10-20% more useful computation per frame. Higher FPS, better
graphics, or more complex game logic — same hardware.

### Game Stdlib Already Exists

26 game/media stdlib modules are built:
sdl, gl, image, font, mesh, gameloop, physics3d, skel, ui, net, camera,
particles, animation2d, audio_mixer, asset, serialize, collision_broad,
raycast, navmesh, terrain, profiler, replay, shader, editor, chunk, touch.

### Existing Games (Steam, etc.)

Existing Linux/Windows games can't run without a compatibility layer.
Options:
1. Port the game to Fides (impractical for most)
2. Port a game engine (Godot-on-Fides, medium effort)
3. Compatibility stratum with POSIX emulation (like WSL/Wine)

**Realistic timeline:**
- Year 1: Simple Fides-native games (2D, puzzle, retro)
- Year 2: Fides-native game engine (3D, physics, audio)
- Year 3: Ported game engines (Godot-on-Fides)
- Year 5: Compatibility stratum for existing games

---

## General-Purpose Desktop

### What a Desktop Needs

| Feature | Linux | Fides OS |
|--------|-------|---------|
| Window manager | Mutter, KWin | WM stratum |
| File manager | Nautilus | Fides file manager |
| Web browser | Firefox, Chrome | Needs porting |
| Email client | Thunderbird | Needs writing |
| Media player | VLC | Needs writing |
| Terminal | gnome-terminal | Fides terminal |
| Text editor | vim, VS Code | Fides editor + LSP |
| Office suite | LibreOffice | Needs porting |

### The Honest Problem: Application Ecosystem

The Stratified Kernel architecture is ready. The application ecosystem is
not. A desktop OS is only useful if people can do their daily work on it.
Linux took 20 years. Fides OS starts from zero.

### What Already Works

- Programming (compiler, debugger, profiler, LSP, formatter, linter)
- Package management (ark)
- Documentation (scribe)
- Code coverage (veil)
- Testing (303 test files, 3 backends)
- Gaming (26 game stdlib modules)
- Shell, file operations, networking, OS primitives

### What Needs to Be Built

- Web browser (HTML/CSS/JS engine — massive undertaking)
- Office suite (word processor, spreadsheet, presentation)
- Image editor, video editor
- Email client, chat applications
- Database tools

### The Strategy: Don't Build Everything

| Phase | Timeline | What works |
|-------|----------|-----------|
| Phase 1 | Now | Development, testing, fixed-purpose systems |
| Phase 2 | 1-2 years | Developer workstation, gaming (Fides-native), AI training |
| Phase 3 | 2-3 years | Gaming (ported engines), media playback |
| Phase 4 | 3-5 years | General desktop, compatibility stratum |

### The Key Insight

You don't need to beat Linux everywhere. You need to be the best OS for
specific use cases:

| Use case | Why Fides wins | Timeline |
|----------|--------------|----------|
| Banking terminal | Zero attack surface | Now |
| Crypto mining | 15-40% faster hashrate | Now |
| AI training | 30-40% faster training | 1-2 years |
| Pacemaker firmware | @medical + formal verification | 1-2 years |
| Industrial control | @realtime + capability isolation | 1-2 years |
| Developer workstation | Integrated tooling, effect safety | 1-2 years |
| Gaming rig | Lower latency, more FPS | 2-3 years |
| General desktop | Security + performance | 3-5 years |

The Lego concept means you ship the first use case now, and add strata over
time. Each new stratum adds a new use case. You don't wait for "everything
works" before shipping something useful.

## Honest Limitations for General-Purpose Use

1. **No web browser (yet):** A modern browser is 25-30M lines. Minimal
   browser in ~6 months, full browser with JS JIT in 2-3 years.

2. **No existing app ecosystem:** No Photoshop, Excel, Discord, Spotify.
   Everything needs to be written in Fides or ported.

3. **No existing driver ecosystem:** No NVIDIA driver, no Realtek, no USB
   camera. Start with narrow hardware (QEMU, specific SBCs/GPUs).

4. **Single-language development:** All strata must be Fides for effect
   verification. C via FFI is an escape hatch.

5. **Learning curve:** Developers need to learn effects, capabilities,
   regions, comptime. LSP helps, but it's a new mental model.

6. **The compatibility paradox:** Adding a Linux compatibility stratum
   reintroduces attack surface. The resolution: it's isolated and
   capability-scoped, so it's more secure than Linux but not as secure as
   Fides-native.
