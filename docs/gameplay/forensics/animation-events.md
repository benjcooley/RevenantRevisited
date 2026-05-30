# Forensics: Animation-Event (I3D Tag) Dispatch System

**Topic:** per-frame consumption of I3D `play` / `beg` / `end` / controller
tags during animation playback — footsteps, weapon-swipe spawn, impact-frame
hit resolution.
**Author:** gameplay/combat forensics agent, 2026-05-19.
**Status:** forensics complete; port not started (research-only brief).

> Headline finding (and a correction to the burndown's framing): the
> `play`-tag → footstep-sound path **is already ported and structurally
> wired** in `src/`. `T3DAnimator::Pulse()` → `T3DImagery::PlaySound()` →
> `TObjectInstance::PlayWave()` → `SoundPlayer.Play()` exists end-to-end and
> matches the 1999 source line-for-line. The real gaps are narrower:
> (a) the `--test=charpreview` rig calls `NextFrame()` but never `Pulse()`,
> so it never *exercises* the path; (b) the legacy `T3DController` tag axis
> (`scrolltex` / `animtex`, and the `RefreshControllers` call in
> `T3DAnimator::Pulse`) was deliberately removed; (c) the weapon swipe is
> **not** tag-driven at all (it is combat-state-driven), and impact hits are
> **not** tag-driven either (they key off `attack->impacttime`, a rules.def
> numeric). The port plan below is therefore mostly "verify + light up + add
> a test", not "build a subsystem".

---

## 0. Source-of-truth note (snapshot ≠ retail)

The repo carries the **full 1999 pre-release source tree** under `legacy/`
(not just the `src/` port). For this subsystem `legacy/` is the strongest
evidence we have short of a Ghidra session, because the dispatch is a
handful of small functions that survived verbatim into the port. Where I
cite `legacy/<file>` I mean the pre-release source; where I cite `src/` I
mean the live port. The retail binary (`data/RevenantDev`) was **not**
re-decompiled for this pass — the `play`/`beg`/`end` strings live in data
and the animator's `Pulse` is merged/missing from the OOAnalyzer output, so
a confirmation would need on-demand `DecompileAddr.java` work. Fidelity of
each finding is rated at the end of its section.

---

## 1. The dispatch architecture (how tags are consumed per frame)

There are **three independent tag-consumption mechanisms**, and they are
*not* a single "scan the tag array each frame" loop. Conflating them is the
trap.

### 1a. `play` tags → sound, via the animator's per-frame `Pulse()`

This is the footstep path and the one the burndown calls "dead".

Per-tick driver chain (current port):

```
TMapPane::Pulse()                         src/mappane.cpp:2600
  ├─ NextFrameObjects()                    src/mappane.cpp:3755  (advances frame FIRST)
  │     └─ inst->NextFrame()               src/object.cpp:1545   (frame += framerate, :1569)
  └─ PulseObjects()                        src/mappane.cpp:3767  (dispatches tags SECOND)
        └─ inst->Pulse()                   src/mappane.cpp:3777
              TCharacter::Pulse()          src/character.cpp:182
                └─ anim->Pulse()           src/character.cpp:190
                      T3DAnimator::Pulse()  src/3dimage.cpp:2243
                        └─ ((T3DImagery*)image)->PlaySound(inst, state, frame)  :2245
```

`T3DImagery::PlaySound` (src/3dimage.cpp:1532):

```cpp
void T3DImagery::PlaySound(TObjectInstance* inst, int32_t state, int32_t frame)
{
    char* soundlist = FindTag((char*)"play", state, frame);
    if (soundlist)
    {
        if (!inst)
            PLAY(listrnd(soundlist));
        else
            inst->PlayWave(listrnd(soundlist));
    }
}
```

`FindTag` (src/3dimage.cpp:1855) is an exact-match lookup over the sorted
`tags` array: it returns the `str` of the first tag whose `(state, frame)`
equals the query and whose `name` equals `"play"`. It relies on the array
being sorted by `(state, frame)` to early-`break`.

`TObjectInstance::PlayWave` (src/object.cpp:2665) is the **3D-positioned**
play: it computes the object's position relative to the map origin and calls
`SoundPlayer.Play(id, volume, freq, &p)`, with `Mount`/`Unmount` around it.

```cpp
bool TObjectInstance::PlayWave(char *soundname, int32_t nr, int32_t volume, int32_t freq)
{
    S3DPoint p, mp;
    GetPos(p);  MapPane.GetMapPos(mp);  p -= mp;     // 3D pos relative to map
    int32_t id = SoundPlayer.FindSound(soundname, nr);
    if (id < 0) return false;
    if (!SoundPlayer.Mount(id)) return false;
    SoundPlayer.Play(id, volume, freq, &p);
    SoundPlayer.Unmount(id);
    return true;
}
```

**This chain is identical to the 1999 source** (`legacy/3dimage.cpp:2903`
`T3DAnimator::Pulse`, `:1819` `PlaySound`, `:2325` `FindTag`;
`legacy/character.cpp:166` `TCharacter::Pulse`). `TCharAnimator` does **not**
override `Pulse()` in either era — it inherits `T3DAnimator::Pulse`
(charanimator.h:21, no `Pulse` member). So the play-tag dispatch *is* the
ported behavior.

**One divergence the port dropped** — the legacy `T3DAnimator::Pulse` did
TWO things; the port kept only the second:

```cpp
// legacy/3dimage.cpp:2903
void T3DAnimator::Pulse()
{
    RefreshControllers(inst->GetState());            // (1) DROPPED in port
    for (int c = 0; c < controllers.NumItems(); c++) //     (controller axis, §1c)
        controllers[c]->Pulse();
    ((PT3DImagery)image)->PlaySound(inst, state, frame); // (2) KEPT
}
```

**Fidelity:** play-tag path = **confirmed via pre-release** (verbatim
match, port + legacy). Retail-binary confirmation deferred (see §0).

### 1b. `beg` / `end` tags → state-transition continuation + interpolation (NOT callbacks)

`beg`/`end` are **not** "state-enter / state-leave event hooks" as the
burndown's phrasing implies. They are string labels used by the
*interpolation and state-chaining* logic to decide whether two states are
the same logical motion. Two consumers, both already ported:

1. **Transition-blend suppression** — `T3DImagery::GetAniKey`
   (src/3dimage.cpp:1246-1252): if the previous state's `end` tag string
   equals the new state's `beg` tag string AND the prev state was on its
   last frame, skip the cross-state interpolation blend (the two states are
   a continuous motion, e.g. `walkf` → `walkr`, so blending would double-up).

   ```cpp
   char* begstate = FindTag((char*)"beg", state);
   char* endstate = FindTag((char*)"end", prevstate);
   if (begstate && endstate &&
       prevframe >= (states[prevstate].frames - 1) &&
       !stricmp(endstate, begstate))
       return true;   // no blend — seamless continuation
   ```

2. **Authored continuation chaining** — `FindTaggedContinuationState`
   (src/3dimage.cpp:2298): given the current state's `end` tag, find a state
   whose `beg` tag matches; `NextRenderKey` (src/3dimage.cpp:2362-2371) uses
   this so a non-looping movement step "rolls into" the next step's frame 0
   instead of holding its last key. This is the port's mechanism for
   chaining `walkf`/`walkr` etc. (note the SAME logic lives in the modern
   pose bridge: `src/i3danimpose.cpp:76-77`).

   The legacy `beg`/`end` consumer set is identical (`legacy/3dimage.cpp:1451`).

**Fidelity:** beg/end semantics = **confirmed via pre-release**; the
continuation-chaining variant (`FindTaggedContinuationState`,
`NextRenderKey`) is a **port-era elaboration** of the legacy behavior
(legacy did the blend-suppression in `GetAniKey` but resolved chaining
through the action state machine, not a render-key helper) — flagged as a
divergence to re-confirm against retail if movement chaining looks wrong.

### 1c. Controller tags (`scrolltex`, `animtex`, …) → `T3DController` objects — REMOVED in port

For I3D `version >= 3`, tags carry an **arbitrary `name`** (3dimage.cpp:663-670)
and the legacy engine treated unrecognized names as **controller types**.
`T3DAnimator::RefreshControllers` (legacy/3dimage.cpp:2780) walked the tag
array on every state change and, for each tag whose `name` matched a
registered `T3DControllerBuilder`, instantiated a `T3DController` parsed from
`tag->str` (the param string). Controllers were then `Pulse()`d and
`Render()`d every frame.

Registered controller types (legacy/3dcont.cpp):
- `scrolltex` (`REGISTER_3DCONTROLLER("scrolltex", …)`, :66) — scrolls a
  texture by `du`/`dv` per frame. Tag str format:
  `scrolltex:obj=<objlist>,du=<udelta>,dv=<vdelta>` (3dcont.cpp:28-34).
- `animtex` (:206) — texture-frame animation.

The port **removed this axis entirely** (src/3dimage.h:315):
> `T3DController + T3DControllerBuilder removed: the legacy 3D-tag-driven
> controller hierarchy (scrolltex etc.) ran during the old D3D render path
> that the drawable/mesh pipeline has fully replaced. No external consumers.`

This is a deliberate, documented decision, not a gap. **However** it means
`RefreshControllers` was also stripped from `T3DAnimator::Pulse`, so any
*new* version>=3 tag name that isn't `play`/`beg`/`end` is currently
silently ignored. If a future asset uses a controller tag, it is dead. For
footsteps/swipe/impact this is irrelevant.

**Fidelity:** **confirmed via pre-release**; removal is **confirmed port
decision** (in-tree comment).

---

## 2. Tag vocabulary (every name the engine recognizes)

| Tag `name` | I3D version | `str` payload | Loaded at | Consumed by | Action |
|---|---|---|---|---|---|
| `play` | all (v<2 via `I3D_HASPLAYSOUND`; v2 inline; v3 generic) | comma-list of sound names | 3dimage.cpp:636 / :656 / :668 | `T3DImagery::PlaySound` (:1532) each `Pulse` | `listrnd(str)` picks one sound at random → `inst->PlayWave` (3D-positioned) |
| `beg` | v2 (from `S3DOldImageryState3.begstate`); v3 generic | a state-group label string | 3dimage.cpp:648 / :668 | `GetAniKey` (:1246), `FindTaggedContinuationState` (:2313), `i3danimpose.cpp:76` | marks the *entry* identity of a state for continuation/blend matching |
| `end` | v2 (from `…endstate`); v3 generic | a state-group label string | 3dimage.cpp:650 / :668 | `GetAniKey` (:1247), `FindTaggedContinuationState` (:2303), `i3danimpose.cpp:77` | marks the *exit* identity of a state; `end[A]==beg[B]` ⇒ A chains seamlessly into B |
| `scrolltex` | v3 only | `obj=…,du=…,dv=…` | 3dimage.cpp:668 | **(removed)** legacy `RefreshControllers` → `TScrollTexController` | scroll UVs per frame |
| `animtex` | v3 only | controller params | 3dimage.cpp:668 | **(removed)** legacy `RefreshControllers` → `TAnimTexController` | texture-frame animation |
| *(other v3 names)* | v3 only | controller params | 3dimage.cpp:668 | **(removed)** | unrecognized ⇒ silently dropped (legacy fell through `GetBuilder` default) |

Notes:
- `S3DTag` is `{ int state; int frame; char* name; char* str; }`
  (3dimage.h:76). `name`/`str` are `strdup`'d copies.
- The "play" sounds are **mounted at load time**: 3dimage.cpp:674-682 walks
  every `play` tag and `SoundPlayer.Mount()`s each sound in the list, so the
  per-frame `PlayWave` doesn't pay a disk hit. (`PlayWave` itself also
  Mount/Unmounts defensively.)
