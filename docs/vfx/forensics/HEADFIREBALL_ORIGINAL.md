# Original headfireball

Exact `0xd000f005` uses `Magic/Hfire.i3d`, SHA256
`f3c52a3402bae39923b913f56f320f5eefdca57d78135d632ac940dfd2615181`.
The complete animator registry selects the default builder5e8508/factory40dc00.
The generic Effect constructor4de770 writes owner vtable5a85ac. This is distinct
from `dragonattack`0xba09fead, whose custom Init505a70/Pulse505a90 consumes its
own target-dependent inputs despite sharing Hfire. No alias credit is granted.

The actual caller is the fire-head trap. Init526c90 derives facing0 from the
literal name `RunFireHeadN`. Spawn526f00 constructs a typed headfireball definition;
the fixture checks its actual class25, class.def type index, flags8 and position.
A declared trap at(0,80,68) and original muzzle offset(0,-20,10) produce the
child at(0,60,78). Resource lookup, map creation/residency and silent sound are
explicit interfaces. One child slot is populated; natural arming and two-child
trap scheduling are not claimed.

The complete original Tick527140 invokes Move526fd0. At frame19 it selects
state1, assigns the actual Angle-derived8-unit negativeY velocity, wraps the
15-frame flight at frame14, and checks the declared character-query planeY=-60.
The real hit branch selects state2 at record32. Original end-frame54 sets
OF_KILL at record86. Character reaction/damage calls are observed interfaces,
not validated combat effects. The source core matches1,100 integer fields over
110 records, including all positions, states, frames, velocities and kill state.

The asset has three20/15/55 states, five objects, twelve vertices, six faces,
two materials and one64x64 RGB565 texture. Its orange flame and short-lived
500-PPS sparks differ from JhagaAttack's purple400-PPS particles. Its `#head`
also retains depth testing, unlike JhagaAttack's `#$head`. The actual default
controller/Render/normal-light/software raster is executed independently.

Private evidence is under RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/headfireball-{preflight,native-state,owner-core,state-first}`.
The initial checkpoint alone does not grant appearance acceptance. Natural trap
stat arming, target selection/damage, audio, generic Effect/script pulse,
resource/COM and map notifications, other orientations, device and exact
framebuffer equality remain open.

## Source and full-film evidence

`HeadFireBall_ORIGINAL` uses the exact typed factory and a real owner `IsAlive`
callback. The default controller admission is restricted to type `0xd000f005`
and the independently pinned Hfire profile. It preserves the head depth test,
three states, original trap velocity, frame wrap, collision transition and
owner kill flag. No renderer or parser changes are required.

The source controller proof matches 517,990 fields over 110 ticks, with maximum
float error 0.000121831 and all 892 RNG calls exact. Skipping Render on two out
of three ticks separately matches 521,320 fields and all 891 calls. Cached
prototype origins match 306 fields across normal, stride-three and early-only
Render controls. Pose extraction matches 4,050 key and 5,568 matrix channels;
170 mutated profiles are rejected and 450 default decoder cases are unchanged.

The frozen full original/default-controller/normal-lit software reference and
two actual Metal policies have 25 identical phases, with 24 Hz Pulse and Render
throughout. Every one of 172 active Metal log records agrees with original
state, frame, position and RNG. The fresh original replay has all 110 states
and six PNGs byte-identical. All submitted triangles over all 110 ticks fit
inside the supported 1280x960 viewport: bounds (369.556,224.801)-(1012.244,613.634).
Camera center is (640,560), world camera zero, original light 38 white with
directional intensity 1. Native depth is 0..65535; ordinary depth 2238..3262
is retained as a diagnostic. No geometry, tint, timing or seed is fitted.

Evidence root: `research/vfx-wide-20261010/hard-alpha/` in RevenantRetailLab.
Review panels and pair manifest are in `headfireball-review-final/`;
full films are `headfireball-native-final/` and `headfireball-metal-final/`.
The root reviewer independently decides scoped appearance acceptance.

- Pair manifest SHA256: `432fae291cca0eeae65b4d92ea9df5bc457a40b744dcfc13ac49ea5172ccbba8`.
- Captured binary SHA256: `31169faeebb3ae705a44c93592491814bc6b22dd00f655ddd75333ce7a55d321`.
- Fresh replay report SHA256: `d19682dfe499e1964213966858b0c0d487c0d16b44c41314665e2244a6675ee0`.
