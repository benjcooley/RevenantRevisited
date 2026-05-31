# VFX Reference Screenshot Manifest

Captured in two batches (2026-05-30 and 2026-05-30 batch 2). 275 in-game
effect screenshots classified against the 174-entry retail effect inventory
by parallel vision agents, then organized into per-effect subfolders for
`snap_ab --ref` consumption.

- Total screenshots: 275 (batch 1: 143, batch 2: 132)
- Classified into per-effect folders: 164 (batch 1: 86, batch 2: 78)
- Borderline (low-confidence + no-effect) parked in `_unclassified/`
  with `maybe_<guess>__` prefix: 111 (batch 1: 57, batch 2: 54)

## Per-effect folders

### Burn/
- Frame count: 8
- Confidence: medium x8
- Files: Burn__01.png ... Burn__08.png (orig shot_112 ... shot_119)

### Faultfire/
- Frame count: 7
- Confidence: medium x7
- Files: Faultfire__01.png ... Faultfire__07.png (orig shot_077 ... shot_083)

### FireBall/
- Frame count: 4
- Confidence: medium x4
- Files: FireBall__01.png ... FireBall__04.png (orig shot_086 ... shot_089)

### Flame/
- Frame count: 3
- Confidence: medium x3
- Files: Flame__01.png ... Flame__03.png (orig shot_090 ... shot_092)

### Halo/
- Frame count: 4
- Confidence: medium x4
- Files: Halo__01.png ... Halo__04.png (orig shot_027 ... shot_030)

### IceBolt/
- Frame count: 5
- Confidence: medium x5
- Files: IceBolt__01.png ... IceBolt__05.png (orig shot_121 ... shot_125)

### Iced/
- Frame count: 6
- Confidence: high x6
- Files: Iced__01.png ... Iced__06.png (orig shot_045 ... shot_050)

### Icedsparks/
- Frame count: 8
- Confidence: medium x8
- Files: Icedsparks__01.png ... Icedsparks__08.png (orig shot_051-055, shot_126-128)

### Lightning/  *(new in batch 2)*
- Frame count: 2
- Confidence: medium x2
- Files: Lightning__01.png ... Lightning__02.png (orig batch2_088, batch2_089)

### Photon/
- Frame count: 13
- Confidence: medium x13
- Files: Photon__01.png ... Photon__13.png (orig shot_036-042, shot_066-071)

### Restorelife/  *(new in batch 2)*
- Frame count: 5
- Confidence: medium x3, high x2
- Files: Restorelife__01.png ... Restorelife__05.png (orig batch2_126 ... batch2_130)
- Notes: Item-consumption rejuvenation buff; subtitle "The item was consumed.
  You feel rejuvenated" confirms identity on frame 04.

### RockStorm/  *(new in batch 2)*
- Frame count: 6
- Confidence: medium x1, high x5
- Files: RockStorm__01.png ... RockStorm__06.png (orig batch2_078 ... batch2_083)
- Notes: Falling boulder sequence — likely Magic\Rocks.I3D.

### Storm/
- Frame count: 5
- Confidence: medium x5
- Files: Storm__01.png ... Storm__05.png (orig shot_018 ... shot_022)

### TeleportDoorInsideB/
- Frame count: 4
- Confidence: high x2, medium x2
- Files: TeleportDoorInsideB__01.png ... TeleportDoorInsideB__04.png
  (orig shot_129, shot_130, shot_131, shot_142)

### Teleporter/  *(pad — Misc\IrisFlare.I3D)*
- Frame count: 15
- Confidence: high x4, medium x11
- Files: Teleporter__01.png ... Teleporter__15.png
  - __01 ... __08: batch 1 (orig shot_000, shot_001, shot_005, shot_056-060) —
    ground-anchored ring-base pillar at Misthaven recall pad, no character
    inside; classic IrisFlare PAD signature.
  - __09 ... __15: batch 2 (orig batch2_090, batch2_091 ... batch2_096) —
    crystalline diamond-lattice pillar at a fixed pavement location; second
    NPC walks into the pillar from outside on __14 (decisive multi-actor
    pad-interaction tell vs caster-centric spell).
- Notes: Split out from the original 40-frame Teleporter capture. Frames that
  showed a caster-centric blue cylinder / rings-around-character / vertical
  beam transport moved to `teleportation/` (the SPELL).

### teleportation/  *(spell — magic\Teleportation.i3d)*
- Frame count: 25
- Confidence: medium x25
- Files: teleportation__01.png ... teleportation__25.png
  - __01 ... __18: batch 2 burst (orig batch2_001 ... batch2_018, formerly
    Teleporter__09 ... __26) — blue cylinder forming AROUND the dark-armored
    caster with stacked rings rising up the body; brief duration.
  - __19 ... __25: batch 2 burst (orig batch2_029 ... batch2_035, formerly
    Teleporter__27 ... __33) — ringed-pillar / beam shaft phase, caster still
    rooted at the base; spell transport visual.
- Notes: Resplit from Teleporter on 2026-05-30 after recognizing the two
  retail effects (stationary pad vs cast spell) share the visual family.

