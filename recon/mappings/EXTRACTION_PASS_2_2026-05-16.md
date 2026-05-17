# VFX recon extraction pass 2 — 2026-05-16

Phase 0.3 continuation. Extends
[EXTRACTION_PASS_2026-05-16.md](EXTRACTION_PASS_2026-05-16.md) — read that
first for the methodology baseline.

This pass produced 22 new candidate yamls (17 INVENTORY rows resolved, 5
new discovery rows, 1 re-attribution).

## Tooling delta vs pass 1

Same XREF-driven attribution, but two improvements:

1. **Extended the effect-name registry scan range.** Pass 1 stopped at
   `.rdata 005e1224` (which is "Speaker", the last name in the main
   effect cluster). Pass 2 added the **`005e1a00..005e1c00`** range
   which contains the *MissileEffect family* registry strings:
   Photon, FireBall, FIRECOLUMN, FireColumn, FLAMEDISC, YFireBall plus
   the FIRESHOOT/FIREHIT event names. This range was missed by pass 1
   and is the reason `TPhotonEffect` was originally misattributed to
   `cls_0x5aacac` (which actually registers "DragonFire").

   Extraction tool: `/tmp/extract_5e1bxx.py` (scratch, kept under /tmp).
   The full attributed table for this range is at the bottom of this file.

2. **Allocation-size cross-check.** For every leaf-class candidate, scan
   for the `FUN_00482fb0(0xNNN)` allocator call inside that class's
   virtual creator. A match between this size and Ghidra's reported
   `Length:` is a sanity check that the class is *not* a misattributed
   multi-class blob.

   Example from this pass: `cls_0x5b0a28` allocates `0x578=1400` bytes
   inside `virt_meth_0x509390`, matching its `Length: 1400`. Compared
   to `cls_0x5a9194` which has `Length: 13504` and `FUN_00482fb0(0x34c0)`
   — *also* self-consistent at 13504 bytes, but suspicious because no
   pre-release effect class is that large. The 13504-byte size most
   likely reflects a Ghidra-merged multi-class file rather than a single
   real class, but the consistency means the merge is at least
   *internally clean* (one fake "class" with many fields).

## Key findings (vs pass 1)

### Two distinct effect hierarchies exist

Pass 1 inferred a single "TEffect base" at the `cls_0x5b0074` chain.
Pass 2 separates two:

1. **TEffect family** rooted at `cls_0x5a47f0` (60-byte slim base, ctor
   takes `TCharacter*`). All confirmed leaf effects (Halo, Drip, Water,
   IceBolt, Streamer, FireSwarm, FaultFire, Ripple, Pixie, Blood, Mist,
   Tornado, Sandswirl, Quicksand, Speaker, Fizzle, etc.) derive from
   this. Every leaf's virtual creator allocates its own size and
   constructs the buffer as `cls_0x5a47f0` (covariant-return
   `TEffect* CreateInstance()`).

   `cls_0x5b0074` (which appears in every effect class's destructor
   chain) is **not** the base — it's an intermediate base whose Ghidra
   Length (4524) is inflated by absorbed free functions. The real base
   is `cls_0x5a47f0`.

2. **MissileEffect family** rooted at `cls_0x5a50e8` (276-byte struct,
   but the file has 107 functions — clearly a misattributed multi-class
   Ghidra blob). All MissileEffect.cpp classes (Photon, FireBall,
   FireColumn, FlameDisc, YFireBall, TMissileEffect itself) chain
   through `cls_0x5a50e8::~cls_0x5a50e8` in their destructors.

   `cls_0x5a50e8`'s real identity is **OPEN**. It is *not* TEffect.
   Hypotheses to test:
   - It's `TObjectInstance` (the heavy base — pre-release source has
     `TMissileEffect : public TObjectInstance`).
   - It's `T3DImage` or similar animated-image base.
   - A previous REVSYNC line in `cls_0x5a50e8.cpp` says TScreen was
     rejected; the disputed file noted "likely an imagery/animation
     class".

### `cls_0x5a47f0` is TEffect, not T3DAnimator

Pass 1 demoted `cls_0x5a47f0 → TLightningAnimator` (correct demotion)
and suspected it might be `T3DAnimator` (the base). Pass 2 confirms it
is **the TEffect base itself**:

- Ctor takes `TCharacter *owner` and stores it at `mbr_0x4`.
- Calls `TCharacter::meth_0x46e8a0(owner)` to seed `mbr_0x8` — likely
  the owner's cached imagery handle, classic TEffect pattern.
- Every leaf effect class's virtual creator does:
  ```
  this_00 = (cls_0x5a47f0 *)FUN_00482fb0(SIZE_OF_LEAF);
  cls_0x5a47f0::cls_0x5a47f0(this_00);
  this_00->vftptr_0x0 = &cls_0x<leaf>__vftable_<leaf>;
  ```
  i.e. allocate the leaf's size, run the TEffect base ctor, then patch
  the vtable to the leaf's own. This is the canonical OOP-in-C pattern
  that maps 1:1 to C++'s "construct base then derived".

