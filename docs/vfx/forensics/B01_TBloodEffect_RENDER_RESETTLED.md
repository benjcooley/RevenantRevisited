# B01 TBloodEffect — Render-state re-examination (clean-slate)

| field | value |
|-------|-------|
| **Scope** | Three questions only: Q1 blend / depth / cull / passes, Q2 particle kind, Q3 transform space. |
| **Trigger** | User has rejected the current port's render-state read (Alpha base + AdditiveStraight overlay, with `EFxPipeline::Billboard`). User is 100% certain blood is **NOT transparent** and **NOT additive**. Re-examined all sources from scratch. |
| **Author / date** | vfx-forensics-agent / 2026-05-29 |
| **Headline** | The previous forensic doc correctly transcribed what the snapshot literally writes — but the snapshot's writes are **dormant**. Neither pass enables `D3DRENDERSTATE_ALPHABLENDENABLE`; on DX6/7 with `ALPHABLENDENABLE=FALSE` the SRC_ALPHA/INV_SRC_ALPHA factors (k=0) and the ONE/ONE factors (k=1) do **nothing**. The texture's black background is masked via **1-bit alpha encoded into the texture by the loader** (`legacy/3dimage.cpp:2079`: *"Should already have keycolor encoded as 1 bit alpha"*). Effective per-pixel behavior is therefore **chroma-keyed opaque** (alpha-test style) for both passes, with the per-vertex `D3DRGB` grayscale modulating the texture. Calling either pass "alpha-blended" or "additive" in the port mistakes dormant register state for visible behavior. |
| **Q-summary** | A1: chroma-keyed (1-bit alpha) opaque, TestNoWrite, no cullmode forcing, scene-lit modulate. A2: textured **4-vert / 2-tri indexed `TriangleList` quad mesh** out of `Misc\Blood.I3D` (8 sub-objects, byte-identical to shipped). A3: **world-space** with per-droplet world matrix; no camera-facing step anywhere. |

---

## §1. RETAIL EVIDENCE

**Retail fidelity for the TBloodSystem render-state body = UNVERIFIED.**

Confirmed retail fingerprints that DO exist in `recon/`:

- `"Blood"` builder/dispatch string at retail data `005df9c0` and `005e11e8`
  (`recon/classes/_data.txt:104235,107565`); the `005e11e8` copy XREFs
  into `cls_0x5acaa8` (`recon/mappings/TBloodEffect_cls_0x5acaa8_candidate.yaml:9`).
- `"Misc\Blood.I3D"` string at retail data `005d7470`, XREF'd by the
  imagery registration `virt_meth_0x47a660` (`recon/classes/_data.txt:86491-86492`).
- TBloodEffect leaf class allocator `0x110` = 272 bytes with
  `vftable_5acaa8` (`recon/classes/cls_0x5acaa8.cpp:124,132`) —
  consistent with a small TEffect shell, matches the snapshot's
  near-empty `TBloodEffect`.
- Caller wiring: `recon/discovered/cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp`
  + `recon/discovered/cls_TCharacter_EffectBurst_4c85d0.cpp` confirm the
  `"blood"` effect-spawn name + the vtable+0x200 `SetParams` calls + the
  `face - 0x80` (180° offset) hangle math. This is **retail-confirmed
  combat wiring** — but it does NOT tell us anything about TBloodSystem's
  render state.

What is **NOT** in `recon/`:

- No `TBloodSystem::Render` body. The `TBloodSystem` class is embedded
  in the blood animator class; Ghidra merged that cluster with TMistEffect
  / TMistFogEffect / TWaterFallEffect and did not isolate Render/Animate
  bodies (per `..._candidate.yaml:26` which calls for a Ghidra rerun).
- Zero hits for the TBloodSystem render fingerprints. I searched
  `recon/classes/` and `recon/discovered/` for:
  - `TBloodSystem`, `TBloodAnimator` → 0 hits
  - `GetClosestLights` (DoLighting fingerprint) → 0 hits
  - `D3DTBLEND_MODULATE`, `D3DBLEND_ONE` inline blend writes → 0 hits
  - The blood asset id `0xddc4042e` → 0 hits
- No retail evidence either way on whether retail kept the 2-pass
  design, kept the `MODULATE`+ONE/ONE inline, or whether retail
  explicitly enabled `ALPHABLENDENABLE` / `ALPHATESTENABLE` in the
  blood path.

**Conclusion §1:** all Q1/Q2/Q3 answers below are derived from
**snapshot source + on-disk .I3D + retail caller wiring**. Retail
fidelity on the render state is **UNVERIFIED — Ghidra decomp not
located after searching recon/classes and recon/discovered for
TBloodSystem / TBloodAnimator / GetClosestLights / 0xddc4042e**.

---

## §2. SNAPSHOT EVIDENCE (verbatim cites)

All line numbers refer to `src/` in this worktree (a checkout of the
main repo, snapshot source unchanged).

### §2.1 Class definitions — `src/effectcomp.h:356-402`

```cpp
356  _STRUCTDEF(SBloodParticle)
357  struct SBloodParticle
358  {
359      hmm_vec3 vel, pos;
360      float scl;
361      bool used;
362      uint8_t size, stage, count, delay;
363  };
364
365  _STRUCTDEF(SBloodSystemParams)
366  struct SBloodSystemParams
367  {
368      T3DAnimator* a;
369      S3DAnimObj* s, m, b, sp, s2, m2, b2, sp2;
370      int32_t maxsize, num, height, hangle, vangle, hspread, vspread;
371      S3DPoint zbuf, effectpos;
372  };
373
374  #define MAX_BLOODS      30
375
376  _CLASSDEF(TBloodSystem)
377  class TBloodSystem
378  {
379    private:
380      SBloodParticle* blood;
381      T3DAnimator* animator;
382      S3DAnimObj* sml, med, big, spl, sml2, med2, big2, spl2;
383      S3DPoint size, eff;
384      bool done;
385      float height, bifscale;
386      int32_t bifon;
387    ...
388      virtual void Init(SBloodSystemParams me);
389      virtual void Animate();
390      virtual void Render();
391      virtual void RefreshZBuffer();
392      virtual void DoLighting(float x, float y, float z, S3DAnimObj* object);
393  };
```

