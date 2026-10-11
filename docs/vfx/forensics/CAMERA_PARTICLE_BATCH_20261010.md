# Ogrestrength and trollblood, 2026-10-10

Exact shipped Ogrestrength `0x42e0fcd0` and trollblood `0xad92bd39` now use
validated production particle controllers and their three animated authored
camera-facing base cards. Both have independent native/compiled frontend proof,
actual map lifecycle captures, and actual Metal/native visual panels. Root
reviewed both six-phase panels and approved overall form/color and three-base
plus trail evolution for the recorded native-domain fixture. Exact pixels and
natural scene/caster acceptance remain open; `accepted=false` is preserved.

## Source and resource contracts

Ogrestrength is `magic/Ogre.i3d`, SHA256
`df40ba633e9db0f0cfbcf8fa819d5a7e8105317707890e0293c5786532755072`:
one looping state, 200 frames, five objects, 617 vertices, eight faces, two
materials, one 64-square RGB565 texture, one literal partsys tag at frame19.
The three `#ogre` emitters are animated authored four-vertex cards; the
`*ogreloop` helper is hidden. Capacity46, three emitters, quality0.

trollblood is `magic/Trollblood.i3d`, SHA256
`8b6bc7b9a2c67fea510fa03bb8107a1661ed9a30194b4cd6f8d2e7fcc1fda95a`:
two states, state0 loops for60 frames, seven objects, 100 vertices, 86 faces,
one material/8-square RGB565 texture, one literal partsys tag at frame1.
Only state0 is admitted. The three animated `#blood` emitter cards are visible;
three `*path` helpers and their authored triangles remain hidden. Capacity41,
three emitters, quality0.

Literal CLASS EFFECT bindings and whole assets are probe-pinned. Engine
admission checks exact state count/flags/length, object names/materials/parents,
packed state0 keys, vertex/face counts and face bins, tag text/order/frame,
material floats, RGB565 descriptor, prototype vertices/indices, and every
visible base card's full vertex/index fingerprint. Hidden geometry has no
new rendering claim. Other states of trollblood retain the existing key policy.

The renderer now feeds independently decoded original `#` camera matrices into
these exact emitters rather than generic bone matrices. The actual source helper
uses the cached integer animator frame for emission. Base draws use the owner's
current integer frame, the same camera orientation, original inherited blend16
and normal-light path, and render after particles. An executed controller render
publishes blend16 even when its sampled scale gate submits no quad. The startup
incoming blend remains an explicit input before that render. Map creation and
normal default animator ownership are retained. A final map inspection exposed
that generic mesh submission checked absent keys but ignored the native per-bone
OBJ3D_HIDE bit: trollblood's three hidden path meshes drew a black shape over
the floor. The exact two-profile map path now respects this bit after particle
prototype submission. The original failed visual inspection (3,296 newly black
floor pixels in its sampled frame) is retained in `map-before-hidden-fix/`.
Final map verification rejects newly opaque-black floor pixels; the refreshed
actual Metal preview must reproduce every already-approved frame pixel hash.

Public API for review admission:
`T3DImagery::HasRetailCameraParticleProfile(uint32_t type_id)` is exact to these
two IDs and requires successful literal profile validation. The existing
`HasRetailStaticParticleProfile(type_id)` also admits both. Review mode files
and central ledger/index were not edited in this batch.

Two named preview entries, `TOgrestrength_AUTHORED_TAGS` and
`TTrollblood_AUTHORED_TAGS`, create real typed EFFECT owners/default animators,
run NextFrame/Pulse/Animate, and submit the same production particles and all
three literal base meshes. They provide reproducible isolated actual Metal
captures, with the explicit existing native-domain projector option. No
stand-in meshes, color adjustment, billboard generation, or new sound was added.

## Executed evidence and review

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-next-ogre-blood-20261010/`.
No licensed assets, binaries or generated screenshots are committed.

- `profile-final/manifest.json`: eight literal/path positives; 1,572 mutations
  rejected, including96 visible-base float and18 index mutations per type.
- `pose-final/manifest.json`: actual production integer decoder and camera
  emitter helper compile under ASan/UBSan and compare all state0 frames to
  original409430/40a420. 12,780 TRS channels and16,640 matrix channels pass;
  maximum matrix error below1.15e-7. These separately compiled poses drive
  the independent production particle frontend.
- `render-final/manifest.json`: two fresh native full-pool/pose replays per type,
  actual full default/named constructors and all93 registrations, native whole
  parser/Initialize/Pulse/sample/matrix/x87 transform, compiled State/SubmitPartSys.
  239,652 meaningful state checks; 5,508 full quads and356 sampled-scale rejections.
  Ten nonempty RGB565/depth pairs, each repeated twice, have zero differing pixels
  and identical depth. This is shared-original-raster frontend evidence.
- `native-visual/manifest.json`: complete independent native particles and all
  three normal-lit bases, exact meshes/texels, original transforms/illumination/
  raster. Source/native pulse cadence is stated: emitter frame tick, base frame
  tick+1, one warmup render. No production geometry/colors are native inputs.
- `metal-visual/{Ogrestrength,trollblood}/manifest.json`: actual private binary,
  60 recorded Metal frames at24Hz, seed1/quality0, fixed origin/facing0, black
 640x340 viewport centered320,170, native domain, source ambient38 and white
  directional light, no point lights, explicit incoming16. Private save folders
  contain fixture Revenant.ini. Both captures finish normally.
- `visual-review/manifest.json`: twelve unfitted full-frame native/Metal pairs at
  captures1/5/15/29/45/59; every phase has equal RNG draw counts. Pixel differences
  remain recorded, with no registration/cropping or appearance fitting.
- `visual-review/Ogrestrength-root-approval.json` and
  `trollblood-root-approval.json`: separate root approvals pin every sample,
  native/Metal manifests, panel and binary SHA, source hashes, scope and open gates.
  Root independently inspected both panels. These are scoped primary visual
  passes, not automatic zero-pixel or complete-game acceptance.
- `map-final/manifest.json`: both real map lifecycles pass eight typed create/
  controller/capacity/MOVE/DELETE/absence observations and48 frames. The authored
  three-base/particle visuals are present, movement is observed, and deletion
  restores the floor exactly. Map panels were inspected separately.

Private build SHA256:
`7a5b36d85018972271d8bc2572917ece05c836a77c2a3753491617451d34d480` (post map-visibility correction).
The private full build, seven camera/quoted/static-particle unit regressions,
and same-source Speed/Quicksilver packet/native software regressions pass.
The incomplete interruption-era unit log is retained as `tests.log`; the clean
completed rerun is `tests-final.log`.

## Bounded deferrals

No jitter feature was added while closing these two types. Dust and charm's
literal tags require unsupported jitter fields. Restorelife's original required
`glint02` key/matrix already rejects atframe0; no replacement pose was supplied.
The original attempts remain under `native-feasibility/`. They have no new
rendered or primary visual credit. Natural caster/parent/alternate-state inputs,
original scene/device/collision/audio and exact Metal/RGB565 pixels remain open.
