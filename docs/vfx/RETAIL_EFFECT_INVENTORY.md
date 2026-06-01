# Retail Effect Inventory (true ground truth)

Extracted **2026-05-30** from the shipped retail `class.def`
(`data/imagery.rvi` → `class.def`, 233,484 B, dated 10-06-1999). This is
the AUTHORITATIVE list of every `EFFECT` type the shipped game knew
about. Anything not on this list is not a retail effect.

## Headline

- **174 retail effect type entries** in `CLASS "EFFECT" / TYPES` block
- **164 distinct `.I3D` assets** referenced (10 entries share assets — the
  CyanFont/RedFont/GreenFont/BlueFont quartet all use `Misc\Sparkle.I3D`;
  Iced/Icedsparks/Snow share `Magic\Iced.i3d`; etc.)
- **154 distinct animator-key roots** (after collapsing color/Y prefix
  suffixes that share an animator class with their base)
- **~39 of these are currently covered by our bespoke wave ports**
- **Gap: ~135 retail effect entries with no bespoke A/B baseline**

## What's in the gap

The 135-entry gap is dominated by these families. Most of them likely
share an animator class with a wave-1/2 port (so the work to add them
is a thin asset/data variant, not a full new class):

### Buff / stat magic effects (~13)

Might, Dexterity, Antimagic, Fmastery, Immortalmight, Ogrestrength,
Regeneration, Shadowfist, Warriorborn, Essencedrain, Restorelife — all
character-attached buff overlays. Likely share `TBuffAnimator` or
similar.

### Color/variant of existing effects (~17)

`FlameB`, `FlameG` (blue/green flame variants of base Flame — share
TFlameAnimator with different I3D); `RibbonB/G/O/P/R/W/Y` (7 colored
Ribbon variants); `YFireBall`, `YFireWind`, `Ymanadrain`, `Ycataclysm`,
`ymaelstrom`, `YAbsorb`, `YPowerUp`, `YEnergy`, `YEnergyLose`, `YLose`,
`YAttack`, `YTelekinesis` (Y-prefix boss/transformed-form variants —
share the base animator class).

### Sign-marker effects (~12)

OlihootSign, OgrokSign, CampSign, TowerSign, DruhgSign, RuinsSign,
BoneSign, YardSign, AncientSign, VillageSign, MistSign, HavenSign — all
likely share `TFountainAnimator` (the sparkle-marker class). Quick win:
the existing TFountainAnimator_Bespoke can be reused with the sign I3D
asset and color variant.

### Teleport door variants (~7)

TeleportDoorInsideB/O/P/R/W/Y/G — 7 colored teleport-door visuals.
Likely share a TTeleportAnimator / TWarpAnimator class.

### Water family extras (~10)

StillWater, FlowWater, BendWater1, BendWater2, SewerWater, Wave, WaveS,
WaveM, WaterFlft, WaterFrt, WaterClft, WaterCrt, RiverFall, MistFog —
many use the existing `TWaterEffect` / `TWaterFallEffect` animator with
different asset paths.

### Unique late-polish retail-only effects (~15)

These don't have a snapshot analog and would need fresh Ghidra-driven
forensics + bespoke port if we want them. Probably most-worth-porting
candidates:

- **dragonfire** — late polish dragon-breath, Magic\FireCone.I3D variant
- **arroweffect** / **queenarrow** — arrow trajectory effects, Misc\Arroweffects.I3D
- **Blast** — generic blast Magic\Blast.I3D
- **Nakrnoth** — boss-specific effect, magic\Nakrnoth.I3D
- **RockStorm** — magic\Rocks.I3D
- **LabyrinthEffect** — late-game labyrinth visual, Starfield.I3D
- **MeteorStorm** — Magic\Comet.I3D (this is partially in our wave-2 already)
- **maelstrom** — magic\maelstrom.I3D
- **cataclysm** — magic\Cataclysm.I3D
- **funnel** — magic\Funnel.I3D
- **gvortex** — Misc\Gvortex.i3d
- **combatflash** — Misc\Impact.i3d
- **goldeffect** — Misc\Goldp.i3d
- **PunchAndJudy** — Misc\PunchAndJudy.i3d
- **sgeyser**, **fgeyser**, **cfire** — cave/environment ambient effects
- **Globe**, **Fairy**, **Fairy2** — misc decorative
- **stoneskin**, **ironskin**, **quicksilver**, **speed**, **charm**,
  **nullifier**, **trollblood**, **invisible**, **puke**, **shriek**,
  **paralize1**, **poison**, **SwiftStrike**, **manadrain**, **heal**,
  **magicshield**, **energyspray**, **setvortex** — many buff/debuff/cast
  spell effects. Most likely share `TBuffAnimator` or `TSpellAnimator`.

### Effects with caveats

- **Labback** — Misc\Labback.I3D — labyrinth background?
- **Fire** uses `Misc\Fire.I3D` (not `Magic\`) — different I3D from F03's bespoke
- **Smoke** — already ported (S08-family)
- **heal** — different from `Heal` in our wave-1 (lowercase variant)
- **speaker** — Misc\Speaker.I3D — likely audio-only "effect" registered as visual placeholder

## How to use this

1. **For A/B against game video**: when you capture a clip of effect
   `<name>`, look up its entry in `/tmp/retail_effect_inventory.tsv` (or
   reproduce by re-extracting `class.def`). The asset path tells you
   which `.I3D` to read; the constants and animator behavior come from
   the matching `T<X>Animator` body in `effect_old.cpp` / `effectcomp.cpp`.

2. **For deciding new ports**: each gap entry maps to ONE of:
   - **Trivial variant** — same animator as an already-ported sister
     (e.g. FlameB shares TFlameAnimator). One-line addition: a new
     harness entry pointing at the variant asset.
   - **New animator** — needs full forensics + bespoke port. Mostly the
     unique late-polish effects.

3. **For tooling**: the existing `tools/refcap` catalog can already
   index reference videos by retail effect name; no schema change needed.

## Extraction recipe (reproducible)

    unzip -p data/imagery.rvi "class.def" > /tmp/retail_class.def
    awk '/^CLASS "EFFECT"/,/^END$/{print}' /tmp/retail_class.def \\
      | grep -E '^\s+"[A-Za-z]+"' \\
      | awk -F'"' '{print $2 "\t" $4}' > /tmp/retail_effect_inventory.tsv

This produces a 174-line TSV: `name<TAB>asset_path`.