Eight sub-object slots = two pass-sets of four
(`sml/med/big/spl` + `sml2/med2/big2/spl2`). Mapped from the .I3D in
`TBloodAnimator::Initialize` (`src/effect_old.cpp:11284-11291`):
`s=GetObject(4) box05, m=5 box06, b=6 box07, sp=7 box08, s2=0 box01,
m2=1 box02, b2=2 box03, sp2=3 box04`.

### §2.2 `SetBlendState()` body — `src/effect_old.cpp:221-233`

```cpp
221  bool SetBlendState()
222  {
223      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
224      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
225      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
226      //if (D3DHWCaps == "FIRE GL 1000")
227      //{
228      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
229      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));
230      //}
231
232      return true;
233  }
```

Sets MODULATE, ZWRITE=off, ZENABLE=on, SRC=SRCALPHA, DEST=INVSRCALPHA.
**Does NOT touch `D3DRENDERSTATE_ALPHABLENDENABLE`. Does NOT touch
CULLMODE. Does NOT touch `COLORKEYENABLE`. Does NOT touch
`ALPHATESTENABLE`.**

### §2.3 `SaveBlendState()` / `RestoreBlendState()` — `src/effect_old.cpp:181-210`

```cpp
181  bool SaveBlendState()
182  {
183      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, &SaveBlendMode));
184      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &SaveZWriteEnable));
185      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_ZENABLE, &SaveZEnable));
186      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &SaveCullMode));
187      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_SRCBLEND, &SaveSrcBlend));
188      TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_DESTBLEND, &SaveDestBlend));
189      return true;
190  }
...
200  bool RestoreBlendState()
201  {
202      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, SaveBlendMode));
203      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, SaveZWriteEnable));
204      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, SaveZEnable));
205      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, SaveCullMode));
206      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, SaveSrcBlend));
207      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, SaveDestBlend));
208      return true;
209  }
```

**Six states captured / restored. ALPHABLENDENABLE is NOT among them.**
So the entire "BlendState" helper family is silent about whether alpha
blending is actually ON.

### §2.4 `SetAddBlendState()` body (NOT called by Blood) — `src/effect_old.cpp:235-244`

```cpp
235  bool SetAddBlendState()
236  {
237      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECALALPHA));
238      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
239      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
240      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
241      TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
242      return true;
243  }
```

For reference. Texture stage = `D3DTBLEND_DECALALPHA` (alpha-blended
decal). **Blood does NOT call this helper** — it inlines its k=1
state with `MODULATE`, not DECALALPHA. That is unusual: every other
additive effect in `effect_old.cpp` calls `SetAddBlendState()` (see
TCureAnimator:1915, TFireFlashAnimator:2398, TFireWindAnimator:3097,
TBurnAnimator:3452, TAuraAnimator:3660, TCreateFoodAnimator:1273).
**Blood is the ONLY effect that hand-rolls its second pass with
MODULATE+ONE/ONE.** That smells like dev experimentation / WIP, not
shipped intent.

### §2.5 `TBloodAnimator::Render` (outer wrapper) — `src/effect_old.cpp:11326-11335`

```cpp
11326  bool TBloodAnimator::Render()
11327  {
11328      SaveBlendState();
11329
11330      bloods.Render();
11331
11332      RestoreBlendState();
11333
11334      return true;
11335  }
```

Save 6 states, run `TBloodSystem::Render`, restore the 6 states. No
other state touched.

### §2.6 `TBloodSystem::Render` body — `src/effectcomp.cpp:1415-1530`

Reproduced verbatim (the live block — lines :1485-1529 are commented-out
"bif" splash dead-code):

```cpp
1415  void TBloodSystem::Render()
1416  {
1417      PS3DAnimObj object;
1418
1419      // basically cycle through the particles and render them...duh!
1420      for(int32_t i = 0; i < MAX_BLOODS; ++i)
1421      {
1422          if (!blood[i].used)
1423              continue;
1424
1425          for (int32_t k = 0; k < 2; k++)
1426          {
1427              if (k == 0)
1428              {
1429                  RestoreBlendState();
1430                  SetBlendState();
1431              }
1432              else
1433              {
1434                  RestoreBlendState();
1435                  TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
1436                  //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
1437                  TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
1438                  TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
1439                  //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
1440              }
1441
1442              if (k == 0)
1443              {
1444                  object = sml;
1445                  if (blood[i].size == 1)
1446                      object = med;
1447  //              else if (blood[i].size == 2)
1448  //                  object = big;
1449              }
1450              else
1451              {
1452                  object = sml2;
1453                  if (blood[i].size == 1)
1454                      object = med2;
1455  //              else if (blood[i].size == 2)
1456  //                  object = big2;
1457              }
1458
1459              animator->ResetExtents();
1460
1461              object->flags = OBJ3D_MATRIX | OBJ3D_VERTS;
1462              D3DMATRIXClear(&object->matrix);
1463
1464              object->scl.x = object->scl.y = object->scl.z = blood[i].scl;
1465              D3DMATRIXScale(&object->matrix, &object->scl);
1466
1467              if (blood[i].stage == BLOOD_FLY)
1468              {
1469                  D3DMATRIXRotateX(&object->matrix, -(float)(90 * TORADIAN));
1470                  D3DMATRIXRotateZ(&object->matrix, -(float)(M_PI / 4.0));
1471              }
1472
1473              object->pos.x = blood[i].pos.x;
1474              object->pos.y = blood[i].pos.y;
1475              object->pos.z = (float)(blood[i].pos.z - height);// + (blood[i].scl - 1.0) * 10);
1476              D3DMATRIXTranslate(&object->matrix, &object->pos);
1477
1478              DoLighting(blood[i].pos.x, blood[i].pos.y, blood[i].pos.z, object);
1479
1480              animator->RenderObject(object);
1481              animator->UpdateExtents();
1482          }
1483      }
```

