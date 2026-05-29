# F09 TFireColumnEffect — Original-Effect Forensics

| field | value |
|-------|-------|
| **Effect ID** | F09 (covers F09 `TFireColumnEffect` + F10 `TFireColumnAnimator`) |
| **Class(es)** | `TFireColumnEffect : TMissileEffect : TEffect` + `TFireColumnAnimator : T3DAnimator`. Shares the **`TMissileEffect` base [BASE]** with F07 FireBall / M07 Photon / F11 FlameDisc / X19 YFireBall — documented in F07 §5–§6. |
| **Status** | forensics-complete (see §13 for the genuine unknowns: dead-status of the effect, the F07-impact-spawn open question, and the snapshot-only animator constants) |
| **Retail fidelity** | **snapshot-only (unverified)** — the `TFireColumnEffect` *Effect* shell is **retail-confirmed** (Initialize + Pulse bodies in `cls_0x5b4534` structurally match the snapshot; size 636 B, allocator 0x27c, `MissileEffect.cpp` assert string lands in this class's vftable[52]); the *animator* + the *asset* + the *registration* are **NOT shipped**: `"FireColumn" / "FIRECOLUMN"` strings exist in the retail binary's `.rdata` but the asset `Magic\FireColumn.I3D` is **NOT in `data/imagery.rvi`** and **the entry is NOT in retail `class.def`**, and no `spell.def` variant / `ATTACHEFFECT` references `FireColumn` (either snapshot or retail). The animator method bodies (Initialize / Animate / Render) are not corroborated by Ghidra. See §2.1. **Fidelity verdict: the C++ class still exists in the retail binary as dead code; the effect was de-listed from the shipped data — there is no in-game way to see it.** |
| **Author / Date** | vfx-forensics-agent (wf_b5d7064f-682-7) / 2026-05-29 |
| **Family** | fire (missile sub-family — vestigial) |
| **Draws** | composite — I3D mesh: 13 trailing **column** sprites (`flamecolum`) + 2 final **explosion** sprites (`flameexplo`), each a scaled/rotated quad, placed along a straight line from the caster |
| **Archetype(s)** | (B) I3D-geo (the two-sub-object STRIP mesh, frame-flipbook texture-animated), (D) trail (15-slot sequentially-spawned vertical-sprite chain along the aim vector — a *positional* trail, not a ring-buffer of past positions), (F2) custom 3-stage state machine (LAUNCH=no-op → FLY=sequential spawn → EXPLODE=reverse-sequence collapse), (G — none, see §9 — light data not authored), (H — none, see §11 — sound mounted but commented). NO continuous particle emitter, NO ScreenAligned billboards, NO shockwave ring. |

---

## 1. Summary

`TFireColumnEffect` is a 15-sprite straight-line **column of fire** authored to
walk away from the caster along the aim direction, terminating in a slightly
bigger explosion. The effect inherits the `TMissileEffect` LAUNCH/FLY/EXPLODE
state machine (F07 [BASE]) but is **immobile** — its `FIRECOLUMN_SPEED = 0` so
the missile object never translates; instead the *visual* walks outward by
seeding 15 columns at fixed offsets `(n+3)*vctr` along the aim vector, advancing
each column's flipbook frame each tick, looping 5 times, then collapsing them
in reverse order during EXPLODE with a 3× faster shrink on the trailing
column and a slow shrink on the head. The first 13 sprites use a thin tall
column sub-object (`flamecolum`); the last 2 use a fatter explosion sprite
(`flameexplo`). All sprites are drawn with `CULLMODE=NONE` + the **Alpha** blend
helper (suspect — see §7 sanity-check; sister fire spells are additive). There
is no continuous particle emitter, no shockwave ring, no per-column lighting,
and the spell `LIGHT` directive is absent.

**The effect is vestigial in shipped retail.** Both the asset `Magic\FireColumn.I3D`
and the `"FireColumn"` Class.Def imagery entry were stripped from the retail
data archive (`data/imagery.rvi` member `class.def` contains no `FireColumn` or
`FIRECOLUMN` line); no `spell.def` variant or `ATTACHEFFECT` directive ever
referenced it, snapshot or retail. The C++ class **still ships** in the binary
(`cls_0x5b4534` Initialize + Pulse bodies match the snapshot), but with no
imagery + no registry + no caller, the constructor's `FindImagery("FireColumn")`
would fail and no instance can be created at runtime in retail.

The pre-release intent was almost certainly the F07-impact-spawned column-of-fire
that the catalog notes — but in the snapshot the FireBall impact is fully
self-contained (burst + ring + sparks) with no `NewObject("FIRECOLUMN")` call,
and the asset/registry are gone from retail. See §13 for the open question.

---

## 2. Sources & evidence

- **Retail decomp:**
  - `recon/classes/cls_0x5b4534.cpp` — **PARTIAL** (Effect side only). Bodies
    `virt_meth_0x5123f0` (Initialize) `:43-55` and `virt_meth_0x512430` (Pulse)
    `:59-75` are readable and match the snapshot structurally. `MissileEffect.cpp`
    assert string XREFs land in this class's `virt_meth_0x512740`
    (`recon/ghidra/_data.txt:108694-108705`, vftable[52]) → confirms cls_0x5b4534
    lives in `MissileEffect.cpp`.
  - `recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml` — MEDIUM
    candidate; size 636 B / 6 methods / allocates 0x27c. Inherits `cls_0x5a50e8`
    (MissileEffect family base, identical to F07).
  - **No animator decomp.** No `cls_*` produces the `Animate`/`Render` bodies
    for the FireColumn animator side. Searching for `framenum`/`numtexframes`
    yields nothing; no decompiled body matches the `for (n = 0; n <
    NUM_FIRE_COLUMNS; n++)` shape.
- **Pre-release (snapshot — authoritative for everything below the Effect shell):**
  - Effect: `src/missileeffect.cpp:1154-1194` (`DEFINE_BUILDER("FIRECOLUMN",
    TFireColumnEffect)` at `:1158`, `Initialize` `:1161-1170`, `Pulse`
    `:1172-1194`). Class declaration `src/missileeffect.h:235-255` (size define
    `FIRECOLUMN_SPEED = 0` at `:239`, field `bool firsttime` at `:245`).
  - Animator: `src/missileeffect.cpp:1196-1448`
    (`REGISTER_3DANIMATOR("FireColumn", TFireColumnAnimator)` at `:1196`,
    `Initialize` `:1207-1241`, `OffsetPoint` `:1243-1246`, `Animate` `:1255-1334`,
    `Render` `:1343-1448`). Class declaration `src/missileeffect.h:257-290`
    (`NUM_FIRE_COLUMNS = 15` at `:261`, fields `vctr`, `p[15]`, `framenum[15]`,
    `rotation[15]`, `scale[15]`, `numtexframes`, `activesparks`, `oldstate`).
  - Whole file is gated by `#if 0` (`src/missileeffect.cpp:15` / `:1702`),
    confirming this is the pre-release WIP source not the live port.
- **Asset:**
  - `legacy/Imagery/Magic/FireColumn.I3D` (175,396 B, MD5
    `7603a16b35fcd7604858261e150fbf8c`) — registered `legacy/Class.Def:2040` as
    `"FireColumn" "Magic\FireColumn.I3D" 0x12da54d2`.
  - I3D header (`CGSR` magic): type `strip`; **3 animation states** `strip launch`,
    `strip fly`, `strip explode`; **2 sub-objects** with names `flamecolum`
    (column) and `flameexplo` (explosion) — derived from binary string-table.
  - **NOT present in retail `data/imagery.rvi`** — `unzip -l` of the shipped
    archive returns no member matching `firecolumn|FireColumn`; the retail
    `class.def` (extracted via `unzip -p data/imagery.rvi class.def`) contains
    **no** `FireColumn` line. The asset was stripped pre-ship.
- **Sister effects consulted (heavily — to fill in the animator-blend gap):**
  - `TFireBallEffect` / `TFireBallAnimator` (`src/missileeffect.cpp:482-1150`,
    F07 doc) — same `TMissileEffect` base; F07 also uses `SetBlendState()` (Alpha)
    on a warm-fire sprite asset.
  - `TFlameDiscEffect` / `TFlameDiscAnimator` (`src/missileeffect.cpp:1450+`) —
    direct sibling in the SAME source file; shares the `state`/`oldstate`/
    `activesparks`/`numtexframes` / `frame = framenum[n] % numtexframes` per-quad
    pattern (F11). Also uses `SetBlendState()` (per `src/missileeffect.cpp:1700`).
  - `TFireFlashAnimator` / `TFireWindAnimator` / `TFireConeAnimator` /
    `TDragonFireAnimator` — all in `src/effect_old.cpp`; all `SetAddBlendState()`
    (Additive). Cross-checked: these are the WARM-FIRE *spell* family.
    `TFireColumnAnimator`'s `SetBlendState()` (Alpha) is the *missile-family*
    pattern (matches FireBall + FlameDisc, not the spell-family).
  - Blend helpers `SetBlendState`/`SetAddBlendState` (`src/effect_old.cpp:221-244`)
    + classification per [knowledge/03_RENDERING_CONVENTIONS.md §1.1](knowledge/03_RENDERING_CONVENTIONS.md).
- **Source-of-truth ranking:**
  1. **Retail decomp** — authoritative for the Effect-shell behavior (Initialize
     + Pulse readable, retail-confirmed) but **silent** on the animator (no
     readable bodies) and **proves the asset/registration are stripped**.
  2. **Snapshot `src/missileeffect.cpp`** — authoritative for the animator
     mechanism (only available source for the 15-column sequential spawn, the
     scale math, the reverse-collapse). Best-evidence, but **the effect doesn't
     ship**, so reconstruction can NOT visually verify against retail (§12).
  3. **Sister-family** — TFlameDisc + TFireBall confirm the missile-family
     `SetBlendState`-Alpha pattern and per-quad `frame = framenum[n] %
     numtexframes` flipbook idiom; TFireFlash/Wind/Cone confirm the *spell*-family
     uses Additive — the divergence is intentional, not a port bug to fix.

### 2.1 Retail-vs-snapshot reconciliation (verdict: snapshot-only)

`src/missileeffect.cpp` is the pre-release snapshot; the whole file is `#if 0`-d
out. Active cross-checks against retail Ghidra + the shipped data archive:

**1. Class-shell match — RETAIL-CONFIRMED (Effect).** The snapshot's
`TFireColumnEffect` is a near-empty `TMissileEffect` derivation with a single
`bool firsttime` field plus FIRECOLUMN_SPEED=0 wired through `SetSpeed` in
`Initialize`. The retail `cls_0x5b4534` matches:
- `Initialize` (`virt_meth_0x5123f0`, `cls_0x5b4534.cpp:43-55`): calls vftable[24]
  (base TMissileEffect::Initialize), then sets `mbr_0xe0 = 0` (= `SetSpeed(0)` →
  speed = 0 * ROLLOVER = 0), `mbr_0x184 = 0x8000` (= range = 32768 — likely a
  re-assignment overlapping the base init), `mbr_0x18c = 1`, `mbr_0x188 = 0`,
  `mbr_0x190 = 1` (= `firsttime = true`).
- `Pulse` (`virt_meth_0x512430`, `cls_0x5b4534.cpp:59-75`): calls
  `cls_0x5b401c::virt_meth_0x510220` (= `TMissileEffect::Pulse`), then `if
  (state == 0) { if (firsttime != 0) firsttime = 0; }` (matches snapshot's
  `case MISSILE_LAUNCH: if (firsttime) { firsttime = false; /* commented sound */ }`),
  `else if (state == 1 && commanddone != 0) range = 0` (matches snapshot's
  `case MISSILE_FLY: if (commanddone) range = 0` — `mbr_0x80` is the
  `commanddone` flag, `mbr_0x184` is `range`).
- Class size 636 B (0x27c) and allocator (`virt_meth_0x514dd0`) FUN_00482fb0(0x27c)
  match: the binary still allocates this class.
- `MissileEffect.cpp` assert string XREFs (`recon/ghidra/_data.txt:108694,108699,108704`)
  land in `cls_0x5b4534`'s vftable[52] (`virt_meth_0x512740`) — CONFIRMS the class
  is from `MissileEffect.cpp`.

⇒ The **Effect class still exists in the retail binary, byte-equivalent in
shape, with Initialize + Pulse retail-confirmed**.

**2. Animator class — NOT FOUND in recon.** No `cls_*` mapping matches
`TFireColumnAnimator`. Constant grep for the snapshot's animator immediates
(`NUM_FIRE_COLUMNS = 15`, scale steps `33`, `8`, the literal `360`, the
multipliers `2`/`3`/`4`/`50`/`100`, `random(0, 5)` for the post-loop frame reroll)
yields no co-located hits in any `cls_0x*` body. The bytewise float constants
the Render method uses (`1.0/100 = 0.01f = 0x3C23D70A`, `2.5f = 0x40200000`,
`3.5f = 0x40600000`, `5.0f`, `50.0f`) return only generic hits unrelated to
missile classes. ⇒ **All §3 animator constants are snapshot-only.**

**3. Asset identity — STRIPPED FROM RETAIL.** This is the headline finding:
- Snapshot asset `legacy/Imagery/Magic/FireColumn.I3D` exists (175,396 B,
  MD5 `7603a16b35fcd7604858261e150fbf8c`).
- Shipped `data/imagery.rvi` (a stored ZIP) contains **no** `firecolumn`
  member: `unzip -l data/imagery.rvi | grep -i firecolumn` → empty.
- Shipped `class.def` (extracted: `unzip -p data/imagery.rvi class.def`)
  contains **no** `FireColumn` or `FIRECOLUMN` line. Comparable fire entries
  do ship (`"FireBall" "Magic\NewFireBall.I3D"`, `"FireCone" "Magic\FireCone.I3D"`,
  `"FireWind" "Magic\Firewind.I3D"`, `"YFireBall" "Magic\YFireBall.I3D"`,
  `"headfireball" "magic\Hfire.i3d"`), but NOT `FireColumn`.
- `FLAMEDISC` is also stripped from the retail `class.def` (sister F11
  `TFlameDiscEffect` — same fate; see EXTRACTION_PASS_2 note that both share
  this region).

⇒ **The asset and its Class.Def imagery entry were deliberately removed before
ship.** Without an imagery registration, the `OBJCLASS_EFFECT` builder
`"FIRECOLUMN"` cannot resolve a `TObjectImagery*` at construction; instantiation
would fail. The class is dead code in the retail binary.

**4. Registration + naming — STRINGS PRESENT, NOT WIRED.** The literal strings
`"FIRECOLUMN"` (`recon/classes/_data.txt:108743-108747`, at 005e1b34, XREF
single from 0x005123d0) and `"FireColumn"` (`:108748-108752`, at 005e1b40,
XREF single from 0x00512470) both ship in the retail binary's `.rdata`.
0x005123d0 and 0x00512470 are top-level static-init functions in the same
registrar table (the array at 0x005c55c0+, `:56288/56290`) — i.e. the
`DEFINE_BUILDER`/`REGISTER_3DANIMATOR` ctors **still run at startup**, putting
the names in the global builder/animator dictionaries. But with no `class.def`
imagery entry, nothing in the shipped data calls `NewObject("FIRECOLUMN")` or
`FindObjType("FireColumn")`, so the entry sits in the registry unused.

**5. Caller / spell wiring — ABSENT (snapshot + retail both).** Searches across
`spell.def`, `Class.Def`, `object.def`, `monster.def` (legacy) + the retail
`class.def`/`spell.def` (extracted from `data/imagery.rvi` / `data/Resources/`)
turn up **no `ATTACHEFFECT "FIRECOLUMN"`**, **no SPELL `EFFECT "firecolumn"`**,
and **no source-code `NewObject("FIRECOLUMN")`** anywhere. Specifically:
- `src/missileeffect.cpp` (TFireBallAnimator EXPLODE state, `:680-757`) has
  NO `NewObject`/`FindObjType("FIRECOLUMN")` — impact is fully self-contained
  (verified in F07 §6.2, §13.3).
- No `EXPLODEEFFECT`/`IMPACTEFFECT` style hook in spell.def points at
  FireColumn for any spell.

**6. Constants — NOT corroborated (snapshot-only).** Every tuning number in §3
(NUM_FIRE_COLUMNS=15, FIRECOLUMN_SPEED=0 unverified beyond mbr_0xe0=0, the
sequential spawn rule "every 6th column triggers, loop 5 times", the column
scale math `(2, 3, 4)*scale/100` and `(2.5, 2.5, 3.5)*((scale/2 + 50)/100)`,
the −33/−8 shrink rates, the 360°-radian rotation seed `random(0, 360f *
TORADIAN)`, the +5 z-lift, the `(n + 3) * vctr` per-column offset, the
`(0 - (n / 3))` framenum staggering, the `random(0, 5)` frame reroll, the
ConvertToVector(face, 10) initial direction-vector seed) lives inside method
bodies the decomp did not isolate.

**Verdict — snapshot-only (unverified).** The *Effect shell* (Initialize +
Pulse) is retail-confirmed-and-dead; the *animator + asset + caller* are
**absent from the shipped game**. The snapshot is the best (and only) available
evidence for what the visual was intended to look like. **Reconstruction CANNOT
visually verify against retail ground-truth** (the effect cannot be seen in the
shipped game), so any reconstruction has to either (a) treat F09 as an
asset+source-only ghost and not reconstruct it for retail-fidelity work, or
(b) reconstruct from the snapshot as a "what the dev intended" artifact and
accept that the absolute look is unverifiable. The blend, glow color, and
column geometry would be the highest-risk values to get wrong.

---

## 3. Constants

`[BASE]` = inherited from `TMissileEffect` (shared with F07 FireBall / M07
Photon / F11 FlameDisc / X19 YFireBall — full table in F07 §3). `[FC]` =
FireColumn-specific.

| name | value | units | scope | source | confirmed? |
|------|-------|-------|-------|--------|------------|
| FIRECOLUMN_SPEED | 0 | pos-units/tick (pre-ROLLOVER) — **stays at 0; missile is immobile** | [FC] | missileeffect.h:239; set `Initialize` :1165 | yes (retail: mbr_0xe0 = 0 in `virt_meth_0x5123f0:49`) |
| range (init) | 32768 (`0x8000`) | ticks (huge until launched) | [BASE] | missileeffect.cpp:35 | yes (retail: mbr_0x184 = 0x8000 in `virt_meth_0x5123f0:50`) |
| firsttime (init) | true | bool | [FC] | missileeffect.cpp:1169 | yes (retail: mbr_0x190 = 1 in `virt_meth_0x5123f0:53`) |
| states | LAUNCH=0, FLY=1, EXPLODE=2 | enum | [BASE] | missileeffect.h:20-24 | yes (retail: `state == 0`/`1` checks in `virt_meth_0x512430:65,70`) |
| **FLY exit gate** | `if (commanddone) range = 0;` (causes base Pulse to detect range≤0 next tick and set EXPLODE) | — | [FC] | missileeffect.cpp:1189-1190 | yes (retail: `virt_meth_0x512430:70-71` — `state==1 && mbr_0x80!=0` → `mbr_0x184 = 0`) |
| **TMissileEffect base impact** | range--, MOVE_BLOCKED, or any non-invoker enemy within Distance≤32 → EXPLODE | wu | [BASE] | missileeffect.cpp:79-122 (see F07 §6.1) | snapshot-only ([BASE] body unread in Ghidra) |
| character-hit radius | 32 | wu | [BASE] | missileeffect.cpp:105 | snapshot-only ([BASE]) |
| FLY range when launched | `(240 * MISSILE_RANGE) / (speed / ROLLOVER)` — **undefined when speed=0**, see §13 | ticks | [BASE] | missileeffect.cpp:68 | snapshot-only |
| MISSILE_RANGE | 2 | range multiplier | [BASE] | missileeffect.h:27 | snapshot-only |
| ROLLOVER | 65536 (`1<<16`) | sub-unit precision | [BASE] | revdefs.h:447 | snapshot-only |
| NUM_FIRE_COLUMNS | 15 | total column slots (13 column + 2 explosion) | [FC] | missileeffect.h:261 | snapshot-only |
| **column count (drawn as flamecolum)** | 13 (`< NUM_FIRE_COLUMNS - 2`) | count | [FC] | missileeffect.cpp:1357 | snapshot-only |
| **explosion count (drawn as flameexplo)** | 2 (`NUM_FIRE_COLUMNS - 2 .. NUM_FIRE_COLUMNS - 1` = indices 13, 14) | count | [FC] | missileeffect.cpp:1401 | snapshot-only |
| direction-vector seed | `ConvertToVector(inst->GetFace(), 10, vctr)` — speed param = 10 (this seeds the per-column step distance, NOT a missile velocity) | — | [FC] | missileeffect.cpp:1221 | snapshot-only |
| per-column XY offset | `p[n].x = vctr.x * (n + 3)`, `p[n].y = vctr.y * (n + 3)` | wu (per-column step along aim vector) | [FC] | missileeffect.cpp:1229-1230 | snapshot-only |
| per-column z-lift | +5 | wu (constant lift above caster pos.z) | [FC] | missileeffect.cpp:1381,1425 | snapshot-only |
| per-column initial rotation | `random(0, 360 * TORADIAN)` ⇒ uniform in [0, 2π) | radians (about Z) | [FC] | missileeffect.cpp:1237 | snapshot-only |
| per-column initial scale | 100 | scale × 100 (integer-fixed; actual scale = `scale[n] / 100f`) | [FC] | missileeffect.cpp:1238 | snapshot-only |
| per-column initial framenum | `0 - (n / 3)` ⇒ 0, 0, 0, −1, −1, −1, −2, −2, −2, −3, −3, −3, −4, −4, −4 (n=0..14) | flipbook frame index (negatives skip render until ≥ 0) | [FC] | missileeffect.cpp:1226 | snapshot-only |
| numtexframes | reads `Get3DImagery()->GetTexture(0, &tex).numframes` — observed at Init | flipbook frames available in I3D texture | [FC] | missileeffect.cpp:1215-1216 | snapshot-only (frame-count needs visual confirmation off the I3D) |
| LAUNCH duration | one tick (sets `commanddone=true`, animator forces immediate base SetState→FLY on next Pulse via the `commanddone` path) | — | [FC] | missileeffect.cpp:1264-1268 | snapshot-only |
| FLY loop completions | 5 — `activesparks` (re-used here as a loop counter, NOT a particle count) increments when sprite n=14 wraps its flipbook; at 5 the EXPLODE transition fires | count | [FC] | missileeffect.cpp:1281-1286 | snapshot-only |
| FLY framenum advance | +1/tick per sprite | frames/tick | [FC] | missileeffect.cpp:1275 | snapshot-only |
| FLY framenum reroll on wrap | `random(0, 5)` (sets next frame on wrap; comment `// -32768` hints at an earlier "freeze" design) | frame | [FC] | missileeffect.cpp:1288 | snapshot-only |
| activesparks finale value | NUM_FIRE_COLUMNS (= 15) when the loop count hits 5 | sentinel | [FC] | missileeffect.cpp:1284 | snapshot-only |
| EXPLODE shrink fast | scale -= 33/tick on the trailing-active column (`activesparks > 2`) | scale/tick | [FC] | missileeffect.cpp:1311 | snapshot-only |
| EXPLODE shrink slow | scale -= 8/tick on the leading column (`activesparks <= 2`) | scale/tick | [FC] | missileeffect.cpp:1315 | snapshot-only |
| EXPLODE kill-column threshold | scale ≤ 10 → framenum[n] = −1 (stops rendering), activesparks-- | scale | [FC] | missileeffect.cpp:1303-1309 | snapshot-only |
| EXPLODE final done | `scale[NUM_FIRE_COLUMNS - 1] < 10` → SetCommandDone(true) (the LAST column reaching 10 ends the effect) | — | [FC] | missileeffect.cpp:1324-1326 | snapshot-only |
| EXPLODE framenum loop | `framenum[n]++; if >= numtexframes → random(0, 5)` while framenum[n] ≥ 0 | — | [FC] | missileeffect.cpp:1317-1322 | snapshot-only |
| column sprite scale.x | `1 * scale[n] / 100f` (n=0..11) or `2 * scale[n] / 100f` (n=12 the head column) | scale | [FC] | missileeffect.cpp:1369-1378 | snapshot-only |
| column sprite scale.y | `2 * scale[n] / 100f` (n=0..11) or `3 * scale[n] / 100f` (n=12) | scale | [FC] | missileeffect.cpp:1370-1378 | snapshot-only |
| column sprite scale.z | `3 * scale[n] / 100f` (n=0..11) or `4 * scale[n] / 100f` (n=12) | scale | [FC] | missileeffect.cpp:1371-1378 | snapshot-only |
| explosion sprite scale.x | `3 * ((scale[n] / 2 + 50) / 100)` (n=13) or `2.5 * ((scale[n] / 2 + 50) / 100)` (n=14) | scale | [FC] | missileeffect.cpp:1413-1420 | snapshot-only |
| explosion sprite scale.y | same as scale.x (n=13/14) | scale | [FC] | missileeffect.cpp:1414-1420 | snapshot-only |
| explosion sprite scale.z | `4 * scale[n] / 100f` (n=13) or `3.5 * scale[n] / 100f` (n=14) | scale | [FC] | missileeffect.cpp:1415-1421 | snapshot-only |
| CULLMODE | `D3DCULL_NONE` (saved + restored around Render — draw both faces) | — | [FC] | missileeffect.cpp:1352-1353,1443 | snapshot-only |
| **blend (animator)** | `SetBlendState()` ⇒ **Alpha (modulated)**: `D3DTBLEND_MODULATE` + `SRCBLEND=SRC_ALPHA` + `DESTBLEND=INV_SRC_ALPHA` + `ZWRITE=false` + `ZENABLE=true` (effect_old.cpp:221-233) | — | [FC] | missileeffect.cpp:1348-1349 (helper call); helper body effect_old.cpp:221-233 | snapshot-only — **SUSPECT, see §7 sanity check.** Render bodies in retail not decompiled; sister fire-spell family (FireFlash/Wind/Cone/DragonFire) ships **Additive**. |
| TORADIAN | π/180 | rad/deg | — | revdefs.h:25 | snapshot-only |

`random(min, max)` is **inclusive** on both ends (`rand()%(max−min+1)+min`,
`src/revutils.cpp:1597-1612`). All speeds/durations are authored at the
intended **24 Hz sim tick** (NOMENCLATURE §6); `Animate` runs once per tick.

> **The `0 - (n / 3)` framenum staggering is the spawn rhythm.** Columns
> n=0,1,2 start at framenum 0 (immediately render); n=3,4,5 at −1; n=6,7,8 at
> −2; ... n=12,13,14 at −4. While framenum < 0 the column is invisible
> (`if (framenum[n] >= 0)` guard at `:1360,1404`). Each tick framenum++; so
> columns appear in groups of 3, 1 tick apart — a 5-tick total ramp-up before
> all 15 are visible.

---

## 4. Assets

| asset | path | size | role | how loaded (original) |
|-------|------|------|------|-----------------------|
| FireColumn (mesh + textures) | `legacy/Imagery/Magic/FireColumn.I3D` | 175,396 B (MD5 `7603a16b35fcd7604858261e150fbf8c`) | the column + explosion sprite imagery, with a multi-frame texture for flipbook animation | registered `legacy/Class.Def:2040` as `"FireColumn" "Magic\FireColumn.I3D" 0x12da54d2`; loaded via the OBJCLASS_EFFECT registry on spawn (NOT preloaded in `playscreen.cpp`) |

**Asset identity: STRIPPED FROM RETAIL** (see §2.1.3). The asset is in the
snapshot's `legacy/Imagery/Magic/` tree but is NOT a member of the shipped
`data/imagery.rvi`, and the corresponding `class.def` line was removed. So the
asset that the reconstruction would draw against is the snapshot-tree copy,
with no shipped retail to verify against.

### 4.1 I3D structure (read from the binary)

I3D header (file: `legacy/Imagery/Magic/FireColumn.I3D`):
- Magic: `CGSR` (offset 0x00).
- Type tag: `strip` (this is a STRIP-type imagery, NOT a STILL like Blood.I3D).
- 3 animation states: `strip launch`, `strip fly`, `strip explode` — names map
  1:1 to the missile states LAUNCH/FLY/EXPLODE the animator drives via
  `inst->SetAnimState(...)` (handled by the `T3DAnimator` base when the
  underlying instance changes state).
- **2 sub-objects** with names `flamecolum` (the thin tall column quad) and
  `flameexplo` (the fatter explosion quad). Confirmed via `strings` on the I3D
  binary.

### 4.2 Sub-object → animator mapping

| `GetObject(idx)` | I3D sub-object name | role | render loop | cite |
|------------------|---------------------|------|-------------|------|
| `GetObject(0)` | `flamecolum` | **column sprite** — drawn 13× for the trailing positions (n = 0..NUM_FIRE_COLUMNS−3 = 0..12) | `Render` outer loop | missileeffect.cpp:1355,1357 |
| `GetObject(1)` | `flameexplo` | **explosion sprite** — drawn 2× for the head positions (n = NUM_FIRE_COLUMNS−2 .. NUM_FIRE_COLUMNS−1 = 13, 14) | `Render` inner loop | missileeffect.cpp:1399,1401 |

So the column visual is: **13 thin tall column quads strung out along the aim
vector + 2 fatter explosion quads at the head**. The head column (n=12) is
explicitly scaled 2× wider than the trailing 12 columns (`scale.x = 2 * scale /
100f` instead of `1 * scale / 100f`), and the very last sprite (n=14) is
slightly narrower than the n=13 explosion (`2.5 *` vs `3 *`). The column gets
visually bigger from tail → head, then bridges into the wider explosion sprites.

### 4.3 Texture layout — flipbook frames, NOT a UV atlas

`TFireColumnAnimator::Initialize` (`missileeffect.cpp:1214-1216`):
```cpp
S3DTex tex;
Get3DImagery()->GetTexture(0, &tex);
numtexframes = tex.numframes;
```

The animator reads `tex.numframes` — the per-frame texture-handle count on
texture slot 0. This is the same mechanism F03 TFireEffect uses (per-frame
`framehtexs[]` array of texture handles, NOT a UV sub-rect atlas; see
[knowledge/03 §8.6](knowledge/03_RENDERING_CONVENTIONS.md) and F03's INVENTORY
note). The Render body does NOT recompute UVs per frame; instead it writes
`frame = framenum[n] % numtexframes` (`missileeffect.cpp:1390,1434`) — which is
read by `RenderObject` as `obj->frame` (or as the global animator's `frame`
member) to index into the imagery's per-frame texture handle table on draw.

**The I3D texture is a multi-frame ANIMATED texture (`numframes > 1`)** — the
N frames are independent images at the same UV. The forensics can NOT enumerate
the cell layout in advance: there is no grid (1×N flipbook is the term, but
even that is a UV-atlas idiom; here it's per-frame engine texture handles).
Reconstruction must:
1. Read `numframes` off the loaded I3D texture record at runtime.
2. On each column draw, pass `frame = framenum[n] % numframes` to the renderer
   so it samples the corresponding per-frame texture handle.

The frame count `numtexframes` is **not** hardcoded in the snapshot source —
the animator queries it at `Initialize`. To enumerate exactly which frames the
I3D ships, an asset-decode pass on `FireColumn.I3D` is required (see §13.2 —
gap). The Animate loop's hardcoded `random(0, 5)` reroll on flipbook wrap
implies there are at least 6 frames; the negative-framenum staggering implies
the dev expected a frame-cycle of at least ~5 to absorb the staggered start.

**No procedural stand-in.** Reconstruction must draw the real
`Magic\FireColumn.I3D` flamecolum + flameexplo sub-objects with their authored
flipbook texture. Substituting a procedural orange column is exactly the
[feedback-no-standins] failure mode.

---

## 5. Spawn & emit

### 5.1 Effect spawn — N/A in shipped game

There is **no live caller** for `"FIRECOLUMN"` anywhere in shipped data
(retail) or in the pre-release snapshot:
- No spell.def variant (legacy + retail) references `firecolumn`/`FIRECOLUMN`.
- No `ATTACHEFFECT "firecolumn"` directive in `Class.Def` / `char.def` /
  `monster.def` / `object.def`.
- No `NewObject("FIRECOLUMN")` or `FindObjType("firecolumn")` call in any
  `src/*.cpp`.
- No `IMPACTEFFECT` / `EXPLODEEFFECT` keyword in spell.def that names this.

The catalog notes that the pre-release intent was for **the FireBall impact to
spawn a FireColumn** ("F07 sub-effect"). The snapshot does NOT do this —
`TFireBallAnimator::Animate` EXPLODE-entry path (`src/missileeffect.cpp:684-757`,
see F07 §6.2) generates the burst + ring + sparks inline; no NewObject call.
And the retail data evidence (asset stripped, class.def entry removed) suggests
the intent was abandoned, not realized in a different way. See §13.1 for the
open question.

### 5.2 If/when constructed: emit pattern (from snapshot Initialize)

If a caller were to `NewObject("FIRECOLUMN")` at the caster's hand or feet
(face = caster's aim byte-angle), `TFireColumnAnimator::Initialize`
(`missileeffect.cpp:1207-1241`) seeds the 15-sprite chain:

```
on Init (one-shot):
    numtexframes = imagery.texture[0].numframes              // :1215-1216
    state = oldstate = MISSILE_LAUNCH                         // :1219
    ConvertToVector(inst->GetFace(), 10, vctr)                // :1221  aim → unit step
    p[0] = (0, 0)                                             // :1222
    OffsetPoint(0)                                            // :1223  p[0] += vctr
                                                              //        — so p[0] = (vctr.x, vctr.y)
    for n in 0..NUM_FIRE_COLUMNS-1:                           // :1224
        framenum[n] = 0 - (n / 3)                             // :1226 [0,0,0,-1,-1,-1,-2,...,-4]
        p[n].x = vctr.x * (n + 3)                             // :1229 OVERWRITES p[n].x from above
        p[n].y = vctr.y * (n + 3)                             // :1230
        rotation[n] = random(0, 360 * TORADIAN)               // :1237  uniform [0, 2π)
        scale[n] = 100                                        // :1238  integer-fixed scale × 100
    activesparks = 0                                          // :1240  loop counter
```

`OffsetPoint(0)` at `:1223` advances `p[0]` by one `vctr` step, but the loop
body at `:1229-1230` immediately overwrites `p[n].x/y` (for all n including 0)
with `vctr.x/y * (n + 3)`. Net result: `p[n] = (vctr * (n + 3))`, n = 0..14 —
so `p[0]` is 3 vctr steps out, `p[1]` is 4 steps, …, `p[14]` is 17 steps.

**The `OffsetPoint(0)` call is a no-op** (its effect is overwritten the next
line). It's residual code from an earlier "advance the head" design — see §13.4.

### 5.3 Trigger semantics + emit geometry

- **Trigger semantics:** **one-shot** when constructed. The animator runs its
  3-state machine to completion: LAUNCH (one tick, no-op) → FLY (15 columns
  cycle their flipbook 5 times = ~75 ticks if numtexframes ≈ 5) → EXPLODE
  (reverse-collapse, ends when `scale[14] < 10` ≈ ~12 ticks at the slow shrink
  rate of 8/tick).
- **Count per trigger:** 15 sprite slots (13 column + 2 explosion).
- **Initial direction / distribution:** **NOT a radial spray — a straight line**
  along the caster's aim vector. `vctr = ConvertToVector(face, 10)` gives a 2D
  unit step along the byte-angle facing, scaled by 10 (so each `(n+3)*vctr` step
  is 10 wu × (n+3) in the XY plane). The chain has zero spread; each column is
  exactly `10 * (n + 3)` wu out from the effect's world origin along the aim
  direction.
- **Emit anchor convention:** the *effect object* sits at the caster's spawn pos
  (a caller would place it where the missile spawns — typically the caster's
  hand or the impact point); the animator's `pos` (= `inst->pos`) is the
  reference. Per-column world pos = `obj->pos = effect_pos + p[n] + (0, 0, +5)`
  (`missileeffect.cpp:1379-1381,1423-1425`). The constant `+5` z-lift puts the
  columns 5 wu above the effect origin (slightly above the floor when ground-
  spawned).
- **Coordinate space:** mixed. `p[n]` is computed in **world XY** (`vctr` is a
  world-aim vector) but stored on the animator; per-frame the Render code adds
  `effect_pos` to produce the world pos for each sub-object draw. Render flags
  `OBJ3D_MATRIX | OBJ3D_ABSPOS` (`missileeffect.cpp:1366,1410`) mark the
  matrix as absolute world (per `src/3dimage.h:113,127`).
- **Spread / jitter:** **none in position** (deterministic straight chain) +
  **uniform Z-rotation per column** (`random(0, 360 * TORADIAN)`) + the
  staggered framenum start (`0 - (n/3)`). Each column spins in-plane at its own
  random angle.

### 5.4 Spatial diagram

```
   wz (up)
   │           ╔═╗            ← n=13,14 = flameexplo (fatter, 2.5–3× XY, 3.5–4× Z)
   │           ║ ║          
   │           ║ ║          
   │      ║ ║ ║ ║ ║          ← n=12 = head flamecolum (2× wider — n*1, last n)
   │     ║║║║║║║║║║          ← n=0..11 = 12 flamecolum tubes (1× XY, 2-3× Z)
   │     · · · · ·  · ·       (random Z-rotation per column;
   │     ↑ all 5 wu above effect.pos.z (+5 z-lift)               
   ●─────●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●→ wx  (along aim vector)
   │     │
   │   effect.pos (caster face direction)
   │
   └─ p[n] = (n+3) * vctr  where vctr = ConvertToVector(face, 10)
   ╱        so p[0] = 3·(10·cos, 10·sin) = 30 wu out
  ╱         p[14] = 17·(10·cos, 10·sin) = 170 wu out from effect origin
 wy         (one tight line, no spread)

top-down: caster ───→ aim ───→ ●─●─●─●─●─●─●─●─●─●─●─●─●─◐◐  (◐ = explosion)
                                  ^                       ^
                              p[0]=3*vctr           p[14]=17*vctr
                              (30 wu)                (170 wu)
```

The visible result is a **straight line of fire columns walking outward from
the caster's facing**, terminating in 2 explosion puffs at the far end. No spread,
no arc — purely directional. (Contrast B01 blood spray, which fans angles.)

---

## 6. Behavior & per-frame logic

`TFireColumnEffect::Pulse` (`missileeffect.cpp:1172-1194`) drives the standard
`TMissileEffect` 3-state machine. `TFireColumnAnimator::Animate`
(`missileeffect.cpp:1255-1334`) supplies the per-state visual update.

### 6.1 Effect-side Pulse (retail-confirmed)

```
on each sim tick:
    TMissileEffect::Pulse()                                            // :1174 — base
                                                                       //   does Move, range--, hit-detect, state transitions (F07 §6.1)
    switch (state):
        case MISSILE_LAUNCH:                                           // :1178
            if (firsttime):                                            // :1180
                firsttime = false                                      // :1182
                // SoundPlayer.Play(LIGHTNING_SOUND)                   // :1183 — COMMENTED
        case MISSILE_FLY:                                              // :1187
            if (commanddone):                                          // :1189
                range = 0                                              // :1190 — base sees range≤0 next tick → EXPLODE
```

**Retail-confirmed** at `cls_0x5b4534.cpp:59-75` (see §2.1.1). The `commanddone`
gate is the FireColumn-specific exit path: instead of waiting for the missile
to hit a wall/character (which never happens, since `speed=0` means the missile
doesn't move), the **animator** signals end-of-FLY by setting
`inst->SetCommandDone(true)`, and the **effect** observes it next tick and
forces `range = 0`, which makes the base Pulse transition to EXPLODE.

> **`FIRECOLUMN_SPEED = 0` invariant.** Because speed is 0, `TMissileEffect::Pulse`'s
> LAUNCH→FLY transition (F07 §6.1 lines :60-70) skips the `vel = ConvertToVector(...)`
> branch (`if (speed) { ... }`) entirely — the missile **never gets a velocity
> assigned**, never moves, never tests its position against characters within 32 wu
> (the `Distance(chr) > 32` check still runs, but the missile sits at the spawn
> point), and never sets `range = (240 * 2) / (speed / ROLLOVER)` (which would
> divide by zero!). So the FLY-state exit relies ENTIRELY on the animator's
> `commanddone` signal; without it the effect would FLY forever. See §13.3 for
> the divide-by-zero risk if speed != 0 is ever passed.

### 6.2 Animator-side Animate (snapshot-only, `missileeffect.cpp:1255-1334`)

```
on each tick:
    T3DAnimator::Animate(draw)                                         // :1259
    inst->SetCommandDone(false)                                        // :1260

    switch (state):                                                     // mirrors effect's state
        case MISSILE_LAUNCH:                                            // :1264
            // No fancy launch, yet (per source comment :1266)
            inst->SetCommandDone(true)                                  // :1267 — immediately ends LAUNCH
        case MISSILE_FLY:                                               // :1271
            for n in 0..NUM_FIRE_COLUMNS-1:                             // :1273
                framenum[n]++                                           // :1275
                if framenum[n] >= numtexframes:                         // :1277  flipbook wrap
                    if n == NUM_FIRE_COLUMNS - 1:                       // :1279  the very last sprite (n=14)
                        activesparks++                                  // :1281  loop counter (RE-USED here, NOT particle count)
                        if activesparks == 5:                           // :1282  5 loops total = end of FLY
                            activesparks = NUM_FIRE_COLUMNS             // :1284  flip sentinel to 15 for EXPLODE phase
                            inst->SetCommandDone(true)                  // :1285  signal Effect: end FLY
                    framenum[n] = random(0, 5)                          // :1288  reroll start frame
        case MISSILE_EXPLODE:                                           // :1294
            for n in 0..NUM_FIRE_COLUMNS-1:                             // :1296
                if activesparks > 2:                                    // :1299  reverse-collapse phase
                    if n <= (NUM_FIRE_COLUMNS - activesparks):          // :1301  the trailing column slot
                        if scale[n] <= 10:                              // :1303  too small to see
                            if framenum[n] >= 0:                        // :1305  haven't been killed yet
                                activesparks--                          // :1307  advance the collapse pointer
                                framenum[n] = -1                        // :1308  hide forever (negative framenum skips render)
                        else:
                            scale[n] -= 33                              // :1311  fast shrink on the trailing column
                else:                                                   // :1314  the leading 2 columns
                    scale[n] -= 8                                       // :1315  slow shrink

                if framenum[n] >= 0:                                    // :1317
                    framenum[n]++                                       // :1319  continue flipbook in EXPLODE
                    if framenum[n] >= numtexframes:                     // :1320
                        framenum[n] = random(0, 5)                      // :1321

                if scale[NUM_FIRE_COLUMNS - 1] < 10:                    // :1324  the very LAST sprite (head explo)
                    inst->SetCommandDone(true)                          // :1326  end EXPLODE → effect KillThisEffect
    oldstate = state                                                    // :1333
```

**Behavior characterization:**

- **LAUNCH** is a 1-tick no-op. The animator just sets `commanddone=true`; on
  the next Pulse, the base sees status≠? (actually, status comes from a
  separate path) — actually, looking at `TMissileEffect::Pulse` LAUNCH branch
  (`missileeffect.cpp:58-72`), the base only transitions LAUNCH→FLY when
  `status` is true. `TFireColumnAnimator` does **not** call `SetStatus(true)`
  (compare to F07 fireball, which does in its LAUNCH animator). So how does it
  get to FLY?

  **The answer:** `inst->SetCommandDone(true)` is the FireColumn-specific
  signal — but the base's LAUNCH-exit check is `if (status)`, not
  `if (commanddone)`. So strictly per-snapshot, the animator's
  `SetCommandDone(true)` doesn't directly transition LAUNCH. Instead, the
  *effect* sees `commanddone` only in the FLY case (`:1189`). The LAUNCH→FLY
  transition would have to come from elsewhere. **This is a snapshot WIP gap**
  — likely the animator was meant to call `SetStatus(true)` (mirroring F07) but
  the call is missing. See §13.5.

- **FLY** runs until the LAST column (n=14) has flipbooked 5 times. Each tick
  every column advances its flipbook frame; on wrap the frame rerolls to
  `random(0, 5)` (which keeps the columns desynced as they cycle). The
  `activesparks` counter (despite the misleading name — it's NOT a particle
  count, just the *loop counter* for "how many times has n=14 finished its
  flipbook cycle") goes 0→1→2→3→4→5; at 5 it flips to NUM_FIRE_COLUMNS=15 as a
  sentinel for the EXPLODE phase to track which column is the "current
  collapse target".

- **EXPLODE** is the reverse-collapse phase. With `activesparks=15` initially,
  the condition `n <= NUM_FIRE_COLUMNS - activesparks` becomes `n <= 0` —
  only n=0 (the column closest to the caster, the FIRST in the chain) starts
  shrinking at 33/tick. When its scale hits ≤10, framenum[0] = -1 (hides), and
  `activesparks--` (→ 14). Now `n <= 1` — n=0 (already hidden) AND n=1 are the
  collapse targets, but n=0 already has framenum[0]=-1, so only n=1 shrinks.
  This pattern walks down the chain: the head sprite (n=14, last in the chain
  spatially) is the **last** to collapse. The collapse is **front-to-back from
  the caster's POV** — the column nearest the caster vanishes first, then
  walks outward.

  The leading 2 columns (n=13, 14, the two explosion sprites) are handled in
  the `else` branch (`activesparks <= 2`, lines :1314-1315) with a slow 8/tick
  shrink — this only kicks in *after* the 13 column sprites are gone and only
  the 2 explosion sprites remain.

  All columns continue cycling their flipbook during EXPLODE (`:1317-1322`)
  until they're hidden via `framenum[n] = -1`.

  End condition: `scale[NUM_FIRE_COLUMNS - 1] = scale[14] < 10` →
  `SetCommandDone(true)` → effect dies.

- **Velocity / motion field:** **NONE** in the animator. Columns are static at
  their seeded `p[n]` positions; only their flipbook frame, scale, and rotation
  change over time (well, rotation is set at Init and never re-rolled — see
  §13.6).

- **No alpha curve.** Nothing modifies a per-sprite alpha. The Render code
  doesn't set per-vertex color (no `D3DRGB(...)` writes to lverts) — the
  brightness is whatever the I3D texture frame is, blended via the Alpha helper.

### 6.3 Phase / state machine

```
state:      LAUNCH (1 tick)    │ FLY (5 flipbook loops ≈ 5·numtexframes ticks)    │ EXPLODE (~12-15 ticks reverse collapse) │ dead

per-column staggered start:    │ n=0,1,2:     visible at tick 0                   │
   framenum init [0,0,0,        │ n=3,4,5:     visible at tick 1                   │
   -1,-1,-1, -2,-2,-2,          │ n=6,7,8:     visible at tick 2                   │
   -3,-3,-3, -4,-4,-4]          │ n=9,10,11:   visible at tick 3                   │
                                │ n=12 (head): visible at tick 4                   │  collapse pointer walks
                                │ n=13,14 (explo): visible at tick 4               │  n=0 → n=1 → … → n=12
                                │ then all cycle their flipbook, n=14 wraps 5×     │  (scale -= 33/tick;
                                │                                                  │   hide at scale ≤ 10)
                                │                                                  │  then n=13, n=14 (explo)
                                │                                                  │   shrink at 8/tick
exit:                           │ activesparks reaches 5 → SetCommandDone(true);   │  scale[14] < 10
                                │ effect sees commanddone → range = 0 → EXPLODE    │  → SetCommandDone(true)
                                │                                                  │  → base sees HasAnimator()
                                │                                                  │     no-longer-relevant; KillThisEffect
```

### 6.4 Temporal diagram

```
scale[n] over time, per column n (×100 = author units):

100 ─┤●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●          ← LAUNCH+FLY: scale[n] = 100, constant
     │                                       \
     │                                  trailing  ← EXPLODE (n=0..12): scale -= 33/tick
     │                                  column     ← drops 100→67→34→1 in 3 ticks, then framenum=-1
     │                                  collapse
  10 ─┤- - - - - - - - - - - - - - - - - - - -●-●-  ← scale ≤ 10 cutoff → framenum[n] = -1 (hidden)
     │                                                                                       
   0 ─┴───────────────────────────────────────────── ticks
       tick 0          tick ~5·numframes             EXPLODE ~12 ticks then dead
       LAUNCH-FLY entry  EXPLODE entry               (head explo n=13,14 last to drop, slow 8/tick)

per-column visibility staggering during FLY (negative framenum → invisible until framenum >= 0):
       n=0,1,2:    █████████████████████  visible from tick 0
       n=3,4,5:    ·████████████████████  visible from tick 1
       n=6,7,8:    ··███████████████████  visible from tick 2
       n=9,10,11:  ···██████████████████  visible from tick 3
       n=12,13,14: ····█████████████████  visible from tick 4
                   ramp-up                full chain

activesparks loop counter:  FLY phase  0→1→2→3→4→5 (each ++ when n=14 wraps its flipbook)
                                                  └─→ flips to 15 = "EXPLODE collapse pointer"
                            EXPLODE:    15→14→13→…→3→2 (each -- when a column's scale ≤ 10)
                                                       └─→ now in the "leading 2 columns" slow-shrink
```

---

## 7. Rendering (original render state + geometry)

`TFireColumnAnimator::Render` (`missileeffect.cpp:1343-1448`) wraps both
sub-object draw loops in:
1. `SaveBlendState()` + `SetBlendState()` (outer wrapper)
2. `D3DRENDERSTATE_CULLMODE = D3DCULL_NONE` (saved + restored — so both faces
   of every column quad render)
3. Two sequential loops over the 15 columns: first 13 with `GetObject(0)` =
   `flamecolum`, then 2 with `GetObject(1)` = `flameexplo`.
4. `RestoreBlendState()`.

```
SaveBlendState()                                                                    // :1348
SetBlendState()                                                                     // :1349 (Alpha)
saved = GetRenderState(CULLMODE)                                                    // :1352
SetRenderState(CULLMODE, D3DCULL_NONE)                                              // :1353

obj = GetObject(0)                              // flamecolum (column sprite)        // :1355
for n in 0..NUM_FIRE_COLUMNS-3:                 // 0..12, the 13 column sprites      // :1357
    if framenum[n] < 0: continue                // skip until staggered start fires // :1360
    ResetExtents()
    obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS    // matrix override + world-space   // :1366
    obj->scl = (n == 12 ? (2,3,4) : (1,2,3)) * scale[n] / 100f                     // :1367-1378
    obj->pos = effect.pos + (p[n].x, p[n].y, +5)                                   // :1379-1381
    D3DMATRIXClear(obj->matrix)
    D3DMATRIXScale(obj->matrix, obj->scl)                                          // :1385
    D3DMATRIXRotateZ(obj->matrix, rotation[n])  // per-column random Z-spin         // :1386
    D3DMATRIXTranslate(obj->matrix, obj->pos)                                       // :1387
    frame = framenum[n] % numtexframes          // pick flipbook frame              // :1390
    RenderObject(obj)                                                               // :1392
    UpdateExtents()

obj = GetObject(1)                              // flameexplo (explosion sprite)   // :1399
for n in 13..14:                                // the 2 head explosion sprites    // :1401
    if framenum[n] < 0: continue
    ResetExtents()
    obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS
    obj->scl = (n == 13 ? (3, 3, 4*scale/100f) : (2.5, 2.5, 3.5*scale/100f))       // :1411-1421
               * ((scale[n] / 2 + 50) / 100)    // the XY axes use the squashed scale
    obj->pos = effect.pos + (p[n].x, p[n].y, +5)
    matrix = Scale · RotateZ(rotation[n]) · Translate(pos)
    frame = framenum[n] % numtexframes
    RenderObject(obj)
    UpdateExtents()

SetRenderState(CULLMODE, saved)                                                     // :1443
RestoreBlendState()                                                                 // :1445
```

### 7.1 What it draws

For each visible column slot (out of 15): **one I3D sub-object quad** drawn
once per Render call, with an explicit `OBJ3D_MATRIX` transform (scale × Z-rot
× translate). The column slots 0..12 use `flamecolum`; slots 13, 14 use
`flameexplo`. Total visible draws: up to 15 quads per tick (fewer during the
staggered start and during EXPLODE collapse).

The flipbook frame is picked via `frame = framenum[n] % numtexframes`
(`missileeffect.cpp:1390,1434`), which the renderer reads to index the I3D's
per-frame texture handle table for the texture record (§4.3).

### 7.2 Blend mode (original) + sanity check

**Helper traced:** `SetBlendState()` (`missileeffect.cpp:1349`). Helper body
(`src/effect_old.cpp:221-233`):
- `TEXTUREMAPBLEND = D3DTBLEND_MODULATE`
- `ZWRITEENABLE = false`
- `ZENABLE = true`
- `SRCBLEND = D3DBLEND_SRCALPHA`
- `DESTBLEND = D3DBLEND_INVSRCALPHA`

⇒ Classify as **Alpha (modulated)** per [NOMENCLATURE.md §3](NOMENCLATURE.md).

#### Blend sanity-check (mandatory)

**Conflict flagged: code traces Alpha, sprite design suggests Additive, sister
fire-spell family is Additive. Treat the Alpha trace as a snapshot-only WIP
artifact; reconstruction must visually vet.**

| signal | reads |
|--------|-------|
| traced helper (snapshot) | `SetBlendState()` ⇒ **Alpha** |
| sprite design | `flamecolum`/`flameexplo` are warm fire on a chroma-keyed (black) background — **textbook ADDITIVE** sprite (black = transparent under additive) |
| sister missile-family (FireBall, FlameDisc) | Alpha (consistent with this trace) |
| sister fire-SPELL family (FireFlash, FireWind, FireCone, DragonFire, Burn) | **Additive** (`SetAddBlendState`, `effect_old.cpp:2398,3097,3451,3659`) |
| retail Render body | **NOT decompiled** (no readable animator body in `cls_*`) — high snapshot-drift risk |
| sparks family precedent | the sparks animator traced Alpha in snapshot but shipped **Additive** (video-confirmed, see SPARKS doc) |

**Verdict.** The snapshot Alpha trace is consistent with the missile-family
pattern (FireBall + FlameDisc both Alpha), so it could be the real shipped
choice. But for a *fire* asset of this kind — bright warm flame on black
background — **additive is the textbook idiom** and the fire-spell siblings
all go additive. The snapshot-Alpha is **suspect**. Reconstruction should
hypothesis-test against the asset (load and view; if the column quads are
clearly chroma-keyed bright fire, additive is right) and against any reference
capture if one can be made (§13.2 — but the effect doesn't ship, so retail
ground-truth is unavailable). Default: trust the trace (Alpha) but log this as
the #1 visual-vetting risk.

### 7.3 Other render state

- **Lit vs self-lit:** **self-lit / Unlit.** The Render body does NOT call any
  `DoLighting` / `GetClosestLights` / vertex-color sampling pattern (contrast
  TBloodSystem `effectcomp.cpp:1372-1413`). No per-vertex `D3DRGB(...)` is
  written. The pixel color comes purely from the texture frame × MODULATE
  (×identity vertex color = white) = the texel as-is.
- **Depth / Z:** **TestNoWrite** (from `SetBlendState()`: ZWRITE off, ZENABLE
  on). There is **NO `RefreshZBuffer` override** in `TFireColumnAnimator` (no
  body in the snapshot under `missileeffect.cpp`) — the base
  `T3DAnimator::RefreshZBuffer` (likely a no-op or default-extents repair)
  applies. Contrast F07 FireBall which has an explicit `RefreshZBuffer` that
  walks the trail.
- **Orientation:** **per-column world-space placement with Z-rotation only.**
  Matrix = `Scale · RotateZ(rotation[n]) · Translate(world pos)`. There is no
  `RotateX(-π/2)` tip (the I3D's `flamecolum`/`flameexplo` quads are presumed
  authored vertical-aligned — the column's tall axis is local Y or local Z,
  rotated about world Z). There is no camera-facing math. This makes it
  closest to a **WorldUpAligned** orientation (NOMENCLATURE §2 — a future
  enum, the doc flags it as a needed extension) but with a *fixed* Z spin per
  column, not camera-following. In practice: the columns are fixed billboards
  in world space, each spun at a random Z-angle, that the camera sees in
  perspective — they will NOT track the camera. (Tip the camera 90° and the
  columns become foreshortened.) See §13.6 for whether the lack of camera
  tracking is intentional.
- **Per-quad transform:** explicit `OBJ3D_MATRIX | OBJ3D_ABSPOS` (matrix
  override + world-space pos). Computed each tick from `scale[n]`, `rotation[n]`,
  `p[n]`. The `OBJ3D_ABSPOS` bit means the renderer treats `obj->pos` as
  already-world (NOT to be added to the parent instance's pos again) — required
  because the matrix translate has `effect.pos + p[n]` already baked in.
- **Per-vertex color packing:** none — see "Lit vs self-lit" above. The lverts
  are not repainted; whatever vertex colors the I3D's `flamecolum`/`flameexplo`
  ship with are used as-is and the MODULATE stage multiplies them by the
  flipbook texel.

### 7.4 Per-column scale math

The column scale is **integer-fixed × 100** (`scale[n]` starts at 100 = "1.0×",
shrinks in integer steps). The Render code converts to float via `/ 100f` per
axis with **anisotropic multipliers**:

| sprite | n | scale.x | scale.y | scale.z | reads as |
|--------|---|---------|---------|---------|----------|
| trailing column (12 of them) | 0..11 | 1 × scale/100 | 2 × scale/100 | 3 × scale/100 | a tall thin tube (Y twice X, Z three times X) |
| **head column** | 12 | **2** × scale/100 | **3** × scale/100 | **4** × scale/100 | a noticeably bigger tube (2× wider, 3× taller in Y, 4× in Z) |
| **inner explosion** (n=13) | 13 | **3** × ((scale/2 + 50)/100) | **3** × ((scale/2 + 50)/100) | **4** × scale/100 | a wide-and-tall puff; XY uses the "(scale/2 + 50)/100" formula = (scale + 100)/200 — at scale=100 this is 1.0; at scale=10 this is 0.55 — so XY barely shrinks while Z shrinks normally |
| **outer explosion** (n=14) | 14 | **2.5** × ((scale/2 + 50)/100) | **2.5** × ((scale/2 + 50)/100) | **3.5** × scale/100 | similar but slightly smaller XY than n=13, and Z scales fully |

The `(scale[n] / 2 + 50) / 100` formula on the explosion XY axes is the
"persist-large" math: the explosion sprite's footprint stays roughly 1× even
as the column scale variable shrinks to 10 (where XY = `(10/2 + 50)/100 =
0.55`); only the Z (height) collapses with `scale`. So during EXPLODE the
explosion puffs *flatten* without shrinking sideways — a "dissipating cloud"
look.

The column tubes (n=0..12) collapse uniformly in all 3 axes — they shrink
to a point.

---

## 8. Texture animation

**Mechanism: flipbook via `numframes` (per-frame engine texture handles in
the I3D's texture record), NOT a UV sub-rect atlas, NOT UV scroll.**

- `Initialize` reads `numtexframes = imagery.GetTexture(0).numframes`
  (`missileeffect.cpp:1215-1216`).
- Per-column flipbook index `framenum[n]` advances `+1/tick` during FLY and
  EXPLODE (`:1275, 1319`).
- On wrap (`framenum[n] >= numtexframes`), the next frame is rerolled to
  `random(0, 5)` (`:1288, 1321`) — so columns don't re-sync after a cycle.
- Per draw, `frame = framenum[n] % numtexframes` (`:1390, 1434`) is written to
  the animator's `frame` member; the renderer's RenderObject body reads it to
  pick the per-frame texture handle from `obj->texture[0].framehtexs[frame]`
  (the I3D loader-side mechanism, same as F03 — see knowledge/03 §8.6 and
  F03's INVENTORY note).
- Negative framenum (`< 0`) skips the draw — used by the staggered start (Init
  seeds framenum[n] = `0 - (n/3)`, so until framenum++ pushes it to 0, the
  column is invisible) AND by EXPLODE column-kill (sets framenum[n] = −1 to
  retire a sprite).

**Per-instance phase offset:** the staggered Init `framenum[n] = -(n/3)` plus
the per-wrap `random(0, 5)` reroll keep the 15 columns visibly desynced —
they don't all flicker the same frame at the same time.

No UV manipulation: `tu/tv` are not written by the animator; the I3D's
authored corner UVs are used as-is and the *texture handle* swaps per-frame
under those UVs.

`numtexframes` value is **not in the source** — it's whatever the loaded
`FireColumn.I3D` ships with. The `random(0, 5)` reroll suggests the dev
expected at least 6 frames; the staggered `−(n/3)` start suggests at least 5
frames to absorb the ramp-up. Asset-decode (§13.2) needed to enumerate.

---

## 9. Associated light

**N/A — none emitted, none authored.**

- The animator body has no `AddPointLight` / dynamic-light call (search
  `src/missileeffect.cpp:1207-1448` for `light` / `Light` → nothing).
- The Effect has no `spell.def` entry (snapshot or retail; §2.1.5), so no
  `LIGHT COLOR ... INT ... MULT ...` data line attaches a per-cast light to
  this effect (contrast F07 FireBall which has `LIGHT COLOR 255,130,0 INT 255
  MULT 20` at `spell.def:844`).
- No `EFFECTLIGHT` directive in any def file.

If the effect were spawned (which it can't be in retail), the columns would
add NO illumination to the scene — they would self-light their own pixels (via
the additive-suspect blend, §7) but the world around them would stay as-lit
as if no fire were there.

This is a **plausible WIP gap** rather than a deliberate "no light" design
decision — fire of this size in this engine almost certainly was *meant* to
emit a warm light, and the missing `LIGHT` directive is consistent with the
effect's overall unfinished state (no spell, no caller, no shipped asset).
But documenting per `clean-slate`: as the original source stands, NO light is
emitted.

---

## 10. Color

- **Source:** the **authored `Magic\FireColumn.I3D` texture frames**
  (`flamecolum`/`flameexplo` sub-objects share texture slot 0's per-frame
  imagery). Color is purely the texture's pixels — no per-vertex color repaint
  in the animator, no spell color wiring (no spell), no hardcoded tint.
- **Exact values:** not enumerable from source — depends on the I3D's authored
  texture palette. Based on the sub-object names (`flame*`) and the family
  context (a fire effect), the expected hue is **warm orange/yellow/red** with
  black chroma-key background.
- **Expected visual:** **richly saturated warm fire** — like a flame torch
  carved into a column shape, walking along the aim. Pale/gray/white at
  reconstruction = port broken (stand-in texture, missed chroma-key, wrong
  blend, or texture frames not being indexed).
- **Normalization / boosts:** none. The MODULATE texture stage multiplies the
  texel by identity vertex color (white, no override), so the texel reads
  unmodified through the blend.

> **Color caveat from the unverified shipped state.** Since the effect doesn't
> ship in retail, there is NO in-game capture to compare against. The
> reconstruction has to trust that the snapshot-tree `FireColumn.I3D`'s
> authored palette matches the dev intent. If the file was last edited before
> a possible palette retune in the build pipeline, the snapshot's colors might
> diverge from "what the dev would have shipped." This is unrecoverable from
> the snapshot alone.

---

## 11. Audio coupling

- **`LIGHTNING_SOUND` mounted but commented out.** The Effect's `Initialize`
  has `//    SoundPlayer.Mount(LIGHTNING_SOUND);` (`missileeffect.cpp:1167`)
  and the destructor has `~TFireColumnEffect() { /*SoundPlayer.Unmount(LIGHTNING_SOUND);*/ }`
  (`missileeffect.h:250`); the LAUNCH branch of Pulse has
  `//              SoundPlayer.Play(LIGHTNING_SOUND);` (`missileeffect.cpp:1183`).
  All three are commented in the snapshot — and the retail Pulse body
  (`virt_meth_0x512430`) likewise sets `mbr_0x190 = 0` (= `firsttime = false`)
  without any sound-play call (visible in the body — there is no
  `cls_0x*::Play` invocation or sound-key string lookup in this method).
- **No retail audio strings linked to FireColumn.** The recon data table
  (`recon/ghidra/_data.txt`) has no `"FIRECOLUMN"`-XREF'd sound key beyond the
  "lightning" string which is shared by Photon/FlameDisc (per
  `recon/mappings/TFireColumnEffect_cls_0x5b4534_candidate.yaml:15` — "lightning"
  XREFs land in cls_0x5b4534 at +0x772; but cross-referencing the candidate
  yaml's caveat, that XREF most likely belongs to FlameDisc's `cls_0x5b45ac`
  via the merged FLAMEDISC discussion — not FireColumn).
- **No additional sound names.** No `FIRECOLHIT`/`FIRECOLSHOOT`/etc. event
  strings appear in `_data.txt` paired with this class.

**Audio status: no audio coupling found in the source.** The source comment +
the dtor's commented-out Unmount imply *intent* to play LIGHTNING_SOUND on
launch (matching Photon/FlameDisc), but the live code does not. Record:
silent in the snapshot; would presumably have been wired to play `LIGHTNING`
on LAUNCH had the effect been finished.

---

## 12. Triggers & in-game appearance

- **Spawned by:** **NO LIVE CALLER ANYWHERE.** Confirmed by exhaustive search:
  - `DEFINE_BUILDER("FIRECOLUMN", TFireColumnEffect)` registered at
    `missileeffect.cpp:1158` and `REGISTER_3DANIMATOR("FireColumn",
    TFireColumnAnimator)` at `:1196` — both go into the global dispatch tables
    at startup (the retail static-init functions at LAB_005123d0 / LAB_00512470,
    referenced from `recon/ghidra/_data.txt:56288/56290`).
  - But **nothing constructs them**: no `NewObject("FIRECOLUMN")` in any
    `src/*.cpp` (verified via grep), no `ATTACHEFFECT "FIRECOLUMN"` in any
    `*.def` file (snapshot or retail), no `EFFECT "firecolumn"` in any
    spell.def variant (snapshot or retail).
  - The shipped `class.def` (extracted from `data/imagery.rvi`) has **NO**
    `"FireColumn"` line — so even if a caller existed, `LoadImagery`/
    `FindImagery` would fail to resolve the asset.
- **Where to see it in the original game:** **NOWHERE.** The effect cannot be
  triggered through any normal gameplay path in retail. It exists only as:
  1. A C++ class compiled into the binary (`cls_0x5b4534`, dead code).
  2. Two registered name-strings (`"FIRECOLUMN"`/`"FireColumn"`) in the binary's
     dispatch tables, with no caller.
  3. The pre-release asset file `legacy/Imagery/Magic/FireColumn.I3D` on the
     dev tree, NOT in the shipped data.
- **Vestigial?** **Yes — vestigial in the strongest sense.** Compare to other
  vestiges:
  - F03 TFireEffect: registered in retail Class.Def, asset shipped, but no
    spell or ATTACHEFFECT triggers it. (Half-vestigial: the *infrastructure*
    shipped, the *trigger* didn't.)
  - F09 TFireColumnEffect: **fully vestigial** — the asset AND the Class.Def
    entry were stripped before ship. The dev decided to actively remove it,
    not merely leave it dormant.

### 12.1 The F07-impact-spawn open question

The catalog claimed FireBall impact spawns this effect. Two scenarios:
1. **Catalog conflated the in-mesh `explode` state with a separate effect.**
   The FireBall mesh has a `MISSILE_EXPLODE` state with its own visual; this
   may have been mistaken for a sub-spawned `FireColumn`. F07 §13.3 already
   flagged this.
2. **Pre-release intent that was abandoned.** The two effects are designed
   for adjacent roles (`TFireBall` = flying explosion, `TFireColumn` = vertical
   flame chain at the impact point), and they share `TMissileEffect` base —
   plausibly the dev planned `TFireBallAnimator::Animate EXPLODE` to call
   `NewObject("FIRECOLUMN", impact_pos, impact_face)`, but the call was never
   wired (snapshot has no such call), and during the pre-ship cleanup the
   abandoned FireColumn was stripped from data.

**Evidence favors scenario 2:** the F07 retail recon (TFireBallEffect
`cls_0x5b4290_candidate`) notes the **`FIRECOLUMN` string XREFs into this
class** (the FireBall code) at +0x280. Possible interpretations:
- (a) The FireBall class's static-init function at LAB_005123d0 — i.e. the
  `DEFINE_BUILDER("FIRECOLUMN", ...)` call — happens to live in code that
  shares a section with FireBall (linker layout artifact, not a runtime ref).
- (b) The shipped FireBall code still has the **stub** of a NewObject("FIRECOLUMN")
  call in its EXPLODE handler that the snapshot doesn't show — i.e. retail
  *added* the spawn after the snapshot but never had the asset to drive it.

Without readable bodies for the FireBall animator's EXPLODE state in recon, this
remains an open question. Per the asset-stripped + Class.Def-stripped evidence,
the most likely truth is that the spawn (if it ever was wired) was disabled by
removing the imagery — i.e. **the shipped FireBall does not spawn a FireColumn
because the asset isn't there to resolve.** Reconstruction should treat F09 as
**not reachable in shipped retail** and not chase the spawn-on-FireBall-impact
catalog claim until and unless retail captures confirm it.

---

## 13. Gaps & uncertainties

- **13.1 No live caller anywhere — the effect is vestigial.** The biggest fact
  about F09 is that it doesn't ship. Reconstruction must decide whether to:
  (a) skip F09 entirely as out-of-scope for retail-fidelity work, or (b)
  reconstruct it as a "what was intended" demo, knowing the result cannot be
  verified against retail. The forensics records what the snapshot says; the
  reconstruction's policy call is which scope applies.

- **13.2 Asset-decode gap — `numtexframes`.** The animator queries
  `numtexframes` off the loaded I3D at Initialize time, so the *number* of
  flipbook frames isn't a source constant. An asset-decode pass on
  `legacy/Imagery/Magic/FireColumn.I3D` would enumerate:
  - The exact `numframes` for texture slot 0.
  - Each per-frame texture handle and its image data (color palette,
    transparency-key pixel).
  - The two sub-objects' authored quad geometry (UV corners, vertex normals if
    any).
  Tried: `strings` on the I3D yields `flamecolum`/`flameexplo` only; no
  enumerable frame count from string-based inspection. A proper I3D decoder
  pass is required. This is the **dominant unknown** for visual reconstruction.

- **13.3 `FIRECOLUMN_SPEED = 0` and the LAUNCH→FLY transition.** The base
  `TMissileEffect::Pulse` LAUNCH branch (`missileeffect.cpp:58-72`) transitions
  to FLY only if `status` is true. The animator does NOT call `SetStatus(true)`
  (compare F07's `TFireBallAnimator` which does at `missileeffect.cpp:662`). And
  with `speed = 0`, the `if (speed) { ... range = (240 * 2) / (speed / ROLLOVER)
  ... }` block is skipped — but a hypothetical reconstruction that gives the
  missile any nonzero speed would **divide by zero** in that range computation.
  The reconstruction must either (a) match the snapshot's exact `speed=0`
  invariant + accept that the effect needs a different LAUNCH-exit (e.g. fire
  it directly from the caller as MISSILE_FLY), or (b) supply a small nonzero
  speed and a sane range (deviating from the snapshot to make it functional —
  flagged here as a port-WIP gap).

- **13.4 `OffsetPoint(0)` at `:1223` is a no-op.** Its effect on `p[0]` is
  immediately overwritten by the per-column loop at `:1229-1230`. Likely
  residual code from an earlier "the head advances each tick" design that was
  abandoned in favor of the static-positions-with-staggered-flipbook approach
  the rest of the code implements. Do not reconstruct `OffsetPoint` as a live
  helper.

- **13.5 No `SetStatus(true)` in the LAUNCH animator branch.** See §6.2: the
  source has `inst->SetCommandDone(true)` in LAUNCH (`:1267`), but the base
  Pulse's LAUNCH-exit gate is `status`, not `commanddone`. This is either:
  (a) a snapshot bug (the dev meant `SetStatus(true)`, mirroring F07), or
  (b) the dev was going to remove the `MISSILE_LAUNCH` state from this
  effect's lifecycle and start it directly in `MISSILE_FLY`. The source comment
  `// No fancy launch, yet` (`:1266`) hints at (b) but no code path actually
  implements it. Reconstruction must pick: either inject `SetStatus(true)` in
  LAUNCH or override the Initialize to start at `MISSILE_FLY` directly.

- **13.6 `rotation[n]` is set at Init and never updated.** Each column gets a
  uniform random Z-angle at construction and then holds that angle forever
  (no `rotation[n] +=` in Animate, no re-roll on flipbook wrap). So the columns
  are statically-rotated quads — no spin. Combined with no camera-facing math
  (§7.3) this means the columns lean at fixed world-space angles regardless of
  view. Looks acceptable from the "in-place flame columns" interpretation, but
  if the dev intended camera-following or per-tick spin, that's a snapshot gap.

- **13.7 The Render body has a stray `n` reuse.** At `:1401` the second loop
  reads `for (; n < NUM_FIRE_COLUMNS; n++)` — using the `n` left over from the
  first loop, which ended with `n = NUM_FIRE_COLUMNS - 2 = 13`. This is
  intentional, not a bug — the second loop continues from where the first left
  off. Documented for clarity since "loop continues with implicitly-shared
  loop variable" is a C-style idiom not all readers expect.

- **13.8 Blend mode unverified.** Per §7.2 sanity check, the snapshot Alpha
  trace is suspect for a fire sprite of this kind. With no retail visual to
  vet against (§2.1.3), reconstruction has to either (a) trust the trace and
  ship Alpha (matching the missile-family pattern: FireBall + FlameDisc), or
  (b) hypothesis-test Additive against the asset's actual pixel distribution
  (a black-keyed bright-fire texel histogram is the diagnostic). I recommend
  shipping Alpha to match the trace + sister missile siblings, with Additive
  flagged as the alternative if visual output reads "muddy" rather than
  "self-luminous."

- **13.9 The F07-impact-spawn open question** — see §12.1. Whether the shipped
  FireBall's EXPLODE state contains a (currently failing) `NewObject("FIRECOLUMN")`
  call cannot be determined without a readable retail decompilation of the
  FireBall animator's EXPLODE state. The asset being stripped from `class.def`
  + the snapshot not having the spawn call + no spell ever referencing FireColumn
  collectively suggest the spawn was either never wired or removed during
  pre-ship cleanup.

- **13.10 Effect class still ships as dead code.** The reconstruction's port
  should NOT include this effect by default (no caller, no asset in retail).
  If the project decides to reconstruct it anyway (as a "demonstrate the dev's
  intent" exhibit, similar to F03's vestigial reconstruction), document the
  decision explicitly so future agents know it's intentionally a snapshot-only
  reconstruction, not a retail-fidelity port.

---

## 14. Reconstruction burndown

```
- [ ] DECISION: F09 is vestigial in shipped retail — asset + Class.Def entry
      stripped, no caller, no spell. Either skip reconstruction entirely OR
      reconstruct as a "snapshot intent" demo, knowing retail-fidelity is
      unverifiable. (§2.1, §12, §13.1)
- [ ] (if reconstructing) Load Magic\FireColumn.I3D (175,396 B, snapshot-tree
      ONLY — not in data/imagery.rvi). Address its 2 sub-objects:
      GetObject(0) = flamecolum (column sprite, n=0..12), GetObject(1) =
      flameexplo (explosion sprite, n=13, 14). Read the texture slot 0
      numframes off the loaded I3D — required for the flipbook. (§4)
- [ ] Drive it as TFireColumnEffect + TFireColumnAnimator. TFireColumnEffect
      inherits the TMissileEffect 3-state machine (LAUNCH/FLY/EXPLODE — F07
      §6.1 [BASE]); its Pulse adds the FireColumn-specific MISSILE_FLY exit
      gate `if (commanddone) range = 0;`. (§6.1, retail-confirmed)
- [ ] Effect Initialize: TMissileEffect::Initialize (sets speed=16 default,
      state=LAUNCH, range=32768, status=false) → override SetSpeed(0) (the
      `FIRECOLUMN_SPEED = 0` invariant — missile is IMMOBILE; do NOT change
      this without also fixing the divide-by-zero in TMissileEffect::Pulse's
      LAUNCH→FLY range calc, §13.3) → firsttime = true. (§3, retail-confirmed)
- [ ] Animator Initialize: read numtexframes off the I3D texture, set
      state=oldstate=LAUNCH, vctr = ConvertToVector(face, 10), then seed all
      15 slots: p[n] = (vctr.x*(n+3), vctr.y*(n+3)) — a straight chain along
      the aim vector with per-column spacing 10 wu; framenum[n] = -(n/3)
      (staggered visible-start across 5 tick groups of 3 columns each);
      rotation[n] = random(0, 2π); scale[n] = 100 (×100 fixed); activesparks=0. (§5.2, §3)
- [ ] LAUNCH animator: one tick, set commanddone=true. NOTE the snapshot bug
      (§13.5): commanddone doesn't drive LAUNCH→FLY in the base — the
      reconstruction must either inject SetStatus(true) on LAUNCH-tick (mirror
      F07) or initialize state directly to FLY. (§6.2, §13.5)
- [ ] FLY animator (24Hz): each tick advance framenum[n]++ for all n; on wrap
      (framenum[n] >= numtexframes) reroll to random(0, 5); when the LAST
      sprite (n=14) wraps, activesparks++. After 5 such wraps (activesparks=5)
      flip activesparks=NUM_FIRE_COLUMNS(15) and set commanddone=true (signals
      effect to transition to EXPLODE next tick). (§6.2)
- [ ] EXPLODE animator: front-to-back reverse-collapse driven by the
      activesparks pointer. While activesparks > 2: the column at slot
      n = NUM_FIRE_COLUMNS - activesparks shrinks scale -= 33/tick; on scale
      <= 10 set framenum[n] = -1 (hide) and activesparks--. When activesparks
      <= 2: the 2 head explosion sprites shrink scale -= 8/tick (slow). All
      visible columns (framenum >= 0) continue cycling their flipbook with the
      same wrap-and-reroll. Exit when scale[14] < 10 → SetCommandDone(true) →
      effect KillThisEffect. (§6.2)
- [ ] Core geometry: per visible slot, one I3D sub-object quad placed by
      explicit matrix OBJ3D_MATRIX | OBJ3D_ABSPOS: scale × Z-rotate(rotation[n])
      × translate(effect.pos + p[n] + (0,0,+5)). 13 column sprites use
      GetObject(0) with anisotropic scl = (1,2,3)*scale/100 (n=0..11) or
      (2,3,4)*scale/100 (n=12 head); 2 explosion sprites use GetObject(1) with
      XY = (3,3)*((scale/2 + 50)/100) (n=13) or (2.5,2.5)*((scale/2 + 50)/100)
      (n=14), Z = 4 or 3.5 * scale/100. The XY formula keeps explosion footprint
      large while Z collapses. (§7.4, §3)
- [ ] Blend: SetBlendState() = Alpha (MODULATE + SRC_ALPHA/INV_SRC_ALPHA,
      ZWRITE off, ZTEST on). SUSPECT — sister fire-spell family is Additive;
      sister missile-family (FireBall/FlameDisc) is Alpha; effect is bright-fire
      on chroma-key (textbook Additive sprite). Default to Alpha per snapshot
      trace + missile-family pattern; flag the visual outcome as #1 vetting
      risk. CULLMODE=NONE wrapping both loops (draw both faces of every quad). (§7.2)
- [ ] Lit-mode: Unlit / self-lit. No DoLighting, no vertex-color repaint. (§7.3)
- [ ] Depth: TestNoWrite (ZWRITE=false, ZENABLE=true). NO RefreshZBuffer
      override — base T3DAnimator::RefreshZBuffer is whatever it defaults to. (§7.3)
- [ ] Orientation: per-column world-space matrix with per-instance Z-rotation
      only. NOT screen-aligned (no camera-facing math); NOT WorldXY tipped (no
      RotateX(-π/2)). The I3D's authored quad is presumed vertical-aligned;
      Z-rotation gives each column its random in-plane spin. Closest match in
      NOMENCLATURE: WorldUpAligned-with-fixed-Z-spin. (§7.3, §13.6)
- [ ] Texture animation: flipbook via numtexframes per-frame texture handles
      (NOT UV atlas, NOT UV scroll). Per-quad frame = framenum[n] %
      numtexframes; renderer indexes obj->texture[0].framehtexs[frame].
      Negative framenum skips render. random(0, 5) frame reroll on wrap keeps
      columns desynced. (§8)
- [ ] Color: pure texture-driven from the I3D's flipbook frames — no vertex
      tint, no spell color wiring (no spell), no hardcoded RGB. Expected
      warm-fire orange/yellow. Pale/gray = port broken. (§10)
- [ ] Associated dynamic light: NONE (snapshot has no AddPointLight call; no
      spell.def LIGHT directive since no spell exists). This is likely an
      unfinished gap rather than a "no light by design" choice, but document
      the original as NO light emitted. (§9)
- [ ] Sub-effects spawned: NONE. The animator does not create child effects.
      The 15 columns are slots inside ONE animator instance, not 15 separate
      effects. (§6)
- [ ] Audio: NONE in source (snapshot has LIGHTNING_SOUND mounted-but-commented
      AND play-but-commented; retail Pulse has no Play call either). Audio
      phase: if implementing, mirror Photon/FlameDisc and play "lightning" on
      LAUNCH. (§11)
- [ ] Trigger wiring: NONE in any data file or source. The DEFINE_BUILDER
      registration alone does not produce a live spawn. To exercise the effect
      for a snapshot-intent demo, add a test-mode invoker that calls
      NewObject("FIRECOLUMN") at a chosen pos+face. NO retail spell or impact
      hook to restore. (§12, §13.1)
- [ ] Do NOT reconstruct: OffsetPoint as a live helper (it's a no-op overwritten
      next line — §13.4); rotation-update math (rotation[n] is set once and
      held — §13.6); a missile velocity (speed=0 is invariant — §13.3); a
      shockwave ring or impact burst (none in source — that's F07, not F09).
- [ ] OPEN: confirm whether retail FireBall's EXPLODE branch contains a
      now-failing NewObject("FIRECOLUMN") call (catalog claim). Requires a
      readable retail decomp of the FireBall animator's EXPLODE state — not
      present in current recon. Until verified, treat F09 as NOT reachable in
      shipped retail. (§12.1, §13.9)
```

**Definition of done (snapshot-intent reconstruction, if pursued):** a
test-mode invocation `NewObject("FIRECOLUMN", pos, face)` produces a straight
chain of 15 warm-fire column quads stretching ~30→170 wu out from the spawn
point along the face vector, the trailing 13 thin tubes (flamecolum) widening
slightly at the head into 2 explosion puffs (flameexplo). The sprites appear in
staggered groups of 3 across the first 5 ticks, each spun at its own random
Z-angle, then cycle their flipbook frames for ~5 loops (≈ 5·numtexframes ticks)
in desync. Then the chain collapses front-to-back: nearest-to-caster column
vanishes first at fast shrink (33/tick), then the next, walking outward; the
2 explosion puffs at the head flatten more than they shrink (XY stays ≈1.0
while Z drops with `scale`), shrinking last at 8/tick. The effect dies when
the very last sprite scale drops below 10. Drawn Alpha (modulated MODULATE +
SRC_ALPHA/INV_SRC_ALPHA, CULLMODE=NONE, self-lit, depth-test no-write); NO
emitted light, NO sound, NO continuous emitter, NO shockwave ring. If the
visual reads "muddy black-edged fire" rather than "self-luminous flame," flip
the blend to Additive (sister fire-spell pattern) as the per-§7.2 sanity-check
hypothesis.
