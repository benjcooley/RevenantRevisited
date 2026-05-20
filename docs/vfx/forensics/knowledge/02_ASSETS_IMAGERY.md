# 02 — Asset & Imagery System (original 1998/99 VFX)

> **Scope.** Pure reference for VFX forensics agents. Describes how the original
> Revenant effect system *loaded and addressed its visual assets*: the `.I3D`
> mesh/imagery format, the named imagery registry, per-frame texture handles,
> the chroma-key/premultiply conversion, UV-coordinate animation, and where the
> asset files physically live. Engine-side porting detail (sokol pipelines,
> renderer submission) is a separate doc — this one stops at "how do I find an
> effect's asset and what does that asset contain".
>
> Every concrete claim cites `file:line`. Gaps are flagged inline as **GAP**.

---

## 0. Terminology note — NOMENCLATURE.md is missing

The brief referenced `docs/vfx/forensics/NOMENCLATURE.md` and
`docs/vfx/EFFECT_USAGE_MAP.md` as canonical sources. **Neither file exists in
the tree** (this is the first doc written into `docs/vfx/forensics/`). **GAP:**
until NOMENCLATURE.md is authored, the pipeline codes used here and in
`04_EFFECT_CATALOG.md` come from the legend in `docs/vfx/INVENTORY.md:30-48`:

| Code | Pipeline | Code | Pipeline |
|------|----------|------|----------|
| `FB` | Flipbook billboard | `IM` | I3D mesh effect component |
| `PE` | Particle emitter   | `LS` | Light source |
| `SR` | Strip / ribbon     | `VO` | Volumetric / overlay |
| `MP` | Mesh projectile    | `CX` | Composite (multi-pipeline) |
| `-`  | No render output   |      | |

Blend modes use the names from the renderer port:
`AdditiveStraight` (`D3DBLEND_ONE/ONE`), `PremulAlpha`, and the legacy
straight-alpha `SRCALPHA/INVSRCALPHA` — see §4.

---

## 1. The `.I3D` mesh / imagery format

`.I3D` ("Imagery 3D") is the on-disk format for every animated/static 3D object
in the game, including the meshes used by effects (fireball cores, flame cards,
ripples, spell discs, …). It is loaded into a `T3DImagery` resource, which is a
`TObjectImagery` (`src/3dimage.h:166`, `src/imagery.h:91`).

### 1.1 What an `.I3D` holds

A loaded `T3DImagery` exposes these parallel resource arrays
(`src/3dimage.h:177-191`):

| Member | Type | Meaning |
|--------|------|---------|
| `verts` | `S3DVertex***` | Global vertex list, indexed `[state][frame]` for morph meshes (`src/3dimage.h:177-178`) |
| `faces` | `S3DFace*` | Global triangle/index list (`src/3dimage.h:180-181`) |
| `materials` | `T3DMatArray` | Color/material descriptors + a linked texture index (`src/3dimage.h:183`, `S3DMat` at `:35-40`) |
| `textures` | `T3DTexArray` | Per-texture surface + per-frame texture handles (`src/3dimage.h:184`, `S3DTex` at `:44-56`) |
| `objects` | `T3DObjArray` | **Sub-objects / sub-meshes** (named, hierarchical) (`src/3dimage.h:185`, `S3DObj` at `:60-73`) |
| `tags` | `T3DTagArray` | Per-(state,frame) named tag strings (e.g. sound triggers) (`src/3dimage.h:186`, `S3DTag` at `:77-87`) |
| `motion` | `SMotionData**` | Per-(state,frame) root-motion (dist/vert/angle/rot) (`src/3dimage.h:189`, `GetMotion` `:308`) |
| `icons` | `S3DImageryIcons*` | Inventory bitmap/anim per state (`src/3dimage.h:191-193`) |

A **sub-object** (`S3DObj`, `src/3dimage.h:60-73`) is a named sub-mesh with its
own vertex span (`startvert`/`numverts`), face span (`startface`/`numfaces`),
per-texture face lists (`texfaces`/`numtexfaces`), a material index, an animation
hierarchy (`parent[state]`), and animation keys (`anikeys`/`numanikeys`). This is
the unit an effect addresses when it does `GetObject(0)` — most simple effects
animate sub-object 0.