#### What is and is NOT written, pass-by-pass

- **k=0** (`:1429-1430`): `RestoreBlendState()` resets the 6 captured
  states, then `SetBlendState()` (§2.2) writes MODULATE + ZWRITE=off +
  ZENABLE=on + SRC=SRCALPHA + DEST=INVSRCALPHA.
  - **NOT written**: ALPHABLENDENABLE, ALPHATESTENABLE, CULLMODE (inherits
    from `SaveBlendState` capture), COLORKEYENABLE.
- **k=1** (`:1434-1440`): `RestoreBlendState()` again, then 3 inline
  writes: MODULATE, SRC=ONE, DEST=ONE. **ZWRITEENABLE=false (:1436)
  and ZENABLE=true (:1439) are COMMENTED OUT**, so k=1 inherits
  whatever the prior `RestoreBlendState()` restored to (the scene's
  pre-effect Z state — typically ZWRITE=true / ZENABLE=true on the
  shipped mesh pipeline).
  - **NOT written**: ALPHABLENDENABLE, ALPHATESTENABLE, CULLMODE,
    COLORKEYENABLE, and crucially ZWRITE/ZENABLE.

#### The dormant-register catch — `ALPHABLENDENABLE` is NEVER enabled

Audit (this worktree):

```
grep -rln "ALPHABLENDENABLE" src/
# only:
#   src/stripeffect.cpp     (TStripAnimator @ :1264, :1273, :1361)
#   src/effectcomp.cpp      (TShockWaveAnimator @ :701, :759, :766)
```

So only TStripAnimator and TShockWaveAnimator explicitly flip
`ALPHABLENDENABLE`. TBloodSystem, TBloodAnimator, SetBlendState,
SetAddBlendState, and 3DScene init do NOT. (Cross-checked legacy:
`legacy/charanimator.cpp:1096` TWeaponSwipe also enables it. Nothing
else in legacy either.)

Direct3D 6/7 default for `D3DRENDERSTATE_ALPHABLENDENABLE` is **FALSE**.
When FALSE, the rasterizer **ignores SRCBLEND/DESTBLEND entirely** and
writes texture×diffuse straight to the destination. So in the literal
snapshot:

- **k=0** (intended "alpha base") actually renders as **OPAQUE
  modulated texture** — SRC_ALPHA/INV_SRC_ALPHA are dormant.
