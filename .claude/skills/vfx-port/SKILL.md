---
name: vfx-port
description: This skill should be used when the user asks to "port" a Revenant visual effect, "reconstruct" an effect from forensics, write a "bespoke port", or dispatch a port-phase agent for a single effect. It transcribes the snapshot D3D Render body cited by a completed forensics doc into a working bespoke port via the `d3d::*` shim at `src/d3dport.h`. The output is mechanical code transcription — no convention discovery, no creative interpretation. Pre-condition is a transcription-ready forensics doc (otherwise use vfx-forensics first).
version: 0.1.0
---

# VFX Port

Port one Revenant visual effect from its forensics doc into a working bespoke
render via the `d3d::*` shim. **The forensics doc is the spec; the shim is the
stdlib; you are the transcriber.** Visual judgment is not your job — make the
code work the same as the snapshot says, and the visual follows.

The full protocol, file layout, shim API, gotcha checklist, and worked example
are the version-controlled source of truth at:

**`docs/vfx/PORT_PROTOCOL.md`**

Read that file in the VFX worktree
(`/Users/benjamincooley/projects/RevenantRevisited/worktrees/vfx`) and follow it
exactly. Also read first:

- `src/d3dport.h` — the shim API you'll be calling
- `src/effects/fountain.cpp` — the canonical worked example (~250 lines)
- `docs/vfx/D3D_PORT_SHIM.md` — the shim's design notes + convention coverage
- `docs/vfx/forensics/<EFFECT_ID>_<Class>.md` — your effect's spec

## Mission

Transcribe the snapshot Initialize/Animate/Render bodies cited by the
forensics doc into a new `src/effects/<effect_name>.cpp` using the `d3d::*`
shim. Wire a harness entry in `src/vfxtest.cpp` registering
`T<Class>_SHIM`. Verify visually via `--vfx=T<Class>_SHIM`.

**No invention.** You are recreating the behavior of a classic game that must be faithful to the original. Every numeric constant, every blend mode, every 
UV cell, every transform comes from a forensics-cited snapshot line. If you 
would need to pick a value to make it "look right," stop and post a gap.

**No discovery work.** If the shim doesn't cover what you need (texture
animation, particle buckets, strip primitives), name the gap precisely as a
shim-extension request — don't paper over with a bespoke approximation.

## Hard preconditions (gates before you start)

1. Forensics doc exists at `docs/vfx/forensics/<ID>_<Class>.md`
2. Forensics §2 rates **retail-confirmed** or **retail-partial** (NOT snapshot-only
   without verification + acknowledgment)
3. Forensics §7 Rendering cites the snapshot Render body with explicit `file:line`
4. Asset MD5 verified or gap honestly flagged
5. (Optional but valuable) i3ddump output exists at
   `RevenantRevisited/i3d_dump_all/<asset_basename>/` — manifest, OBJ, PNG
   atlas — confirm sub-object count + texture layout matches forensics §4

If any precondition fails → STOP. Either invoke `vfx-forensics` to fill the
gap, or ask the user.

## What the deliverable IS

- `src/effects/<effect_name>.cpp` — the transcription
- Harness entry in `src/vfxtest.cpp` registering `T<Class>_SHIM`
- A render filmstrip proving the visual works

## What the deliverable is NOT

- A new bespoke class in `src/effect.cpp` (the legacy monolith — DO NOT touch)
- A wrapping around the existing `_Bespoke` class (the existing port is for A/B
  comparison only, not for reuse)
- Custom matrix code, blend-state plumbing, or callback-scope reasoning — those
  live in the shim, not the port

## Source-of-truth order (mirrors forensics protocol)

1. **Forensics doc §7 (Rendering)** — the canonical source you transcribe
2. **Forensics §3 (Constants)** — every named value with its source-line cite
3. **Forensics §4 (Assets)** — sub-object list + texture layout + materials
4. **Snapshot source the forensics cites** — verify wording matches doc
5. **i3ddump output** — empirical asset structure
6. **Shim primitives** (`src/d3dport.h`) — the only translation layer
7. **A canonical worked example** (`src/effects/fountain.cpp`) — pattern reference

