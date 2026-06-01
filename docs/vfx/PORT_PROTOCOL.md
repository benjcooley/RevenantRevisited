# VFX Port Protocol

The full version-controlled procedure for porting one Revenant visual effect
from a completed forensics doc into a working bespoke render via the `d3d::*`
shim at `src/d3dport.h`.

This protocol is what `.claude/skills/vfx-port/SKILL.md` points at. Read it
end-to-end before starting a port.

The session in which the shim model was discovered + validated produced this
doc; see `src/effects/fountain.cpp` as the canonical worked example (~250
lines, 1:1 of `effect_old.cpp:3797-3887`).

---

## 0. When to use this skill / NOT use this skill

**Use it when:**

- A forensics doc exists at `docs/vfx/forensics/<ID>_<Class>.md` rated
  **retail-confirmed** or **retail-partial** (NOT snapshot-only).
- The effect uses billboard sub-objects (sparkle/photon/flame/aura family) or
  WorldMesh sub-objects (icebolt-style cylinders/rings) the shim covers.
- The user explicitly asks for a "port", "shim port", "bespoke port",
  "reconstruction" of an effect, or "the next effect from forensics".

**Do NOT use it when:**

- No forensics doc exists → invoke `vfx-forensics` first.
- Forensics is snapshot-only without retail verification → escalate; don't
  build a port on unverified ground.
- The effect needs a shim primitive that doesn't exist yet (texture animation,
  particle bucket emitter beyond billboards, strip with per-vertex jitter) →
  STOP and post a shim-extension gap, don't paper over.
- Forensics §7 is incomplete or has wrong sub-object enumeration (the
  Lightstrip case: doc said 3 sub-objects, actual I3D had 13) → re-run
  forensics with the i3ddump output, then port.

---

## 1. Mission

Transcribe the snapshot Initialize/Animate/Render bodies cited by the
forensics doc into a new file at `src/effects/<effect_name>.cpp` using only
the `d3d::*` shim primitives. Wire a harness entry in `src/vfxtest.cpp`
registering `T<Class>_SHIM`. Verify visually via headless filmstrip.

**The forensics doc is the spec. The shim is the stdlib. You are the
transcriber.** Visual judgment is not your job — make the code work the same
as the snapshot says, and the visual follows.

---

## 2. Hard preconditions (gates before you start)

Verify each. If any fails, STOP. Do not improvise.

| # | Gate | Verification |
|---|---|---|
| P1 | Forensics doc exists | `ls docs/vfx/forensics/<ID>_<Class>.md` |
| P2 | Retail fidelity rated | grep for "Retail fidelity" in the doc header; must be `retail-confirmed` or `retail-partial`; `snapshot-only` without verification is a STOP |
| P3 | §7 cites snapshot Render() | `grep -n "Render():" docs/vfx/forensics/<ID>_<Class>.md` — should point at a specific snapshot file:line block |
| P4 | §3 lists every constant | every numeric value needs a `file:line` cite + retail-confirmed/snapshot-only marker |
| P5 | §4 enumerates sub-objects | named list with role per sub-object; cross-check against `RevenantRevisited/i3d_dump_all/<basename>/manifest.txt` if dump exists |
| P6 | Asset accessible | `unzip -l data/imagery.rvi | grep -i <asset_basename>` — confirms the I3D ships |
| P7 | Shim covers the API surface | open `src/d3dport.h` and cross-check the snapshot's D3D calls against the shim table (see §6) |

A "skip a gate just this once" attitude is the failure mode this protocol was
written to prevent. If P2 fails (snapshot-only), the agent will produce a
visually-divergent port that wastes budget. If P5 fails (incomplete
sub-object enumeration), the agent will miss sub-objects (the Meteorstorm
iter6 failure).

---

## 3. Source-of-truth order

Read sources in this order. Earlier sources override later ones.

1. **Forensics doc §7 (Rendering)** — the canonical D3D body you transcribe
2. **Forensics §3 (Constants)** — every named value
3. **Forensics §4 (Assets)** — sub-object list + texture layout + materials
4. **Forensics §6 (Behavior)** — Animate body for per-tick state machinery
5. **Snapshot source the forensics cites** — verify wording matches; if there's
   a discrepancy, the forensics doc is wrong (re-run vfx-forensics)
