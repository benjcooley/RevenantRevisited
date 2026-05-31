# VFX Reference Screenshot Manifest

Captured in three batches (2026-05-30 batch 1, batch 2, and batch 3 v2
Yaghoro endgame). 351 in-game effect screenshots classified against the
174-entry retail effect inventory by parallel vision agents, then
organized into per-effect subfolders for `snap_ab --ref` consumption.

- Total screenshots: 351 (batch 1: 143, batch 2: 132, batch 3 v2: 76)
- Classified into per-effect folders: 235 (batch 1: 86, batch 2: 78,
  batch 3 v2: 71)
- Borderline (low-confidence + no-effect) parked in `_unclassified/`
  with `maybe_<guess>__` prefix: 116 (batch 1: 57, batch 2: 54,
  batch 3 v2: 5)

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

### TeleportDoorInsideB/  *(portal door — Magic\WarpB.I3D, blue variant)*
- Frame count: 11
- Confidence: high x2, medium x9
- Files: TeleportDoorInsideB__01.png ... TeleportDoorInsideB__11.png
  - __01 ... __04: batch 1 (orig shot_129, shot_130, shot_131, shot_142) —
    standalone tall blue lattice pillar.
  - __05 ... __11: batch 2 (orig batch2_090 ... batch2_096, formerly
    Teleporter__09 ... __15) — crystalline diamond-lattice pillar at a
    fixed pavement location with NPC inside; on __10 a second NPC walks
    into the pillar from outside (multi-actor portal interaction).
- Notes: Resplit from Teleporter/ on 2026-05-30 after recognizing the two
  retail pad-class effects: pulsing iris ring (Teleporter / Misc\IrisFlare.I3D)
  vs rotating crystalline-lattice rings (TeleportDoorInsideX /
  Magic\Warp{B|G|O|P|R|W|Y}.I3D). Blue is the default / most-common variant.

### Teleporter/  *(pad — Misc\IrisFlare.I3D, town return / Misthaven recall)*
- Frame count: 8
- Confidence: high x4, medium x4
- Files: Teleporter__01.png ... Teleporter__08.png
  - __01 ... __08: batch 1 (orig shot_000, shot_001, shot_005, shot_056-060) —
    ground-anchored smooth blue/white iris-ring pillar at Misthaven recall
    pad on stone/tree environment; no character inside; classic IrisFlare
    pulsing PAD signature.
- Notes: Resplit on 2026-05-30 — frames showing stacked rotating crystalline
  lattice rings with NPC walkthrough moved to `TeleportDoorInsideB/` (the
  portal-door pad). Earlier resplit (commit 0ed4151) moved caster-centric
  rings-around-character / vertical-beam transport visuals to `teleportation/`
  (the SPELL).

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
- Frame count: 13  *(+4 in batch 3 v2)*
- Confidence: medium x3, high x10
- Files: YAbsorb__01.png ... YAbsorb__13.png
  - __01 ... __09: batch 2 (orig batch2_062, batch2_064, batch2_065,
    batch2_066 ... batch2_071) — purple-energy dome charging effect
    around kneeling figure.
  - __10 ... __13: batch 3 v2 (orig batch3v2_028 ... batch3v2_031) —
    bright GREEN vertical absorb beam/pillar on Yaghoro target with
    ascending orbs + floor ring. Confirms GREEN as the canonical
    Yaghoro-arena tint (the batch-2 purple dome may be a parallel
    variant or a different lighting condition).

### Ymanadrain/  *(new in batch 2)*
- Frame count: 4  *(+1 in batch 3 v2)*
- Confidence: medium x4
- Files: Ymanadrain__01.png ... Ymanadrain__04.png
  - __01 ... __03: batch 2 (orig batch2_025, batch2_026, batch2_027) —
    dramatic purple/violet swirling cloud around character.
  - __04: batch 3 v2 (orig batch3v2_060) — faint translucent green
    skull-shape forming over Yaghoro-arena scene.

### cataclysm/
- Frame count: 20  *(+4 in batch 2, +12 in batch 3 v2)*
- Confidence: medium x20
- Files: cataclysm__01.png ... cataclysm__20.png
  - __01 ... __04: batch 1 (orig shot_095 ... shot_098)
  - __05 ... __08: batch 2 (orig batch2_084 ... batch2_087, rocks on floor +
    cyan bursts + red ribbon-trail perimeter sweep)
  - __09 ... __20: batch 3 v2 (orig batch3v2_016 ... batch3v2_027) —
    golden/orange swirling funnel + downward column + expanding ground
    ring in Yaghoro arena. Two distinct bursts (016-022, 023-027).

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
- Frame count: 10  *(+2 in batch 3 v2)*
- Confidence: medium x10
- Files: ymaelstrom__01.png ... ymaelstrom__10.png
  - __01 ... __08: batch 2 (orig batch2_036 ... batch2_043) — vertical
    column of stacked blue glowing rings/orbs forming a twisting vortex.
  - __09 ... __10: batch 3 v2 (orig batch3v2_048 ... batch3v2_049) —
    massive GREEN eye-shaped vortex with curving green tendril streams
    in Yaghoro arena.

### maelstrom/  *(new in batch 3 v2)*
- Frame count: 15
- Confidence: medium x3, high x12
- Files: maelstrom__01.png ... maelstrom__15.png
  (orig batch3v2_001 ... batch3v2_015)
- Notes: Base (non-Yaghoro) maelstrom — orange/gold vortex column,
  swirling ground disc, orbiting golden orbs at ring perimeter. Color
  separates base from `ymaelstrom/` (green).