There are **two on-disk variants**, both decoded by `T3DImagery`:
- `OldInitializeMesh(SOld3DImageryBody*)` — legacy/morph layout
  (`src/3dimage.cpp:177`).
- `InitializeMesh(S3DImageryBody*)` — the shipped layout (`src/3dimage.cpp:380`+;
  dispatch at `:385-388`).

The version/flags come off the mesh header (`flags`/`version`,
`src/3dimage.h:174-175`; `I3D_ISMORPH` selects per-frame morphing vertex sets,
`src/3dimage.cpp:191,1456`).

### 1.2 Naming + enumerating sub-objects

The animator/effect addresses sub-objects by index or by name
(`src/3dimage.h:234-237`):

| API | Purpose | Cite |
|-----|---------|------|
| `NumObjects()` | sub-object count | `src/3dimage.h:234`, impl `src/3dimage.cpp:904` |
| `GetObjectName(objnum)` | name string of sub-object | `src/3dimage.h:235` |
| `GetObjectNum(objname)` | reverse lookup name → index | `src/3dimage.h:236` |
| `GetObject(objnum, &obj)` | copy the `S3DObj` record | `src/3dimage.h:237` |
| `GetObjectParent(objnum, state)` | parent index for hierarchy | `src/3dimage.h:245` |

On the animator side (`T3DAnimator`), the per-instance equivalents are
`GetObjectNum(name)`, `GetObject(objnum)`, and the spatial query
`GetObjectMatrix(objnum, &m)` / `GetObjectPos(...)` / `GetObjectMapPos(...)`
(`src/3dimage.h:463-473`). `GetObjectMatrix` is how an effect reads where a bone
landed after animation (used to attach sub-effects, lights, particles to a moving
mesh node).

### 1.3 Animation: states, frames, ani-keys

`T3DImagery` is **shared** across every `T3DAnimator` built from it
(`src/3dimage.h:210-212`) — vertices/faces/objects/materials/textures are shared
mutable state. Animation is per-(state, frame): `GetAniKey(objnum, state, frame,
pos, rot, scl)` returns the interpolated TRS for a sub-object
(`src/3dimage.h:241`); `GetUninterpolatedAniKey` returns the raw key
(`:239`). State metadata (length, flags, ping-pong, names) is on the
`TObjectImagery` base (`src/imagery.h:304-327`). The per-frame bone-pose hot path
lives in `T3DAnimator::UpdateBoneTransforms` (`src/3dimage.h:440-448`).

---

## 2. The imagery registry

### 2.1 Named asset → id, via `Class.Def`

Effect assets are registered by **name** in the object-class registry file
`legacy/Class.Def` (present in the main checkout; **GAP:** the `legacy/` tree is
`.gitignore`d and absent from VFX worktrees — read it from the main checkout at
`/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited/legacy/Class.Def`).

Effects live under `CLASS "EFFECT"` → `TYPES` (`legacy/Class.Def:2009-2092`).
Each line is:

```
"Name"  "subdir\file.I3D"  0xHASH   [{regpoints}]  [{...}]
```

Examples (`legacy/Class.Def:2014-2092`):

```
"Blood"      "Misc\Blood.I3D"      0xddc4042e
"Fire"       "Misc\Fire.I3D"       0x119f01e3
"Photon"     "Magic\Photon.I3D"    0x113803f4
"FireBall"   "Magic\NewFireBall.I3D" 0x63fd382a
"FlameDisc"  "Magic\FlameDisc.I3D" 0x12daffce
"FireColumn" "Magic\FireColumn.I3D" 0x12da54d2
"MeteorStorm" "Magic\Meteor.I3D"   0xf32bcfac
"Halo"       "Magic\Halo.I3D"      0xdedbdbee
"Ripple"     "Magic\ripples.I3D"   0x12309867
"Drip"       "Magic\drip.i3d"      0x5975abde
"Mist"       "Magic\mist.i3d"      0x2093487a
"IceBolt"    "Magic\icebolt.I3D"   0xb1c4c90f
"Speaker"    "Misc\Speaker.I3D"    0x00000302
```

