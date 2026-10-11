# Original Dragonattack caller and owner

Exact `dragonattack` type `0xba09fead` shares `Magic/Hfire.i3d` with
headfireball, but has its own builder66d108/factory509d50, owner vtable5b16ac,
Init505a70 and Pulse505a90. It is not admitted or credited through the trap
effect's binding.

The native fixture boots the source-matched original HUD world, selects the
original PlayScreen prefix, constructs two real Acolytes at (3185,3185,16) and
(3465,3185,16), and executes original BeginFighting. It loads the original
spell definitions and constructs the named `Sid Dragon Attack` TSpell before
binding it through the original inline SetSpell interface. The actual typed
Dragonattack factory and complete owner Pulse execute. No actor, stat, action,
ground, character-query or damage function is substituted.

These actors are deliberately not map-resident. All 45 observed owner ground
and character-query results are zero. The owner therefore impacts empty-world
terrain, not a character. Actor health and action/target records remain
unchanged, and no spell/character damage call occurs. This does not certify
natural dragon behavior, bone attachment, map/character Pulse, spell casting,
combat or audio.

The constructed invoker's actual initial facing96 gives the original
Angle(face,20) muzzle offset (14,14); the owner adds100Z. Its first position is
(3199,3199,116). The original normalized20-unit fixed velocity is
(1200051,-63160,-523330). Each Pulse drops fractional movement remainders;
in particular its negativeY velocity does not accumulate into a laterY step.
Flight starts at record11; empty-ground impact occurs at32, at (3595,3199,-38).
The original end counter54 sets OF_KILL at86 and the110-record trace retains
the frozen tail. The compiled source core matches all1,210 integer fields
under ASan/UBSan, including the entire three-state lifecycle.

The null-invoker fallback is not used: original505c0f writes a different stack
temporary from the one consumed for target coordinates. A genuine Fighting
target keeps this fixture on the defined original target branch.

Private evidence is under RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/dragonattack-actor-first/` and
`dragonattack-owner-core/`. This checkpoint proves the owner dependency and
state machine only. Full controller, original render and actual Metal
appearance remain pending.

`dragonattack-actor-first/manifest.json` SHA256: `d597f61a7136ba20c70ddedb311207f14f6a16d2afdb2b2e7aee5f048959c036`.

`dragonattack-owner-core/manifest.json` SHA256: `a49604e2b2e2d7a6327e41b8c103056668e20e2ad961e87cb269cd3c700b2b1a`.

The second fresh genuine-actor run reproduces all original owner fields from the private feasibility run exactly.