- **k=1** (intended "additive overlay") also renders as **OPAQUE
  modulated texture** — ONE/ONE are dormant, the second pass simply
  **OVERWRITES** the first with the box01..box04 sub-object set
  (because ZWRITE was off, so k=0 didn't shut k=1 out via depth).

This is the snapshot's actual on-screen behavior in isolation. The
black-keyed background of the texture would render as **solid black**
unless something else masks it.

#### The masking mechanism — 1-bit alpha baked into the texture

`legacy/3dimage.cpp:2079` (the imagery loader's
`ConvertTexture` call site) carries the comment:

```
Scene3D.ConvertTexture(srcsd, pixels[f], palette, &dstsd, dstpixels, dstpal);
    // Should already have keycolor encoded as 1 bit alpha
```

So the loader converts the 16-bit RGB565 textures into a format with
the keycolor (the texture's black background) baked as a **1-bit
alpha** channel: pixel-alpha 1 = visible droplet, pixel-alpha 0 =
masked. For the 1-bit alpha to actually MASK the destination, you
need either:

1. `ALPHATESTENABLE=TRUE` with `ALPHAREF=0` / `ALPHAFUNC=GREATER` — a
   per-pixel kill of alpha-0 fragments (binary cut), OR
2. `ALPHABLENDENABLE=TRUE` with the SRC_ALPHA/INV_SRC_ALPHA factors —
   blend that multiplies by binary alpha (effectively the same cut).

The snapshot writes the factors for option 2 but never enables either.
**Neither alpha-test nor alpha-blend is on, so the 1-bit alpha is
silently ignored, and the black background draws as solid black.**
That is almost certainly NOT shipped behavior — retail would have
flipped at least one of ALPHATESTENABLE / ALPHABLENDENABLE on
(globally or in the blood path) to make the 1-bit alpha mask
actually fire.

**Most likely shipped behavior (cannot be retail-confirmed but is
strongly implied by:** (a) the texture is loaded with the 1-bit alpha
mask, which only makes sense if the runtime consumes it; (b) the
snapshot is pre-release WIP and TWeaponSwipe at
`legacy/charanimator.cpp:1096` shows the engine did know to enable
ALPHABLENDENABLE per-effect; (c) the user's gameplay-memory says
blood is not transparent and not additive):

→ **`ALPHATESTENABLE=TRUE`, `ALPHAREF=0`, `ALPHAFUNC=GREATER`**
(or equivalent), giving a **chroma-keyed opaque** draw. Each droplet
is one fully-opaque quad with the black background cut out. The
per-vertex `D3DRGB` grayscale from `DoLighting` (§2.7) modulates the
visible (dark-red) pixels. **No blending. No additive. No
transparency.** Z behavior = TestNoWrite (test scene depth so droplets
don't z-fight behind walls, don't write depth so the per-droplet quads
don't break later effects' depth reads).

The "second pass" (k=1) is also opaque and just OVERWRITES the first
with a slightly different sub-object set (box01..box04 vs box05..box08).
There's no visual reason to do this when alphatest is on; the most
plausible reading is that the second pass is **dead-code-in-WIP** —
either left over from an experiment, or intended for a feature that
never landed (the bif splash is in the same Render body, also dead
code, commented out at :1485-1529; the big/med trail loops in
Init :1242-1283 are also dead, also commented out). Shipping retail
**probably draws ONE pass, not two**, but I cannot prove that from the
recon decomp.

### §2.7 `TBloodSystem::DoLighting` — `src/effectcomp.cpp:1372-1413`

```cpp
1372  void TBloodSystem::DoLighting(float x, float y, float z, PS3DAnimObj object)
1373  {
1374      float r, g, b, temp;
1375      r = g = b = 0;
1376      int32_t l1, l2, l3, tx, ty, tz;
1377      tx = (int32_t)(eff.x + x);
1378      ty = (int32_t)(eff.y + y);
1379      tz = (int32_t)z;
1380      ...
1381      Scene3D.GetClosestLights(tx, ty, tz, l1, l2, l3);
1382      ...                            // brightness sum up to MaxLights lights
1400      temp = (float)(MapPane.GetAmbientLight() / 255.0f);
1402      r += temp * 4.0f;
1404  #define DIVISOR  1.5f
1405      if (r > DIVISOR)
1406          r = DIVISOR;
1407      r /= DIVISOR;
1408      g = b = r;
1409      for(int32_t j = 0; j < 4; j++)
1410      {
1411          object->lverts[j].color = D3DRGB(r, g, b);
1412      }
1413  }
```

Writes **grayscale** (`r=g=b`) to the per-vertex diffuse color of all
4 verts via `D3DRGB(r,g,b)`. `D3DRGB` packs `0xFF000000 | rgb`, so the
**vertex-color alpha is forced to 1.0** every frame, every droplet,
every vertex. This is structural confirmation that there is **no
per-droplet alpha fade** — the only "fade" is the SHRINK stage's scale
ramp.

Loop counter `j < 4` is a tight piece of evidence for §3: each
sub-object has exactly 4 lverts → 4-vertex quad mesh.

---

## §3. .I3D ASSET EVIDENCE

The brief mentions `Particles\sml.i3d` / `Particles\med.i3d` — those
paths do NOT exist in `data/imagery.rvi` (the retail archive). They
belong to a different effect (the prior forensics show
`Particles\Sparks.I3D` is the sparks effect; blood uses ONLY
`Misc\Blood.I3D`, not split per droplet size). For blood, the only
asset is **`Misc\Blood.I3D`** with 8 box sub-objects.

### §3.1 File identity

| | path | size | MD5 |
|---|---|---|---|
| legacy (snapshot) | `legacy/Imagery/Misc/blood.i3d` | 68820 B | `9c0596112cd46d6e3cb91df9f97770df` |
| legacy (also) | `legacy/Imagery/Magic/blood.i3d` | 68820 B | `9c0596112cd46d6e3cb91df9f97770df` |
| **retail** (shipped) | `data/imagery.rvi:Imagery/Misc/blood.i3d` | 68820 B | `9c0596112cd46d6e3cb91df9f97770df` |

**All three are byte-identical.** Confirmed by extracting the retail
archive (`unzip data/imagery.rvi Imagery/Misc/blood.i3d`) and running
`md5`. **Asset is shipped-identical and reconstruction must use it
literally — no procedural substitute.**

### §3.2 File-layout walk (parsed directly)

File header is the standard `CGSR` resource:

```
0x00..0x13  FileResHdr (`src/resourcehdr.h:17-28`, 20 bytes):
  0x00:  'C','G','S','R' = RESMAGIC
  0x04:  topbm        = 0
  0x06:  comptype     = 0
  0x07:  version      = 1
  0x08:  datasize     = 0x00010C6C  (68716 bytes of payload)
  0x0C:  objsize      = 0x00010C6C
  0x10:  hdrsize      = 0x00000054  (84 bytes of header)
0x14..0x67  SImageryHeader (84 bytes hdrsize):
  0x14:  imageryid    = 1 = OBJIMAGE_MESH3D (`src/imageres.h:36`)
  0x18:  numstates    = 1
  0x1C:  animname[0]  = "STILL" (state's animname[32])
  ...
0x68..end   S3DImageryBody (body)
```

`LoadResource` does `fseek(fl, frh.hdrsize + sizeof(FileResHdr), 0)`
(`src/resource.cpp:96`), confirming body begins at file offset 0x14 +
0x54 = **0x68**. Reading the body fields directly:

| field | value (hex / dec) |
|---|---|
| `flags` | `0x0000003C` = `I3D_HASICONS \| I3D_HASHIERARCHY \| I3D_FACINGMOTION \| I3D_3DIMAGEBODY2` |
| `version` | 1 |
| `numverts` | `0x20` = **32** |
| `numfaces` | `0x10` = **16** |
| `numobjects` | `0x08` = **8** |
| `numtextures` | 2 |

(`grep -aob "box0[0-9]" blood.i3d` reports `box01` at file offset 180
(0xB4), `box02` at 228 (0xE4), then every 48 bytes through `box08` at
516 (0x204) — confirming the 8-entry object table with the documented
48-byte `S3DImageryObject` stride.)

### §3.3 Sub-object geometry (this is the load-bearing read)

- **8 sub-objects** (box01..box08) × **4 verts each** = 32 total verts.
- **8 sub-objects** × **2 triangles each** = 16 total faces.
- → Each sub-object is a **planar 4-vertex / 2-triangle quad mesh**.

Reading the first sub-object's verts directly (S3DVertex = pos[3 floats]
+ normal[3 floats] + uv[2 floats] = 32 bytes; verts span starts at
file offset 0x2DC):

```
vert 0:  pos = (-5.0258, -5.0258,  0.0)  normal = (~0, ~0, ~1.0)  uv = (0.508, 0.510)
vert 1:  pos = (+5.0258, -5.0258,  0.0)  normal = (~0, ~0, ~1.0)  uv = (0.982, 0.510)
vert 2:  pos = (-5.0258, +5.0258,  0.0)  normal = (~0, ~0, ~1.0)  uv = (0.508, 0.984)
vert 3:  pos = (+5.0258, +5.0258,  0.0)  normal = (~0, ~0, ~1.0)  uv = (0.982, 0.984)
```

(`pos.x = 0xC0A0D3A6` decodes to `-5.0258`; `normal.z = 0x3F800000` = 1.0.)

**All 4 verts have `pos.z = 0.0` exactly.** It is a **flat 10×10 wu
quad in the local XY plane**, normal facing +Z. UVs map roughly the
upper-right quadrant of the 128×160 texture sheet — i.e. one of the
droplet/splat cells. **NOT a tetrahedron. NOT a cube. NOT a volumetric
mesh.** It is one rectangle, two triangles. (`box02..box08` follow the
same 4-vert / 2-tri layout — the global counts in §3.2 prove this.)

### §3.4 Textures

- `numtextures = 2` (one main sheet + one auxiliary).
- 16-bit RGB565 (per the prior forensic doc and consistent with the
  header texture-descriptor format — a 128×160 sheet with 64×120
  sub-cells).
- **Source format has NO alpha channel.** The transparency mechanism
  is the loader's "keycolor encoded as 1 bit alpha" conversion
  (`legacy/3dimage.cpp:2079`) which examines each pixel against the
  bitmap's keycolor (the texture's black background) and writes a 1-bit
  alpha channel into the converted GPU texture. The runtime then has to
  consume that 1-bit alpha (via alpha-test or alpha-blend) — which, as
  §2.6 documents, the snapshot's blood pass does NOT do.

