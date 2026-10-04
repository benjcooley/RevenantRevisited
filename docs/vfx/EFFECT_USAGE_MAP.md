# VFX Inventory Usage Map

## How to Use This Document

This map shows where each visual effect is used in-game and what assets it depends on.
Prioritized for reference image capture: **High** effects have I3D mesh assets that need
retail screenshots. **Medium** effects use procedural stand-ins or custom textures.
**Low** effects are particle/procedural only and can often be reverse-engineered from code.

**For capture planning:** Load the game and navigate to the locations listed in each effect.
Use the priority to decide which effects to prioritize if time is limited.

## High Priority (17 effects)

| ID | Class | Status | Pipeline | In-Game Location | Asset Dependencies |
|----|----|----|----|----|----|
| B01 | TBloodEffect              | current-frame   | PE           | Impact blood burst (melee/hit)           | Misc\Blood.I3D (2x2 atlas)          |
| B03 | TBloodSystem              | not-started     | PE           | Blood system manager                     | Misc\Blood.I3D                      |
| B04 | TPulpEffect               | not-started     | PE+IM        | Pulp/viscera particles                   | PULP mesh asset                     |
| F01 | TFlameEffect              | normal          | FB           | Torch/ambient flame (static or spell-attached) | Magic\flame.i3d (single flipbook)   |
| F07 | TFireBallEffect           | not-started     | IM+PE+LS     | Fireball projectile missile              | projectile mesh + particles         |
| F09 | TFireColumnEffect         | not-started     | IM+PE+LS     | Fire column area effect                  | Fire column mesh + particles        |
| F11 | TFlameDiscEffect          | not-started     | IM+PE        | Disc-shaped flame effect                 | Flame disc mesh                     |
| H04 | TDripEffect               | normal          | PE           | Drip splash effect                       | Magic\drip.i3d                      |
| L03 | TBallAnimator             | not-started     | IM+LS        | Light ball effect                        | Ball/orb mesh                       |
| M01 | TAuraEffect               | not-started     | VO+LS        | Character aura/glow overlay              | Aura mesh or particles              |
| M05 | TMistEffect               | normal          | PE (was VO; VO deferred per PHASE1_SPINE) | Ambient mist wisps (continuous)          | Magic\mist.i3d                      |
| M07 | TPhotonEffect             | not-started     | IM+LS        | Projectile light bolt                    | Photon/light mesh                   |
| W01 | TMeteorStormEffect        | not-started     | IM+PE+LS+SR  | Meteor storm area effect                 | Meteor mesh + particles             |
| W02 | TTornadoEffect            | not-started     | IM?+PE+SR    | Tornado swirl effect                     | Tornado mesh/vortex                 |
| X03 | TFlareAnimator            | not-started     | FB+LS        | Lens flare effect                        | Flare mesh/billboard                |
| X19 | TYFireBallEffect          | not-started     | IM+PE+LS     | Fireball projectile with particles       | Fireball missile mesh               |
| X20 | TDragonFireEffect         | not-started     | FB+PE        | Dragon breath fire effect                | Dragon fire mesh                    |

## Medium Priority (12 effects)

| ID | Class | Status | Pipeline | In-Game Location | Asset Dependencies |
|----|----|----|----|----|----|
| B02 | TBloodAnimator            | not-started     | -            | Animator for TBloodEffect                | Bridge class                        |
| F02 | TFlameAnimator            | not-started     | FB           | Bridge to TFlameEffect                   | TBD                                 |
| F03 | TFireEffect               | current-frame   | FB           | Scatter-patch fire spell effect          | Misc\Fire.I3D or procedural texture |
| F04 | TFireAnimator             | not-started     | FB           | Bridge to TFireEffect                    | TBD                                 |
| F08 | TFireBallAnimator         | not-started     | -            | Animator for TFireBallEffect             | Bridge class                        |
| F10 | TFireColumnAnimator       | not-started     | -            | Animator for TFireColumnEffect           | Bridge class                        |
| M02 | TAuraAnimator             | not-started     | -            | Animator for TAuraEffect                 | Bridge class                        |
| M04 | TBurnEffect               | not-started     | VO           | Burn/damage glow                         | Burn glow overlay                   |
| M09 | TTeleporterAnimator       | normal          | FB           | Teleport spell vortex                    | teleportation.I3D, gvortex.I3D [STAND-IN: procedural] |
| S03 | TWindStripAnimator        | not-started     | -            | Animator for TWindStripEffect            | Bridge class                        |
| S05 | TShockAnimator            | not-started     | SR           | Shock animator                           | Bridge class                        |
| S06 | TStripAnimator            | not-started     | SR           | Strip animator                           | Bridge class                        |

## Low Priority (36 effects)