The hash is an asset content/version key (used by the resource cache). The trailing
`{...}` brace groups are registration points / extra params (mostly absent for
effects — present for equip items like `"Arrow3D"` at `Class.Def:1954`).

`04_EFFECT_CATALOG.md` reproduces the full `CLASS "EFFECT"` list mapped to effect
classes. The complete on-disk block is `legacy/Class.Def:2009-2092`.

### 2.2 Registry API (`TObjectImagery`)

The registry is a flat sparse array keyed by **filename string**
(`src/imagery.cpp` `EntryArray`). The static API (`src/imagery.h:110-148`):

| Function | Behavior | Cite |
|----------|----------|------|
| `RegisterImagery(filename, header?, headersize?)` | Adds a file to the registry, dedupes on filename (case-insensitive), loads the header if not supplied, returns an id. Called by the object-class loader when `Class.Def` is parsed. | `src/imagery.h:111`, impl `src/imagery.cpp:195-330` |
| `FindImagery(name)` | Linear scan of the registry by filename, returns id or `-1`. | `src/imagery.h:133`, impl `src/imagery.cpp:431-445` |
| `LoadImagery(id)` / `LoadImagery(name)` | Builds (or ref-bumps) the `TObjectImagery` for that id via the per-type `TImageryBuilder`. Body is lazy-loaded. | `src/imagery.h:137-139`, impl `src/imagery.cpp:447-487` |
| `GetImageryEntry(id)` | Returns the `SImageryEntry` slot. | `src/imagery.h:115`, struct `:77-89` |
| `SetImageryPath` / `GetImageryPath` | Sets/reads the imagery root dir prefix. | `src/imagery.h:129-131`, impl `src/imagery.cpp:392-398` |

The per-type builder is selected by `header->imageryid`
(`src/imagery.cpp:466`), i.e. `OBJIMAGE_MESH3D` → `T3DImagery`
(`DEFINE_IMAGERYBUILDER(OBJIMAGE_MESH3D, T3DImagery)`, `src/3dimage.h:313`;
registered via `REGISTER_IMAGERYBUILDER(T3DImagery)`, `src/3dimage.cpp:163`).

> **Key forensic point — dedup is by filename, not by name.** Two registry
> *names* can point at the same file (e.g. `"Teleporter"` and `"IrisFlare"` both
> map to `Misc\IrisFlare.I3D`, `Class.Def:2036,2039`; the four `*Font` font
> sparkles all share `Misc\Sparkle.I3D`, `Class.Def:2023-2026`). They resolve to
> the **same** registry id and the **same** shared `T3DImagery`.

### 2.3 Pre-indexed vs. explicit `RegisterImagery`

The `Class.Def` `CLASS "EFFECT"` block is the **pre-indexed** set: those names
get a registry id at class-load time. Everything in that block is loadable by name
without an explicit call.

Assets **not** in a `Class.Def` `TYPES` block (e.g. ad-hoc `Misc\*.I3D` an effect
loads directly, or spell-supplied imagery) must be brought in by an explicit
`RegisterImagery(path)` / `LoadImagery(path)` call — `RegisterImagery` will
load the header on demand if the file is not already registered
(`src/imagery.cpp:207-213`). Spell variants do this via the `IMAGERY "magic\..."`
directive in `spell.def` (e.g. `data/Resources/spell.def:125,203,267,309`),
which supplies a per-cast imagery path rather than relying on the EFFECT registry.

---

## 3. Texture access + the `framehtexs` per-frame handle array

### 3.1 `S3DTex` and `framehtexs[]`

Each entry in `T3DImagery::textures` is an `S3DTex` (`src/3dimage.h:44-56`):

```c
struct S3DTex {
    SSurfaceDesc    desc;        // source surface description
    int32_t         numframes;   // number of frames
    int32_t         framenum;    // current frame index
    TTextureHandle* framehtexs;  // per-frame engine texture handles  <-- the atlas
    bool            copyframes;  // current frame selected by htexture, not a GPU copy
    TTextureHandle  htexture;    // current frame's engine texture handle
};
```

