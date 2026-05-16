# `--test=vfx` Specification

The VFX restoration test harness. Boots a minimal scene that displays exactly
one effect at a time, with keyboard navigation through the full catalogue.

This is the gate: an effect is not `validated` until it renders correctly in
this harness. See [AGENT_GUIDE.md §4](AGENT_GUIDE.md).

## 1. Invocation

```sh
./build/revenant --test=vfx
```

Optional arguments (future):

```sh
./build/revenant --test=vfx:flame              # jump straight to the named effect
./build/revenant --test=vfx --vfx-loop=3.0     # auto-restart finite effects every N seconds
```

These are nice-to-haves. The base form must work first.

## 2. UI layout

```
+----------------------------------------------------------------+
| [ Effect Browser ] (ImGui window, top-left)                    |
|   < Prev   [ TFlameEffect           ▼ ]   Next >               |
|   Family: fire   Pipeline: FB   Status: debug-solid            |
|   Debug mode: [ Normal / Solid / FullTexture / CurrentFrame ]  |
|   [ Restart ] [ Pause ] [ Step ]                               |
|                                                                |
| [ Scene ] (full background)                                    |
|   - neutral ground plane (~10x10 tiles)                        |
|   - one effect anchor in the center                            |
|   - simple fixed-perspective camera                            |
|   - deferred-lit, world-space (per project_world_space)        |
+----------------------------------------------------------------+
```

### 2.1 Effect Browser panel

- **Combo box** lists every effect registered in the test catalogue.
- **Prev / Next** buttons match the Left/Right arrow keys.
- **Family / Pipeline / Status** are read from the same metadata that powers [INVENTORY.md](INVENTORY.md) — see §3 below.
- **Debug mode** cycles the [PARTICLE_EFFECTS.md §3.2](../PARTICLE_EFFECTS.md) diagnostic ladder. The radio selection drives `TEffect::SetDebugMode(...)`.
- **Restart** kills and re-spawns the effect. Necessary for finite effects (a blood splat finishes in <2s).
- **Pause / Step** freeze the effect-system update for visual inspection.

### 2.2 Scene

- A flat 10×10 tile ground plane (any one of the bring-up tiles is fine).
- Camera fixed at a 3/4 angle, ~5 tiles away.
- A single deferred light so lit effects (auras, halos) show.
- The effect's anchor point is the world origin. Effects that need an "owner"
  (e.g. flame attached to a torch) get a stub `TObjectInstance` planted at the origin.

The scene is not the test. The effect is.

## 3. Adding an effect to the browser

The browser is **data-driven from a single registry** so adding a new effect
is one line, not a switch-case sprawl.

```cpp
// src/vfxtest.cpp
VfxTestRegistry::Register({
    .id          = "TFlameEffect",
    .family      = "fire",
    .pipeline    = "FB",
    .factory     = [] { return TFlameEffect::SpawnForTest(/* origin */); },
});
```

The `factory` returns a fully-constructed effect plus its stub owner. The
registry sorts by family then id for the combo box.

**Convention:** every effect class adds a `static SpawnForTest()` method that
constructs whatever owner / parameters it needs for a standalone preview.
Keeps the test wiring out of the effect's main code path.

## 4. Success criteria

`--test=vfx` is considered "done" (Phase 1 exit) when:

- [ ] Boots without crashing.
- [ ] Lists every effect in the registry (≥ the canonical first one, torch flame).
- [ ] Left/Right arrows cycle effects.
- [ ] Switching effect cleanly tears down the previous one (no leaked particles, lights, allocations).
- [ ] All four debug modes selectable per effect.
- [ ] Pause / Step / Restart all work.
- [ ] ASan-clean over a full cycle of every registered effect.

A single effect is `validated` (per [INVENTORY.md](INVENTORY.md) legend) when:

- [ ] It renders correctly in `--test=vfx` Normal mode.
- [ ] Side-by-side with retail (or with the user's judgment) matches.
- [ ] It survives all four debug modes without artifacts.
- [ ] No new build warnings or ASan reports introduced by its commit.

## 5. Integration with the `--test=` dispatch

Per the existing pattern ([src/testmodes.cpp:2208](../../src/testmodes.cpp)):

- Add `vfx` cases to the three dispatch tables: `Initialize`, `Close`, `Render`.
- Implementation lives in `src/vfxtest.cpp` (new file) and `src/vfxtest.h`.
- Input forwarding follows existing pattern ([src/testscreen.cpp:62](../../src/testscreen.cpp) → [src/testmodes.cpp:2346](../../src/testmodes.cpp)) — vfx mode handles `VK_LEFT`, `VK_RIGHT`, `VK_SPACE`, `VK_RETURN`, `R`.

## 6. Key bindings

| Key             | Action                                       |
|-----------------|----------------------------------------------|
| `Left arrow`    | Previous effect                              |
| `Right arrow`   | Next effect                                  |
| `R`             | Restart current effect                       |
| `Space`         | Pause / unpause effect-system update         |
| `.`             | Single-step (one tick, while paused)         |
| `D`             | Cycle debug mode (Normal → Solid → FullTex → CurrentFrame → Normal) |
| `1`–`4`         | Jump directly to debug mode                  |
| `Esc`           | Quit                                         |

Mouse: ImGui handles its own. Camera is fixed — no orbit / pan in v1
(adding orbit later is fine; not a v1 requirement).

## 7. Out of scope for the test mode

- No spawn-rate sliders, no live emitter parameter editing. That belongs in a
  later "effect editor" mode, not the verification harness.
- No in-game context (no character, no script, no combat). If an effect can
  only be reasoned about with a character attached, give it a stub.
- No retail-side rendering — the test harness only drives our engine. Retail
  comparison is done out-of-band (video / screenshot).
- No automated visual diff. Human verification per [AGENT_GUIDE.md §4.2](AGENT_GUIDE.md).
