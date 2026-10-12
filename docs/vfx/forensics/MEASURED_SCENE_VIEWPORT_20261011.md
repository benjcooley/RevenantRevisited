# Measured sector-test viewport

`--scene-viewport=W,H` selects an explicit test-only output viewport when `--scene-camera` is used. The existing640×340 default remains unchanged. This allows the port to match the original game's measured452×420 Pit viewport while retaining the same declared world camera; it changes neither geometry nor camera/projection policy.

Parsing rejects missing, zero, negative, overflow, trailing text and dimensions beyond1920×1080. Scene startup also rejects a viewport larger than the actual renderer display. `tools/test_sceneviewport.cpp` runs the actual shared parser/admission functions under ASan/UBSan and covers those negatives, the default, measured452×420 and maximum1920×1080.

Private full build passes. An actual saved Pit sector test with `--resolution=640x480 --scene-camera=48,19186,19996,144 --scene-viewport=452,420` produces two452×420 PNGs, the exact viewport marker and clean exit. `--scene-viewport=641,340` on that display produces the explicit admission-rejection marker. Test-screen initialization rejects it without loading the scene; the diagnostic application still exits0, so the rejection marker is the oracle.

Evidence: `/Users/benjamincooley/RevenantRetailLab/research/vfx-next-waterflft-insitu-20261011/viewport-smoke/manifest.json`. This verifies measured framing and invalid-bound admission only. Native/port water appearance, map residency/lights/occlusion and natural gameplay acceptance remain separate.

Root integrated build and ASan/UBSan parser/admission checks pass. Independent
actual Pit smoke at `research/vfx-water-viewport-root-20261011/manifest.json`
passes two452×420 frames/clean exit and641×340 display-bound rejection, preserving
module/camera/ambient. Root binary SHA256:
`55e2a55296308290db4dbbbbb1d3d2ab976169c5d5f88d2e777590a15a7bdd49`.
This framing test does not grant a new VFX appearance or map-context approval.
