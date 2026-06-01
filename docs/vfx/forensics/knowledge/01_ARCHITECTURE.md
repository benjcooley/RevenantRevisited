# 01 — Effect Architecture & Lifecycle (original 1998/99 engine)

Reference doc for VFX forensics agents. Describes how Revenant's original
spell-effect / animator system was structured so you can interpret what you find
in `src/effect_old.cpp` and the related sources. Pure reference — no engine code
here. Every concrete claim cites `file:line`.

Vocabulary is [NOMENCLATURE.md](../NOMENCLATURE.md). Sister doc:
[03_RENDERING_CONVENTIONS.md](03_RENDERING_CONVENTIONS.md) (render state, blend,
coordinate spaces, geometry).

This doc describes only how the **original 1998/99 engine** worked. It does not
discuss the modern port's structure.

Source-of-truth order (per [AGENT_GUIDE §3.1](../../AGENT_GUIDE.md)): retail
decomp `recon/classes/` wins for *behavior*; pre-release `src/effect_old.cpp`
etc. is *intent* and is where almost all numeric constants and the per-frame
math live. Where they differ, both are noted.

> **Where the pre-release source lives.** `src/effect_old.cpp` is a single
> archive TU. The real 1998 bodies are inside the **second** `#if 0` region that
> begins at `src/effect_old.cpp:126` (the file's own header note,
> `src/effect_old.cpp:1-44`, explains the gate). Lines 48-124 above the gate are
> modern *no-op link stubs* (e.g. `SaveBlendState`/`SetBlendState` at
> `src/effect_old.cpp:122-124`). Cite the gated bodies, not the stubs.

---

## 1. The `TEffect` base class

`TEffect` is the base for every spell-spawned visual effect object. It is a plain
game object — a subclass of `TObjectInstance` — not a renderer or particle
system. Declared `src/effect.h:46`:

```
class TEffect : public TObjectInstance      // src/effect.h:46
```

### 1.1 What it is in the object hierarchy

`TObjectInstance` (`src/object.h:671`) is the universal in-world object: it
carries position, facing, state/frame, flags, imagery, an optional animator
component, an optional script. `TEffect` adds only the *spell linkage*:

| member | type | meaning | cite |
|--------|------|---------|------|
| `spell` | `PTSpell` | the `TSpell` block that spawned this effect | `src/effect.h:65` |
| `subspell` | `int32_t` | "pseudo-talisman" sub-spell selector (e.g. ice-bolt tiers) | `src/effect.h:66` |
| `angle` | `int32_t` | cast/aim direction, resolved lazily by `GetAngle()` | `src/effect.h:67` |

Construction sets `OF_IMMOBILE | OF_PULSE` (`src/effect.h:49-50`): effects don't
walk the map mover, but they *do* get a `Pulse()` every sim tick (flag bit at
`src/object.h:540`).

> The repo also has `TComplexObject` (referenced in the task) — but in the
> shipped effect path the effect base is `TEffect : TObjectInstance`. No effect
> in `effect_old.cpp` derives from `TComplexObject`; could not find a
> `TComplexObject` effect base in the available source. Treat the effect base as
> `TEffect`/`TObjectInstance`.

### 1.2 What every effect inherits (the methods that matter)

From `TEffect` itself:

| method | role | cite |
|--------|------|------|
| `Pulse()` | per-tick hook; base resets frame 0 + clears command-done | `src/effect_old.cpp:418-424` |
| `KillThisEffect()` | flags `OF_KILL \| OF_PULSE`; also signals the parent spell's death | `src/effect_old.cpp:426-432` |
| `GetAngle()` | resolves aim from spell invoker→target each call | `src/effect_old.cpp:434-455` |
| `SetSpell()/GetSpell()` | wires the spawning `TSpell` | `src/effect.h:54-55` |
| `SetSubSpell()/GetSubSpell()` | sub-spell selector accessors | `src/effect.h:57-58` |

From `TObjectInstance` (the lifecycle virtuals, §3): `Pulse`, `Move`, `Animate`,
`NextFrame`, `OnScreen`, `OffScreen` — declared `src/object.h:1046-1061`.

---

## 2. The Effect + Animator split

This is the single most important structural fact for forensics. **An effect is
two cooperating objects**:

```
  TXxxEffect   (the game OBJECT — TObjectInstance subclass)   "what + when"
  TXxxAnimator (the per-frame BEHAVIOR + RENDER — T3DAnimator) "how it looks/moves"
```

