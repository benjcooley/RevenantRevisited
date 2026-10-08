# FireFlash bounded retail reference probe

The actual software editor accepted `addat 10000 10000 FireFlash` at camera `(10000,10000,16)`, ambient32, on the nine-tile `MCP_AB` floor. DOSBox recording began before the command. The retained [native diagnostic](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-quick-fireflash-20261005/manifest.json) contains62 distinct viewport images across26.169 seconds. Visible changes stop at9.867 seconds from recording start; the bright accumulated flame remains for the rest of the recording. The console later reports `FireFlash: Context not found` for `FireFlash.delete`, supporting natural owner expiry while its pixels remain. A one-unit camera nudge and return restores the exact ground image below the FPS band.

This is a deferred redraw/device diagnostic. It is not an accurate reference or an accepted A/B result. Do not tune a port's brightness, geometry or lifetime to this frozen image. Keep the existing placeholder unaccepted, and advance the easy reference queue before implementing a larger controller without a useful visual baseline.

The snapshot `effect.cpp:1988–2365` supports a standalone editor spawn: initial75 sphere particles in150 slots, optional target/damage paths guarded by a spell, and owner kill at tick100. Tick35 frees sphere slots and emits `SMOKEY1`; ring initialization is commented out. A future port must follow that active code rather than the ring comments. The source uses two shipped quads, distinct material emissive values, literal rotations,24Hz updates and additive blending.

The old-layout `Magic/Fireflash.I3D` has8 vertices, two objects (`smoke`, `smoke01`), four triangles, two materials and two single-frame64×64 RGB565 textures. SHA256 is `7c7471fced7e8175a56327ad2bfa370579d2cc08400ce735e4fe63104244d526`. The word at offset4 is the vertex count, not a version number. Object0 emissive is zero; object1 emissive is one. Both must retain their authored geometry, full UVs and material behavior.

## FPS-enabled retry, 2026-10-05

`captures/runs/sw-fps-fireflash-20261005/manifest.json`: FPS-enabled reference now animates cleanly and naturally expires to exact ground. It supersedes the earlier FPS-off residue as the active native reference. Source-authoring correction and actual runtime/A/B remain to be checked.