### YPowerUp/  *(new in batch 3 v2)*
- Frame count: 6
- Confidence: medium x6
- Files: YPowerUp__01.png ... YPowerUp__06.png
  (orig batch3v2_032 ... batch3v2_037)
- Notes: Green swirling funnel/cyclone rising around Yaghoro — charging
  / power-up tell preceding a cast.

### YFireBall/  *(new in batch 3 v2)*
- Frame count: 10
- Confidence: high x10
- Files: YFireBall__01.png ... YFireBall__10.png
  - __01 ... __08: orig batch3v2_038 ... batch3v2_045 — green spherical
    fireball with orange/yellow orbital streamer trails.
  - __09 ... __10: orig batch3v2_057 ... batch3v2_058 — green fireball
    in flight with dense green motion-blur tail.

### YFireWind/  *(new in batch 3 v2)*
- Frame count: 10
- Confidence: medium x10
- Files: YFireWind__01.png ... YFireWind__10.png
  - __01 ... __07: orig batch3v2_050 ... batch3v2_056 — sustained green
    flame engulfing player (bright yellow-green core, green outer
    flames).
  - __08 ... __10: orig batch3v2_064 ... batch3v2_066 — multi-orb green
    spread variant; alt id YFireBall barrage but spread favors firewind.

### YEnergy/  *(new in batch 3 v2)*
- Frame count: 1
- Confidence: medium x1
- Files: YEnergy__01.png (orig batch3v2_059)
- Notes: Green energy spray — many small green orbs scattering outward
  from a central point near a column.

### Ycataclysm/  *(new in batch 3 v2)*
- Frame count: 8
- Confidence: medium x8
- Files: Ycataclysm__01.png ... Ycataclysm__08.png
  - __01 ... __02: orig batch3v2_062 ... batch3v2_063 — green ring
    AoE on ground with multiple bright white energy bursts inside.
  - __03 ... __08: orig batch3v2_067 ... batch3v2_072 — green radial
    ray-burst on player with orange-tinted ground ring at feet.
- Notes: Y-prefix green variant of `cataclysm/`; distinguishing detail
  is the orange ground halo riding underneath the green starburst.

### YTelekinesis/  *(new in batch 3 v2)*
- Frame count: 2
- Confidence: medium x2
- Files: YTelekinesis__01.png ... YTelekinesis__02.png
  (orig batch3v2_073 ... batch3v2_074)
- Notes: Body lifted / airborne with motion-blur ghosting during
  Yaghoro telekinesis throw. The effect IS the throw — no separate
  spell-body visual on the victim.

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

## Batch 3 v2 (Yaghoro endgame) additions (2026-05-30)

76 endgame screenshots captured against Yaghoro (final boss) in the
green-lit arena. Yaghoro carries the `Y`-prefix late-game effect family
and all his magic is GREEN. Frame batch3v2_000 was not produced
(numbering starts at 001); batch3v2_076 had no classification and was
parked in `_unclassified/`.

- Total batch3v2 screenshots on disk: 76 (batch3v2_001 ... batch3v2_076)
- Organized into per-effect folders: 71
- Borderline / unclassified: 5

### Newly created folders

- `maelstrom/` (15) — orange/gold cast — base (non-Yaghoro) maelstrom
  vortex column with swirling ground disc + orbiting orbs. Frames
  001-015. Confidence: medium x3, high x12.
- `YPowerUp/` (6) — green swirling charging funnel around Yaghoro,
  pre-cast power-up. Frames 032-037. Confidence: medium x6.
- `YFireBall/` (10) — green spherical fireball projectile (orange
  orbital streamer trails on early frames, dense green motion-blur tail
  on later frames). Frames 038-045 + 057-058. Confidence: high x10.
- `YFireWind/` (10) — sustained green flame engulfing player (column +
  spread variant). Frames 050-056 + 064-066. Confidence: medium x10.
- `YEnergy/` (1) — green energy spray, many small green orbs scattering
  from central point. Frame 059. Confidence: medium x1.
- `Ycataclysm/` (8) — Yaghoro green AoE cataclysm; green radial
  ray-burst + ground ring (orange-tinted ground halo on close hits).
  Frames 062-063 + 067-072. Confidence: medium x8.
- `YTelekinesis/` (2) — body lifted / airborne with motion blur during
  Yaghoro telekinesis throw; the throw IS the effect. Frames 073-074.
  Confidence: medium x2.

### Existing folders that received batch-3-v2 frames

- `cataclysm/` (+12 → 20) — golden/orange swirling funnel + ground ring
  sweep in Yaghoro arena. Frames 016-027 → cataclysm__09 ... __20.
- `YAbsorb/` (+4 → 13) — bright green vertical absorb beam/pillar with
  rising orbs and floor ring on Yaghoro victim. Frames 028-031 →
  YAbsorb__10 ... __13. Confidence: high x4.
- `ymaelstrom/` (+2 → 10) — massive green eye-shaped vortex with
  curving green tendril streams (Yaghoro variant). Frames 048-049 →
  ymaelstrom__09 ... __10.
- `Ymanadrain/` (+1 → 4) — faint translucent green skull-shape forming
  over scene. Frame 060 → Ymanadrain__04.

### Batch-3-v2 maybe_ distribution (5 frames)

- maybe_YFireWind: 2 (batch3v2_046, 047 — wide green low-spreading
  flame sweep, could also be YEnergy spray)
- maybe_YPowerUp: 1 (batch3v2_061 — bright white-green orb with green
  halo, charging bloom)
- maybe_goldeffect: 1 (batch3v2_075 — small yellow/gold sparkle motes
  at base of obelisk; could be residual loot/altar sparkle)
- maybe_no-effect: 1 (batch3v2_076 — present on disk but absent from
  classification list)
