# VFX Wave-1 Lessons (2026-06-06)

Compiled from the four parallel sonnet ports — **B01 Blood / M04 Burn / I22 Iced
/ F-FIREFLASH** — and the round-1/2/3 bug-fix passes. Read this before
dispatching wave 2.

---

## 1. Cross-cutting gotchas (every port will hit these)

### 1.1 Material `emissive=(1,1,1)` saturates WorldMesh ports white

Our `kHelperMeshFs` shader (`src/renderer.cpp:~962`) applies emissive
**additively**: `col = tex.rgb * diffuse * lit + emissive.rgb`. Many original
I3Ds were authored with `D3DMATERIAL.Emissive = (1,1,1)` as a default that the
1998 D3D pipeline ignored. With our shader, that default = +1 on every channel
= white saturation regardless of texture.

**Hit by**: Iced, FireFlash. **Will hit**: every WorldMesh port from a D3D
asset.

**Port-side workaround** (current discipline): after `d3d::LoadMaterial(...)`
in `Spawn()`, zero `st->emissive[i][0..2]` for every sub-object you draw.

**Open**: long-term fix is shader semantics — decide whether to honor authored
emissive (only valid if assets explicitly use it) or treat it as ignored.

### 1.2 `DoLighting()` returns 0 in the test rig

No level loaded → `AmbientLight` global stays 0; `ClearPointLights()` runs at
setup. Any Lit effect that multiplies texture by computed brightness will show
as 0 → invisible on every background.

**Hit by**: Blood. **Will hit**: every Lit effect (most damage/spell visuals).

**Port-side workaround**: brightness floor — `max(DoLighting(...), 1.0f)`.
In-game `GetAmbientLight()` is always ≥ 10 so the floor never triggers in
the real game path.

**Open**: rig-level fix would be to seed `AmbientLight` to a reasonable
default in vfx test mode.

### 1.2a Effects must declare scene-lit vs self-lit (lighting tag)

Every effect needs an explicit lighting category at port time:

| Category | What it means | Render path |
|---|---|---|
| **Scene-lit** | Effect is a physical object lit by the world (blood, ice, status overlays). Brightness = `DoLighting(closest lights + ambient)`. | Unlit shader + CPU-side brightness via `obj.diffuse[0..2]`. **No emissive.** |
| **Self-lit (light emitter)** | Effect IS the light source (fire, sparkles, glowing magic). Brightness is authored by the asset. | Unlit shader + Additive blend. **Zero emissive too** (the additive blend IS the self-emission). |

> Quote from the original developer: *"They will look weird if they include
> their own internal lights."* — scene-lit effects with non-zero emissive
> fight the scene lighting and look pasted-on.

**Practical rules for the port**:
- Zero `st->emissive[i][0..2]` after `LoadMaterial` regardless of category
  (per §1.1 — emissive is never authored intentionally in 1998 D3D assets).
- Set the lighting tag in the forensics doc + restate it in the port's
  top-banner comment ("Scene-lit (DoLighting)" or "Self-lit (additive)").
- Scene-lit ports must compute DoLighting CPU-side and pass through diffuse
  (canonical example: `src/effects/blood.cpp`).
- Self-lit ports skip DoLighting entirely; the additive blend handles
  brightness (canonical examples: `burn.cpp`, `fireflash.cpp`).

### 1.3 ~1.5 Y scale for 3D objects → world space

The original engine multiplied 3D mesh Y by ~1.5 when transforming into game
world space — to reconcile tile-space vs 3D-render-space coordinate systems.
WorldMesh-rendered effects without this look vertically squashed.

**Hit by**: Iced (suspected), FireFlash (suspected). **Will hit**: every
WorldMesh / OBJ3D_MATRIX port.

**Status**: needs A/B with retail to pin the exact value + axis. Not yet
applied anywhere. See `project_3d_world_y_scale.md` in auto-memory.

### 1.4 Framerate-independent tick is non-negotiable

Don't hardcode `1000.0 / 60.0` or `1000.0 / 24.0` ms per Tick call. At 120fps
(common on Mac) you run 2× too fast.

**Hit by**: Iced. **Will hit**: every port that doesn't follow the pattern.

**Pattern**: accumulate `TTime::DeltaTime() * 1000.0` (or seconds, whichever
fits your accumulator) and step the sim once per accumulated `kSimTickMs`.
Canonical examples: `src/effects/fizzle.cpp`, `src/effects/burn.cpp`.

### 1.5 Matrix indexing: row-major, column-vector

`d3dport.h`'s `Matrix` is row-major (`m[row*4 + col]`), column-vector
(`wp = M * vp`). For column N: `m[N], m[N+4], m[N+8]`.

