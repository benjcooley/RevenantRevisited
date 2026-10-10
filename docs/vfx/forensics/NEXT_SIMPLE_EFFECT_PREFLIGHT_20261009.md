# Next simple effect admission: full native constructors

The bounded preflight of Labback, Createfood, energyspray and Quick finds no
new static admission. It verifies their exact shipped CLASS EFFECT rows,
assets, complete original registration constructors and actual leaf factories.
It does **not** execute their Init/Animate/Render methods or grant a rendered
frontend pass, reference, runtime or visual credit.

## Constructor correction to admission methodology

The existing `default_static_probe.original_registry` scanner calls the shared
registration base `0x40db90`. That call writes builder vtable `0x5a359c`.
The complete named static constructor subsequently overwrites that vtable for
custom animators. Therefore reading the factory after only the shared base
registration can incorrectly label a custom animator generic.

This new effect-specific preflight recovers the same 92 registration sites,
resets the registry count, executes full default EFFECT constructor `0x406160`,
then executes every complete named constructor at registration-call address
minus ten. The final registry has 93 entries. The default constructor writes
builder `0x5e8508`'s vtable `0x5a359c`, inserts it in the registry and stores
the EFFECT name. No existing shared probe is changed.

For each exact candidate, original lookup `0x40dca0`, original heap allocation
`0x482fb0` and the entire selected leaf factory execute. Only base owner
attachment `0x445940` and the two array constructors `0x41c7f0` are explicit
API boundaries. Their calls and original argument values are recorded. No
hook writes or selects the leaf vtable. Full game startup is outside scope.

## Ranked candidates and deferrals

1. `energyspray:0xad92bc1c`, `Magic\Energyspray.I3D`, SHA256
   `bdca48f08b388c385c78acf6e3b01e35ca5d7969ff910b8a9dd2dbd66f1ac75f`.
   V2, 11 vertices, five faces, three independent roots, two textures, no tags.
   Full constructor `0x502c30` selects builder vtable `0x5b0a24`, factory
   `0x509390`, animator vtable `0x5b0a28`. Init/Animate/Render are
   `0x502c50`/`0x502da0`/`0x503110`, 332/869/994 bytes including final returns.
   The disassembly shows 20 color/angle slots, 25 radial particles, actor-hand
   or null-spell anchoring, owner phase gates and RNG during Render. This is
   substantially more work than the tiny Warp UV controller.
2. `Createfood:0x838cffba`, `Magic\Createfood.I3D`, SHA256
   `54bcee48d62c0d16995c26f034425dd1c04b355478f83f3a8a726e0c088c043d`.
   V1, eight vertices, four faces, two roots/textures, no tags. Full constructor
   `0x4dfdb0` selects builder vtable `0x5a8aa0`, factory `0x4f4d00`, animator
   vtable `0x5a8aa4`. Init/Animate/Render are
   `0x4e0040`/`0x4e0260`/`0x4e0650`, 540/1006/1034 bytes. Food inventory,
   actor methods, particle updates and V1 uncompressed keys remain required.
   The preflight records the old key count without misdecoding it as V3 scalars.
3. `Labback:0xdcc4011d`, `Misc\Labback.I3D`, SHA256
   `1291c57862c12eec8e691585228905a0c1a0257a823704e203ac02df8b914dbc`.
   Actual default factory `0x40dc00` produces generic vtable `0x5a370c`.
   V3, 151 vertices, 242 faces, five roots, two textures. Its `blendcont=litadd`
   and `partsys` tags plus three 408-word animated sphere tracks prevent a
   constant static proof. The partsys emits `#sparks` from `fgeys`; ignoring
   that controller would omit authored output.
4. `Quick:0xbabfface`, `Magic\Quick.i3d`, SHA256
   `2cf07d02d4839b63de3d38706312c0b0dcc0b96d8282b18262b0480d20242198`.
   Actual default factory/leaf agree with Labback. V3, 28 vertices, 14 faces,
   eight roots, three textures, 56 frames. Six animated quad streams plus
   `play=quicksand`, `partsys` and `blendcont=litalpha` need full controller
   submission. No guessed speed billboard substitutes for those instructions.

The wider pending no-tag V2/V3 root scan found no uncredited visible default
static asset. ArrowEffect and QueenArrow initialize/animate through generic
tail jumps but their 536/571-byte render bodies consume owner velocity, color
selection and random trail jitter. Invisible and LabyrinthEffect also have
substantial custom controllers. StrikeEffect's Dummy has zero triangles.
FlameB/G already have bounded frontend credit and were excluded from new credit.

## Reproduce and evidence

```sh
PYTHONPATH=tools/retail_asm python tools/retail_runtime/simple_effect_preflight.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe --output <private-directory>
PYTHONPATH=tools/retail_asm python -m unittest discover \
  -s tools/retail_runtime -p test_simple_effect_preflight.py
```

One native regression passes. It verifies the complete 93-builder registry,
custom versus generic leaf selection, exact controller tags and explicit
no-render/no-credit scope. Original licensed imagery, baseline EXE and Unicorn
are required. No DOSBox, asset substitution or production change is used.

Retained evidence:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-20261009/simple-effect-preflight/manifest.json`.
Manifest SHA256:
`91bfd733ae6336e75d6c330fdd0b0f2c8b02f90acb39046395629a12a96e560e`.