> **`framehtexs[]` is NOT a UV-subrect atlas.** It is an array of *independent
> GPU texture handles* — one fully decoded texture per animation frame
> (`src/3dimage.h:50`, `src/3dimage.cpp:1663-1665`). Selecting a frame swaps the
> bound texture handle; it does **not** offset UVs into a packed sheet. This is
> the distinction that trips up forensics: a flipbook here is N separate textures,
> not one texture with N tiles. (Contrast §5 / §6's "atlas-style" wording in
> some port notes, which describe procedural stand-in textures, not the on-disk
> `.I3D` layout.)

### 3.2 Frame load

In `T3DImagery::LoadTexture` (`src/3dimage.cpp:1637-1760`):

- If `frames > 1`, allocate `framehtexs[frames]` and decode **each frame
  separately** into its own renderer texture asset
  (`src/3dimage.cpp:1661-1666,1750-1751`).
- If `frames <= 1`, `framehtexs` is null and the single texture goes in
  `htexture` (`src/3dimage.cpp:1667-1670,1752-1753`).
- After loading, `htexture` is primed to frame 0: `tex->htexture =
  tex->framehtexs[0]` (`src/3dimage.cpp:1756-1757`).

`AddTexture` is the public entry that allocates the `S3DTex` and calls
`LoadTexture` (`src/3dimage.cpp:1626-1635`). It is called once per texture during
mesh init (`src/3dimage.cpp:322-329` old layout, `:572-579` new layout), pulling
`textureframes[c]` / `textures[c].frames` as the frame count.

### 3.3 Frame selection

| API | Behavior | Cite |
|-----|----------|------|
| `GetTexture(texnum, &tex)` | copies the `S3DTex` record | `src/3dimage.h:281`, impl `src/3dimage.cpp:1783-1788` |
| `GetTextureHandle(texnum)` | current frame's handle (`htexture`) | `src/3dimage.h:282`, impl `:1790-1795` |
| `GetTextureFrame(texnum)` | current `framenum` | `src/3dimage.h:283`, impl `:1807-1812` |
| `SetTextureFrame(texnum, framenum)` | selects a frame: wraps modulo `numframes`, then `htexture = framehtexs[framenum]` (when not `copyframes`) | `src/3dimage.h:284`, impl `:1814-1836` |

The render path picks the frame per sub-object texture slot in
`T3DImagery::RenderObject` (`src/3dimage.cpp:1476-1494`): for textured slots
(`t > 0`), it chooses `texframe = (flags & OBJ3D_TEXFRAME) ? animobj->textureframe[t]
: frame` and calls `SetTextureFrame(t-1, texframe)` (`src/3dimage.cpp:1479-1482`).
So the per-instance frame override comes from
`S3DAnimObj::textureframe[MAXTEXTURES+1]` (`src/3dimage.h:160`) and is only
honored when the `OBJ3D_TEXFRAME` flag is set (`src/3dimage.h:125`); otherwise the
animation's current `frame` drives texture frame selection. **This is the flipbook
mechanism** — writing `textureframe[1] = N` and flagging `OBJ3D_TEXFRAME` plays
texture-slot-1 frame N independently of mesh animation.

---

## 4. Chroma-key + premultiplied alpha

> **The transparency convention is original; the specific detection threshold
> below is port loader code.** `.I3D`-era effect/sprite textures store
> transparency as a **pure black background** (no alpha channel) — that is the
> original asset convention, confirmed by the asset data and the loader comment
> (`src/3dimage.cpp:1684-1688`). The 20 % auto-detect rule is the port's
> reconstruction of how to honor that convention (flagged again in §4.3).

### 4.1 The conversion (auto-fired in the loader)

The current loader auto-detects black-keyed textures and converts them in
`T3DImagery::LoadTexture` after `DecodeTextureFrameRGBA`
(`src/3dimage.cpp:1684-1722`):

**The exact rule** (`src/3dimage.cpp:1701-1721`):

1. Count pure-black pixels (`r==0 && g==0 && b==0`) over the whole frame
   (`:1704-1712`).
2. If `black_count * 5 > px_count` — i.e. **>20 % of the texture is pure black**
   — treat the texture as chroma-keyed (`:1713`).
3. For every pure-black pixel, set `a = 0` (rgb already 0 = premultiplied form);
   leave opaque pixels at `a = 255` (already premultiplied since `a == 1.0`)
   (`:1715-1721`).

### 4.2 What it does to edges + why it matters