---

## §4. `animator->RenderObject` path

The Render body submits each per-tick configured sub-object via
`animator->RenderObject(object)` (`effectcomp.cpp:1480`). `animator`
is a `T3DAnimator*` (stored in TBloodSystem from
`me.a = this` at `TBloodAnimator::Initialize`, `effect_old.cpp:11283`).

`T3DAnimator::RenderObject` is an inline forward — `src/3dimage.h:467-468`:

```cpp
467  bool RenderObject(S3DAnimObj* animobj, int32_t tex = -1, bool uselastmatrix = false)
468    { return Get3DImagery()->RenderObject(animobj, state, frame, &matrix, tex, uselastmatrix); }
```

`T3DImagery::RenderObject` body — `src/3dimage.cpp:1364-1519`. Key cites:

```cpp
1389      // ---- position ----
1390      if (!(animobj->flags & OBJ3D_MATRIX) && !uselastmatrix)
1391          CalcObjectMatrix(animobj, state, frame, pos, false);
...
1395      if (!(flags & I3D_ISMORPH) ||
1396          (animobj->flags & (OBJ3D_POSMASK | OBJ3D_ROTMASK | OBJ3D_SCLMASK | OBJ3D_MATRIX)))
1397      {
1398          if (animobj->flags & OBJ3D_ABSPOS)
1399          {
1400              Scene3D.SetTransform(ERender3DTransform::World, &animobj->matrix);
1401          }
1402          else
1403          {
1404              hmm_mat4 world;
...
1415              MtxMultiply(&world, &animobj->matrix, pos);
...
1417              Scene3D.SetTransform(ERender3DTransform::World, &world);
1418          }
1419      }
...
1508          Scene3D.DrawIndexedPrimitive(
1509              ERender3DPrim::TriangleList,
1510              vrttype,
1511              vrt,
1512              numvrt,
1513              (uint16_t*)fac,
1514              numfac * 3,
1515              0);
```

What this means for blood:

- The caller sets `OBJ3D_MATRIX | OBJ3D_VERTS` (`effectcomp.cpp:1461`),
  so the `CalcObjectMatrix` branch (:1390) is skipped — **the animobj's
  own matrix is used**.
- `OBJ3D_ABSPOS` is NOT set, so the body LEFT-multiplies the animobj's
  matrix by the animator's `pos` matrix (`:1415`,
  `MtxMultiply(&world, &animobj->matrix, pos)`) — i.e. the droplet's
  local-space matrix is composed into world space via the effect
  object's own world transform. **The droplet lives in world space.**
- The final draw is **`Scene3D.DrawIndexedPrimitive(TriangleList, …)`**
  (:1508) — a real **indexed mesh** submission through the standard
  world-transform 3D pipeline. The vertex buffer comes from
  `animobj->verts` (because `OBJ3D_VERTS` is set, :1446-1451), which is
  the per-effect lvert buffer the animator allocated for each
  sub-object (`effectcomp.cpp:1195-1202` calls `animator->GetVerts(...)`
  with vertex-type `D3DVT_LVERTEX`).

⇒ **Each droplet is a 4-vertex, 2-triangle indexed `TriangleList` mesh
draw with a world transform.** It is NOT a billboard pipeline call.
It is NOT a screen-space `TLVertex` sprite. It is NOT a particle
batch. It is a real 3D mesh — just a very small one (one quad).

---

## §5. ANSWERS

### A1 — BLEND / DEPTH / CULL / PASSES

Confidence: **Snapshot literal = Snapshot-only; effective shipped
behavior = inferred (Retail UNVERIFIED at Render level).**

**What the snapshot literally writes** (§2.6):

- 2 passes per droplet.
- **k=0**: `TEXTUREMAPBLEND=MODULATE`, `ZWRITEENABLE=false`,
  `ZENABLE=true`, `SRCBLEND=SRC_ALPHA`, `DESTBLEND=INV_SRC_ALPHA`.
- **k=1**: `TEXTUREMAPBLEND=MODULATE`, `SRCBLEND=ONE`, `DESTBLEND=ONE`,
  Z states inherited (the `false`/`true` lines are commented).