| ID | Class | Status | Pipeline | In-Game Location | Asset Dependencies |
|----|----|----|----|----|----|
| F05 | TFireSwarmEffect          | not-started     | PE           | Small fire particles (swarm)             | -                                   |
| F06 | TFaultFireEffect          | not-started     | PE           | Fault Fire spell variant particles       | -                                   |
| H01 | TWaterEffect              | not-started     | PE           | Water surface effect                     | -                                   |
| H02 | TWaterFallEffect          | not-started     | CX           | Waterfall area effect                    | -                                   |
| H03 | TRippleEffect             | normal          | FB           | Ripple rings on water impact             | -                                   |
| L01 | TLightSource              | not-started     | LS           | Light source (no visual)                 | -                                   |
| L02 | THaloEffect               | normal          | FB+LS        | Halo/shield shimmer glow                 | -                                   |
| M03 | THealEffect               | not-started     | VO+PE        | Healing spell visual                     | Heal effect particles               |
| M06 | TSpellBlock               | not-started     | IM+VO        | Spell block shield                       | -                                   |
| M08 | TPixieEffect              | not-started     | PE+LS        | Pixie sparkle particles                  | -                                   |
| S01 | TStripEffect              | normal          | SR           | Sword slash trail ribbons                | -                                   |
| S02 | TWindStripEffect          | not-started     | SR           | Wind strip effect                        | -                                   |
| S04 | TLightningAnimator        | not-started     | SR           | Lightning bolt strips                    | -                                   |
| S07 | TStreamerEffect           | not-started     | SR           | Streamer effect                          | -                                   |
| S08 | TMissileEffect            | not-started     | IM           | Missile projectile effect                | -                                   |
| S09 | TWeaponSwipe              | normal          | SR           | Weapon melee swipe trail                 | -                                   |
| W03 | TVortexEffect             | not-started     | PE+SR        | Vortex effect strips/particles           | -                                   |
| W04 | TSandswirlEffect          | not-started     | PE           | Sandswirl particles                      | -                                   |
| W05 | TQuicksandEffect          | not-started     | PE           | Quicksand pit effect                     | -                                   |
| W06 | TFogAnimator              | not-started     | VO           | Fog ambient particles                    | -                                   |
| W07 | TStormAnimator            | not-started     | PE+SR        | Storm wind particles                     | -                                   |
| X01 | TFlyEffect                | not-started     | PE           | Flying insect particles                  | -                                   |
| X02 | TAmbSoundEffect           | skip-render     | -            | Ambient sound (no visual)                | -                                   |
| X04 | TFountainAnimator         | not-started     | PE           | Fountain spray particles                 | -                                   |
| X05 | TBlueFountainAnimator     | not-started     | PE           | Blue fountain spray                      | -                                   |
| X06 | TCyanFountainAnimator     | not-started     | PE           | Cyan fountain spray                      | -                                   |
| X07 | TGreenFountainAnimator    | not-started     | PE           | Green fountain spray                     | -                                   |
| X08 | TRedFountainAnimator      | not-started     | PE           | Red fountain spray                       | -                                   |
| X09 | TShieldAnimator           | not-started     | VO+mesh      | Shield/protective glow                   | -                                   |
| X10 | TSymGlowAnimator          | not-started     | FB+LS        | Symbol glow effect                       | -                                   |
| X11 | TRibbonAnimator           | not-started     | SR           | Ribbon/banner effect                     | -                                   |
| X12 | TAmbSoundAnimator         | skip-render     | -            | Ambient sound animator (no visual)       | -                                   |
| X13 | TTest3DAnimator           | not-in-1999     | -            | Test 3D animator                         | -                                   |
| X17 | placeholder flare (vfx test) | wip             | LS           | -                                        | -                                   |
| X18 | TArrowEffect              | not-started     | PE           | Arrow projectile trail                   | -                                   |
| X21 | TFizzleEffect`?           | not-started     | PE           | Spell fizzle/dispel effect               | Fizzle burst particles              |


## Known Issues & Unknowns

### Procedural Stand-Ins (Need Real Assets)

- **M09** (TTeleporterAnimator): Currently ports with procedural rotating glow.
  Retail uses `teleportation.I3D` + `gvortex.I3D` meshes. User should capture the
  Misthaven teleport effect (spell location) as reference for mesh animation.

- **F03** (TFireEffect): Uses procedural orange/yellow gradient texture (64x64).
  Asset path uncertain (Misc\Fire.I3D vs. Magic\Fire.I3D) — both shipped.

### Bridge/Animator Classes

Effects like F02, F04, F08, F10, B02, etc. are bridge/animator classes.
They don't spawn independently — they're paired with the main effect class.
Capture them alongside their main effect (e.g., F02 with F01).

### Unstarted / Requires Extraction

Many effects lack Ghidra decompilation or are waiting on infrastructure (PE pipeline).
These may change priority once asset refs are confirmed. See INVENTORY.md for details.


## Top 5 Highest-Priority Captures

1. **F01** (TFlameEffect) - Magic\flame.i3d flipbook
   - User can find this: Torch/candle objects in any area, or spell-cast flames
   - Status: normal (ported)

2. **B01** (TBloodEffect) - Misc\Blood.I3D atlas
   - User can find this: Melee attack hit effects, character damage
   - Status: current-frame (in-port)

3. **M05** (TMistEffect) - Magic\mist.i3d
   - User can find this: Ambient mist wisps in damp areas (BLOODMIRE, MISTHAVEN)
   - Status: normal (ported)

4. **M09** (TTeleporterAnimator) - teleportation.I3D + gvortex.I3D [STAND-IN]
   - User can find this: Cast Teleportation spell from Misthaven
   - **CRITICAL**: Real asset much more complex than current procedural stand-in
   - Status: normal (ported, but needs asset replacement)

5. **F07/F09** (TFireBallEffect / TFireColumnEffect) - Missile projectile meshes
   - User can find these: Fire spell projectiles (Fire Ball, Fire Column spells)
   - Status: not-started (need I3D mesh + particle rig)


---

## Summary

- Total effects: 65
- High priority (I3D mesh assets): 17
- Medium priority (procedural stand-ins / custom texture): 12
- Low priority (particle/procedural only): 36

Effects with shipped porting: F01, F03, B01, M05, M09, H03, H04, L02, S01, S09