Without this, bilinear sampling across the splat→background boundary produces
dark transition pixels (a visible dark fringe / halo) — comment at
`src/3dimage.cpp:1684-1688`. Setting the keyed pixels to **premultiplied** `a=0,
rgb=0` means bilinear interpolation toward the background contributes *nothing*
(neither color nor alpha), so edges blend cleanly.

> **Forensic implication for blend mode.** Bucket/draw consumers must pair this
> conversion with the **`PremulAlpha`** blend mode for correct edges
> (`src/3dimage.cpp:1693-1695`). If a chroma-keyed effect is drawn with plain
> straight-alpha or additive without premultiplying, you'll see the dark fringe or
> washed color — a classic "port broken" signal.

### 4.3 Caveats / GAPs

- The 20 % rule is a **heuristic** stand-in for a missing per-asset flag. The
  code itself flags this: "TODO: drive this from an explicit per-asset flag once
  the tile-set loader gets one" (`src/3dimage.cpp:1697-1700`). **GAP:** a texture
  that legitimately has <20 % black background, or a mostly-black opaque texture,
  will be mis-keyed. Forensics agents debugging a missing/over-transparent effect
  should check whether this heuristic fired correctly.
- This conversion is in the **port** loader. The original D3D3 path's keying
  mechanism is not in this file; **GAP:** confirm the original used the same
  black-key convention against retail decomp before treating the 20 % threshold
  as authentic (the *convention* — black = transparent — is confirmed by the asset
  data and the comment; the *threshold* is a port choice).

---

## 5. UV-coordinate animation (texture scrolling) — distinct from `framehtexs`

> **Author confirmation:** the original effects used UV animation (scrolling
> texture coordinates over time) **extensively**, separate from the per-frame
> texture-handle flipbook in §3. Both exist in the source; here is how to tell
> them apart.

### 5.1 Two independent texture-animation mechanisms

| Mechanism | What changes per frame | Where it lives | Tell-tale in source |
|-----------|------------------------|----------------|---------------------|
| **Frame flipbook** (§3) | the bound *texture handle* (`htexture = framehtexs[N]`) | `T3DImagery` texture state | `SetTextureFrame()`, `textureframe[]`, `OBJ3D_TEXFRAME` |
| **UV scroll** (this section) | the *texture coordinates* (`tu`/`tv`) on the mesh vertices, sampling the *same* texture | per-vertex `S3DVertex.tu/tv` mutated in the animator | `verts[n].tu += du`, `lverts[i].tu += ...`, `ScrollTexture()` |

A given effect can use one, the other, or both.

### 5.2 Per-vertex UV scroll (mesh effects)

Many mesh effects scroll the texture by mutating `tu`/`tv` on the sub-object's
vertices each `Animate()` call:

- **`TFaultFireAnimator`** — scrolls `tu` by a random `du = random(2,8)/100` per
  frame across all verts of sub-object 0 (`src/effect_old.cpp:11174-11179`); also
  advances a `th` angle phase (`:11170-11172`). This is rising-fire-on-a-fault
  via a continuously scrolling flame texture on a fixed mesh.
- **`TSymGlowAnimator`** — scrolls `tu` by a member `u` per frame
  (`src/effect_old.cpp:4651-4652`) and pulses scale (`:4654-4656`). Glowing
  rune/sigil overlay.
- **Barrier walls** — `TBarrierAnimator` (and the commented retail original)
  scroll both `tu` and `tv` by `0.01` per frame across the barrier mesh
  (`src/effect_old.cpp:4601` commented-original, `:6177-6178` live).
- Generic helper: `T3DImagery`/animator code adds a UV offset across a vertex
  span at `src/effect_old.cpp:4652` (`obj->verts[n].tu += u`) and
  `:11178` (`obj->verts[n].tu += du`).

In `effect2.cpp` the same pattern recurs on lit-vertex (`lverts`) meshes:
`TParaEffect` scrolls `tu`/`tv` by `val*80` (`src/effect2.cpp:649-650`), and
another effect scrolls `tu` by `0.06` per vertex group (`:1920,1935`).

### 5.3 Strip / ribbon UV scroll (the `SR` pipeline)