The Effect lives in the map, holds the spell linkage, drives gameplay
(damage/heal/state). The Animator is the *visual component* attached to that
object's 3D imagery; it owns the particle arrays, the per-frame integration, and
the draw.

### 2.1 Division of responsibility

| concern | lives in the **Effect** | lives in the **Animator** |
|---------|-------------------------|----------------------------|
| spell linkage (invoker/target/variant) | yes (`spell`, `GetAngle`) | reads it via `inst` cast |
| gameplay payload (damage, heal, knockback) | yes — in `Initialize`/`Pulse` | sometimes (on impact) |
| lifecycle flags / kill | yes (`KillThisEffect`) | requests kill via `((PTEffect)inst)->KillThisEffect()` |
| particle arrays / state machine | no | yes |
| per-frame integration (gravity, drag, fade) | no | yes (`Animate`) |
| geometry build + draw | no | yes (`Render`) |
| z-buffer patch | no | yes (`RefreshZBuffer`) |

The Animator reaches back to its Effect through the `inst` pointer it inherits
from `T3DAnimator`/`TObjectAnimator`, casting it to the concrete effect type:
e.g. `((PTEffect)inst)->GetSubSpell()` (`src/effect_old.cpp:8470`),
`((PTDripEffect)inst)->GetParams(...)` (`src/effect_old.cpp:10994`).

### 2.2 Cited example pairs

| Effect (object) | Animator (visual) | builder name | cites |
|-----------------|-------------------|--------------|-------|
| `TFlameEffect` | `TFlameAnimator` | `"Flame"` | header `src/effect.h:430,929` |
| `THealEffect` | `THealAnimator` | `"Heal"` | `src/effect_old.cpp:461-462,618` |
| `TTeleporterEffect` | `TTeleporterAnimator` | `"Teleporter"` | `src/effect_old.cpp:5471,5495` |
| `TFireBallEffect` | `TFireBallAnimator` | `"FireBall"` | `src/missileeffect.cpp:479,498` |
| `TDripEffect` | `TDripAnimator` | `"Drip"` | `src/effect_old.cpp:10951,10979` |
| `TRippleEffect` | `TRippleAnimator` | `"Ripple"` | `src/effect_old.cpp:10686,10705` |
| `TStripEffect` | `TLightningAnimator` | `"lightstrip"` | `src/stripeffect.h:7-11`, `src/effectcomp.h:273` |

Note the *names* in the two registries are matched on the same string (§4) — that
is the glue that pairs an Effect with its Animator.

> **Not every effect has a custom Animator, and vice-versa.** Some effect names
> map only to the generic `TEffect` builder (water/wave/speaker variants are all
> registered to a `TGenericEffectBuilder`, `src/effect_old.cpp:58-71`). Some
> animators are stand-alone visuals with no bespoke effect class. When forensics
> finds a name, check *both* registries.

---

## 3. Object lifecycle — which method fires when

Two cadences interleave: a **sim-tick** path (gameplay/state) and a
**render-frame** path (animate + draw). In the original engine these were the
same loop, but the modern port separates them (see
[memory: tick and draw are separate concerns]), and forensics should record the
*intended* cadence as 24 Hz sim ticks (NOMENCLATURE §6).