NEVER look at the existing `T<X>_Bespoke` in `src/effect.cpp` for "how to port."
Only as a final A/B render comparison after your port is done.

## The shim primitives (everything you call)

From `src/d3dport.h`:

| D3D snapshot pattern | Shim equivalent |
|---|---|
| `D3DMATRIXClear(&obj->matrix)` | `d3d::MatrixClear(m)` |
| `D3DMATRIXScale(&m, &v)` | `d3d::MatrixScale(m, sx, sy, sz)` |
| `D3DMATRIXRotateX/Y/Z(&m, rad)` | `d3d::MatrixRotateX/Y/Z(m, rad)` |
| `D3DMATRIXTranslate(&m, &p)` | `d3d::MatrixTranslate(m, tx, ty, tz)` |
| `SaveBlendState() / RestoreBlendState()` | `d3d::BlendStateGuard guard;` (RAII) |
| `SetBlendState()` / `SetAddBlendState()` | `d3d::SetBlendState()` / `SetAddBlendState()` |
| `obj->flags = OBJ3D_*` | `obj.flags = d3d::Flag_*` |
| `obj->scl.x/y/z = ...` | `obj.scl[0/1/2] = ...` |
| `obj->pos = ...` | `obj.pos[0/1/2] = ...` |
| `obj->lverts[i].tu/tv = ...` | `obj.lverts[i].tu / .tv = ...` |
| `animator->RenderObject(obj)` | `d3d::RenderObject(*Renderer, obj, orientation, texture, &inst_world)` |
| `T3DImagery::GetMaterial(...)` | `d3d::LoadMaterial(img3d, sub_obj, diffuse, emissive)` |
| Per-sub-object I3D mesh registration | `d3d::RegisterSubMesh(*Renderer, img3d, sub_obj, num_tex)` |

If you need a D3D pattern not in this table → don't invent. Name the missing
primitive precisely as a shim-extension gap.

## File template (mirrors `src/effects/fountain.cpp`)

```cpp
#include <cstdint>
#include <cstring>
#include "../d3dport.h"
#include "../logging.h"
#include "../renderer.h"
#include "../imagery.h"
#include "../math3d.h"

extern TRenderer* Renderer;

namespace <effect>_shim {

// === §3 Constants — every value cited from forensics §3 ===
constexpr int32_t kNum<X> = ...;        // forensics §3 cite
constexpr float   k<X>... = ...;
constexpr const char* kImageryPath = "...";

// === Per-effect state ===
struct State {
    // Snapshot's per-instance arrays go here, plus resolved assets
    T3DImagery*    imagery     = nullptr;
    TTextureHandle texture     = kInvalidTexture;
    float          diffuse[4]  = {1,1,1,1};
    float          emissive[4] = {0,0,0,1};
    float          uv_rect[4]  = {0,0,1,1};
    // ... per-particle p[], scale[], rise[], framenum[] etc.
};

// === Initialize — transcribed from forensics §6 / snapshot Initialize ===
static void Initialize(State* st) { /* mechanical from forensics */ }

// === Animate — transcribed from forensics §6 / snapshot Animate ===
static void Animate(State* st)    { /* mechanical from forensics */ }

// === Render — transcribed from forensics §7 / snapshot Render ===
static void Render(State* st) {
    if (!Renderer || !st->alive) return;
    // Lazy texture bind on first call (gotcha #1)
    // ...
    d3d::BlendStateGuard scope;
    d3d::SetBlendState();   // or SetAddBlendState() per forensics §7
    d3d::Matrix inst_world; ...
    d3d::Obj obj; ...
    for (...) {
        obj.flags = d3d::Flag_Scl1 | d3d::Flag_Pos2 | d3d::Flag_Verts;
        // ... write obj.scl, obj.pos, obj.lverts from per-particle state
        d3d::RenderObject(*Renderer, obj, d3d::Orientation::ScreenAligned,
                          st->texture, &inst_world);
    }
}

State* Spawn(const S3DPoint& origin, /* effect-specific args */);
void   Tick(State*);
void   Submit(State*);
void   Destroy(State*);

} // namespace
```

## Four gotchas (every billboard port hits these)