Strip effects scroll the texture along the U axis as the ribbon flows. The
canonical mechanism is `TStripAnimator::ScrollTexture(deltau)`
(`src/effectcomp.cpp:863-871`): it advances a `uoff` offset and wraps it within
`[ubeg, uend]`. The lightning/strip animator drives it each frame:
`TLightningAnimator` calls `mystrip->ScrollTexture(-0.1f)` per frame
(`src/stripeffect.cpp:817`; described `:31`). This is what makes a lightning bolt
or wind streak appear to "flow" along its length.

The U range/frequency is set up in the strip generator
(`TStripAnimator::SetupU`-style code, `src/effectcomp.cpp:855-860`; strip build at
`GenerateStrip`, `:873`+).

### 5.4 How to tell which an effect uses (forensic checklist)

1. Grep the animator's `Animate()` for `.tu`/`.tv +=` or `ScrollTexture(` →
   **UV scroll**.
2. Grep for `SetTextureFrame(` / `textureframe[` / `OBJ3D_TEXFRAME` →
   **frame flipbook**.
3. If neither, the texture is static and only the mesh transform / particle
   system animates.

**GAP:** UV scroll *rate* fields are mostly hard-coded constants in the animator
bodies (e.g. `-0.1`, `0.01`, `0.06`, `random(2,8)/100`), not data-driven. There
is no central "uv scroll rate" asset field in `S3DTex`/`S3DObj`; the animation is
imperative. Forensics agents porting an effect must read the scroll rate out of
the specific `Animate()` body, not from the asset.

---

## 6. How the original addressed sub-mesh geometry for drawing

This doc describes the *original* asset format and access; how the modern port
re-extracts geometry for sokol (`ExtractSubMeshTextureSlot` /
`BuildAnimPoseObjectMatrix` in `src/meshextract.cpp`) is reconstruction-side —
**see the reconstruction/rendering docs**, not here.

In the original, drawing a sub-object went through
`T3DImagery::RenderObject(animobj, state, frame, …)` (`src/3dimage.h:254`), which
per texture slot walks the sub-object's vertex span (`obj->startvert` …
`+numverts`, `src/3dimage.cpp:1456-1459`) and per-slot face span
(`obj->startface + obj->texfaces[t]`, count `obj->numtexfaces[t]`,
`src/3dimage.cpp:1469-1471`), selects the texture frame (§3.3), and issues the
draw (`Scene3D.DrawIndexedPrimitive`, `src/3dimage.cpp:1508`). UVs come straight
off the stored `S3DVertex.tu/tv`. That is the complete original geometry-access
path: spans into the shared global vertex/face lists, addressed by sub-object and
texture slot.

---

## 7. Asset locations on disk

### 7.1 The `legacy/Imagery/` tree

Effect assets live under `legacy/Imagery/` in two subdirs (read from the main
checkout — see §2.1 GAP about `legacy/` being gitignored):

```
legacy/Imagery/
  Magic/   <- spell-cast effect meshes (Fire, FireBall, FireColumn, FlameDisc,
              Photon, Meteor, Tornado, Vortex, Halo, ripples, drip, mist,
              icebolt, iced, faultfire, fireswarm, fireflash, firewind, ...)
  Misc/    <- ambient / utility / shared effect meshes (Blood, fire, flare,
              Sparks, Sparkle, IrisFlare, Pulp, Smoke, Speaker, water, ...)
  Chars/ Equip/ Cave/ Dungeon/ Forest/ Keep/ ... <- world & character imagery
```

(Directory listing confirmed at the main-checkout `legacy/Imagery/`.)

### 7.2 Magic-vs-Misc duplicate-asset pattern

Several effect assets exist in **both** `Magic/` and `Misc/` as near-duplicates,
with the `Class.Def` registry pointing at one of them:

| Asset | `Magic/` size | `Misc/` size | `Class.Def` points to |
|-------|--------------:|-------------:|-----------------------|
| `Fire.I3D` / `fire.i3d` | 278 048 | 277 600 | `Misc\Fire.I3D` (`Class.Def:2015`) |
| `drip.i3d` | 8 916 | 8 916 | `Magic\drip.i3d` (`Class.Def:2068`) |
| `mistfog.i3d` | 8 916 (Magic) | (Misc copy) | both registered (`Class.Def:2070,2074`) |
| `FireBall.I3D` | 144 780 (also `NewFireball.I3D` 144 664) | 10 372 | `Magic\NewFireBall.I3D` (`Class.Def:2031`) |