| method | declared / defined | cadence | purpose |
|--------|--------------------|---------|---------|
| **`Initialize()`** | per-effect / per-animator | once, at spawn | one-shot gameplay payload (deal damage, heal), seed particle arrays. Effect side: `THealEffect::Initialize` defers to first `Pulse` (`src/effect_old.cpp:464-467`); `TFireFlashEffect::Initialize` damages the target immediately (`src/effect_old.cpp:1991-2004`). Animator side: `T3DAnimator::Initialize` (`src/3dimage.h:420`) then per-animator seeding, e.g. `TFlareAnimator::Initialize` zeroes the particle pos/vel and the material (`src/effect_old.cpp:517-550`). |
| **`Pulse()`** | `TObjectInstance::Pulse` (`src/object.h:1046`); `TEffect::Pulse` (`src/effect_old.cpp:418`) | per **sim tick** (only if `OF_PULSE`) | gameplay master tick. `TEffect::Pulse` resets to frame 0 + clears command-done. Effect subclasses do gameplay here (heal-over-time, range damage). |
| **`Animate(bool draw)`** | `TObjectInstance::Animate` (`src/object.h:1054`); `T3DAnimator::Animate` (`src/3dimage.cpp:2539`) | per **render frame** | advance the visual: integrate particle motion, advance counters, re-jitter strips, scroll UVs. The animator override always calls `T3DAnimator::Animate(draw)` first (refreshes bone hierarchy + mirrors pos, `src/3dimage.cpp:2539-2553`), then does its own integration. The `draw` flag tells the animator whether this frame is actually being rendered. |
| **`Render()`** | `T3DAnimator::Render` (`src/3dimage.cpp:2608`, default), per-animator override returning `bool` | per render frame (when on-screen) | build geometry + submit the draw. Wrapped in `SaveBlendState()/SetBlendState()…/RestoreBlendState()` (§03). Sets `obj->flags` (the `OBJ3D_*` transform-override bits, `src/3dimage.h:99-130`), pos/rot/scl, calls `RenderObject(obj)`, then `UpdateExtents()`. |
| **`RefreshZBuffer()`** | `T3DAnimator::RefreshZBuffer` (`src/3dimage.h:476`), per-animator override | per render frame | restores the scene Z under the effect's screen footprint so the (depth-write-disabled, §03) translucent effect doesn't punch a hole in the depth buffer. Pattern: project the effect origin with `WorldToScreen`, call `RestoreZ(x,y,w,h)` over the bounding patch — e.g. `THealAnimator::RefreshZBuffer` (`src/effect_old.cpp:835-850`). |
| **destructor** | `~T3DAnimator` → `Close()` (`src/3dimage.cpp:2125-2128`) | once, at kill | free per-animator particle arrays / owned vert+face buffers (objects flagged `OBJ3D_OWNSVERTS/OWNSFACES`, `src/3dimage.h:120,123`). |