- **NEVER touched**: `ALPHABLENDENABLE`, `ALPHATESTENABLE`, `CULLMODE`
  (inherits scene), `COLORKEYENABLE`.

**What that literally produces on DX6/7** (with default
`ALPHABLENDENABLE=FALSE`): both passes write **OPAQUE textured pixels**
(MODULATE × per-vertex grayscale), and the black-keyed background
draws as **solid black**. The SRC_ALPHA/INV_SRC_ALPHA factors of k=0
and the ONE/ONE of k=1 are **dormant**. This is not what the user is
seeing in ported gameplay; it is what the snapshot literally writes.

**Effective shipped behavior** (the most defensible inference,
matching the texture-load comment, the user's gameplay memory, and the
absence of any non-binary alpha in DoLighting):

- **Chroma-keyed opaque draw** — the 1-bit alpha encoded in the texture
  by the loader (`legacy/3dimage.cpp:2079`) is consumed via
  `ALPHATESTENABLE=TRUE, ALPHAREF=0, ALPHAFUNC=GREATER` (or
  equivalent) — fragments with alpha=0 are discarded, fragments with
  alpha=1 write **opaque** color. **No blending. No additive. No
  transparency.**
- **Z**: `TestNoWrite` — test scene depth (so blood occludes behind
  walls correctly) but don't write depth (so droplets don't break
  later effects' depth reads). Snapshot's `RefreshZBuffer` runs after
  Render to patch the Z buffer separately.
- **Cull**: inherits scene (snapshot never forces it). Two-sided
  effectively, because the scene's default for 3D imagery is typically
  `D3DCULL_NONE` for effects (so the quad is visible from both sides).
- **Number of passes**: probably **ONE**, not two. The k=1
  ONE/ONE-MODULATE inline write is the only non-`SetAddBlendState`
  additive in the entire `effect_old.cpp` + `effectcomp.cpp` codebase,
  and the commented-out Z-state lines suggest the author was in the
  middle of debugging the second pass and left it half-done. Combined
  with the other dead-code in the same Render body (the bif splash,
  the big/med trail loops), this is most plausibly a WIP artifact —
  shipped retail probably draws one pass.

**Confidence rating**:
- "Snapshot writes MODULATE+SRCALPHA/INVSRCALPHA + MODULATE+ONE/ONE" =
  **Snapshot-verified** (read directly from `effectcomp.cpp:1428-1440`).
- "ALPHABLENDENABLE never touched" = **Snapshot-verified**
  (`grep -rln "ALPHABLENDENABLE" src/` finds only TStripAnimator +
  TShockWaveAnimator).
- "Shipped uses ALPHATEST chroma-key opaque, single pass" =
  **Inferred / UNVERIFIED** at the retail decomp level; this is the
  most defensible reading given the texture-load comment, the user's
  gameplay-memory, and the absence of per-vertex alpha math in
  `DoLighting`.

### A2 — PARTICLE KIND

Confidence: **Retail-confirmed asset, Snapshot-verified draw path.**

Each droplet is a **textured 4-vertex / 2-triangle indexed quad mesh**
out of `Misc\Blood.I3D`, submitted via
`T3DImagery::RenderObject → Scene3D.DrawIndexedPrimitive(TriangleList, …)`
through the standard world 3D pipeline.

- Asset (byte-identical to shipped, MD5
  `9c0596112cd46d6e3cb91df9f97770df`): **8 sub-objects** (`box01..box08`),
  each a planar 4-vert quad with normal +Z, ~10×10 wu, mapped to a
  cell of the 128×160 texture sheet.
- Draw path: real `TriangleList` indexed mesh through the world
  transform pipeline. **NOT** a billboard quad emitter. **NOT** a
  screen-space sprite. **NOT** a particle batch.

If the geometry were a tetrahedron or N-gon or volumetric mesh the
.I3D's vert/face counts would be larger. They are exactly 8×4=32
verts and 8×2=16 faces (read directly from the body), which is the
unique signature of "8 planar quads".

### A3 — TRANSFORM SPACE

Confidence: **Snapshot-only** (Retail Render body not isolated).

**World-space, with a per-droplet world matrix**. Sequence at
`effectcomp.cpp:1462-1476`:

```
D3DMATRIXClear(...)
D3DMATRIXScale(scl, scl, scl)
if (stage == BLOOD_FLY):
    D3DMATRIXRotateX(-90°)
    D3DMATRIXRotateZ(-45°)
D3DMATRIXTranslate(pos.x, pos.y, pos.z - height)
```

