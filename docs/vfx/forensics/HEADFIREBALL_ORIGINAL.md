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
This checkpoint does not grant actual Metal appearance acceptance. Natural trap
stat arming, target selection/damage, audio, generic Effect/script pulse,
resource/COM and map notifications, other orientations, device and exact
framebuffer equality remain open.
