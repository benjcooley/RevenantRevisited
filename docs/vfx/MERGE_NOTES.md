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
lab. Gameflow/retail-trace work is owned by its separate branch.

Validation is performed from an isolated checkout against local main
`d876a3f`, using a fresh CMake build directory. Emulator tests use locally linked
baseline/experiment/evidence inputs; see `tools/retail_runtime/SETUP.md` for
dependencies and reference prerequisites. Those ignored inputs are not required
to compile the game, and are not part of the source commits.

Review corrections include named-event final-handle lifetime, reliable warm
instruction patch/restore, and once-per-gather late effect replacement dispatch.
The final validation record is retained in the local lab's
`research/merge-preparation/` directory. Per-item shared scratch-buffer capacity admission preserves earlier valid
submissions; focused packing regressions cover overflow and recovery.

The original shared worktree is preserved. Its existing icon-export commit is
already on local main and is not part of this branch's diff against main. Remote
main currently precedes that commit; integrate local main's existing work before
opening a remote PR if a VFX-only remote diff is desired.

Completed validation:

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