- For v2, `play` tags are stored grouped by state (the loader iterates
  states and drains the playsound array in order, 3dimage.cpp:643-661),
  which is what lets `FindTag`'s sorted-array early-`break` work.

**Fidelity:** **confirmed via pre-release** (loader + consumer both read).

---

## 3. `play` tag semantics (footsteps specifically)

- **Resolution:** `str` is a comma-separated **list** of sound names.
  `listrnd(str)` (revutils.cpp:1675) picks one at random — so a `walk`
  animation with two footstep tags each carrying `"step1,step2,step3"`
  gives variation. `SoundPlayer.FindSound(name, nr)` resolves the name to a
  mounted sound id (object.cpp:2672).
- **Positioning:** 3D. `PlayWave` computes `objpos - mappos` and passes the
  S3DPoint to `SoundPlayer.Play(id, volume, freq, &p)` (object.cpp:2677), so
  footsteps attenuate/pan by world distance from the camera/listener.
- **Gating:** the *only* gate is `FindTag` matching the exact landed
  `(state, frame)`. There is **no** PlaySpeech-style mutex, no distance cull
  before `Play` (distance is handled inside the audio backend via the
  position), no per-actor footstep cooldown. A tag fires whenever the frame
  pointer lands on it. Volume/freq default to `-1` (engine default) since
  `PlaySound` calls the 1-arg `PlayWave(soundname)` form.
