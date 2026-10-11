# Dexterity authored stars and trails, 2026-10-10

Exact `Dexterity:0x8cd3ea0f`, `Magic\Dexterity.i3d`, SHA256
`c92edf6d6add68bf9e7e1b9d0093562de269feb7c151a3f25b8a9de8b41db71c`.
The complete 93-builder original registry selects default factory `40dc00`
and animator `5a370c`. State 0 loops over 60 frames; its four camera-facing
`#dexter` objects move independently and emit the separate `#particle` trail.
The hidden `*circle01` path is not drawn. No actor or replacement pose is used.

The source adapter preserves tag emitter order (objects 5, 1, 4, 3), base draw
order (1, 3, 4, 5), inclusive authored keys, cached emitter timing, constant
scale jitter and original randomized local rotation. The latter consumes RNG
inside Render; both native and source comparisons retain these calls. Tag
frame 10 does not delay construction or particle pulse: the actual full caller
shows four base stars at 0/1 and the first particle draws at 2.

The independent source decoder matches all 360 object/frame poses: 3,240 key
fields and 5,760 matrix fields, with an unchanged-default decoder control.
All 57,785 particle fields over 123 ticks match within 0.00005 (observed maximum
0.0000133339); RNG, live slots, age and lifespan match exactly. Actual original
NextFrame `470cc0` and effect Pulse `4de800` wrap 59→0 without killing. Clearing
only the original loop bit is the negative control: the owner instead kills at
60. The source/gallery adapter uses real NextFrame, Pulse and Alive callbacks.

The native reference executes whole Animate/Render every frame, including
controller pulse, cached poses, randomized render samples and RenderObject.
Fourteen selected frames execute the original raster; other frames stop at
`56eb30` after the full draw caller. Exact immutable decoded resources, texture
COM residency, state-0 header queries, flat ground 0 and the base script/audio
Pulse boundary are explicit. No particle geometry or fitted positions/colors
are inserted into the reference.

The base stars inherit original normal RGB565 mode before particles exist and
additive mode afterward. Source quads use the proven software RGB565 additive
policy, followed by the four normal-lit helpers. The particle-free cold tick 1
has its own actual 48 Hz, warmup-1 capture; its two frames are identical. Cold
native depth-write versus helper no-write remains a limit, not an occlusion
acceptance. No renderer, shader, shared parser or simulator edits are included.

Both actual Metal films contain 122 visible frames with the correct two loops.
Every sampled/logged draw count and owner frame agrees with the original.
Twelve selected active phases plus the cold pair retain original, ordinary
projection and explicit native-domain comparisons without a timing adjustment.
Build, native owner/loop test, six preview-map tests, source ASan/UBSan state
and pose probes pass.

Configuration: 640×480 black; owner/camera 0; source ambient 38/directional 1;
MSVC seed 1; quality 0; flat ground 0; active films 24 Hz. Natural caller,
end-state trigger, actor/scene attachment, gameplay, audio, exact pixels and
device parity remain outside this state-0 appearance scope. Common projection
is retained separately.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.

Frozen manifests (SHA256):

- `dexterity-state-first/manifest.json`: `c14cf4ce42d283c59aca28303a3e53975726d6d574caec9405de66efdb2740d1`.
- `dexterity-poses-first/manifest.json`: `56b92a0b203a66b6f7a7d92427cfcb194b955e730e833aee1a4afb91fd167fa8`.
- `dexterity-native-second/manifest.json`: `86fe4c5b3a3fd8a9d790dcc3c4a94bc4c470e5c19c250ef497b7ce6adaf68a39`.
- `dexterity-metal-domain-first/manifest.json`: `ca9604329548a97c5dbf927ddcc738a6d0b8b0d0ad9ac2f695b6c87056eaf90b`.
- `dexterity-metal-common-first/manifest.json`: `adfa2b1308a24557000fd388807c9d897db458f4c20c67c92644f3bf1df0ea65`.
- `dexterity-metal-cold-final/manifest.json`: `e50606fa80d1a41b2ea27333e2e6ee910b44abe355fff0e28e3ee200f8e32103`.
- `dexterity-review-final/manifest.json`: `be5b96af5ec87cbc785697c0e666765a356db41f84612d2e26cecf8a5e37810f`.
- `dexterity-review-final/cold-manifest.json`: `195cdd40b4a4c5f5f78c6f9de1024b01cffe666b11c87aa49d3b6f5a29e50c06`.

Metal binary: `0f597d78d6ecb6d8af6c5213718f4d88146409c6705c2fc1b966279dbaa8ff29`.

Root independently reviewed the cold full pair, all twelve active phases and
actual-size crops at 16/40/90/120. Scoped state-0 native-domain appearance is
approved: four moving blue stars, their trails and both wrap boundaries align.
The approval retains all limits above. Frozen `dexterity-review-final/root-approval.json`
SHA256: `7e3aa095ae364471f765670661c92eb87b3970c8a8734a49711be014d2e5ecdf`.
