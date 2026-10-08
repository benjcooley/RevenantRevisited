# W03 — sgeyser / fgeyser retail runtime audit

Status, 2026-10-04: **unsupported retail general particle components**. The guessed preview has been removed; neither effect is source-behavior audited, runtime verified, visually accepted, or complete. No geyser runtime builder/component was registered in this change.

## Correct identity and caller chain

Shipped `imagery.rvi` member `class.def` registers class EFFECT (25) `sgeyser` **0xad92bd30** with `cave\Cavsgeyser.i3d`, and `fgeyser` **0xad92bd31** with `cave\Cavfgeyser.i3d`. These assets exist in the archive. The older claim that they were absent was based on loose-file searches.

Separate TRAP objects `CavFGeyser` **0xe18f05eb** and `CavSGeyser` **0xe18f05ec** use `Misc\dummy.i3d`. Retail startup **0x526740/0x526760** registers these TRAP builders. **0x526780**, and constructor paths **0x527d00/0x527e10**, choose the `fgeyser` or `sgeyser` EFFECT type index by comparing the trap name with CavFGeyser. Activation **0x526860** constructs the corresponding sound name, then builds a zero-initialized SObjectDef: class **25** at **0x526918**, cached EFFECT type at **0x5268e5**, exact trap XYZ at **0x5268d5..0x526924**, current level, face zero. **0x526929** calls MapPane.NewObject (**0x450e40**), then **0x526934** resolves its resulting index. It does not initialize 100 ballistic particles.

The original port incorrectly grouped this TRAP code with PoisonCloud's animator through OOAnalyzer `cls_0x5b79ac`. The **0x526040** random initializer belongs to the preceding **PoisonCloud** registration at **0x526020**; **0x526230** calls it 100 times during animator initialization. That number cannot establish geyser population, cycle, gravity, tint, or warmup. The snapshot has no implementation of these retail TRAP/particle tags.

## Authored geometry and animation

Both resources are CGSR/version1 wrappers with version3 I3D bodies, flags **0x000000dc**, state `start`, animation flags **0x2000**. Steam has **45** authored frames, **8** vertices / **4** faces, one texture/material, three subobjects: `#steam` (four vertices), `steam` (zero-vertex emitter), `#blast` (four vertices). Fire has **90** frames, **12** vertices / **6** faces, one texture/material, five subobjects: `#fire` (four vertices), `fgey` (zero-vertex emitter), `#sparks` (four vertices), `fgeys` (zero-vertex emitter), `#glow` (four vertices).

Each source material has diffuse/ambient/emissive RGBA `(1,1,1,1)`, specular approximately `(0.9,0.9,0.9,1)`, power0, texture slot0. `assets.json` retains every actual corner, normal, UV, face group, material value, compressed animation-key hash and exact tag string. A single synthetic screen billboard with a guessed color ramp loses this structure. `#` naming/visibility and template-versus-emitter semantics must be recovered from retail component code rather than inferred from the names alone.

## Exact tag timeline

### sgeyser

Member `Imagery/Cave/CavSGeyser.I3D`, SHA-256 `34082b08a2e2dc5a895c3f10467b6ed0c68f2b9cca0a6f0f3840eb3675ea042a`.

State **0**, authored frame **10**, tag `partsys`:

```text
obj=(steam),particle=#steam,pps=[0:0,5:250,25:150,30:0],lifespan=7:10,initialvelocity=20:35,scale=[0:0.5,100:2],rlocalrotation=[0:(0,45,0),100:(0,360,0)],color=[0:(100,100,100),100:(0,0,0)],spread=1,friction=[0:0.1],gravity=[0:0.3]
```

State **0**, authored frame **30**, tag `partsys`:

```text
filename vapor
```

### fgeyser

Member `Imagery/Cave/cavfgeyser.i3d`, SHA-256 `64102fd39d039f0ca24af8b1b39eff4529ea988a69471fe49728b8932984c77b`.

State **0**, authored frame **0**, tag `blendcont`:

```text
litadd
```

State **0**, authored frame **1**, tag `partsys`:

```text
obj=(fgey),particle=#fire,pps=[0:0,5:75,10:100,20:45,30:0],lifespan=10:35,initialvelocity=10:15,scale=[0:0.25,100:2],rlocalrotation=[0:(0,0,0),100:(0,45,0)],color=[0:(255,255,255),15:(255,175,45),45:(255,125,1),70:(100,25,1),100:(0,0,0)],spread=10,alpha=[0:1,100:0],friction=[0:0.1],gravity=[0:0.3],emittertype=circle,emittersize=1
```