**Hit by**: FireFlash (used m[4]/m[6] for column 1, should have been
m[1]/m[5]/m[9]). **Will hit**: every port that hand-composes orbit math from
the matrix.

**Mental check**: column 0 = m[0], m[4], m[8]. Column 1 = m[1], m[5], m[9].
Column 2 = m[2], m[6], m[10]. Translate column = m[3], m[7], m[11].

---

## 2. Orientation chooser

The snapshot's `SetTransformations` matrix often reads as `RotateX(-90°)` for
"flat on ground". Don't transcribe that literally — our shim has explicit
orientation knobs that may invert the literal §7 read. Decide visually first:

| Effect visual | Orientation | Notes |
|---|---|---|
| Sprite always facing camera (sparkles, droplets in air) | `ScreenAligned` | Add `rotation_rad` for in-plane spin |
| Decal lying flat on the floor (splat, ground ring) | `WorldXY` | The shim's RenderObjectSpinning is ScreenAligned-only; go direct `SubmitFxBillboard` |
| 3D mesh at a world position | `WorldMesh` | OBJ3D_MATRIX path; uses `submit_world` callback |
| Composite (e.g. blood flies as ScreenAligned then splats as WorldXY) | both | Two render branches by state |

The snapshot may LITERALLY contain a `RotateX(-90°)` for what we'd call
WorldXY, OR for what we'd call ScreenAligned — depends on whether the 1998
engine's "default" orientation was Y-up camera-facing or X-up. **Trust the
user's visual call over a literal §7 read** if they conflict.

---

## 3. Shim gaps still open

### 3.1 `d3d::RenderObjectSpinning` is ScreenAligned-only

Hardcoded `EFxBillboardOrientation::ScreenAligned` since the fizzle fix
(d3dport.cpp:432). For WorldXY+rotation, port code goes direct
`SubmitFxParticle`.

**Future cleanup**: split into `RenderObjectSpinningScreenAligned` (current)
+ `RenderObjectSpinningWorldXY` (new) — agents could then pick by name.

### 3.2 `EFxLightMode::LitFlat` is enumerated but the shim uses Unlit only

For DoLighting / scene-lit effects, compute brightness CPU-side (like Blood
does) and pass through `obj.diffuse[0..2]`. The renderer multiplies texture
by `color_rgba` under Unlit, achieving identical visual to LitFlat for
grayscale per-vertex lighting.

### 3.3 No texture-frame animation primitive

Per-frame `framehtexs[]` cycling (used by Photon, Flame, Fire family) is
not yet in the shim. Don't pick these for fan-out until added.

### 3.4 No spawn-anchor character-bone matrix in test harness

Effects that spawn from `ca->GetObjectMatrix(j)` (random body part) — Burn,
Blood — must fall back to a world-origin anchor and document this in code:
`// test harness anchor; real spawn site is per-bone in-game`.

---

## 4. Test-harness wiring patterns

### 4.1 Two-callback ports (submit + submit_world)

WorldMesh draws (`d3d::RenderObject` mesh variant) must come from
`submit_world` (runs inside `BeginTilePass`). Tick goes via `submit`.
Pattern:

```cpp
void IcedShimSubmit(void* cp, EFxDebugMode) { ... Tick(c->state); ... }
void IcedShimSubmitWorld(void* cp, EFxDebugMode) { ... SubmitWorld(c->state); ... }

iced_shim_entry.submit       = ...;
iced_shim_entry.submit_world = ...;   // BOTH must be wired
iced_shim_entry.pipeline     = "IM";  // SubmitHelperMesh
```

Billboard-only ports (Burn, Blood, Fountain, Sparks, Fizzle) use only
`submit`.

### 4.2 Auto-respawn for one-shot effects

Effects that self-kill (FireFlash → 100-frame cap) need a wrapper that
auto-respawns. Expose an `IsAlive(State*)` from the namespace so the
forward-declared State can be queried:

```cpp
// In src/effects/<effect>.cpp:
bool IsAlive(const State* st) { return st && st->alive; }

// In src/vfxtest.cpp:
namespace effect_shim { ... bool IsAlive(const State*); }

void EffectShimTick(void* cp, EFxDebugMode) {
    auto* c = static_cast<SCtx*>(cp);
    if (!c->state || !effect_shim::IsAlive(c->state)) {
        c->gap -= float(TTime::DeltaTime());
        if (c->gap <= 0.0f) {
            effect_shim::Destroy(c->state);   // MUST destroy before respawn
            c->state = effect_shim::Spawn(c->origin);
            c->gap   = 1.0f;
        }
        return;
    }
    effect_shim::Tick(c->state);
}
```