- **Looping:** because frame advance wraps to 0 on `AF_LOOPING`
  (object.cpp:1614-1620), a footstep tag on frame 0 (or any frame) of a
  looping `walk`/`run` cycle **re-fires every loop** — which is exactly what
  you want for continuous walking. No dedup is needed because the frame
  pointer only equals a given frame once per cycle.

Speech (`TCharacter::Say`) is a *separate* path and the only other
`SoundPlayer.Play` site in character.cpp — it is unrelated to tags.

**Fidelity:** **confirmed via pre-release**; "no extra gating" is an
inference from absence-of-code (rate as **inferred** — retail may have added
a footstep throttle late; low risk).

---

## 4. Frame-advance hook point (exact site for the tag scan)

The animation frame number changes in exactly one place per tick:

```cpp
// src/object.cpp:1545  TObjectInstance::NextFrame()
frame += framerate;                       // :1569  <-- THE advance
// then loop/pingpong/clamp handling      // :1571-1640
```

`framerate` is normally `±1` (object.cpp:1048/1053/2425), so playback visits
**every** frame — no skip in the common case (see §5 for the skip edge).

The dispatch hook is **already correctly placed**: `TMapPane::Pulse` runs
`NextFrameObjects()` (advance) **before** `PulseObjects()` (dispatch)
(mappane.cpp:2607 then :2610). So when `T3DImagery::PlaySound` reads
`(state, frame)` it sees the **landed** frame, which is the retail-correct
ordering. The actual scan lives in `T3DAnimator::Pulse` (src/3dimage.cpp:2243).

