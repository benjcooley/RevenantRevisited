# LightStrip: original moving three-state leaf

The new `LightStrip_AUTHORED_TERRAIN` entry executes the recovered original
LightStrip behavior. The existing `LightStrip` hybrid remains available.
`lightstrip_original::Spawn(source,target)` exposes the same controlled leaf to
review callers; its target is an explicit terrain-wall boundary, not a fabricated
character collision.

Identity is `0x10da54d0`, `Magic/NewLightStrip.I3D`, SHA-256
`5745140fc8e916df54521ae8c0c6014e47fad75726bc275fc03f5deb23ad46a5`.
Original owner Init/Pulse are `522310/522360`; animator factory/vtable are
`5236b0/5b5ebc`; Init/Animate/Render are `522740/522750/522760`, wrapping the
actual generic controller and full normal-lit renderer. The fixture executes
original `470cc0` NextFrame and `46f250` SetState, rather than supplying a guessed
phase schedule. Original executable SHA-256 is
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

The literal asset has start/flight/end lengths 10/31/29, thirteen objects,
two RGB565 textures, three PartSys tags and one immediate `litaddz` companion.
Strict admission pins all state keys, objects, geometry, materials, tags and
texture formats. Controllers see the animator's cached frame until Animate,
including multiple Pulses between renders. The flare and particle prototype
retain original `#` camera matrices; ordinary bolt columns use original R*S*T.
Full rendering submits source particles before texture-major base objects.
Bases retain mode 80 without depth; particles retain their own mode 16 RGB565
software modulation. No renderer/shader change or fitted tint/geometry was used.

The shared `scljitter` dependency is commit `c7df79871874d9725dfab58858b6d0e2796805bb`:
original `405134` parses the range, and `402c6b..402ca8` samples tenths after
alpha jitter and before positional jitter. LightStrip authors `-1:1` in all
three states. The unrelated default-zero path remains unchanged.

The controlled world starts at `(-180,0,78)`, facing 64, with floor height 1 and
wall height 10000 at `x>=180`; owner and particles query the same terrain.
Original movement is 20 units per Pulse. State 1 starts at tick 9; tick 28 detects
the wall, takes the original final movement step to x200, and enters state 2.
Tick 57 kills the owner. Explicit `ForcePos` establishes the fixture's negative
coordinates before animator creation; normal map-sector placement is outside
this isolated reference.

Evidence is frozen beneath
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`:

- `lightstrip-native-final`: actual full original normal-lit base and prelit
  particle rendering, literal decoded resources, original software raster.
- `lightstrip-pose-final`: all 910 object/state frames; 8,190 key and 12,656
  matrix components pass. 1,307 profile mutations reject; default decoder
  matches explicit-false on all 910 frames. ASan/UBSan pass.
- `lightstrip-state-final` and `lightstrip-state-stride3`: 340,008 and 341,172
  full-pool fields pass, every RNG seed exact; float tolerance 0.001 world units.
  These comparisons use native cached emitter inputs; the independent pose
  proof establishes the source decoder and matrix separately.
- `lightstrip-metal-position`: two actual 80-frame Metal films, common/native
  projector policies, each 56 nonempty frames followed by the empty tail.
- `lightstrip-review-final`: eighteen matched phases, all 14 logged runtime
  RNG controls exact, and a fresh native replay with identical 80 state/pool/
  RNG/draw records and 22 PNGs. Pair manifest SHA-256
  `59c489f7c7d41c6014525e5eac6a5d7f266f6c774771e94a740db8ac071c9c75`;
  cases SHA-256
  `2fd929f65e9d3d8e4d3d0e766aec5d7376ec7a3112e4b9f01779537a400e0238`.

Captured binary SHA-256:
`01624d0b96a79d9164ba80baadb67b362086070384d3d9bb56f48340c6fb7cb4`.
Native light is ambient 38/directional 1; Metal requests scene ambient 32 white.
Camera `(0,0,0)`, screen center `(512,485)`, 1024x768, seed 1, 24Hz, warmup 1:
capture N follows original Pulse N+1. Both captures exited cleanly.

Retained `test_lightstrip.py` passes original lifecycle/pool/RNG checks at
normal and stride 3 cadence plus independent pose/admission checks. Existing
Speed profile, Quicksilver packets, three static particle profiles and cached
pose/default fallback tests pass; the private application build passes.

Acceptance is limited to this null-spell moving leaf and declared terrain.
Natural character selection/damage, audio, script/reaper integration and exact
framebuffer/device equivalence remain open. The fixed viewport clips occasional
long branches and fast outer sparks; off-screen appearance is not certified.
Common-world projection and raster quantization differences remain explicit.