### YAbsorb/  *(new in batch 2)*
- Frame count: 9
- Confidence: medium x3, high x6
- Files: YAbsorb__01.png ... YAbsorb__09.png
  (orig batch2_062, batch2_064, batch2_065, batch2_066 ... batch2_071)
- Notes: Boss-tier purple-energy dome absorbing/charging effect around
  kneeling figure. Y-prefix late-game variant.

### Ymanadrain/  *(new in batch 2)*
- Frame count: 3
- Confidence: medium x3
- Files: Ymanadrain__01.png ... Ymanadrain__03.png
  (orig batch2_025, batch2_026, batch2_027)
- Notes: Dramatic purple/violet swirling cloud around character — late-game
  boss magic (Y-prefix variant).

### cataclysm/
- Frame count: 8  *(+4 in batch 2)*
- Confidence: medium x8
- Files: cataclysm__01.png ... cataclysm__08.png
  - __01 ... __04: batch 1 (orig shot_095 ... shot_098)
  - __05 ... __08: batch 2 (orig batch2_084 ... batch2_087, rocks on floor +
    cyan bursts + red ribbon-trail perimeter sweep)

### cfire/  *(new in batch 2)*
- Frame count: 3
- Confidence: medium x3
- Files: cfire__01.png ... cfire__03.png
  (orig batch2_108 ... batch2_110)
- Notes: Stationary brazier flame — cave/environment ambient fire.

### combatflash/
- Frame count: 2  *(+1 in batch 2)*
- Confidence: medium x2
- Files: combatflash__01.png ... combatflash__02.png
  - __01: batch 1 (orig shot_064)
  - __02: batch 2 (orig batch2_053, large bright white-yellow burst on
    sword-strike impact)

### fgeyser/
- Frame count: 6
- Confidence: medium x6
- Files: fgeyser__01.png ... fgeyser__06.png (orig shot_006 ... shot_011)

### magicshield/  *(new in batch 2)*
- Frame count: 5
- Confidence: medium x5
- Files: magicshield__01.png ... magicshield__05.png
  (orig batch2_072 ... batch2_076)
- Notes: Purple/violet energy bubble around character with crackling sparks.
  Could alternatively be a YAbsorb variant — verify against retail asset.

### ymaelstrom/  *(new in batch 2)*
- Frame count: 8
- Confidence: medium x8
- Files: ymaelstrom__01.png ... ymaelstrom__08.png
  (orig batch2_036 ... batch2_043)
- Notes: Vertical column of stacked blue glowing rings/orbs forming a
  twisting vortex column. Late-game boss-tier blue spell.

## Batch 2 additions (2026-05-30)

Newly created folders:
- `Lightning/` (2)
- `Restorelife/` (5)
- `RockStorm/` (6)
- `YAbsorb/` (9)
- `Ymanadrain/` (3)
- `cfire/` (3)
- `magicshield/` (5)
- `ymaelstrom/` (8)

Existing folders that received batch-2 frames:
- `Teleporter/` (+32 → 40)
- `cataclysm/` (+4 → 8)
- `combatflash/` (+1 → 2)

Batch-2 frames sent to `_unclassified/` (low confidence): 54
- Halo: 1 (batch2_019)
- Antimagic: 5 (batch2_020-024)
- Teleporter: 1 (batch2_028, start of medium burst — kept separate per
  low-confidence rule)
- ymaelstrom: 2 (batch2_044-045, aftermath sparkles)
- YAbsorb: 10 (batch2_046-048 crystal/gem; batch2_056-061, 063 purple-orb
  warm-up)
- YEnergy: 5 (batch2_049-051, batch2_124-125)
- StrikeEffect: 1 (batch2_052)
- combatflash: 2 (batch2_054-055, blue-white sparkle variant)
- no-effect / unknown: 2 (batch2_077 thin red sliver, batch2_132 unlabelled)
- Teleporter: 7 (batch2_097-103, tail of teleport-pillar burst)
- LabyrinthEffect: 4 (batch2_104-107, dark labyrinth interior, very low
  intensity)
- Ycataclysm: 13 (batch2_111-123, full purple-sparkle cast sequence by
  tentacled caster — long burst kept together)
- Sparks: 1 (batch2_131)

## _unclassified/

111 borderline frames total (batch 1: 57, batch 2: 54). All are
low-confidence guesses or "no-effect" frames, prefixed with
`maybe_<guess>__` so a human reviewer can scan the agent's best guess
without it being trusted as ground truth.

Batch-2 maybe_ distribution (54 frames):
- maybe_Antimagic: 5 (batch2_020-024)
- maybe_Halo: 1 (batch2_019)
- maybe_LabyrinthEffect: 4 (batch2_104-107)
- maybe_Sparks: 1 (batch2_131)
- maybe_StrikeEffect: 1 (batch2_052)
- maybe_Teleporter: 8 (batch2_028, batch2_097-103)
- maybe_YAbsorb: 10 (batch2_046-048, batch2_056-061, batch2_063)
- maybe_YEnergy: 5 (batch2_049-051, batch2_124-125)
- maybe_Ycataclysm: 13 (batch2_111-123)
- maybe_combatflash: 2 (batch2_054-055)
- maybe_no-effect: 2 (batch2_077, batch2_132)
- maybe_ymaelstrom: 2 (batch2_044-045)