**If you add anything (e.g. the test rig, or a generic tag callback), it
goes inside `T3DAnimator::Pulse` alongside the existing `PlaySound` call**,
because that runs once per object per tick *after* the frame has advanced
and is the same site the 1999 engine used.

Caveat re: the project-mappane-renderer split — `NextFrameObjects` and
`PulseObjects` iterate `TMapIterator(..., OBJSET_ANIMATE/OBJSET_PULSE)`,
which is unaffected by the renderer owning loaded sectors; the iterator
walks the live object set, and `OF_PULSE | OF_ANIMATE` are set on every
character (character.cpp:77). Frame advance is **not** in the renderer or
the C3/C4/C5 skeleton bone path — `UpdateBoneTransforms`/`Animate` only
*read* `(state, frame)` to build poses; they don't move the frame pointer.
So the C5 migration does not touch the tag-dispatch site.

**Fidelity:** **confirmed via pre-release + current port read.**

---

## 5. Edge cases

| Case | Behavior | Evidence |
|---|---|---|
| **Looping anim** | Tag re-fires every loop (frame returns to the tag's frame once per cycle). Correct for footsteps. | object.cpp:1614-1620 wrap to 0; FindTag exact-match. |
| **Ping-pong** | Frame reverses at the ends (`framerate` flips sign, object.cpp:1591/1626). A mid-cycle tag therefore fires **twice** per full ping-pong (once each direction). Usually fine; be aware for sounds on ping-pong states. | object.cpp:1589-1593, 1624-1628. |
| **State transition (`SetState`)** | `prevstate=state; prevframe=frame; state=new; ResetState()` (object.cpp:1106-1112). Frame jumps to the new state's start. `beg`/`end` are consulted by the interpolation path, not by a per-state callback — there is **no** "fire all frame-0 tags on enter" behavior. | object.cpp:1060-1112; §1b. |
| **Same-state re-request on a looping anim** | Early-returns without resetting (object.cpp:1071-1089) — does **not** restart the cycle, so footstep cadence is preserved (no pop). | object.cpp:1076-1088. |
| **Frame skip (framerate magnitude > 1)** | `FindTag(state, frame)` matches only the **landed** frame; tags on jumped-over frames are **missed**. In normal play `framerate==±1` so this never happens, but anything that fast-forwards animation (cutscene scrub, time-scale) would drop tags. The 1999 engine had the same limitation. | NextFrame :1569 + FindTag exact-match. |
| **Playback speed** | There is no sub-frame accumulator; speed is integer `framerate` only. Tag timing is therefore frame-quantized at `FRAMERATE` (24 Hz legacy tick). | object.cpp:1569; testmodes drives off `TTime::LegacyFrameCount()` (testmodes.cpp:718). |

**Fidelity:** **confirmed via pre-release** for loop/pingpong/transition;
frame-skip behavior **inferred** from the exact-match lookup.

---

## 6. Downstream behaviors — what actually depends on tags

| Behavior | Tag-driven? | Real trigger | Status |
|---|---|---|---|
| **Footsteps** (walk/run/sneak) | **YES** — `play` tags | `T3DAnimator::Pulse → PlaySound` on the landed frame | **Ported & wired.** Lights up the moment a pulsed character with `play` tags on its walk states runs through the live game loop. The asset's walk states must actually carry `play` tags (verify with the I3D state dump, AGENT_GUIDE §3.1). |
| **Weapon-swipe VFX spawn** | **NO** | `TCharAnimator::Render` draws the swipe whenever `((TCharacter*)inst)->IsAttack()` is true (legacy/charanimator.cpp:97-98; `IsAttack()` = `IsDoing(ACTION_ATTACK)`, character.h:238). The swipe samples the weapon-bone trail each rendered frame; it is gated by combat **state**, not a frame tag. | The VFX track's `TWeaponSwipe` should hook the `ACTION_ATTACK` state window, **not** wait for a tag. (Cross-ref docs/vfx — the swipe needs the attacking character's prime-hand bone each frame while `IsAttack()`.) |
| **Impact-frame hit resolution** | **NO** | `if (attack && frame == attack->impacttime) ResolveHit(...)` inside the ACTION_ATTACK handler (src/character.cpp:1757; legacy/character.cpp:1663). `impacttime` is a numeric from the attack's `SCharAttackData` (rules.def), **not** a tag. Pre-impact movement is also gated on `frame < attack->impacttime` (character.cpp:1648). | **Already ported** and matches legacy verbatim. Independent of the tag system. |

