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
`dragonattack-owner-core/`. The first checkpoint proves the owner dependency and state machine.
The follow-up below executes the complete original controller and renderer;
production admission and actual Metal appearance remain pending.

`dragonattack-actor-first/manifest.json` SHA256: `d597f61a7136ba20c70ddedb311207f14f6a16d2afdb2b2e7aee5f048959c036`.

`dragonattack-owner-core/manifest.json` SHA256: `a49604e2b2e2d7a6327e41b8c103056668e20e2ad961e87cb269cd3c700b2b1a`.

The second fresh genuine-actor run reproduces all original owner fields from the private feasibility run exactly.

The genuine-VM render follow-up executes actual CreateAnimator before the
first owner Pulse, then NextFrame, whole owner Pulse, Animate, BeginScene,
PreRender, Render, PostRender and EndScene. Both BeginScene and PreRender
rebind the HUD surfaces; the declared 1280×960 RAM target is selected through
the original camera API after PreRender. Camera (3185,3185,0), center (640,480)
and ambient38/directional1 are explicit comparison inputs. Loaded original
I3D objects, vertices and COM textures are retained; no synthetic color or
geometry is supplied. The only new COM boundary supplies material LightState1
get/set. Original srand1 and unmodified CRT rand are observed.

The base Effect Pulse runs the animator before the custom owner moves or
changes state. Controller creation therefore follows owner transitions by one
Pulse: flight at12 and impact at33. The independent shared-particle comparator
uses each actual controller's state, rather than the owner's later state.
It matches 514,238 fields (maximum error 5.0e-6) and every CRT seed under
ASan/UBSan; the actual controller uses809 random draws and owner kill remains86.

The longer Dragon flight also disproves transferring the trap's final-zero
prototype shortcut. Original particle Render leaves object zero's last
sample matrix: zero for a rejected zero scale, otherwise the absolute MODELZ
sample center. Once the authored flight emission stops, the final sample is
nonzero. At impact initialization, original4033db/40efc0 reads cached
(3451.3125,3199,60.98197937011719), then adds owner XYZ. The resulting large
relative-velocity kick is original behavior in this declared world fixture.
The compiled `PrototypeOrigin` helper matches all339 cached/initialization
channels. No coordinate rescaling or velocity correction is applied. Submitted
triangles extend well beyond the current RAM viewport, so these images are
partial diagnostics and do not establish whole-effect appearance.

Two fresh110-record full-render runs use different final-raster subsets. All
before/after owner, controller, raw pool, random state and submitted bounds
are identical. Four common sampled PNGs (21,33,49,85) are byte-identical. The
source must still receive a separate typed adapter, preserve this caller
ordering and cache behavior, and obtain supported whole-view Metal/native
coverage before visual credit.

`dragonattack-native-first/trace.json` SHA256: `40a0aee7b8af4000705274d752a65d501fb80c6d1771114832f6c6e67348b7e7`.

`dragonattack-state-first/manifest.json` SHA256: `9a2d50c25b56b2c856ff25bbec7f570994eb678e03c09c8bb7881c08c4a24fe6`.

`dragonattack-cache-first/manifest.json` SHA256: `75d6b86bdcb4b7bf53875bee1ba2842bcac59418119205aa0754119084ea2ce6`.

`dragonattack-native-repeat/repeat.json` SHA256: `3414adc3644d71dc7acb05b51ec4edd4b570bce79357d1f4d1370163e9080886`.

The separate production route is now `Dragonattack_ORIGINAL`, using exact
ID0xba09fead plus the strict Hfire resource fingerprint. Its controlled owner
keeps the genuine caller's base-before-custom Pulse order, muzzle(14,14),
100Z launch, target(sourceX+280,sourceY,ground0), three states and real OF_KILL.
It exports IsAlive. The original trap route and all other profile admissions
remain unchanged. The isolated test camera uses the explicitly supplied source
XY, retaining world coordinates rather than shifting simulation inputs.

The first actual Metal run matches946 witness fields over86 active renders:
owner state/frame/position, live particle count/capacity, all809 random draws
and prototype-cache XYZ. Its1280×960 images are still partial diagnostics.
The existing exact Hfire profile/key regression passes (170 rejected mutations,
450 default-key controls). The full private build passes.

A whole-view control rejects simply repeating Render: original Hfire samples
CRT random values during Render (11 calls at record37), so a second Render
cannot be compared without restoring the pre-Render random and caller state.
Additional coverage therefore uses a full VM/host checkpoint before Render,
then reproduces canonical Render before continuing simulation. A viewport-
dependent cache difference has not been established by that failed control.
Whole appearance remains open.

`dragonattack-metal-first/manifest.json` SHA256: `b10150e156fd40af69ac9a2a44a917e4d5a67ba5fb7d9df4ae9de0efdaafd66c`.

`dragonattack-metal-first/state-witness.json` SHA256: `ad9da483eb0a963ecc896a9eeb40b8b7fc7c031024dcba018221d2528c53c34b`.


Whole coverage follow-up (pending independent appearance approval)
----------------------------------------------------------------

The completed immutable grid has16 supported1920×1080 views with origins
(-256,-832*i), i=0..15, relative to the original1280 viewport. World camera,
source, target, lighting, geometry, RGB and timing are unchanged. Every
submitted triangle at all110 ticks fits wholly inside at least one view;
no coverage gaps remain. At the25 selected phases, six views intersect
triangles; the others are declared empty by conservative submitted-triangle
bounds rather than synthetic images. The main view and five distant views
retain raw overlap, without claiming one exact giant framebuffer.

Five additional original renders restore the full pre-Render VM, host and
CRT checkpoint. Each produces exactly the canonical post-Render owner,
controller, pool, matrix cache and random state. Replaying the canonical
Render resumes simulation. All110 before/after records also match the fresh
1280 reference exactly, including allocation addresses and all raw pool bytes.
The earlier repeated-Render failure is therefore an invalid RNG control, not
evidence of a viewport-dependent controller.

Six independent actual Metal films use every24Hz simulation/render tick and
only sparse PNG storage. Actual image dimensions are1920×1080, native depth
is0..65535, and the supplied source remains(3185,3185,16). All5,676 owner,
particle-count, random-count and cache witness fields pass. The review packet
contains19 active raw tile pairs and four actual blank lifecycle controls.
The original full110 simulation continues beyond real kill86 through109;
record97 is the sampled blank post-kill tail. Common-policy images remain a
separate diagnostic. All25 actual HeadFireBall regression images are
byte-identical to the previously frozen trap film.

Private packet: `hard-alpha/dragonattack-review-final/`, with `main-life.png`,
`distant-impact.png`, all actual raw-pair references and a frozen captured
binary. Source checkpoint260defc3 supplies the exact real-IsAlive route.
Natural dragon/bone attachment, resident actor/map/combat/audio integration,
other caller inputs, device rendering, overlap/edge quantization and exact
framebuffer equality remain open. No primary visual acceptance is recorded
by this lane; the root reviewer owns that decision.

Final review pair-manifest SHA256: `0b62f4520668b6cc9442b938cc1ccd5c2ead9ec9f3e350cf6f18e29dfb05f600`.

Captured binary SHA256: `a0134abe73ca2c0a4d6636eeabc6af422e1dc58f12f5536dd43c12fec6431d23`.