6. **i3ddump output** at `RevenantRevisited/i3d_dump_all/<basename>/`:
   - `manifest.txt` — sub-object enumeration + bbox + material colors
   - `<basename>.obj` — combined Wavefront OBJ (open in Blender if needed)
   - `texture_*.png` — actual atlas images
7. **Shim primitives** (`src/d3dport.h`) — what's available to call
8. **Canonical worked example** (`src/effects/fountain.cpp`) — pattern reference

NEVER read these:

- The existing `T<X>_Bespoke` class in `src/effect.cpp` — it's for A/B
  comparison ONLY after your port is rendering, not for "how to" guidance.
  Reading it before your port is done biases you toward its choices, which
  may be approximations, not snapshot-faithful.
- Prior in-progress port attempts on the same effect (if any in
  `src/effects/<effect>.cpp` already exists, treat it as compromised; clean
  slate per forensics protocol hard-rule 8).

---

## 4. Output layout

Each port produces three artifacts:

### 4.1 `src/effects/<effect_name>.cpp`

The transcription. Mostly mechanical. Use lowercase + underscore for the
filename (`fountain.cpp`, `fireball.cpp`, `flame.cpp`).

File structure (mirror `src/effects/fountain.cpp`):

```cpp
// =========================================================================
// * Header banner: effect name, shim validation port, forensics ref,      *
// * retail fidelity, source-of-truth note                                 *
// =========================================================================

#include <cstdint>
#include <cstdlib>     // for std::rand if you need random()
#include <cstring>
#include "../d3dport.h"
#include "../logging.h"
#include "../math3d.h"
#include "../imagery.h"
#include "../renderer.h"

extern TRenderer* Renderer;

namespace <effect>_shim {

// === §3 Constants — every value cited from forensics §3 ===
constexpr int32_t kNum<X>          = ...;    // forensics §3 cite
constexpr float   k<X>...          = ...;
constexpr const char* kImageryPath = "...";

// === Per-effect state ===
// Snapshot's per-instance arrays + resolved asset data.
struct State { /* ... */ };

// === §6.1 — Initialize ===
static void Initialize(State* st) {
    // Mechanical transcription of snapshot Initialize body.
    // Cite the snapshot line range above the function.
}

// === §6.2 — Animate (per-tick state update) ===
static void Animate(State* st) {
    // Mechanical transcription of snapshot Animate body.
}

// === §7 — Render ===
static void Render(State* st) {
    if (!Renderer || !st->alive) return;
    // [Gotcha #1] Lazy texture bind: handle may be invalid at Spawn time.
    // [Gotcha #2] UV sub-rect: resolve at Spawn, encode in obj.lverts.
    // [Gotcha #3] Billboard size: scale × base_size constant.
    // [Gotcha #4] WorldMesh: must call from submit_world callback.
    d3d::BlendStateGuard scope;
    d3d::SetBlendState();    // or SetAddBlendState() per forensics §7
    d3d::Matrix inst_world; /* T(base_pos) */
    d3d::Obj obj; /* shared per-frame */
    for (int n = 0; n < kNum<X>; ++n) {
        if (/* per-particle alive condition */) {
            obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
            // ... write obj.scl, obj.pos, obj.lverts
            d3d::RenderObject(*Renderer, obj,
                              d3d::Orientation::ScreenAligned,  // or WorldMesh
                              st->texture, &inst_world);
        }
    }
}

// Public C-style entry points the harness calls.
State* Spawn(const S3DPoint& origin, /* effect-specific args */);
void   Tick(State*);
void   Submit(State*);
void   Destroy(State*);

} // namespace
```

### 4.2 Harness entry in `src/vfxtest.cpp`

Add three pieces:

1. Forward declaration at FILE SCOPE (NOT inside the anonymous namespace —
   linker won't find the symbols otherwise):

```cpp
// Above the `namespace { ... }` block:
namespace <effect>_shim {
    struct State;
    State* Spawn(const S3DPoint& origin, /* effect-specific args */);
    void   Tick(State*);
    void   Submit(State*);
    void   Destroy(State*);
}
```

2. Spawn/Destroy/Submit wrappers (inside the existing anonymous namespace,
   next to the existing `_Bespoke` wrappers):

```cpp
struct S<X>ShimCtx {
    ::<effect>_shim::State* state = nullptr;
};

template <int32_t kArg>
void* <X>ShimSpawn(const S3DPoint& origin) {
    auto* c = new S<X>ShimCtx();
    c->state = <effect>_shim::Spawn(origin, kArg);
    if (!c->state)
        log_warn("[vfx] <effect>_shim::Spawn returned null");
    return c;
}

void <X>ShimDestroy(void* cp) {
    auto* c = static_cast<S<X>ShimCtx*>(cp);
    if (c) {
        <effect>_shim::Destroy(c->state);
        delete c;
    }
}

void <X>ShimSubmit(void* cp, EFxDebugMode) {
    auto* c = static_cast<S<X>ShimCtx*>(cp);
    if (c && c->state) {
        <effect>_shim::Tick(c->state);
        <effect>_shim::Submit(c->state);
    }
}
```

3. Registration (next to the existing `_BESPOKE` registration in the bootstrap
   block):

```cpp
VfxTest::SEffect <x>_shim_entry = {};
<x>_shim_entry.id            = "T<Class>_SHIM";
<x>_shim_entry.family        = "magic";   // or appropriate per forensics
<x>_shim_entry.pipeline      = "FB";      // or "FB+IM" if WorldMesh too
<x>_shim_entry.preview_style = VfxTest::EVfxPreviewStyle::Static;
<x>_shim_entry.factory       = [](const S3DPoint& o) -> void* { return <X>ShimSpawn<0>(o); };
<x>_shim_entry.submit        = [](void* c, EFxDebugMode d) { <X>ShimSubmit(c, d); };
<x>_shim_entry.destroy       = [](void* c) { <X>ShimDestroy(c); };
VfxTest::DeferredRegister(<x>_shim_entry);
```

### 4.3 A validation filmstrip

Render headless via `--vfx=T<Class>_SHIM` and save the filmstrip alongside
the existing bespoke for A/B comparison. See §8.

---

## 5. The four gotchas (every billboard port hits these)

These are the recoverable surprises. Each was discovered the hard way; you
should hit none of them if you read this section before starting.

### Gotcha 1 — Texture handle invalid at Spawn

The I3D loader doesn't always upload textures to GPU before your `Spawn`
function returns. Reading `tex.htexture` at Spawn time may give `kInvalidTexture`
even though the imagery loaded successfully.

**Pattern**: cache `T3DImagery*` at Spawn; resolve `texture` lazily on the FIRST
`Submit()` call.

```cpp
struct State {
    T3DImagery*    imagery = nullptr;   // set at Spawn
    TTextureHandle texture = kInvalidTexture;
    // ...
};

State* Spawn(...) {
    // ... FindImagery + LoadImagery + dynamic_cast ...
    st->imagery = img3d;
    // DO NOT cache tex.htexture here.
    return st;
}

void Render(State* st) {
    if (st->texture == kInvalidTexture && st->imagery) {
        S3DTex tex = {};
        st->imagery->GetTexture(0, &tex);
        st->texture = tex.htexture;
        if (st->texture == kInvalidTexture) return;  // wait another tick
    }
    // ... use st->texture
}
```

### Gotcha 2 — I3D textures are atlases; sub-objects use UV sub-rects

A "single 64×64 texture" in the manifest may be a 2×2 or 4×4 atlas of color
variants. Each sub-object's authored vertex UVs pick ONE quadrant. Sampling
the whole atlas (default `uv_rect = (0,0,1,1)`) renders all 4 colors at once
(this is the bug the Fountain shim hit on iter1: cyan + red + green + blue
sparkles instead of just cyan).

**Pattern**: resolve the chosen sub-object's UV bbox at Spawn, write the
corners into `obj.lverts[]`, set `Flag_Verts` so `RenderObject` honors them.

```cpp
const int32_t nv = img3d->NumObjVerts(sub_obj_num);
std::vector<S3DVertex> vbuf(size_t(nv), S3DVertex{});
img3d->GetObjVerts(sub_obj_num, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
float minu = vbuf[0].tu, maxu = vbuf[0].tu;
float minv = vbuf[0].tv, maxv = vbuf[0].tv;
for (int32_t i = 1; i < nv; ++i) {
    minu = std::fmin(minu, vbuf[i].tu);
    maxu = std::fmax(maxu, vbuf[i].tu);
    minv = std::fmin(minv, vbuf[i].tv);
    maxv = std::fmax(maxv, vbuf[i].tv);
}
st->uv_rect[0] = minu;
st->uv_rect[1] = minv;
st->uv_rect[2] = maxu - minu;
st->uv_rect[3] = maxv - minv;
```

Then in Render():

```cpp
obj.flags |= d3d::Flag_Verts;
const float u_lo = st->uv_rect[0];
const float v_lo = st->uv_rect[1];
const float u_hi = u_lo + st->uv_rect[2];
const float v_hi = v_lo + st->uv_rect[3];
obj.lverts[0].tu = u_lo; obj.lverts[0].tv = v_lo;   // TL
obj.lverts[1].tu = u_lo; obj.lverts[1].tv = v_hi;   // BL
obj.lverts[2].tu = u_hi; obj.lverts[2].tv = v_lo;   // TR
obj.lverts[3].tu = u_hi; obj.lverts[3].tv = v_hi;   // BR
```

### Gotcha 3 — Billboard size = scale × base_size

The snapshot's `obj->scl = 2.0` is a multiplier on the I3D mesh's authored
half-extent (e.g. photon sub-object is ~5.25 wu half-extent, so `2.0 × 5.25 ×
2 = 21 wu` final billboard size). Our `SubmitFxBillboard.size_wu` is the FULL
WU size, with the renderer handling projection. Setting `size_wu = 2.0`
renders sub-pixel.

**Pattern**: multiply snapshot scale by an explicit base size constant. The
existing bespoke ports use ~64 wu for sparkle family. Match that. Don't
invent a different base.

```cpp
constexpr float kBaseSizeWu = 64.0f;   // cite the bespoke or
                                        // forensics if doc gives a value
// ...
const float size = st->scale[n] * kBaseSizeWu;
obj.scl[0] = obj.scl[1] = obj.scl[2] = size;
```

### Gotcha 4 — WorldMesh from submit_world callback only

`SubmitHelperMesh` (the call `d3d::RenderObject` makes for
`Orientation::WorldMesh`) MUST be invoked from the harness `submit_world`
callback (inside `BeginTilePass` scope, see `src/vfxtest.cpp:1515-1529`).
Calling it from the regular `submit` hook causes `BeginTilePass` to clear the
transparent_world_queue before your draw lands → silently invisible.

This bit icebolt and meteorstorm hard. Both eventually figured it out the
hard way.

**Pattern**: for WorldMesh ports, register BOTH callbacks in vfxtest.cpp:

```cpp
<x>_shim_entry.submit       = [](void* c, EFxDebugMode d) { <X>ShimSubmit(c, d); };       // billboards
<x>_shim_entry.submit_world = [](void* c, EFxDebugMode d) { <X>ShimSubmitWorld(c, d); };  // helper meshes
<x>_shim_entry.pipeline     = "FB+IM";  // FB + IM tag indicates both pipelines
```

And split your render logic into two methods: one for billboards (called from
`Submit`), one for world meshes (called from `SubmitWorld`). See
`src/effect.cpp:8243-8480` (icebolt's `SubmitWorldMeshes_BESPOKE`) for the
working pattern.

ScreenAligned billboards work from either submit hook. Use submit for them.

---

## 6. Shim API surface

Every D3D pattern you'll encounter in a snapshot Render body maps to a `d3d::*`
call. If the pattern isn't in the table, name it precisely as a shim-extension
gap; don't paper over.

### Matrix ops

| Snapshot | Shim |
|---|---|
| `D3DMATRIXClear(&m)` | `d3d::MatrixClear(m)` or `MatrixIdentity(m)` |
| `D3DMATRIXScale(&m, &scl)` | `d3d::MatrixScale(m, sx, sy, sz)` |
| `D3DMATRIXRotateX(&m, rad)` | `d3d::MatrixRotateX(m, rad)` |
| `D3DMATRIXRotateY/Z(&m, rad)` | `d3d::MatrixRotateY/Z(m, rad)` |
| `D3DMATRIXTranslate(&m, &p)` | `d3d::MatrixTranslate(m, tx, ty, tz)` |
| Animator's world matrix | `d3d::MatrixWorldFromBase(m, base_pos, face_rad)` |
| `m = a * b` | `d3d::MatrixMultiply(out, a, b)` |

The shim handles the row-vs-column convention internally. Use snapshot's
literal order; don't reverse.

### Blend state

| Snapshot | Shim |
|---|---|
| `SaveBlendState() / RestoreBlendState()` | `d3d::BlendStateGuard guard;` (RAII) |
| `SetBlendState()` (Alpha) | `d3d::SetBlendState();` |
| `SetAddBlendState()` (Additive) | `d3d::SetAddBlendState();` |
| Read current mode | `d3d::CurrentBlend()` |

The current blend is consumed by every `RenderObject` call. No need to pass
explicitly.

### Obj struct

`d3d::Obj` mirrors the retail per-draw working state:

```cpp
struct Obj {
    uint32_t       flags;          // OBJ3D_* equivalents (Flag_Scl1, Flag_Pos2, ...)
    Matrix         matrix;         // OBJ3D_MATRIX path
    float          pos[3];         // OBJ3D_POS*
    float          scl[3];         // OBJ3D_SCL*
    LightingVertex lverts[4];      // OBJ3D_VERTS: per-corner UVs
    int32_t        objnum;         // sub-object identity
    MeshHandle     mesh;           // pre-registered (for WorldMesh)
    float          diffuse[4];     // authored material
    float          emissive[4];
};
```

Flag constants match snapshot OBJ3D_* semantics:

| Flag | Meaning |
|---|---|
| `Flag_Matrix` | Use `obj.matrix` directly (full custom transform) |
| `Flag_AbsPos` | `obj.pos` is absolute world (not relative to animator) |
| `Flag_Verts`  | Honor `obj.lverts[]` for atlas UV sub-rects |
| `Flag_Scl1`   | Apply scale FIRST in the composition |
| `Flag_Pos2`   | Apply translate SECOND |
| `Flag_Pos1`   | Apply translate FIRST |
| `Flag_Scl2`   | Apply scale SECOND |

Combine with `|`. The most common pattern is `Flag_Scl1 | Flag_Pos2 |
Flag_Verts` (scale, translate, custom UVs — the Fountain pattern).

### RenderObject (the actual draw)

```cpp
d3d::RenderObject(
    TRenderer&  renderer,
    const Obj&  obj,
    Orientation orientation,    // ScreenAligned | WorldMesh
    TTextureHandle texture,
    const Matrix*  inst_world   // optional: per-effect-instance transform
);
```

Dispatches based on `orientation`:

- `Orientation::ScreenAligned` → `SubmitFxBillboard` with `size_wu` from
  `obj.scl`, position from `inst_world * obj.pos`, UVs from `obj.lverts[]`
  if `Flag_Verts` set
- `Orientation::WorldMesh` → `SubmitHelperMesh` with world matrix composed
  from `obj.flags` + `obj.scl` + `obj.pos` + `obj.matrix`

### Asset helpers

| Pattern | Shim |
|---|---|
| Per-sub-object material color | `d3d::LoadMaterial(img3d, sub_obj, diffuse[4], emissive[4])` |
| Per-sub-object mesh registration | `d3d::RegisterSubMesh(*Renderer, img3d, sub_obj, num_tex)` |

`RegisterSubMesh` handles degenerate UVs (binds white texture + routes
material color) automatically. Don't replicate the logic inline.

### What's NOT yet in the shim (= valid gap to flag)

- **Texture animation / frame cycling** for animated atlases — neither
  `T3DImagery::framenum` advancement nor the `framehtexs` array is wired
  through. Port that needs it should flag this gap.
- **Particle bucket emitters** beyond billboards (Sparks-style continuous
  emission with per-particle physics) — use `SubmitFxParticleBucket` directly
  for now.
- **SubmitFxStrip** for procedural strips (LightStrip's jittered triangle
  strip) — not yet abstracted. Port that needs it should flag.
- **Animator state machines** (multi-phase effects with state transitions
  beyond a simple framenum counter) — encode directly in the State struct;
  no shim helper.
- **Scene light posting** (the `spell.def LIGHT COLOR ... INT ... MULT ...`
  entry that tints the scene around a cast) — not yet abstracted; defer per
  forensics §9 (this is a spell-side concern, often).

---

## 7. The four-step transcription workflow

Once preconditions are met, execute in order:

### Step 1 — Read

1. Read the forensics doc end-to-end. Pay particular attention to:
   - §2 Sources & retail fidelity
   - §3 Constants table
   - §4 Assets table + sub-object enumeration
   - §6 Behavior pseudocode
   - §7 Rendering pseudocode + blend mode + sub-object choice
   - §13 Gaps (anything you'll need to work around)
2. Read the snapshot file:line cited by §7. Confirm the pseudocode in the
   forensics matches the actual source.
3. Read `RevenantRevisited/i3d_dump_all/<basename>/manifest.txt` if dump
   exists. Confirm sub-object count + texture size match §4.
4. Skim `src/effects/fountain.cpp` for the pattern reference.
5. Skim `src/d3dport.h` and confirm the API surface covers what §7 calls for.

### Step 2 — Write

1. Create `src/effects/<effect_name>.cpp` from the template (§4.1).
2. Transcribe constants (§3 of forensics) into `constexpr` declarations,
   each with a forensics cite as a comment.
3. Transcribe Initialize (§6.1) into a `static void Initialize(State*)`.
4. Transcribe Animate (§6.2) into a `static void Animate(State*)`.
5. Transcribe Render (§7) into a `static void Render(State*)`, using the
   shim primitives only. Handle gotchas 1-4 inline (lazy texture, UV sub-rect
   resolution at Spawn, base size multiplier, submit_world if WorldMesh).
6. Wire `Spawn` (loads imagery + runs Initialize), `Tick` (calls Animate),
   `Submit` (calls Render), `Destroy`.

### Step 3 — Wire

1. Forward-declare your namespace at FILE SCOPE in `src/vfxtest.cpp` (above
   the anonymous namespace).
2. Add Spawn/Destroy/Submit wrappers next to the existing `_Bespoke` wrappers.
3. Register a `T<Class>_SHIM` entry in the bootstrap block next to the
   existing `_BESPOKE` registration.
4. Build: `cmake --build build -j`. Expect a clean build. If the linker
   complains about missing `(anonymous namespace)::<X>_shim::*` symbols, your
   forward decl is in the wrong scope.

### Step 4 — Validate

1. Kill any stray test processes: `pkill -f "build/revenant --headless"`
2. Render filmstrip:
   ```bash
   mkdir -p filmstrips/vfx_shim/<effect>
   build/revenant --headless --test=vfx --vfx=T<Class>_SHIM \
     --filmstrip=9,0.5 --vfx-bg=black \
     --snapprefix=vfx_shim/<effect>/iter1_
   ```
3. Read the resulting `iter1_filmstrip.png` (multimodal, via Read tool).
4. Compare against:
   - Reference frames in `images/vfx/<NN>_<EffectName>/`
   - Existing bespoke render via `--vfx=T<Class>_BESPOKE`
5. Decide verdict per §9.

---

## 8. A/B comparison standard

The existing `T<X>_BESPOKE` rendering is the visual baseline. Your shim port
should match it OR be more snapshot-faithful (the bespoke may carry approximations
the snapshot doesn't justify).

**CLOSE** = visually matches bespoke + reference; all snapshot semantics
preserved.

**IMPROVED** = bespoke had a visible gap (wrong color, missing layer); your
port fixes it. Document what changed and why (cite the snapshot line that
justified the change).

**DIFFERENT** = your port looks substantively different from the bespoke
AND the reference. Two cases:
- You missed something snapshot-cited → re-read §7 and fix
- Snapshot disagrees with what shipped → escalate as retail-fidelity gap;
  don't tune by feel

**BLOCKED** = forensics or shim has a precise gap that prevents transcription.
Name the gap (file:line cite + minimum API needed) and stop.

---

## 9. Anti-patterns (instant red flags)

If you find yourself doing any of these, STOP and re-read the relevant
forensics section.

| Anti-pattern | Why it's wrong | Correct response |
|---|---|---|
| "Let me try a different blend mode to see if it looks better" | Forensics §7 names the blend; tuning by feel introduces drift | Cite §7; use that blend; if it looks wrong, the gap is in §7's SUSPECT flag, not yours to override |
| "I'll multiply size by 1.5 to make it more visible" | Magic numbers without forensics justification | Find the missing multiplier in the snapshot (gotcha 3 base size); cite it |
| "The agent's iter6 did X, let me do that" | Inherits compromises from a prior in-progress port | Clean slate; re-derive from forensics |
| "This sub-object isn't in the forensics but maybe it's needed" | Speculation, not transcription | If forensics §4 doesn't enumerate it for THIS animator, don't render it. If you think forensics is wrong, re-invoke vfx-forensics |
| "The texture didn't load so I'll use a procedural sparkle" | Procedural stand-ins for missing assets | The BLOCKER is in the loader; flag it as a gap, don't paper over |
| "I'll skip this sub-object since it's harder to render" | Coverage drift | Either render it correctly via the shim or flag the shim-extension gap |
| "I'll write this matrix code by hand instead of using `d3d::*`" | Bypasses convention handling; reintroduces all the gotchas | Use the shim primitives. If they don't cover your case, flag a precise extension request |

---

## 10. Stopping conditions

Stop and post a gap if:

- Forensics doc is missing a §7 cite for behavior you need to port (forensics
  gap)
- Shim is missing a primitive for a D3D call you need (shim-extension gap;
  name the call precisely)
- I3D doesn't contain a sub-object the forensics says is there (asset gap)
- Snapshot lines cited don't say what the forensics doc claims (forensics
  accuracy gap)
- Render output is substantively wrong AND you've re-read §7 + §3 + §4 with
  no missing transcription (deeper gap; escalate)

Do NOT try to "make it work" by improvising. The whole value of this skill
is mechanical transcription — improvising is what we're avoiding.

---

## 11. Final report (what you hand back)

After validation:

- **Path** to new `src/effects/<effect>.cpp`
- **Harness ID** registered (`T<Class>_SHIM`)
- **Filmstrip path** of the validation render
- **Verdict** per §8 (CLOSE / IMPROVED / DIFFERENT / BLOCKED)
- **Constants summary** (sanity-check vs forensics §3 — list each transcribed
  constant + value)
- **Shim-extension requests** (precise: which D3D pattern, what minimum API
  would cover it; e.g. "texture frame cycling via `T3DImagery::framehtexs` —
  needs `d3d::AdvanceTextureFrame(obj, framenum)` or equivalent")
- **Forensics gaps** discovered during transcription (anything the doc
  should have said but didn't)

---

## 12. Worked example

`src/effects/fountain.cpp` is the canonical worked example.

Forensics: `docs/vfx/forensics/FOUNTAIN_TFountainAnimator.md` (973 lines,
94 cites, retail-partial fidelity).

Snapshot: `src/effect_old.cpp:3797-3887` (Initialize at :3797, Animate at
:3822, Render at :3861).

Port: `src/effects/fountain.cpp` (~250 lines, mostly comments). Every
constant cites a forensics §3 line. Render body is a 1:1 transcription using:

- `d3d::Matrix` + `MatrixIdentity` + `MatrixTranslate` for `inst_world`
- `d3d::Obj` with `Flag_Scl1 | Flag_Pos2 | Flag_Verts`
- `obj.lverts[]` populated from the sub-object's vertex UV bbox (gotcha 2)
- `obj.scl = scale[n] * kFountainBaseSizeWu` (gotcha 3)
- `d3d::BlendStateGuard` + `d3d::SetBlendState()` (Alpha, per forensics §7)
- `d3d::RenderObject(... Orientation::ScreenAligned, st->texture,
  &inst_world)`

Harness ID: `TFountainAnimator_SHIM`, family `magic`, pipeline `FB`.

Validation: cyan sparkle column renders matching the bespoke. CLOSE verdict.

Time to write the port from scratch (with this protocol read first): ~30
minutes. Without it: hours, doom-loop risk.