1. **Texture handle may be invalid at Spawn time.** The I3D loader doesn't
   always upload to GPU before your Spawn returns. Cache `T3DImagery*` at Spawn;
   resolve `texture = GetTexture(0).htexture` lazily on the FIRST `Submit()` call.

2. **I3D textures are atlases; sub-objects use UV sub-rects.** A "single
   texture" in the manifest may carry 2x2 or 4x4 colored variants. Resolve the
   per-sub-object UV bbox at Spawn:
   ```cpp
   std::vector<S3DVertex> vbuf(...);
   img3d->GetObjVerts(sub_obj_num, vbuf.data(), 0, 0, ERender3DVertex::Vertex);
   // compute min/max tu/tv → uv_rect
   ```
   Then write the corners to `obj.lverts[]` and set `Flag_Verts`. Otherwise
   you'll render all 4 colors at once.

3. **Snapshot `scale` is a multiplier on authored mesh half-extents.** Our
   `SubmitFxBillboard.size_wu` is the full WU size — multiply the snapshot
   scale by an explicit base (the existing bespokes use ~64 wu for sparkle
   family). Without this, billboards render sub-pixel.

4. **WorldMesh draws (cylinders, rings, planes) MUST go through `submit_world`
   callback, not the regular `submit`.** Calling `SubmitHelperMesh` from
   regular `submit` clears the queue silently. (ScreenAligned billboards work
   from either; submit_world is consistent.) See `src/effect.cpp:8243-8480` for
   the icebolt pattern.

## Validation

1. Build: `cmake --build build -j`
2. Render filmstrip:
   ```bash
   build/revenant --headless --test=vfx --vfx=T<X>_SHIM \
     --filmstrip=9,0.5 --vfx-bg=black \
     --snapprefix=vfx_shim/<effect>/iter1_
   ```
3. Read the filmstrip + compare against:
   - Reference frames in `images/vfx/<NN>_<EffectName>/` (gameplay capture)
   - Existing bespoke render via `--vfx=T<X>_BESPOKE` (A/B sanity)
4. If visual matches → done. If not → re-read forensics + snapshot; don't
   tune by feel.

## Anti-patterns (instant red flags)

- **"Let me try a different blend mode"** — forensics §7 names the blend; if
  it says Alpha, use Alpha. If snapshot is genuinely SUSPECT (§7 sanity-check),
  the forensics will say so; otherwise no creative blend swaps.
- **"I'll multiply by 1.5 to make it bigger"** — no magic numbers. Use the
  forensics-cited constants. If size is wrong, find the missing multiplier
  in the snapshot, don't invent one.
- **"The agent's iter6 used this UV rect, let me do the same"** — never
  inherit decisions from a prior in-progress port. Re-derive from forensics.
- **"This sub-object isn't documented but maybe it's needed"** — if forensics
  §4 doesn't enumerate it as used by THIS animator, it's not yours to render.
- **"The texture didn't load so I'll use a procedural sparkle"** — no
  procedural stand-ins. If asset binding fails, the BLOCKER is in the loader,
  not your port.

## Stopping conditions

Stop and post a gap if:

- Forensics doc is missing a §7 cite for a behavior you need to port
- Shim is missing a primitive for a D3D call you need (name it precisely)
- I3D doesn't contain a sub-object the forensics says it should (asset gap)
- Snapshot lines cited don't actually exist or don't say what forensics
  claims (forensics gap)

Do NOT try to "make it work" by improvising. The whole value of this skill
is mechanical transcription; improvising is exactly what we're avoiding.

## Final report

After validation, briefly report:

- Path to new `src/effects/<effect>.cpp`
- Harness ID registered (`T<Class>_SHIM`)
- Filmstrip output path
- Verdict: CLOSE (visually matches reference/bespoke) / IMPROVED (better than
  no port, recognizable, partial) / BLOCKED (specific gap, named precisely)
- Constants you transcribed (sanity-check vs forensics §3)
- Any shim-extension requests (precise: which D3D pattern, what minimum API
  would cover it)

Working example you can hold up: `src/effects/fountain.cpp` is ~250 lines,
mostly comments, ports `TFountainAnimator` 1:1 from `effect_old.cpp:3797-3887`
with only the shim as the translation layer. A second billboard effect should
take ~30 min following the same template.
