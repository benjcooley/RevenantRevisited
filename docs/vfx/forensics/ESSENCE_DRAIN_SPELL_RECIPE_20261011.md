# Essence Drain spell versus Essencedrain/Worm prototype

The shipped named **Essence Drain** spell selects **ManaDrain**, not the
`Essencedrain` EFFECT prototype (`0xdefeca7e`, `Magic/Worm.i3d`). The genuine
74-spell loader reads variant `Essence Drain`, effect `ManaDrain` at variant
`+0x2a`, and damage range 110–115 at `+0x54/+0x58`. This recipe uses the actual
ManaDrain EFFECT (`0xad92bc13`) with genuine Acolyte caster/fighting target and
its real spell, actor and mana APIs. It is a supplementary named spell preview;
it never substitutes ManaDrain for the Worm type or adds another coverage row.

The reported stuck Worm preview is reproducible. Its three keyed cylinders
start with uniform scale 1.19140625, then their literal modern 32-bit scale
words are `0x4a800003` (two cylinders) and `0xe0400003` (one cylinder): X/Y scale
zero and fixed positive/negative Z. The original decoder and matrix builder
reject those cylinders after the first phase. The other 26 roots have no keys
and retain identity matrices. The `scrolltex` tag targets only the rejected
cylinders. Thus the previously approved **default-owner** appearance is static;
that approval did not prove the named draining spell or its gameplay caller.
No keys, bones, motion, brightness or resource have been fabricated to animate it.

`manadrain_preview` now exposes `SpawnEssenceDrainTargetFixture` (explicit capture
seed) and `SpawnEssenceDrainConfiguredTarget` (live RNG, no reseed). Both share
the existing real 20/50/20-tick ManaDrain lifecycle and geometry. The new strict
variant guard requires the shipped name/effect/range. Existing Mana Drain
callbacks retain their 60–62 variant. Isolated preview IDs are
`EssenceDrain_SPELL_AUTHENTIC_TARGET` and `EssenceDrain_SPELL_CONFIGURED_TARGET`.
The gallery should label the spell and Worm prototype separately.

Private evidence:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-essence-viewer-20261011`.
The original named positive runs 100 ticks, transfers 115 mana at tick21
(target252→137, caster252, health102/102 unchanged), enters decay at71 and kills
at91. Complete original owner/animator/Render/RNG executes each tick; selected
whole software rasters cover1/20/21/40/70/90/91/100, including blank tail.
Compiled actual source core versus original packets passes10,495 state/RNG
fields and19,680 matrix fields (max4.88e−8 /4.77e−6), including two causal
negative controls. Genuine camera3185,3185,0, output1280×960 centered640,640,
ambient38 and directional1 remain explicit isolated interfaces. Natural map
insertion, moving actors, audio, hardware rendering and exact pixels remain open.

The initial checker used effect offset44 rather than42; its failed attempt is
retained as `first-spell-probe-failure.json`, corrected before owner execution.
WaterFlft in-situ work remains safely checkpointed at original full screen10.

The first100-frame GPU run was correctly finite but blank: its declared display
origin3185,3185 was incompatible with the isolated VFX world camera0,0 (the
`--vfx-camera` argument controls screen pixels). This input failure remains in
`metal-spell-final`. The corrected protocol displays at0,0,16, exactly removing
the original camera3185,3185 through the existing algebraic relocation. No
geometry, camera coefficient, controller or phase was changed to fit images.

Final actual GPU100-frame run `metal-spell-local-final` exits0 with90nonblack
frames/91distinct decoded frames; stages1..90 animate and91..100 are blank.
The old Mana Drain2-frame source control also passes with the unchanged named
variant. Standalone full original state/geometry unit passes underASan/UBSan
in3.424s; complete private build passes. Captured binary SHA256:
`7d825920af7bfd32676ca0cfb894e57f4257695f5bed7830499a625f0fee7398`.

Eight unchanged whole/crop phase pairs are at `review-spell-v2/whole.png`
and `detail.png`; review manifest SHA256:
`d8eb917a04417b2c525707d23da5811fa2476aa2e08e0f83359bd72565c0da96`.
Independent appearance approval and integrated live gallery remain pending.
The earlier review checker treated black RGB tuples as truthy; corrected to
Pillow`getbbox()`, with the failed review directory retained.

The declared control uses original public spell creation/SetSpell and standalone
residency association; it bypasses manager casting gates/map insertion, as the
existing Mana Drain fixture does. Essence Drain costs254mana inSPELL.DEF, above
the genuine Acolyte252; no mana/stat override or natural affordable-cast claim
is made. Actor motion, spell scheduling and natural gameplay remain open.
Source/evidence freeze:`spell-freeze.json` SHA256`e874deb7ec9fc4d2cdd46e56eaf72650bb7355512645e07b4446d1cf8c75a3ff`.

Rootindependentintegratedreproduction `research/vfx-essence-spell-root-20261011/first` passes2tests/fresh100native+8rasters/state10495+matrix19680channels and100decodedGPUrepeat,90nonblack/91distinct/blank91..100. ExistingManaDrain2framecontrolpasses. Scrollinggallery nowaddsseparate`Essence Drain (spell)` stationwithactualManaDrainID andunreseededconfiguredtarget callback; Wormtype0xdefeca retainsitsownexplicitprototype label. Root24frameordinarygallerycapture animatesandcleanshutdown, independentliteralphasesreviewed. Noaffordablecast/naturalAI claim.