> **Forensic rule:** to find an effect's *actual* asset, resolve the registry
> name in `Class.Def` (which dir + filename it points to), then read **that**
> file — don't assume the `Magic/` copy when the registry says `Misc/` (or vice
> versa). The `Magic/Fire.I3D` and `Misc/fire.i3d` differ slightly in size and are
> not interchangeable. There are also stale dev variants in `Magic/`
> (`Meteor-bad.I3D`, `TestFireBall.I3D`, `RedPhoton.I3D`) that are *not*
> registered — ignore them unless tracing dev history.

### 7.3 Finding an effect's asset + size (recipe)

1. Effect class → registry name: see the `DEFINE_BUILDER("Name", TFooEffect)`
   line in `04_EFFECT_CATALOG.md` (or `src/effect_old.cpp` /
   `src/effect2.cpp` / `src/missileeffect.cpp` / `src/stripeffect.cpp`).
2. Registry name → file path: `grep "Name" legacy/Class.Def` in the
   `CLASS "EFFECT"` block.
3. File path → bytes: `ls -la legacy/Imagery/<dir>/<file>` in the main checkout.

### 7.4 `data/imagery.rvi`

`data/imagery.rvi` is a **ZIP archive** (`PK\x03\x04`, stored/uncompressed) of the
runtime resource set — it contains the `.def` text files (e.g. `weapon.def`) and
imagery resources, not a separate text index. (Confirmed: `file data/imagery.rvi`
→ "Zip archive data … compression method=store"; first member is `weapon.def`.)
**GAP:** there is no plain-text `imagery.rvi` *index* listing effect assets — the
authoritative effect→file map is `Class.Def`'s `CLASS "EFFECT"` block (§2.1), not
the `.rvi`. Enumerate `.rvi` contents with any zip tool if you need the packaged
file list.

---

## 8. Sounds — audio coupling (record-only; audio port out of VFX scope)

Effects can trigger SFX through two source paths:

- **Imagery tag sounds.** `T3DImagery::PlaySound(inst, state, frame)`
  (`src/3dimage.h:257`) fires sounds attached to the mesh's per-(state,frame)
  `S3DTag` entries (`src/3dimage.h:77-88`, `FindTag` `:296`). This is the
  data-driven "play this sound on frame N of this animation" mechanism baked into
  the `.I3D`.
- **Ambient-sound effects.** `TAmbSoundEffect` / `TAmbSoundAnimator` (registry
  name `"Speaker"`, `Misc\Speaker.I3D`) is a render-less effect whose entire job
  is positional audio — see `04_EFFECT_CATALOG.md` row X02. Defined
  `src/effect_old.cpp:13629-13757` (`DEFINE_BUILDER("Speaker", TAmbSoundEffect)`
  `:13629`).

Some effect bodies also issue direct sound plays in their `Pulse()`/`Animate()`
(e.g. drip/water landing sounds). **GAP:** a full effect→SFX map was not built
here (audio is out of VFX scope). Forensics should record, per effect, *whether*
it couples to audio (tag-driven vs. direct play vs. Speaker) so the audio port can
pick it up later; this is noted per-row where visible in `04_EFFECT_CATALOG.md`.

---

## 9. Cross-references

- **Effect taxonomy + which asset each effect loads:**
  `docs/vfx/forensics/knowledge/04_EFFECT_CATALOG.md`.
- **Port status / pipeline assignment per effect:** `docs/vfx/INVENTORY.md`.
- **For the architecture/rendering forensics doc (parallel agent):** the
  original per-effect blend selector is the `SetBlendState`
  (`SRCALPHA/INVSRCALPHA`) vs `SetAddBlendState` (`ONE/ONE`) split at
  `src/effect_old.cpp:221-244` — that, plus the black-key transparency convention
  (§4) and the two distinct texture-animation mechanisms (frame flipbook §3 vs.
  UV scroll §5), are the original drawing facts that doc should carry. Engine
  reimplementation mapping is reconstruction-side.