### `cls_0x5b0a28` is TLightningAnimator (and Effect3.cpp's owner)

Pass 1 flagged this as a "better candidate" hypothesis. Pass 2 confirms:

- `cls_0x5b0a28` has 5 methods. The first one (`virt_meth_0x502b70`) is
  a registrar+Update fn that contains XREFs to:
  - The **`d:\revenant\Effect3.cpp`** assert string (at `.rdata 005e14cc`)
  - The **`LightningStorm`** registry name (at `.rdata 005e1598`)
  - **`ENERGYSPRAY`** (at `.rdata 005e14b8`)
  - **`rhand`** (at `.rdata 005e14c4`)
- Size 1400 bytes; allocates 0x578 in `virt_meth_0x509390`; consistent.
- Destructor chains through `cls_0x5a7e38::virt_meth_0x40de10` then
  `cls_0x5b0074::~cls_0x5b0074` — the standard TEffect chain.
- The state machine in `virt_meth_0x502b70` (counter at `mbr_0x184`,
  timer at `mbr_0x188` with cycle values 0x3c→0x32→0x19) reads as a
  multi-strike storm cycle.

The 1400-byte size hints Effect3.cpp may contain only TLightningAnimator
(no second class merged in).

### Ghidra-merged class files (need rerun)

Pass 1 flagged 3 "Conflicted" classes; pass 2 confirms they all contain
≥2 distinct C++ classes that Ghidra merged:

| Ghidra file       | Registry names hit                          | Allocator sizes seen        |
|-------------------|---------------------------------------------|-----------------------------|
| `cls_0x5ab460`    | Tornado, Sandswirl, Quicksand               | 0x884 (2180) + 0x234 (564)  |
| `cls_0x5acaa8`    | Blood, Mist, MistFog, WaterFall             | 0x110 (272) + 0x19c (412)   |
| `cls_0x5a9194`    | FireFlash, FireWind, BURN                   | 0x34c0 (13504) only         |

These need a Ghidra rerun with stricter scoping. Until then, INVENTORY
rows for the affected effects point at the merged `cls_0x*.cpp` file
with a `(merged)` qualifier.

## Effect-name registry inventory (full)

This is the master reference: every effect-class type name in the retail
binary's `.rdata 005e1xxx + 005e1axx` registry, with its Ghidra-class
attribution. Format: `address | name | best-attributed cls_0x* | confidence`.

### Main effect cluster (.rdata 005e10xx..005e1224)

| addr      | registry name | cls_0x*      | yaml                                          |
|-----------|---------------|--------------|-----------------------------------------------|
| 005e1080  | FireFlash     | cls_0x5a9194 | TFireFlashEffect_cls_0x5a9194_candidate.yaml  |
| 005e108c  | FireWind      | cls_0x5a9194 | (same merge)                                  |
| 005e10dc  | Ribbon        | cls_0x5b9fac | TRibbonAnimator_cls_0x5b9fac_candidate.yaml   |
| 005e10f4  | FLAME         | cls_0x5a9d88 | TFlameAnimator_cls_0x5a9d88_candidate.yaml    |
| 005e10fc  | SymGlow       | cls_0x5a9e50 | TSymGlowAnimator_cls_0x5a9e50_candidate.yaml  |
| 005e110c  | IrisFlare     | cls_0x5a9f2c | TFlareAnimator_cls_0x5a9f2c_candidate.yaml    |
| 005e1148  | Smoke         | cls_0x5a7e38 | (cls_0x5a7e38 is too suspect — see below)     |
| 005e1170  | DragonFire    | cls_0x5aacac | TDragonFireEffect_cls_0x5aacac_candidate.yaml |
| 005e117c  | IceBolt       | cls_0x5aaf28 | TIceBoltEffect_cls_0x5aaf28_candidate.yaml    |
| 005e1184  | Iced          | (status tag) | TIcedEffect_cls_0x5aaf28_note.yaml            |
| 005e118c  | Quicksand     | cls_0x5ab460 | TStormAnimator_cls_0x5ab460_candidate.yaml    |
| 005e1198  | Sandswirl     | cls_0x5ab460 | (same)                                        |
| 005e11a4  | Tornado       | cls_0x5ab460 | (same)                                        |
| 005e11ac  | Streamer      | cls_0x5abbb0 | TStreamerEffect_cls_0x5abbb0_candidate.yaml   |
| 005e11b8  | FireSwarm     | cls_0x5abe4c | TFireSwarmEffect_cls_0x5abe4c_candidate.yaml  |
| 005e11c4  | Halo          | cls_0x5ac0cc | THaloEffect_cls_0x5ac0cc_candidate.yaml       |
| 005e11cc  | Ripple        | cls_0x5ac340 | TRippleEffect_cls_0x5ac340_candidate.yaml     |
| 005e11d4  | Drip          | cls_0x5ac5b0 | TDripEffect_cls_0x5ac5b0_candidate.yaml       |
| 005e11dc  | FaultFire     | cls_0x5ac83c | TFaultFireEffect_cls_0x5ac83c_candidate.yaml  |
| 005e11e8  | Blood         | cls_0x5acaa8 | TBloodEffect_cls_0x5acaa8_candidate.yaml      |
| 005e11f0  | Mist          | cls_0x5acaa8 | (same merge)                                  |
| 005e11f8  | MistFog       | cls_0x5acaa8 | (same merge)                                  |
| 005e1200  | WaterFall     | cls_0x5acaa8 | (same merge)                                  |
| 005e120c  | Water         | cls_0x5ad26c | TWaterEffect_cls_0x5ad26c_candidate.yaml      |
| 005e1214  | Pixie         | cls_0x5ad4e4 | TPixieEffect_cls_0x5ad4e4_candidate.yaml      |
| 005e121c  | Fizzle        | cls_0x5ad758 | TFizzleEffect_cls_0x5ad758_candidate.yaml     |
| 005e1224  | Speaker       | cls_0x5ad9d4 | TAmbSoundEffect_cls_0x5ad9d4_candidate.yaml   |

