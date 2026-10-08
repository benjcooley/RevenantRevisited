# FireWind bounded retail reference probe

The actual software editor accepted `addat 10000 10000 FireWind` on the nine-tile `MCP_AB` floor at camera `(10000,10000,16)` and ambient32. DOSBox recording started before spawn. The [native diagnostic](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-quick-firewind-20261005/manifest.json) retains30.583 seconds and84 distinct viewport images. It shows the initial flame, expanding rings and rectangular areas of floor restoration. Viewport changes stop at17.4 seconds; frozen bright trails remain at the left edge through the recording tail.

`FireWind.delete` reports `FireWind: Context not found`. The retained screenshot still has trail pixels, with mean viewport error40.3014 below the FPS band. A one-unit camera nudge and return restores exact ground. This is evidence of a native redraw/device problem after owner expiry, not an accurate reference for port fidelity. No reference, visual, source-fix or runtime acceptance credit is added.

The snapshot `effect.cpp:2388–3080` supports a bounded standalone visual probe: spell damage is guarded, the animator initializes400 particle slots,100 initial sphere particles and one later fire ring. Unlike FireFlash, its active code initializes explosion-ring particles at tick40. It ends after tick140. The renderer uses two authored objects, literal rotations and additive blending. The current bespoke port is a placeholder and remains unaccepted. Defer the full controller until a clean reference is available, preferably in its natural setting; advance the easy reference queue.

## FPS-enabled retry, 2026-10-05

`captures/runs/sw-fps-firewind-20261005/manifest.json`: FPS-enabled reference still retains left-edge residue after natural owner expiry:mean error1.35331. Camera nudge restores the canonical floor exactly. This is a bounded shared offscreen/dirty-region redraw diagnostic, not an accurate reference or implementation/visual credit.