This is the **animobj's per-particle LOCAL matrix**.
`T3DImagery::RenderObject` then left-multiplies it by the animator's
world matrix (effect object's world pos) via
`MtxMultiply(&world, &animobj->matrix, pos)` (`3dimage.cpp:1415`) and
sets `Scene3D.SetTransform(World, &world)`.

- **NO billboard step anywhere.** No view-matrix read, no `lookat()`,
  no camera basis extract, no face-camera math. The rotation values
  (-90° around X, -45° around Z) are **fixed constants** — the quad
  is fixed in world orientation, not screen orientation. If you walk
  around a FLY droplet in 3D you see it tipped to a fixed vertical
  pose with a 45° in-plane spin; if you walk around a SPLAT droplet
  you see it lying flat on the ground (no rotation applied).
- **Stage-dependent orientation**:
  - **FLY**: starting from a flat XY quad (normal +Z), `RotateX(-90°)`
    tips it to the XZ plane (normal +Y), `RotateZ(-45°)` spins that
    45° around world Z. Net: a **vertical quad with a 45° in-plane
    spin**, fixed in world space.
  - **SPLAT / SHRINK**: no rotation. The quad keeps its authored
    orientation (normal +Z), so after the effect's world matrix it
    lies **flat on world XY** (ground decal facing up).

⇒ **World-space objects with their own world matrices.** They tumble
relative to the camera depending on viewing angle. Camera-pinned
billboards they are NOT.

---

## §6. IF DIFFERS FROM CURRENT PORT

The brief references the previous-generation port at
`src/effect.cpp:1382..1755` on `feature/vfx` (Alpha + AdditiveStraight
2-pass through `EFxPipeline::Billboard` with `EFxBillboardOrientation::
{WorldXY,ScreenAligned}` and `EFxLightMode::LitFlat`). On this agent
worktree's branch that older code is preserved under `#if 0` at
`src/effect.cpp:1432-1526` (the `*_BESPOKE` versions), and the live
port has moved to a `TParticleBucket` / `effects.def` driven path
(`TBloodEffect::TickAndSubmitForTest` at `src/effect.cpp:1406-1423`).

§6 calls out the brief-referenced (feature/vfx) port's mistakes.
Because that port submits via `SubmitFxBillboard`, every line listed
below is wrong against §5 in a structural way (not just a tuning
mismatch).

### §6.1 `pipeline_id = uint16_t(EFxPipeline::Billboard)` — `effect.cpp:1705` (feature/vfx)

**Wrong.** Per §4 the snapshot submits via
`Scene3D.DrawIndexedPrimitive(TriangleList, …)` — a real indexed mesh
through the world transform pipeline, using the loaded box01..box08
4-vert/2-tri sub-objects. The port should submit through the
**mesh-3D pipeline** (whatever in `EFxPipeline` corresponds to a
`T3DImagery::RenderObject`-style indexed mesh draw, e.g.
`EFxPipeline::Mesh3D` or the FB renderer's equivalent), feeding the
real per-particle 4-vert lvert buffer + 2-tri index buffer.

`effect.cpp:1705`  `EFxPipeline::Billboard` → `EFxPipeline::Mesh3D`
(or the engine's mesh-submission pipeline analog).

### §6.2 `orientation = (FLY:WorldXY, else:ScreenAligned)` — `effect.cpp:1718-1720` (feature/vfx)

**Wrong on both prongs.**

- The `EFxBillboardOrientation` enum implies a renderer-side
  face-camera / world-axis-align step. Per §5/A3 the snapshot has **no
  camera-facing step**; the quad's orientation comes from fixed
  rotations in the per-droplet matrix (`RotateX(-90°)·RotateZ(-45°)`
  for FLY, nothing for SPLAT/SHRINK).
- `ScreenAligned` for SPLAT/SHRINK is doubly wrong — it would make
  splats face the camera, but the snapshot lays splats **flat on the
  ground** (normal +Z, no rotation, composed through the effect
  object's world matrix → world XY ground decal).
- `WorldXY` for FLY (if the enum means "lying flat on world XY plane,
  normal +Z") is the **opposite** of what FLY needs — FLY needs a
  **vertical quad** (normal in the XY plane), not a flat-on-ground
  quad.

`effect.cpp:1718-1720`  remove the `EFxBillboardOrientation` enum
entirely; instead build a per-particle world matrix in the port as
`Scale(scl) · [FLY: RotX(-90°)·RotZ(-45°)] · Translate(pos.x, pos.y,
pos.z - height)` and submit that matrix as the mesh's world transform
(mirroring `effectcomp.cpp:1462-1476` directly).

### §6.3 `key.blend = uint8_t(EFxBlend::Alpha)` (k=0) — `effect.cpp:1732` (feature/vfx)

**Wrong.** Per A1 the snapshot literally writes
SRC_ALPHA/INV_SRC_ALPHA but does NOT enable `ALPHABLENDENABLE`, so the
factors are dormant. The intended (shipped) behavior — consistent
with the 1-bit alpha keycolor mask baked by the texture loader — is
**chroma-keyed opaque** (alpha-test cut, no blend). Treating it as
`EFxBlend::Alpha` (semi-transparent SRC_ALPHA / INV_SRC_ALPHA blend
with linear alpha) produces washed-pink/translucent droplets — which
is exactly the "blood is not transparent" complaint.

`effect.cpp:1732`  `EFxBlend::Alpha` → `EFxBlend::AlphaTest` (or
`EFxBlend::Opaque` with alpha-test enabled / `MaskedOpaque` /
whatever the renderer's "1-bit chroma-key opaque" mode is called).
The droplet body should read **fully opaque dark red** with the black
background **cut out**.

### §6.4 `key.blend = uint8_t(EFxBlend::AdditiveStraight)` (k=1) — `effect.cpp:1750` (feature/vfx)

**Wrong (drop entirely).** Per A1 the snapshot's k=1 writes ONE/ONE
with MODULATE texture stage but ALPHABLENDENABLE is dormant — so it
just OVERWRITES k=0 with a different sub-object set. Combined with
all the OTHER dead-code in the same Render body (bif splash, big/med
trail loops) and the fact that **blood is the only effect in
`effect_old.cpp` that doesn't use `SetAddBlendState()` for its second
pass**, this k=1 pass smells like WIP that didn't ship.

`effect.cpp:1745-1757`  **delete the k=1 pass entirely**. Render the
droplet once, opaque chroma-keyed (per §6.3), using the box05..box08
sub-objects (the k=0 set, indices 4..7) — drop the box01..box04 (k=1
set) draw. If a later visual comparison against in-game ground-truth
demands the second pass back, add it with explicit alpha-test +
opaque mode (NOT additive), drawing the same sub-object — but the
defensible default is "one pass only".

### §6.5 `depth_mode = uint8_t(EFxDepthMode::TestNoWrite)` — `effect.cpp:1706` (feature/vfx)

**Right.** Matches snapshot k=0 (ZWRITE=off, ZENABLE=on). Snapshot k=1
inherits scene Z (commented-out lines :1436,1439) which is most likely
an oversight; treating both passes as TestNoWrite is the defensible
read. **No port change.**

### §6.6 `light_mode = EFxLightMode::LitFlat` — `effect.cpp:1713` (feature/vfx)

**Approximately right; verify the math.** Per §2.7 the snapshot's
`DoLighting` writes a grayscale per-vertex color = sum of brightnesses
from up to `MaxLights` closest world lights + ambient×4, clamped to
[0, 1.5] and normalized by /1.5, applied uniformly to all 4 verts via
`D3DRGB`. LitFlat in the FB renderer is the closest analog. Two
caveats to verify against the LitFlat implementation:

- The ambient×4 boost is unusual — verify LitFlat's ambient weight
  matches (or stays brighter than) `ambient/255 × 4.0`.
- DoLighting forces vertex-color alpha = 1.0 (D3DRGB packs `0xFF` for
  alpha). The port must preserve alpha=1.0 — no per-droplet alpha
  fade.

**Minor port change**: ensure the LitFlat pipeline forces vertex
alpha to 1.0 in this path. No blend-mode change.

### §6.7 `color_rgba[3] = 1.0f` — `effect.cpp:1701-1704` (feature/vfx)

**Right.** Unit RGBA so it doesn't fight LitFlat. No change.

### §6.8 Size-2 collapsed to small — `effect.cpp:1695` (feature/vfx)

**Right.** Matches snapshot: the `object = big` lines are commented
in `effectcomp.cpp:1447-1448,1455-1456`. A size-2 droplet falls
through to the small sprite. No change.

### §6.9 Summary table — port changes needed

| `effect.cpp` line (feature/vfx) | current | target (per §5) | rationale | confidence |
|---|---|---|---|---|
| 1705 | `pipeline_id = Billboard` | `pipeline_id = Mesh3D` (or engine's indexed-mesh pipeline) feeding the real loaded `box0N` 4-vert / 2-tri sub-object as a `TriangleList` | snapshot draws via `DrawIndexedPrimitive(TriangleList,…)` (§4) | High |
| 1718-1720 | `orientation = (FLY:WorldXY, else:ScreenAligned)` | remove the orientation enum entirely; build the world matrix as `Scale·[FLY: RotX(-90°)·RotZ(-45°)]·Translate(pos.x, pos.y, pos.z - height)` and submit it as the mesh's world transform | no camera-facing step in snapshot (§5/A3) | High |
| 1732 | `blend = Alpha` (k=0) | **`blend = AlphaTest` / `MaskedOpaque`** (1-bit chroma-key cut, no semi-transparent blend) | 1-bit alpha keycolor mask from loader (§3.4, `legacy/3dimage.cpp:2079`); user's gameplay-memory says blood is not transparent (matches §5/A1) | High intent / Med vs literal snapshot |
| 1745-1757 | `blend = AdditiveStraight` (k=1) | **delete the k=1 pass entirely** | only effect in `effect_old.cpp` that hand-rolls additive (smells WIP); commented-out Z state lines (`effectcomp.cpp:1436,1439`); user's "blood is not additive" memory; surrounded by dead-code (bif splash, big/med trail loops) | Med |
| 1706 | `depth_mode = TestNoWrite` | unchanged | matches snapshot k=0 (`SetBlendState` :224-225) | High |
| 1713 | `light_mode = LitFlat` | keep; verify LitFlat preserves vertex alpha=1 and the ambient×4 boost matches | matches snapshot's grayscale `D3DRGB` per-vertex modulation (§2.7) | Med |
| 1701-1704 | `color_rgba = (1,1,1,1)` | unchanged | unit RGBA doesn't fight LitFlat | High |
| 1695 | `cell = (size==1 ? 1 : 0)` (collapses big to sml) | unchanged | matches snapshot commented `object=big` lines (`effectcomp.cpp:1447-1448`) | High |
| (cullmode) | not set | unchanged (scene default) | snapshot also doesn't force CULLMODE | High |
| (colorkey D3D state) | not set | unchanged | snapshot uses 1-bit alpha mask, NOT D3D's COLORKEYENABLE state | High |

**Net effect once fixed**: blood draws as a **single-pass, chroma-keyed
opaque, dark-red textured quad mesh** out of `Misc\Blood.I3D`,
oriented in world space (vertical-tipped 45°-spun for FLY, flat
ground-decal for SPLAT/SHRINK), scene-lit so it darkens in shadow,
TestNoWrite depth. No transparency. No additive sheen. No
camera-facing billboard.

---

## §7. SUMMARY (one-line per Q)

- **Q1 BLEND (snapshot literal: snapshot-only; effective shipped:
  inferred / Retail UNVERIFIED):** snapshot writes
  MODULATE+SRC_ALPHA/INV_SRC_ALPHA (k=0) then MODULATE+ONE/ONE (k=1)
  but **NEVER enables `ALPHABLENDENABLE`** — so on DX6/7 both passes
  are dormant-opaque. Effective shipped behavior is almost certainly
  **chroma-keyed opaque** (1-bit alpha mask via alpha-test, TestNoWrite
  depth), almost certainly **single-pass** (k=1 is most likely WIP
  dead-code, consistent with the bif-splash and big/med-trail dead code
  in the same Render body).
- **Q2 PARTICLE KIND (asset retail-confirmed; draw snapshot-verified):**
  textured **4-vertex / 2-triangle indexed `TriangleList` quad mesh**
  out of `Misc\Blood.I3D` (byte-identical to shipped), drawn via
  `T3DImagery::RenderObject → DrawIndexedPrimitive`. The .I3D
  contains 8 such quad meshes (box01..box08). Not a billboard, not a
  sprite, not a volumetric mesh.
- **Q3 TRANSFORM SPACE (snapshot-only):** **world-space** with a
  per-droplet world matrix `Scale·[FLY only: RotX(-90°)·RotZ(-45°)]·
  Translate(pos.x, pos.y, pos.z-height)`. **No camera-facing step
  anywhere.** FLY = vertical 45°-spun quad fixed in world; SPLAT/SHRINK
  = ground-flat decal.
