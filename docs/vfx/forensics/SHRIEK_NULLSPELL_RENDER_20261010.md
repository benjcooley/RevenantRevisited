# Shriek null-spell original owner and complete renderer

Exact shipped identity `shriek` / `0xad92bc12`, `Magic\\Shriek.I3D`, SHA256
`759b5989dd487c31fa882c016786eeda480028786c927a888af5449c98955d53`.
The asset has four authored meshes, four ARGB4444 textures, 48 vertices and
48 faces, one 100-frame state, no tags, and original eight-byte V1 keys.
The adapter preserves all geometry and ring-index draw order.

The registered factory `500380` / animator vtable `5ae394` executes original
owner Init `4fc810`, Pulse `4fc830`, leaf Init `4fc960`, Animate `4fca70` and
Render `4fcc00`. Eight starts consume MSVC random(0,32) once each. After each
start delay, scale advances by the literal float .12 and position by 3.
Scale greater than 2 resets scale/position; counter45 closes new births and
counter46 executes owner KillThisEffect. Render begins after counter15,
cycles through the four original meshes and uses position
`(-scale, -scale-position, 0)`. No fitted scale, lift, phase or color.

The real null-spell branch skips actor/bone placement. Its original damage
helper `4de3c0` actually executes and immediately returns for null invoker;
there is no invented damage-query callback. Native fixtures execute original
NextFrame/Pulse/Kill, CopyVertices `40a0c0` to FVF_LVERTEX `0x1e2`, full
RenderObject `40a8f0`, owner/projector and software raster. Immutable decoded
resources/COM residency, base bookkeeping/extents and owner script/light/map
notification services are explicit boundaries. ARGB4444 ignores incoming
vertex color, independently checked with zero/white controls.

State proof passes 1,836 fields through 50 ticks, all eight initialization RNG
calls and every subsequent seed; maximum float error 3.7061e-9. Two independent
full native replays match every state, draw packet and framebuffer hash.
Actual source Advance lifecycle test checks external owner death before the
next step, guard-removal negative control, own completion46 and post-kill
no-op. Gallery uses the real owner's completion callback.

The first Metal replay exposed a rectangular skirt from sub-quantum authored
texels. The separate optional source-contribution correction is documented in
`ARGB4444_SOURCE_CONTRIBUTION_20261010.md`. Only explicit native-domain Shriek
opts in; all 50 common-policy frames remain byte-identical to their earlier
capture. Destination rounding/framebuffer precision and exact pixels remain
open. The final fifteen-phase comparison includes blank startup, ring motion,
cutoff/kill46 and blank50. Native/domain bounds agree within one pixel for
these phases; this is not a claim of pixel equality.

Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra`.
Final comparison: `shriek-review-final/{contact.png,detail.png,manifest.json}`.
Manifest SHA256: `616f186dafac87a4e7f67bf48f6f6bb4121b025efcbcfdbd9e13f48ffa3d081b`.
Binary SHA256: `2d7e39ae6db8ee941a36326f3476b6415770fc94b15566c59bfcd4281381101b`.
Original native manifest SHA256: `371845fccba6fc38711f53e68e8588b45968a63860891bdc4e0e2dcd90ea4ab1`.
GPU literal-control review SHA256: `bb9a007bea43142ebe9b4b9876a51bff4df7770d8e84edece8603e0d324c43e0`.

Independent appearance review is pending. Scope is explicit null-spell,
seed1, owner/camera0, 640x480 native-domain software-policy fixture. Natural
invoker/bone placement, damage/audio, device behavior and complete natural
caller remain open; common projection/skirt are separately retained.
