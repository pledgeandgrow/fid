# `fid lint` on the standard library (release-roadmap N5)

`scripts/lint-check.sh` lints every `stdlib/*.fd` module and compares the
result with `scripts/lint-check.baseline` (one `module|state|count` line per
module: `clean`, `warnings` with their count, or `unchecked` when the module
does not pass `fid check`). CI runs it (`.github/workflows/ci.yml`, "lint on
every stdlib module"). `fid lint` resolves `use` imports
(`compiler/compiler.fd` `lint_with_imports`), so a module that needs another
module is linted like any other; `tests/tooling/lint-imports-check.sh` checks
that.

## State on Nov 17

Linted with the Nov 16 compiler (the stage-0 of the day's build), after the
fixes below, on the 163 modules outside the network/crypto set and the 200
modules another session was adding that day: 128 clean, 34 with warnings,
1 unchecked. What is left is not wrong in the modules:

| Kind | Modules | Why it stays |
|------|---------|--------------|
| `unused import` on a module that is used | 32 warnings (string, math, money, calendar, arena, encoding, postgres, datetime, unicode_data, db, ...) | The lint counts an import as used only when a called function starts with `<module>_`. `str_join` (string), `pledge_sqrt`/`vec3` (math), `Money_new`, `cal_*`, `frame_*`, `pg_*` do not. Needs the checker to ask which module registered the called name (`TypeChecker_get_item_module`). `use db` is used for its `cap Database` alone: traits are not registered with a module. |
| `unused function` on a function used as a value or in an initializer | shape `vec_lit` (const initializers), trace `trace_state_new` (a global's initializer), xml `x_no_resolver` (`resolver: x_no_resolver`) | The lint collects calls in function bodies only. |
| Naming | math `vec2 vec3 vec4 mat4 quat` (structs), sdl2_h's C constants (`SDLK_a`, `SDL_ScaleModeNearest`, ...), shape's `shape_2x3` constants | Public names used across the stdlib and its tests (`vec3` by most game modules); sdl2_h mirrors the C header. |
| unchecked | scada | `fid check` rejects `modbus_crc16`: the safety-annotation check resolves `data.len()` on a `Vec<i64>` to `StringBuilder_len` (core's StringBuilder now has a `len` method). A checker bug, not the module's. |

## What was fixed

- Imports nothing used: `use string` in 42 modules, `use math` in plot and
  units, `use tensor` in kv_cache, dp, shard and quant, `use encoding` in
  unicode; a repeated `use` line in audio_mixer.
- Import order (core, vec, string, then by name) in 15 modules.
- Unused locals (bigint, camera, clinical, collections, fs, math slerp, skel,
  tz) and dead private helpers (json `j_lit`/`j_scan_*` wrappers, regex's
  token accessors, log `log_safe_byte`, semver `sv_full`, virtio `vu64`);
  acpi `sdt_sig_str` is public now.
- Parameters a stub or a fixed answer does not read are named `_x` (atomic's
  `Order`, arena, async, ecs, events, game, world's refusing handlers, ...).
- Unused parameters that were bugs:
  - scene: `scene_update_entity`, `scene_set_velocity`, `scene_damage`,
    `scene_set_tag` and `scene_despawn` changed a copy of the entity; they
    store it back now (`tests/bootstrap/test_scene_entity_updates.fd`).
  - sprite: `sprite_controller_update` dropped the advanced animation and
    `sprite_controller_play` did not start the track; streaming:
    `streaming_set_chunk_data` wrote nothing
    (`tests/bootstrap/test_sprite_stream_updates.fd`).
  - gl: `gl_compile_shader_src` passed null to glShaderSource and to
    glGetShaderiv/glGetProgramiv (no source compiled; the status read
    crashed or was always 0); it passes the source pointer array and reads
    the status from a block now.
- terrain's `hL`/`hR`/`hD`/`hU` are `h_l`/`h_r`/`h_d`/`h_u`.

`scripts/lint-check.baseline` records these 163 modules, core and the 37
network/crypto modules (201 lines). The 200 modules the other session was
still writing are not in it: the gate requires a module missing from the
baseline to be clean, so record them (`--save`) once that work is done.
On Nov 21 the baseline still has those 201 lines; `stdlib/` has 403
modules.

## Running it

```bash
VERBOSE=1 bash scripts/lint-check.sh release/fid.exe   # every warning
bash scripts/lint-check.sh --save release/fid.exe      # record the state
```