### MissileEffect cluster (.rdata 005e1a00..005e1c00) — NEW in pass 2

| addr      | registry name | cls_0x*      | yaml                                          |
|-----------|---------------|--------------|-----------------------------------------------|
| 005e1a78  | FIRESHOOT     | cls_0x5b4290 | (event name for TFireBallEffect)              |
| 005e1a84  | FIREHIT       | cls_0x5b4290 | (impact event for TFireBallEffect)            |
| 005e1b20  | Photon        | cls_0x5b401c | TPhotonEffect_cls_0x5b401c_candidate.yaml     |
| 005e1b28  | FireBall      | cls_0x5b4290 | TFireBallEffect_cls_0x5b4290_candidate.yaml   |
| 005e1b34  | FIRECOLUMN    | cls_0x5b4290 | (also handled by TFireBallEffect class)       |
| 005e1b40  | FireColumn    | cls_0x5b4534 | TFireColumnEffect_cls_0x5b4534_candidate.yaml |
| 005e1b4c  | FLAMEDISC     | cls_0x5b45ac | TFlameDiscEffect_cls_0x5b45ac_candidate.yaml  |
| 005e1b58  | YFireBall     | cls_0x5b4814 | TYFireBallEffect_cls_0x5b4814_candidate.yaml  |

### Spell/script keywords (not effect-class registry — for reference)

Strings at `005e1228..005e19ff` are spell names (Cataclysm, Crystalism,
Armageddon, …), door/exit keywords (OPEN/CLOSE/LOCK/…), and item-script
tags (FOOD/POTION/HLTH/MANA/…). These are NOT effect-class registry
entries; they're consumed by `cls_0x5b8e94_TObjectInstance`,
`cls_0x5a50e8` and `cls_0x5b0074` for script-side event posting and
shouldn't be confused with effect-class registration.

## Lessons / notes for the next agent

1. **Always scan the full .rdata effect-name range** (`005e1000..005e1c00`).
   Pass 1's narrower range caused the Photon misattribution.

2. **The MissileEffect family has its own base.** Don't assume every
   effect-shaped class inherits from `cls_0x5a47f0` (TEffect). Missile
   effects derive from `cls_0x5a50e8`, which is an as-yet-unidentified
   animation/object base. Source-side, TMissileEffect inherits from
   TObjectInstance, not TEffect.

3. **`cls_0x5a50e8` is a high-value next target.** Resolving its
   identity (probably TObjectInstance or a T3DImage base) would
   unblock the entire missile family port and clarify the L01
   TLightSource situation (which was demoted in pass 1 — its candidate
   class `cls_0x5427f0` calls into TCharacter::method_0x51b580, similar
   methods may pattern-match against cls_0x5a50e8's vtable).

4. **Allocator-size sanity check**: always check that a leaf class's
   `FUN_00482fb0(0xNNN)` size matches its Ghidra `Length:`. If they
   differ, the file is likely a merged multi-class blob (3 files
   in this pass: `cls_0x5ab460`, `cls_0x5acaa8`, `cls_0x5a9194`).

5. **`cls_0x5a7e38` is suspect.** Pass 1 noted it "looks like a
   character class". Pass 2 finds it has `cls_0x5a47f0` embedded at
   offset 0 (so it inherits from TEffect) BUT is 3964 bytes — too
   large for a regular effect. It might be:
   - The full TEffect implementation that effect leaves inherit *via*
     `cls_0x5b0074` as an intermediate vtable layer
   - OR another Ghidra-merged blob containing TEffect base methods
     plus some other class's body
   The "Smoke" string XREFs into this class (delta +0x100 in fn 0x4e7c60),
   so cls_0x5a7e38 may also be TSmokeEffect's home. Needs further
   investigation; tentative.

6. **Don't promote candidates to confirmed without source-side struct
   compare.** Field layouts beyond the TEffect base (offset > 0x40)
   diverge per-class; map them to `src/effect.h` declarations and
   verify each member offset before running `rename_classes.py`.

## Files produced in this pass

See INVENTORY.md mapping pass log section "2026-05-16 — extraction pass 2".