Forgetting the `Destroy()` leaks; forgetting the `!IsAlive` check makes the
effect run once and screen goes black.

### 4.3 File-contention discipline (parallel agents)

Each agent edits exactly ONE new file: `src/effects/<name>.cpp`. They report
their REGISTRATION block in `.vfx_logs/<name>.md` for the orchestrator to
splice into `src/vfxtest.cpp` after all agents return. CMakeLists is
`GLOB_RECURSE` — new files picked up automatically.

This worked cleanly with 4 parallel agents. No actual contention; the only
build breaks during the run were transient (an agent's in-progress code).

---

## 5. Lazy texture binding (Gotcha #1 from PORT_PROTOCOL)

Always defer texture handle resolution to the first `Render()` call:

```cpp
if (st->texture == kInvalidTexture && st->imagery) {
    st->texture = ResolveTexture(...);
    if (st->texture == kInvalidTexture) return;  // wait another tick
}
```

`TObjectImagery::LoadImagery` returns a valid imagery pointer immediately,
but the GPU texture handle is set later by the upload pass. Submitting with
`kInvalidTexture` shows nothing.

---

## 6. Visual verification

### 6.1 Reference folder names are not authoritative

`images/vfx/15_FireFlash` is actually FireWind or FireSwarm captures —
mislabeled. Always cross-check folder content against:

- Spell damage range (Fire Flash 50-55, Fire Wind 96-124, Fireball 200-246)
- Visual description in forensics
- Whether a real spell.def caller exists for the candidate class

If the user describes the effect differently than the folder suggests,
trust the user.

### 6.2 Headless filmstrips leave zombie processes

`--headless --filmstrip` mode does not exit cleanly on macOS — the binary
finishes writing frames but doesn't shut down sokol gracefully. Each run
needs `pkill -9 -f revenant` after. This makes parallel headless renders
infeasible from a single binary.

For wave 2, **prefer foreground user-driven verification** over headless
batch. Ask the user to run each port and report visually.

**Open**: fix the headless shutdown deadlock.

### 6.3 Forensics doc ≥ pixel reference

When no reference frame exists for an effect (FireFlash, Blood), trust the
forensics doc + the user's mental-model description. Three of the four wave-1
ports had no clean reference; all verified plausible by the user's
recognition of the effect's identity.

---

## 7. Sequence for wave-2 dispatch

1. **Pick effects that don't need still-missing shim primitives**:
   - ✅ ScreenAligned billboards (with/without rotation)
   - ✅ WorldXY billboards
   - ✅ WorldMesh (with submit_world)
   - ✅ Multi-pass (Alpha + Additive) — just call Submit twice
   - ❌ Texture-frame animation (Photon, Flame, Fire-family flipbook)
   - ❌ Vertex-animated WorldMesh (lightning curves, beam paths)

2. **Prefer effects that have**:
   - Complete forensics in `docs/vfx/forensics/`
   - A confirmed reference folder (or your willingness to trust forensics)
   - No VESTIGIAL flag in INVENTORY (no live caller = not worth porting)

3. **Brief each agent with**:
   - Path to forensics doc
   - PORT_PROTOCOL.md + this WAVE_1_LESSONS.md
   - Worked examples: `src/effects/fountain.cpp`, `sparks.cpp`, `fizzle.cpp`,
     `burn.cpp`, `blood.cpp`, `iced.cpp`, `fireflash.cpp`
   - Hard constraint: don't touch `vfxtest.cpp` / `d3dport.h` / shaders /
     other ports' files
   - Hard constraint: don't render headless filmstrips
   - Add the same diagnostic log pattern at Spawn + first render + end-of-life

4. **After all agents return**:
   - Wire all REGISTRATION blocks into vfxtest.cpp in one orchestrator pass
   - Build clean
   - Hand the user the foreground verification commands
   - Iterate per-effect on user feedback

---

## 8. Open architectural questions (defer; revisit as we get more data)

- **~1.5 Y scale**: where to apply (shim mesh path? renderer? per-effect?).
  A/B with retail required first.
- **Emissive=(1,1,1) shader semantics**: zero in shader, or honor authored?
  Need to confirm whether ANY shipped asset actually wanted self-illumination.
- **`d3d::RenderObjectSpinning` shim split**: ScreenAligned + WorldXY
  variants. Trivial change; do when next port needs it.
- **`EFxLightMode::LitFlat`**: GPU-side variant could be added; current
  CPU-side workaround via `obj.diffuse` is acceptable.
- **Headless shutdown deadlock**: blocks parallel batch verification. Worth
  one focused fix-up pass before wave 3+.
- **Test-rig AmbientLight default**: would let Lit effects render correctly
  in test without per-port brightness floors.