The burndown's "port the tag dispatch, then footsteps + swipe-spawn +
impact-frame hits all light up" is therefore **only true for footsteps**.
Swipe and impact are state-/rules-driven and are already in place (impact)
or belong to the VFX track keyed on combat state (swipe). Document this so
the porting agent doesn't go hunting for swipe/impact tags that don't exist.

**Fidelity:** swipe trigger = **confirmed via pre-release** (legacy
charanimator.cpp); impact trigger = **confirmed via pre-release + ported**.

---

## 7. Verification rig (`--test`)

Goal: prove a `play` tag fires on the right `(state, frame)` for a real
walk cycle, on the real code path.

The existing `--test=charpreview` (testmodes.cpp:713 `RenderCharPreviewMode`)
is *almost* the rig but has the critical gap: it calls
`g_charPreview.inst->NextFrame()` (testmodes.cpp:722) and `Animate(false)`
but **never `Pulse()`** — so it advances frames and poses but never
dispatches tags. That is the concrete reason footsteps look "dead" in
isolation testing even though the path is wired.

Proposed rig (smallest viable):

1. Spawn a character with a known walk animation (reuse `charpreview`'s
   roster spawn). Confirm via the AGENT_GUIDE §3.1 anim-dump that the walk
   state(s) carry `play` tags (`img->FindTag("play", walkstate)`); if they
   don't, the asset — not the code — is the gap.
2. Drive the **full per-tick order**: `inst->NextFrame()` **then**
   `inst->Pulse()` each legacy tick (mirroring mappane.cpp:2607/2610). The
   `Pulse()` call is the one currently missing from `charpreview`.
3. Instrument: add a temporary `log_info` inside `T3DImagery::PlaySound`
   (gated behind a debug flag) printing `state, frame, soundname` whenever a
   `play` tag fires. Assert the log lines land on the expected frames for a
   full cycle (and re-fire each loop).
4. Audio reaches the gameflow-owned `SoundPlayer`/miniaudio backend; if the
   log fires but it's silent, that's a gameflow bug (AGENT_GUIDE §6), not a
   tag bug. The log line is the ownership boundary.

A two-character combat rig is **not** needed for footsteps (single character
+ ground). It *would* be needed to verify impact hits, but those are already
ported and key off `impacttime`, not tags.

---

## 8. Gaps / unknowns

