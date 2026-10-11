# Teleporter: genuine loaded null-spell portal

Exact Teleporter `0x369a5d2e` constructs owner `5aa200`, animator `5aa400`,
Initialize `4e7160`, owner Pulse `4e6a80`, animator Pulse `4e7210`, default
Animate `40e460`, and custom Render `4e72b0`. The actual vtable puts `4e7210`
at +2c, not +30: older source/preflight notes calling it Animate are obsolete.
The renderer emits five independently rotating/scaled copies of object1 glow.
It is distinct from IrisFlare's five flares and additional cylinder composition.

`rift_teleporter_loaded_preflight.py` executes the genuine cached original class
factory, loaded old-body imagery and COM textures, CreateAnimator, NextFrame,
whole owner Pulse, Animate, BeginScene/PreRender/Render/PostRender/EndScene.
The declared RAM target is rebound after PreRender:1280x960, center640,480,
source/camera0,0,0, ambient38white/directional1. Original controller and raster
code runs; nonsampled ticks may omit only final56eb30 raster dispatch.
No geometry, texture, controller, owner, failed-resource or color seam is used.

The actual null-spell owner duration is50. It selects phase2 at first Pulse,
phase4 at prior life25, and sets real OF_KILL at tick51. Its animator rotates
by the original float0.2, caps iterations at24, and renders ticks modulo50 with
five cylinders. The original object-matrix zero-scale guard omits cylinder0
at tick50. All other49 visible phases have five draws; tick50 has four;51..60
are blank. The production core reproduces the executed owner counter and kill,
not a preview expiry timer. The registration provides the real is_alive hook.

`Teleporter_ORIGINAL_NULL` is an exact typed factory with the full independently
checked shared IrisFlare legacy asset fingerprint, texture descriptors/payloads,
geometry and authored key checks. It uses one stable immutable cylinder mesh,
source-alpha helper mode3 and the existing opt-in native projection; no renderer
or ordinary Teleporter/gameplay behavior changes. The old hybrid stays available.
The controller's source matrices and lifecycle are checked against every genuine
loaded draw under ASan/UBSan:249 draws,3,984 matrix channels,360 lifecycle channels.
The actual native unit passes in11.021s; the production build passes.

The separate real named `Teleport` spell control uses actual SpellList/creator,
public inline SetSpell, two genuine Acolytes and original BeginFighting. Empty
map destination search takes the original failure path; its shipped duration20
kills at21. Both characters retain positions and health. This is a caller control,
not movement/teleport or actor fading acceptance. Actor Pulse, populated sectors,
spell variants, successful destination selection, audio/device/exact pixels remain
open. Its manifest is `teleporter-actor-control-final/manifest.json`, SHA256
`e82dc57ae5c826a2b43c333d752ae549d100b86bb1283abf7df5b3481d859404`.

The initial `teleporter-loaded-first` wrongly called Pulse as Animate and retained
uncached100000-position sentinels; it is a rejected ABI diagnostic. The subsequent
`teleporter-loaded-correct-abi` had correct visible draws but continued killed-owner
updates; the final reference stops on real OF_KILL and supersedes both attempts.
Neither rejected run establishes visual acceptance. Final images/replay follow.

# Rift1 loaded failure

The same genuine cached startup constructs exact Rift1 `0xadbcef18` and reaches
actual CreateAnimator `46e8b0`. The unchanged shipped animtex tag is rejected;
original failed-controller deletion then reads null at `405c14`, before rendering.
This reproduces the earlier cold-heap boundary without its manually installed
imagery/controller setup. No destructor skip, repaired tag, invented texture or
static fallback was used. It grants no appearance credit.

`rift-loaded-final/manifest.json` SHA256
`b9e06a9abbe5328b5b16afa933eba5bf5181d689233f90d3559705e880368901`;
the earlier independent `rift-loaded-first` has identical fault and call stack.
All private evidence lives under the existing lab hard-alpha directory.
