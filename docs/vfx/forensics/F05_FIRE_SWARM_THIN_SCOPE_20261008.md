# FireSwarm: fresh thin-render scope, 2026-10-08

The fresh merged source proof passes through ticks 0–78 against actual retail
Initialize `0x4f0130`, Animate `0x4f0160` and Render `0x4f0270`. It compiles the
current shared production Initialize/Advance/Submit bodies in both preview and
map-owned modes. The complete native state and selected software images repeat
twice. This supplements the older F05 narrative; its snapshot-only tuning/render
claims are superseded within this specific executed scope.

## Classification and exact asset

This is a stationary expanding/shrinking cylinder, not a missile or a
point-to-point effect. Original registration `0x4f00d0` names FireSwarm and
installs effect builder `0x5abec4`; the effect Initialize at `0x4f00f0` is RET.
The source class derives `TEffect`, with an empty Initialize and inherited
Pulse. Animator registration `0x4f0110` installs builder `0x5ac0c8` and animator
vtable `0x5ac0cc`. Its custom state is one frame counter plus horizontal scale,
vertical scale and yaw. It does not establish source/destination endpoints or
missile speed/velocity. No moving-projectile credit is implied.

The shipped `Imagery/Magic/fireswarm.i3d` is pinned to SHA256
`858011f22ae4b4e17be08667a8ea2cc018c71b672889cef822e12c07a57676c0`.
Original Render selects object 1, `tube01`, with 174 vertices and 192 triangles,
local indices covering all vertices, unchanged authored UVs and texture 1's
64×64 ARGB4444 pixels. Object 0 is not substituted. The native state executes
all 237 scale/yaw float32 comparisons exactly and expires at tick 76. Candidate
runtime-owned mode requests its normal kill once; preview uses its own lifetime.

## Visible credit and empty samples

Twelve selected software image/depth pairs match with two complete warm replays.
Only five sampled frames receive nonempty frontend appearance credit:

- Tick 30: 98,187 nonzero reference pixels.
- Tick 36: 107,644 pixels.
- Tick 48: 115,710 pixels.
- Tick 60: 104,060 pixels.
- Tick 72: 26,375 pixels.

Ticks 0, 1, 12 and 24 are empty because the original software dispatcher rejects
triangle edges above 640 pixels horizontally or 480 vertically. The relevant
branch is `0x56d9fd..0x56da7f -> 0x56dba7`. Enlarging the explicit 1024×2048
fixture does not remove that internal limit. Tick 75 is empty at the flattened
end; ticks 76 and 78 follow expiry. Empty matches do not establish early flame
appearance. No scale, camera or raster change hides this limitation.

The imagery boundary supplies the current frontend's triangle-culling policy;
that culling is not independently validated against the original graphics
hardware. The fixture also declares identity owner, null spell, white vertices,
camera zero and z-distance 1925. Original matrix/vertex/projection/raster code
runs unmodified. Loader/component/real-map variations, arbitrary owner poses,
normals/lighting, independent blend/culling/device behavior, early visible
appearance and modern GPU parity remain open. This is one bounded frontend row,
not complete FireSwarm acceptance.

## Reproduce and inspect scope

```sh
python tools/retail_runtime/fireswarm_probe.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --ticks 78 --repeat 2 --output <private-report-directory>
python tools/retail_runtime/fireswarm_render_scope.py \
  <private-report-directory>/manifest.json \
  --output <private-report-directory>/render-scope.json
python tools/retail_runtime/test_fireswarm_render_scope.py
```

The scope reader counts actual RGB pixels in the PNGs written by the software
fixture. Its focused tests reject appearance credit for empty matches and
reject an unequal depth pair. It does not render, patch the original or fit the
candidate.

Fresh local lab evidence is in
`research/vfx-next-20261007/fireswarm-render-current-20261008/manifest.json` and
`render-scope.json`. The manifest SHA256 is
`52895e29063ccccecf25a25a5a3138cc08e00ac3b5b568311aeaa55504d7ebcc`.
The report retains baseline/asset/probe/source-span/compiler/binary hashes,
original state, submitted native geometry and image/depth hashes. No DOSBox,
guest startup, external caller reconstruction or new engine change was used.