State **0**, authored frame **50**, tag `partsys`:

```text
obj=(fgeys),particle=#sparks,pps=[0:0,5:50,7:0],lifespan=30:50,initialvelocity=-6:-10,scale=[0:0.25,50:0.75,100:0.1],color=[0:(255,255,255),65:(255,200,1),100:(0,0,0)],spread=45,azimuth=45,alpha=[0:1,60:0.8,100:0],gravity=1.75,friction=0,emittertype=circle,emittersize=1,bounce=-50:-50
```

## Recovered component route and remaining gaps

Retail startup **0x403300** registers `partsys` (string **0x5c5c8c**) via component registrar **0x40d320**, factory **0x405e20** / vtable **0x5a3544**. The table points to parser **0x4042e0**, initialization **0x403320**, cleanup **0x4036a0**, and update/render entries **0x403760/0x404270**. `blendcont` registers at **0x405750**; parser **0x405770** maps `litadd` to mode **0x10** at **0x405850**. These identities and values are backed by executable instructions / strings, not the guessed preview.

At **0x403596..0x4035bf**, the retail particle initializer scales its maximum rate by graphics quality (2 → factor0.5; 1 → factor0.25). Capacity calculation follows at **0x4035c5..0x4035dc**. This proves that hardcoding 100 particles independent of quality is unsupported. It does **not** by itself prove full capacity rounding, emission cadence or simulation equations.

The current port loads imagery tags (`3dimage.cpp`, version3 tag copying) but has no handlers for `partsys`, `blendcont` or `litadd`. `render_metadata` supplies rendering policy definitions; it is not this tag interpreter. Current particlefx/effects.def support is a newer explicit definition path, without a proven translator for these retail tags.

Next work is to recover and test the small required retail tag subset: frame scheduling/restarts; owner/emitter authored pose; template-object selection/visibility; PPS accumulation/interpolation; lifespan/ranges; random-call order; local rotation; friction/gravity; bounce; quality scaling; source blend/depth/lighting behavior. Steam's `filename vapor` lookup target and error/fallback behavior remain unresolved; no member containing `vapor` was found in imagery.rvi/resources.rvr. Do not interpret it as an optional no-op. Keep the two fire emitters separate and preserve their frame1/frame50 triggers. Only then attach through final saved identity with live owner transform and independently clocked Advance/Submit. Host/retail RNG streams remain unsynchronized; native frame-image counts cannot determine tick rate or justify retuning.

## Bounded production correction and validation

Only `TGeyserEffect_Bespoke`'s header block and cpp section changed. Both existing SpawnForTest APIs now return nullptr with the exact type/asset and actionable unsupported-tag diagnostic. The old invented cycle120, eruption48, two particles per tick, truncated41ms clock, gravity-0.30, velocity4.5, size28 and blue/orange alpha ramps are removed. No imagery reference, map identity, RNG draw or runtime component is created. The manual TickAndSubmit API is retained as a no-op; directly constructed unsupported shells report IsAlive false. Other effects, vfxtest registrations, shared renderer and lighting are untouched.

`lab/check_geyser_failclosed.py` compiled and executed the **extracted production factory and tick bodies**: 100 seeds × both variants = **200** null results with exact variant-specific actionable diagnostics and unchanged next RNG value. Direct tick also preserved RNG and made no draw call. Production effect.cpp passed its compile_commands syntax compile (existing warnings only). This validates the unsupported-path correction, not geyser animation. No full build, port run, guest run or native capture was performed by this agent.

Evidence lives under `/Users/benjamincooley/RevenantRetailLab/research/geyser/`: `assets.json`, exact extracted I3D assets, `executable-provenance.json`, retail assembly ranges, `source-validation.json`, `source-validation.log`. Read-only extractor: `lab/audit_geyser_assets.py`. The extractor validates self-relative offsets, typed vertices/faces/materials/tags and exact original/compat executable code byte identity for the saved ranges. Original EXE SHA-256 **28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5**; compat EXE **33545a2f4e055dfaa3a62e3b03488d6f3bd63fe6c9827b1f88e141e1387073eb**.

Existing map-placement evidence reports **1,433 sgeyser / 3,536 fgeyser saved records**. Preserve repeated layers and unique saved MapIndex values. These counts demonstrate production importance and fixture availability, not successful animation. Whole-scene pixel changes or default mesh animation cannot substitute for a recovered partsys path.
