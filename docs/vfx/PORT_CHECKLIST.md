# Port Checklist — Prescriptive Pre-flight + Lint

This is the **mechanical** companion to `PORT_PROTOCOL.md` + `WAVE_1_LESSONS.md`.
Every port walks down this list. Each line is either a code pattern to copy or a
grep/check to run. If the check fails, fix before declaring DONE.

---

## A. Pre-flight (read these BEFORE writing any code)

1. Open `docs/vfx/PORT_PROTOCOL.md` — the recipe + 4 gotchas.
2. Open `docs/vfx/WAVE_1_LESSONS.md` — cross-cutting bugs found by wave-1.
3. Open `docs/vfx/forensics/<EFFECT>.md` — your spec.
4. Open `src/d3dport.h` — your stdlib. **Verify every primitive you intend to
   use exists.** If you need something not in the shim, STOP and log BLOCKER.
5. **Confirm the asset exists in the shipped data archive.** Many vestigial
   forensics docs cite assets that were dropped before retail ship — porting
   them is wasted work. Check with `find RevenantRevisited/i3d_dump_all -iname
   <asset_basename>` BEFORE writing code. If the asset isn't there, STOP and
   log BLOCKER (the orchestrator will unregister the effect). Examples that
   bit wave-3: FlameDisc.I3D + FireColumn.I3D are both absent from shipped
   data despite their forensics docs being complete.
6. Open the worked example closest to your effect:
   - Camera-facing rotated sprite particle → `src/effects/fizzle.cpp`
   - Camera-facing gravity-driven scatter → `src/effects/sparks.cpp`
   - Camera-facing rising column (additive) → `src/effects/burn.cpp`
   - Camera-facing droplet with ground splat decals → `src/effects/blood.cpp`
   - WorldMesh single object animated transform → `src/effects/fireswarm.cpp`
   - WorldMesh multi-sub-object state machine → `src/effects/iced.cpp`
   - WorldMesh multi-layer composite → `src/effects/icebolt.cpp`
   - WorldMesh + Billboard composite → `src/effects/fireflash.cpp`
   - WorldMesh + Billboard composite with bounded test trajectory →
     `src/effects/fireball.cpp`
   - UV-subrect atlas-cell flipbook → `src/effects/flame.cpp`
   - framehtexs[] frame-array flipbook → `src/effects/flamedisc.cpp` (port)
   - Continuous UV-scroll on a single billboard → `src/effects/faultfire.cpp`

---

## B. Required code patterns

Copy these verbatim into your port. Adjust only the cited constants.

### B.1 — Lighting category declaration (top of file, after includes)

```cpp
// ============================================================================
// Lighting category: SELF-LIT (additive blend; asset color through; no DoLighting)
//   OR
// Lighting category: SCENE-LIT (Unlit shader + CPU-side DoLighting brightness)
// ============================================================================
```

**Decide which based on forensics §10 (color) + §7 (render). Self-lit = additive
blend in the render bracket. Scene-lit = Alpha blend + DoLighting call per
particle/vertex. If forensics doesn't say, ask. Don't guess.**

### B.2 — Emissive zero after every LoadMaterial (REQUIRED for WorldMesh ports)

```cpp
// In Spawn(), for every sub-object you draw:
d3d::LoadMaterial(img3d, sub_obj_index, st->diffuse[i], st->emissive[i]);
// REQUIRED: kHelperMeshFs adds emissive on top of texture; pre-release
// I3Ds default to emissive=(1,1,1) which saturates to white. Zero it.
st->emissive[i][0] = 0.0f;
st->emissive[i][1] = 0.0f;
st->emissive[i][2] = 0.0f;
// (emissive[3] is alpha, leave at 1.0)
```

**Lint check** (run before declaring DONE):

```sh
# Every LoadMaterial call must be followed by an emissive-zero block within 5 lines.
grep -A 5 "LoadMaterial" src/effects/<NAME>.cpp | grep -q "emissive.*= 0" || echo FAIL
```

### B.3 — Scene-lit brightness (only for scene-lit effects)

