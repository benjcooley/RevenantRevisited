# Retail VFX source integration

This branch restores retail effect controllers, authored geometry/UV/material
submission and runtime ownership, and provides a fast x86 retail comparison
toolchain. It is a working VFX milestone; the catalogue's map/character/device
acceptance gates are tracked separately in `EFFECT_BURNDOWN.json`.

Repository scope:

- Game sources, shader headers, runtime integration and native regression tests.
- Current thin-emulator, private-build/assembly generators and VFX probes.
- Developer setup, complete 176-row checklist, source audits and a compact
  hashed evidence index.

Generated executables, assembly listings, captures and large per-frame reports
remain local and ignored. The old Win98 guest-helper/vendor bundle stays in the
lab. Gameflow/retail-trace work is integrated from GitHub main.

Validation is performed from an isolated checkout incorporating GitHub main
`b2d5891`, using a fresh CMake build directory. Emulator tests use locally linked
baseline/experiment/evidence inputs; see `tools/retail_runtime/SETUP.md` for
dependencies and reference prerequisites. Those ignored inputs are not required
to compile the game, and are not part of the source commits.

Review corrections include named-event final-handle lifetime, reliable warm
instruction patch/restore, and once-per-gather late effect replacement dispatch.
The final validation record is retained in the local lab's
`research/merge-preparation/` directory. Per-item shared scratch-buffer capacity admission preserves earlier valid
submissions; focused packing regressions cover overflow and recovery.

The original shared worktree is preserved. Its existing icon-export commit is
already included in GitHub main and is not part of the VFX source diff.

Integration preserves main's gameflow commands, safe map-window ownership,
shared Classic lighting, translucent mesh/depth path and startup flow. Audited
VFX geometry, UV scrolling and prelit retail colour modes extend those shared
paths. Replacement visual simulators own their verified lifetimes; generic
story effects retain main's animation progression and normal cleanup.
Capture tools explicitly select engine assets from `assets/`, matching the
merged game's asset lookup independently of licensed retail data.

Completed validation before integration:

- Fresh game build and native particle, VFS, mesh-lighting, animation, transform,
  parser, DEF document, render-metadata and asset-cache test executables pass.
- 69 retail-runtime tests pass with the local reference inputs attached.
- Synthetic assembly roundtrip passes without a retail input executable.
- A clean-build 145-frame Cure capture completes and exits normally.
- Whitespace checks pass with legacy CRLF source endings respected.

Source-specific rendering regressions cover dispatch and scratch packing;
full natural combat/map and all VFX acceptance are not claimed by these checks.

Final renderer review validation:

- Affected game targets rebuilt successfully after both rendering corrections.
- Late-submission and mixed FX-capacity tests pass and reject their pre-fix logic.
- A post-review clean-build Cure smoke capture completes and exits normally.

Commit organization is game code and native regressions, current developer
emulation tools, then the acceptance checklist/audits/handoff documentation.

Validation after integrating GitHub main:

- Combined game build and all 12 native test executables pass, including main's
  Classic lighting, player stats and audio decoding (with shipped audio inputs).
- 72 retail-runtime tests, a compiled regression covering 27 effect-lifecycle
  scenarios, and the synthetic assembly roundtrip pass.
- The 18-frame Flame frontend/original-raster comparison passes with current
  engine assets; this remains a bounded fixture rather than full-device proof.
- A 145-frame native Cure capture produces visible animation and exits with
  code 0 without forced termination. Quickstart reaches the Keep and spawns
  the opening vortex; the full story chain is outside this merge smoke test.
- Capture success now requires a clean process exit; three subprocess checks
  reject failures even when output frames exist. The runtime ceiling uses
  steady wall time independently of fixed-step simulation.
- The VFX diff against main contains no generated binaries, assembly listings,
  capture images or local report files, and passes whitespace checks.