- **Documented (confirmed pre-release):** the full `play`/`beg`/`end`
  dispatch shape, the per-tick order, 3D positioning, loop re-fire, the
  swipe-is-state-driven and impact-is-rules-driven facts, the controller-axis
  removal.
- **Inferred (absence-of-code):** no footstep throttle/dedup beyond
  frame-quantization; frame-skip drops tags. Low risk; retail may differ.
- **Not yet confirmed against retail binary:** the animator `Pulse`/
  `PlaySound`/`FindTag` were **not** re-decompiled this pass. The
  pre-release match is so tight (verbatim) that retail divergence is
  unlikely, but per the snapshot≠retail rule this is **inferred-from-
  pre-release, not decomp-confirmed**. To promote to confirmed: run
  `DecompileAddr.java` on the `T3DImagery` member that references the `"play"`
  string (find via `FindStringRefs.java play` against `data/RevenantDev`,
  single-writer), verify it does `FindTag → PlayWave`. Suggested as a
  follow-up, not a blocker — the port plan below stands on the pre-release
  evidence.
- **Open question for the user (game-history, in scope to ask):** did
  retail footsteps have any volume/cadence treatment beyond "random sound
  from the list at the tagged frame"? The pre-release does not, but combat/
  audio tuning shifted late.

---

## 9. Proposed port plan

The subsystem is mostly already ported. Order of work, smallest leverage
first:

1. **Footsteps — verify + test (no new dispatch code).**
   - The hook is in place (`T3DAnimator::Pulse` → `PlaySound`,
     src/3dimage.cpp:2243). Do **not** rebuild it.
   - Fix the test rig: make `--test=charpreview` (or a new `--test=footsteps`)
     call `inst->Pulse()` after `NextFrame()` (testmodes.cpp:722), so the
     path is actually exercised. This is the one concrete code change for
     footsteps.
   - Add the debug `log_info` in `PlaySound` (gated) and confirm tags fire
     on the right frames for a real walk cycle (§7).
   - If silent after the log fires, hand to gameflow (SoundPlayer backend).
   - If no tags fire at all, the asset's walk states lack `play` tags —
     report the asset gap, don't fabricate tags.

2. **Swipe spawn — confirm it's hooked to `IsAttack()`, not a tag.**
   - Coordinate with the VFX track: `TWeaponSwipe` should sample the
     prime-hand bone each rendered frame while `((TCharacter*)inst)->IsAttack()`
     (legacy/charanimator.cpp:97). No tag work here. This brief's job is to
     stop the VFX agent from waiting on a tag that doesn't exist.

3. **Impact hits — already ported; no work.**
   - `frame == attack->impacttime` → `ResolveHit` is live (character.cpp:1757)
     and matches legacy. Only verify it's reached once input dispatch issues
     an `ACTION_ATTACK` (gameplay burndown phase A).

4. **(Optional, deferred) Controller tags.** If/when a version>=3 asset uses
   `scrolltex`/`animtex` or a new controller name, the removed axis must be
   re-added — but as a **channel-system layer** (per ANIMATION_SYSTEM.md),
   not the legacy D3D `T3DController` (e.g. `scrolltex` → a UOffset/VOffset
   `EAnimChannel` track; `animtex` → `TextureFrame` channel). Don't resurrect
   `T3DController`. No current asset needs this; track in burndown only.

### Shape of the dispatch (for reference — do not rewrite, this already exists)

```cpp
// T3DAnimator::Pulse(), src/3dimage.cpp — the existing, correct shape:
void T3DAnimator::Pulse()
{
    // (frame has already advanced this tick via TObjectInstance::NextFrame)
    Get3DImagery()->PlaySound(inst, state, frame);   // play-tag → 3D footstep
    // legacy also did RefreshControllers + controllers[c]->Pulse() here;
    // that axis was intentionally removed (src/3dimage.h:315).
}
```

The only *new* code the footstep work needs is the **test-rig `Pulse()`
call** and a **gated debug log** — not a dispatcher.

---

### One-line handoff

Play-tag footstep dispatch is already wired end-to-end and matches the 1999
source; the burndown gap is really "the test rig never calls `Pulse()`" plus
the asset-tag-presence question. Swipe is combat-state-driven and impact is
`impacttime`-driven — neither is tag-driven. Verify + add a footstep test;
don't build a subsystem.