```cpp
// Scene-lit only. Compute DoLighting CPU-side, push through obj.diffuse.
// Brightness floor max(_,1.0) rescues the test rig where AmbientLight is 0;
// in-game GetAmbientLight() is always >= 10 so the floor never triggers.
float brightness = std::max(DoLighting(world_pos), 1.0f);
obj.diffuse[0] = obj.diffuse[1] = obj.diffuse[2] = brightness;
obj.diffuse[3] = 1.0f;
```

Self-lit effects skip DoLighting entirely. The additive blend handles brightness.

### B.4 — Framerate-independent Tick (REQUIRED)

```cpp
// Forensics rates are authored at 24 Hz. Accumulate real wall-clock time and
// step the sim once per accumulated kSimTickMs.
constexpr float kSimHz     = 24.0f;
constexpr float kSimTickMs = 1000.0f / kSimHz;

void Tick(State* st) {
    st->tick_accum_ms += float(TTime::DeltaTime() * 1000.0);
    while (st->tick_accum_ms >= kSimTickMs) {
        AnimateOneSimTick(st);   // your per-tick logic
        st->tick_accum_ms -= kSimTickMs;
    }
}
```

**Anti-pattern (rejected at review):**

```cpp
// WRONG — assumes 60fps; runs 2x too fast at 120fps:
Animate(st, 1000.0f / 60.0f);
```

**Lint check:**

```sh
grep -nE "1000\.?0?\s*/\s*60|1000\.?0?\s*/\s*24" src/effects/<NAME>.cpp \
  && echo "FAIL: hardcoded tick rate"
```

### B.5 — Matrix column extraction (orbit math, mesh transforms)

```cpp
// d3dport.h Matrix is row-major (m[row*4 + col]), column-vector (wp = M * vp).
// Column N axis = (m[N], m[N+4], m[N+8]).
//   Column 0 (X axis after rotation): m[0],  m[4],  m[8]
//   Column 1 (Y axis after rotation): m[1],  m[5],  m[9]
//   Column 2 (Z axis after rotation): m[2],  m[6],  m[10]
//   Translate column:                 m[3],  m[7],  m[11]

// Example: extract Y-axis direction from a rotation matrix M
float y_axis_x = M.m[1];
float y_axis_y = M.m[5];
float y_axis_z = M.m[9];
```