Death is requested, not immediate: an animator (or the effect's own gameplay)
calls `KillThisEffect()` which sets `OF_KILL` (`src/effect_old.cpp:426-432`); the
map pane reaps `OF_KILL` objects on a later tick.

---

## 4. The builder + registry pattern (string-name → class)

Two parallel registries map a **string name** to a class. The same name keys both,
which is how an Effect object and its Animator get paired.

### 4.1 Object builders — `DEFINE_BUILDER` / `REGISTER_BUILDER`

`DEFINE_BUILDER("Name", TClass)` (macro at `src/object.h:95-104`) emits a
`TClassBuilder : TObjectBuilder` whose `Build(...)` does `new TClass(...)` and
whose ctor registers it under `"Name"`. `REGISTER_BUILDER(TClass)`
(`src/object.h:106`) instantiates one static builder so the registration runs at
static-init.

```
DEFINE_BUILDER("Heal", THealEffect)      // src/effect_old.cpp:461
REGISTER_BUILDER(THealEffect)            // src/effect_old.cpp:462
```

Builders live in a flat static array `TObjectBuilder::builders[MAXOBJECTTYPES]`
(`src/object.h:90`, cap 2048 at `src/object.h:64`); lookup is
`TObjectBuilder::GetBuilder(name)` (`src/object.h:83`, impl `src/object.cpp:323`).

### 4.2 Animator builders — `REGISTER_3DANIMATOR`

`REGISTER_3DANIMATOR("Name", TAnimator)` (macro `src/3dimage.h:342-350`) emits a
`TAnimatorBuilder : T3DAnimatorBuilder` whose `Build(oi)` returns `new
TAnimator(oi)`, registered under `"Name"`, plus a static instance.

```
REGISTER_3DANIMATOR("Heal", THealAnimator)   // src/effect_old.cpp:618
```

Animator builders live in `T3DAnimatorBuilder::builders[MAX3DANIMATORTYPES]`
(`src/3dimage.h:337`, cap 128 at `src/3dimage.h:323`); lookup is
`T3DAnimatorBuilder::GetBuilder(name)` (`src/3dimage.cpp:2112-2119`,
case-insensitive `stricmp`). A `REGISTER_MULTI_3DANIMATOR_` variant
(`src/3dimage.h:352-360`) lets several names share one animator class (e.g. the
four fountain colours, `src/effect.h:799-844`).

### 4.3 How a name resolves to both classes

1. `EffectClass.FindObjType(name)` (`src/object.h:389`, impl walks `objinfo`)
   turns the builder *name* into an integer `objtype`. `EffectClass` is the
   `OBJCLASS_EFFECT` object class, declared once: `src/effect_old.cpp:408`,
   `TObjectClass("EFFECT", OBJCLASS_EFFECT, 0)`.
2. Each registered type was bound to its builder + an imagery file by
   `TObjectClass::AddType(name, imgfilename, …)` (`src/object.cpp:2822-2849`),
   which looks the object builder up by *name* (falling back to the class name,
   `src/object.cpp:2824-2826`) and registers the I3D file via
   `TObjectImagery::RegisterImagery` (`src/object.cpp:2831`).
3. At spawn, `TObjectClass::NewObject` (`src/object.cpp:2865-2891`) loads the
   imagery and calls `objbuilder->Build(def, imagery)` → the concrete
   `TEffect` subclass.
4. When that object first needs an animator,
   `T3DImagery::NewObjectAnimator(oi)` (`src/3dimage.cpp:2060-2077`) looks up the
   **animator** registry by `oi->GetTypeName()` — the *same* name — falling back
   to the object's class name. So `"Heal"` → `THealEffect` (object) **and**
   `THealAnimator` (animator).

> **Forensics tell.** If you have a builder name, you can find its visual class
> by grepping `REGISTER_3DANIMATOR("<name>"` and its object class by grepping
> `DEFINE_BUILDER("<name>"`. The match is case-insensitive on the animator side
> (`stricmp`, `src/3dimage.cpp:2115`).

---

## 5. Spawn paths — how an effect enters the world

The canonical spawn is `MapPane.NewObject(SObjectDef{...})`:

```
TMapPane::NewObject(SObjectDef* def)         // src/mappane.cpp:1357
  -> TObjectClass::GetClass(def->objclass)   //   resolve class
  -> oc->NewObject(def)                       //   build instance (§4.3)
  -> AddShadow / AddObject                    //   insert into sector grid
```

The `SObjectDef` carries: `objclass = OBJCLASS_EFFECT`, `objtype =
EffectClass.FindObjType("<name>")`, `pos` (world x/y/z), `level`, `facing`. The
`OBJCLASS_EFFECT` category is the effect object class (`src/effect_old.cpp:408`;
class-ID table referenced at `src/object.h:177-179`).

### 5.1 Attaching to an owner / inheriting position

Effects are placed by absolute world position at spawn (`def.pos`). Owner-follow
is done by *re-positioning each frame*, not by a parent pointer:

- Character-attached effects copy the owner's pos: `TBurnEffect::Set(ch)` does
  `character->GetPos(pos); SetPos(pos);` (`src/effect_old.cpp:90-99`), and the
  burn animator re-reads the character each frame.
- Spell effects inherit the caster/target position via the spell's source
  position (§6).

NOMENCLATURE calls the followed point the **anchor**; the per-frame resolution is
the *emit anchor convention* the forensics protocol insists you record.

---

## 6. Spell invocation — `spell.def` → effect

### 6.1 The VARIANT line references a builder name

`data/Resources/spell.def` defines spells in `SPELL "name" … BEGIN … END`
blocks; each playable form is a `VARIANT` line. The field layout is documented
in-file at `data/Resources/spell.def:77-78`:

```
// NAME, TYPE, TALISMANS, EFFECT_NAME, MANA, WAIT_NEXT, MIN_D, MAX_D, SKILL, HEIGHT, FACING, LOOP_TIME
VARIANT "Heal", TP_BASIC, "BEA", "heal", 140, 70, 0, 203, 11, 0, TRUE, 70   // spell.def:744
```

The **4th field** (`"heal"`, `"lightstrip"`, `"fireball"`, `"firewind"`, …) is the
effect *builder name* — exactly the string keyed in §4. Examples:
`"fireflash"` (`spell.def:79`), `"lightstrip"` (`spell.def:688`), `"Quicksand"`
(`spell.def:306`), `"Funnel"` (tornado, `spell.def:477`).

### 6.2 A cast creates the effect

`TSpellManager::CastByName` (`src/spell.cpp:492`) builds a `TSpell` from the
variant. The actual effect object is created lazily in `TSpell::Timer()` once
`wait` elapses (`src/spell.cpp:431-458`):

```
def.objclass = OBJCLASS_EFFECT;                    // src/spell.cpp:440
def.level    = MapPane.GetMapLevel();              // :441
invoker->GetPos(def.pos);                           // :442  (caster pos…)
if (GetSourcePos(spos)) def.pos += spos;            // :444-445 (…+ hand/source)
else def.pos.z += variant->height;                  // :447  (HEIGHT field fallback)
if (variant->facing) def.facing = invoker->GetFace();// :448-449 (FACING field)
def.objtype = EffectClass.FindObjType(variant->effect); // :452 (EFFECT_NAME → type)
effect = MapPane.GetInstance(MapPane.NewObject(&def));   // :454
if (effect) ((PTEffect)effect)->SetSpell(this);     // :457 (wire spell back)
```

So the VARIANT's `HEIGHT` and `FACING` columns feed `def.pos.z` and `def.facing`;
`EFFECT_NAME` feeds `def.objtype`; `SetSpell(this)` is the linkage the effect
later reads.

### 6.3 The `spell->GetXxx()` / damage callbacks

After `SetSpell`, the effect calls back into its `TSpell` for gameplay context:

| callback | what it returns | cite |
|----------|-----------------|------|
| `spell->GetInvoker()` | caster object | `src/spell.h:129` |
| `spell->GetTarget(n)` | nth target | `src/spell.h:131-132` |
| `spell->GetSourcePos(pos)` | cast source (hand or explicit) | `src/spell.h:136` |
| `spell->VariantData()` | the SPELL.DEF variant struct (name, damages, height…) | `src/spell.h:143` |
| `spell->Damage(ch)` | applies `mindamage..maxdamage` of `spell->damagetype` to `ch`, honoring magic resistance | `src/spell.cpp:463-476` |

The **fire-damage hook** the task asks about is this `spell->Damage(target)` path
keyed on the variant's `DAMAGETYPE` (e.g. `DT_BURN`, `data/Resources/spell.def:40`).
`TFireFlashEffect::Initialize` is the clean example: it grabs the invoker's
current fight target and calls `spell->Damage(target)`
(`src/effect_old.cpp:1991-2004`). There is no literal `GetFireDamage()` symbol;
damage flows through `TSpell::Damage` + the range helpers below.

Area effects use the free helpers in `effect_old.cpp` (all gated bodies):
`DamageCharactersInRange` (`:247`), `BlastCharactersInRange` (`:278`, adds
knockback), `BurnCharactersInRange` (`:314`), `PulpCharactersInRange` (`:344`).
Each iterates `TMapIterator(OBJSET_CHARACTER)`, range-checks `Distance`, skips
friends via `IsEnemy`, then applies the payload.

---

## 7. Sub-effects (one effect spawns another)

An animator can spawn a *child* effect by calling `MapPane.NewObject` itself.
Canonical case: **drip → ripple** (NOMENCLATURE *sub-emit*; INVENTORY H04→H03).

`TDripAnimator::AddNewRipple(x,y,z,len)` (`src/effect_old.cpp:11007-11026`)
builds a fresh `SObjectDef`, sets `objclass = OBJCLASS_EFFECT` and `objtype =
EffectClass.FindObjType("ripple")`, positions it at the drip's landing point,
spawns it, then configures the child: `((PTRippleEffect)effect)->SetLength(len)`.
It is invoked from `TDripAnimator::Animate` when the falling drip reaches `z<=0`
(`src/effect_old.cpp:11057-11063`), and plays the `"drip"` sound on the same
event.

This is the general pattern: **a child effect is just another `NewObject` with
its own builder name, positioned by the parent and optionally parameterized via a
setter.** The fireball/lightning *trails and sparks* are not sub-effects — they
are internal sub-systems of one animator (see
[03 §8](03_RENDERING_CONVENTIONS.md)).

---

## Gaps / could-not-determine

- **`TComplexObject` as an effect base** — not found in the available source;
  the effect base is `TEffect : TObjectInstance` (§1.1).
- **Exact retail (Ghidra) per-effect constants** — this doc grounds constants in
  the pre-release source. Where `recon/classes/` has a mapped class
  (e.g. `cls_0x5b0a28.cpp` for `TLightningAnimator`, per `src/stripeffect.h:10`),
  retail wins; per-effect forensics docs must cross-check.
- **Whether `Pulse` ran strictly at 24 Hz in 1998** — the original loop ran
  pulse+animate together; the 24 Hz sim-tick gate is a NOMENCLATURE/port
  invariant (§6 of NOMENCLATURE). Record cadence as *intended* 24 Hz.