**Anti-pattern (caused FireFlash's orbit bug):**

```cpp
// WRONG — m[4] and m[6] are NOT column 1; they're row 1 col 0 and row 1 col 2:
float bad_x = M.m[4];  // ← actually row 1
float bad_z = M.m[6];  // ← actually row 1
```

### B.6 — WorldMesh → submit_world callback (Gotcha #4)

If your effect calls `d3d::RenderObject` with a mesh handle (WorldMesh variant),
its draw MUST be in a separate `SubmitWorld()` entry point routed to the
`submit_world` test-harness callback. Pattern:

```cpp
// In your effect namespace, expose THREE entry points:
namespace <effect>_shim {
    void Tick(State*);          // sim only — no draws
    void Submit(State*);        // Billboard draws (no-op stub if you have none)
    void SubmitWorld(State*);   // WorldMesh draws
    bool IsAlive(const State*); // for auto-respawn wrapper
}

// In your REGISTRATION block (.vfx_logs/<name>.md):
//   submit       = ... Tick(st);  Submit(st); ...
//   submit_world = ... SubmitWorld(st); ...
```

**Lint check:**

```sh
# If RenderObject mesh variant is called, SubmitWorld must exist.
if grep -q "d3d::RenderObject.*mesh\|RegisterSubMesh" src/effects/<NAME>.cpp; then
  grep -q "void SubmitWorld" src/effects/<NAME>.cpp || echo "FAIL: missing SubmitWorld"
fi
```

### B.7 — IsAlive helper for one-shot effects

If your effect self-kills (`KillThisEffect()` equivalent — sets `alive = false`
after lifetime), expose an `IsAlive` query so the test-harness wrapper can
auto-respawn:

```cpp
// In your effect namespace:
bool IsAlive(const State* st) { return st && st->alive; }
```

This is necessary because vfxtest.cpp only sees a forward-declared `State`
type — it can't access `st->alive` directly.

The orchestrator wires the auto-respawn pattern in vfxtest.cpp using your
namespace's IsAlive. Include a recommended wrapper template in your REGISTRATION
block (see canonical examples in `iced.md`, `fireflash.md`, `fireswarm.md`).

### B.7a — Texture index resolution (1-based texslot bites)

I3D manifest `texslot=N` is **1-based** — texture handle index = `texslot - 1`.
Hardcoding `texslot=1 → texture_index=1` (off by one) was the IceBolt round-2
regression: box01's glow material has texslot=1 which maps to texture[0]
(the white glow), but the agent's code used texture[1] (icicle stripes),
making the glow render as a striped icicle texture.

```cpp
// CORRECT pattern (from src/effects/fireball.cpp + icebolt.cpp):
int tex_index = d3d::ResolveSubObjTextureSlot(img3d, sub_obj_index);
// returns 0-based; -1 if sub-object has no material
```

**Lint check** — never hardcode texslot to texture-index conversion:

```sh
# Look for hardcoded `texslot = 1` or `texture_index = 1` constants:
grep -nE "texslot\s*=\s*[0-9]|texture_index\s*=\s*[0-9]" src/effects/<NAME>.cpp \
  && echo "FAIL: review — texture index should be resolved at runtime, not hardcoded"
```

### B.8 — Lazy texture binding (Gotcha #1)

```cpp
// Texture handle may be invalid at Spawn — resolve at first Render():
void Render(State* st) {
    if (st->texture == kInvalidTexture && st->imagery) {
        // resolve from imagery's sub-object material
        st->texture = ResolveTexture(st->imagery, sub_obj_index);
        if (st->texture == kInvalidTexture) return;  // wait another tick
    }
    if (st->texture == kInvalidTexture) return;
    // ... build draw items, submit ...
}
```

### B.9 — Diagnostic log lines (REQUIRED — for next user run)

```cpp
// At end of Spawn():
log_info("[<name>-shim] Spawn ok: <key state>");

// On first render frame:
if (!st->logged_first_render) {
    log_info("[<name>-shim] first render: <key state>");
    st->logged_first_render = true;
}

// At end of life:
if (!st->logged_end && /* end-of-life condition */) {
    log_info("[<name>-shim] all <particles/etc> dead");
    st->logged_end = true;
}
```

When the user reports a bug, these grep-able lines tell us instantly if
Spawn ran, if Submit fired, and if the effect ever ended. Without them, every
bug becomes a wild goose chase.

---

## C. Code-level forbidden list

If you write any of these, the review will reject:

| Forbidden | Why | Use instead |
|---|---|---|
| `// TODO bake Y scale here` actually baking ~1.5 Y scale | Y-scale is a pending cross-cutting fix; don't sprinkle it per-port | Leave a `// PENDING: ~1.5 Y scale (project_3d_world_y_scale)` marker if you suspect this effect needs it |
| `pkill`, `kill`, or any process management | Test-rig babysitting is orchestrator's job | Never |
| Edits to `src/vfxtest.cpp`, `src/d3dport.h`, `src/d3dport.cpp`, `src/renderer.cpp`, shaders, other ports' `src/effects/*.cpp` | File contention with parallel agents + breaks shim contract | Edit only `src/effects/<your-effect>.cpp`. Report REGISTRATION block. |
| `--headless --filmstrip` runs by you | Headless mode leaves zombie processes | User verifies foreground; don't render |
| `Tick(st, 1000.0f / 60.0f)` or any hardcoded tick step | FPS-coupled; runs wrong at any other FPS | Use `TTime::DeltaTime()` accumulator (B.4) |
| Skipping `emissive[i] = 0` after `LoadMaterial` | kHelperMeshFs adds emissive — white saturation | Always zero (B.2) |
| `if (!c->state)` in your suggested vfxtest wrapper | State stays non-null after self-kill; respawn never fires | `if (!c->state || !<effect>_shim::IsAlive(c->state))` (B.7 + see fireflash.md round-2 fix) |
| Hardcoded `box01_tex_index = 1` (or any direct texslot→texture-index assignment) | Manifest texslot is 1-based — `texslot=1` maps to texture[0] | Use `d3d::ResolveSubObjTextureSlot(img3d, sub_obj)` (B.7a) |
| Making EVERY draw additive in a "self-lit fire-family" port | Forensics §7 may specify Alpha+MODULATE for ball/glow/trail and Additive only for burst/ring; over-broadly additive saturates to yellow-red pixelated mess (FireBall round-5 regression) | Read forensics §7 per pass; pass a `bool additive` flag per draw site, default to forensics-cited blend |
| Speculatively adding particle systems where snapshot has none | Some snapshot animator bodies are PRE-particle WIP stubs — adding speculative particles changes the visual to something retail never produced (FireSwarm iter3+ regression) | Verify against retail decomp first; if retail body unextracted, ship the snapshot-faithful minimal version + mark as "pending retail extraction" |
| Drawing glow caps / radial-glow sub-objects as WorldMesh 3D spheres | A 4-vert flat quad in the I3D is a billboard sprite, not a 3D mesh — rendering as WorldMesh makes glow tilt edge-on under camera angle (IceBolt round-1 regression) | Check manifest: sub-objects with `verts=4 idxs=6` and flat bbox on one axis are **billboards** — render ScreenAligned |
| Bound-trajectory missile effects that explode off-screen | Test-rig camera has fixed framing; a FIREBALL_SPEED=8 wu/tick across full FLY duration travels hundreds of wu → explosion lands outside frame | Spawn at `(-N, 0, 50)` and fly to `(0, 0, 50)` so explosion lands at camera center; document `// test harness: bounded trajectory` (see fireball.cpp) |

---

## D. Pre-DONE checklist (run before reporting back)

```sh
NAME=<your effect filename, e.g. icebolt>

# 1. File exists
test -f src/effects/${NAME}.cpp || echo "FAIL: missing file"

# 2. Build clean
cmake --build build 2>&1 | tail -5 | grep -q "error" && echo "FAIL: build error"

# 3. No hardcoded tick rate
grep -nE "1000\.?0?\s*/\s*(60|24)" src/effects/${NAME}.cpp \
  && echo "FAIL: hardcoded tick rate (use TTime::DeltaTime accumulator)"

# 4. Every LoadMaterial followed by emissive-zero
LM=$(grep -c "LoadMaterial" src/effects/${NAME}.cpp || echo 0)
EZ=$(grep -cE "emissive\[.\]\[.\].*= 0" src/effects/${NAME}.cpp || echo 0)
[ "$LM" -gt 0 ] && [ "$EZ" -lt "$LM" ] \
  && echo "FAIL: $LM LoadMaterial but only $EZ emissive-zeros (B.2)"

# 5. WorldMesh draws → SubmitWorld present
grep -q "RegisterSubMesh\|d3d::RenderObject.*mesh\|d3d::RenderObject(\*Renderer, mesh" src/effects/${NAME}.cpp \
  && (grep -q "void SubmitWorld" src/effects/${NAME}.cpp \
       || echo "FAIL: WorldMesh used but SubmitWorld not exposed (B.6)")

# 6. IsAlive exposed if effect self-kills
grep -q "alive = false\|KillThisEffect" src/effects/${NAME}.cpp \
  && (grep -q "bool IsAlive" src/effects/${NAME}.cpp \
       || echo "FAIL: self-kill but no IsAlive helper (B.7)")

# 7. Diagnostic logs present
for line in "Spawn ok" "first render" "dead"; do
  grep -q "$line" src/effects/${NAME}.cpp \
    || echo "FAIL: missing diagnostic log: $line (B.9)"
done

# 8. REGISTRATION block in your log
grep -q "^## REGISTRATION" .vfx_logs/${NAME}.md \
  || echo "FAIL: missing REGISTRATION block in log"

# 9. No edits to forbidden files (check git)
git diff --name-only main... 2>/dev/null | grep -vE "^src/effects/${NAME}\.cpp$|^\.vfx_logs/${NAME}\.md$" \
  | grep -E "^src/(vfxtest|d3dport|renderer|shaders)|^src/effects/.*\.cpp$" \
  && echo "FAIL: edits outside allowed file set"

# 10. Headless not invoked by you (no zombies)
ps aux | grep -v grep | grep -q "revenant.*--headless\|revenant.*--filmstrip" \
  && echo "FAIL: you launched headless — kill and don't"
```

**If any check FAILs, fix or document the deviation in your log under
`## DEVIATIONS — <reason>`. If none fail, declare DONE in your log.**

---

## E. REGISTRATION block format (what the orchestrator wires)

Your `.vfx_logs/<name>.md` MUST end with a `## REGISTRATION` section in this
format. The orchestrator copies these verbatim into `src/vfxtest.cpp`.

```markdown
## REGISTRATION

### Forward declarations (FILE SCOPE)

```cpp
namespace <name>_shim {
    struct State;
    State* Spawn(const S3DPoint& origin /*, optional variant args */);
    void   Tick(State*);
    void   Submit(State*);
    void   SubmitWorld(State*);   // only if WorldMesh
    void   Destroy(State*);
    bool   IsAlive(const State*); // only if effect self-kills
}
```

### Wrappers (anonymous namespace)

```cpp
struct S<Name>ShimCtx {
    ::<name>_shim::State* state = nullptr;
    S3DPoint              origin = {0,0,0};
    float                 gap    = 0.0f;   // only if auto-respawn
};

// ...Spawn/Destroy/Submit/SubmitWorld wrappers as needed...
```

### DeferredRegister (in bootstrap block)

```cpp
VfxTest::SEffect <name>_shim_entry = {};
<name>_shim_entry.id            = "T<Name>Animator_SHIM";
<name>_shim_entry.family        = "<family>";
<name>_shim_entry.pipeline      = "<FB | IM | FB+IM | PE | ...>";
<name>_shim_entry.preview_style = VfxTest::EVfxPreviewStyle::Static;
<name>_shim_entry.factory       = [](const S3DPoint& o) -> void* { return <Name>ShimSpawn(o); };
<name>_shim_entry.submit        = [](void* c, EFxDebugMode d) { <Name>ShimSubmit(c, d); };
<name>_shim_entry.submit_world  = [](void* c, EFxDebugMode d) { <Name>ShimSubmitWorld(c, d); };  // if WorldMesh
<name>_shim_entry.destroy       = [](void* c) { <Name>ShimDestroy(c); };
VfxTest::DeferredRegister(<name>_shim_entry);
```

### Filmstrip args (for user foreground verification)

```sh
# Approx ~<lifetime>s coverage:
build/revenant --test=vfx --vfx=T<Name>Animator_SHIM --vfx-bg=black --vfx-no-ui
```

### Expected visual

A short paragraph describing what the user should see + key visual identity
markers. (Color, motion, lifecycle.) This is the orchestrator's verification
brief.
```

---

## F. Common BLOCKERs (when to stop and ask)

Don't half-implement. STOP and log BLOCKER if:

| Situation | What to say |
|---|---|
| Forensics §X is missing or contradictory | "BLOCKED — forensics §X says <A> but snapshot at <file:line> says <B>. Need clarification." |
| Shim doesn't provide a needed primitive | "BLOCKED — need `<primitive>` to render <feature>; closest is `<x>` which doesn't <thing>." |
| Asset path resolves but textures all kInvalidTexture | "BLOCKED — LoadImagery returns null/imagery has no texture frames at sub-obj N. <Hypothesis>." |
| Composite effect has too many sub-systems for one agent | "PARTIAL — phase A done; phase B (<sub-system>) needs <shim/forensics gap>." |

Partial returns are valuable. Half-implementing everything is not.
