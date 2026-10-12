# Vortex gallery context and material omission

The user-reported full gallery reopens ordinary-viewer verification without
removing the previously reviewed explicit native-domain/cull2 fixture. The
frozen user-viewer binary is `07975e4578e04fd7a8bc48a2a76b1abad2686bb0fb8bebbbbffaca53fdf8cf3d`.
It was copied privately; neither the live executable nor root checkout changed.

An eight-frame exact-policy gallery reproduction uses `--vfx-review-source-lighting`,
1280×960, type `0x452dade0`, a fixed first station, no labels and scroll0.1.
Its log proves lighting mode0, RGB source enabled, ambient38/255 and directional1.
It renders the white/gray static cone (139,567 nonblack pixels in frame1), with
upper geometry clipped. This is not a modern-lighting or invisible-output case.
`ComputeMapCameraViewport` explicitly scales the logical640×480 view by2 on this
display. The isolated1280×960 test uses scale1. Their framing cannot be compared
as if the cameras had identical world coverage. No scale/position change is made.

The shipped binding is genuinely static. A fresh cached-startup original VM
loads the real Vortex owner and imagery, obtains default animator `0x5a370c`,
three objects and zero controllers, and executes original whole Pulse/Animate/
PreRender/Render/PostRender for24frames at source3185,3185,16 and camera3185,3185,0.
State/frame remain0; all24framebuffers match in the declared initial CULL_NONE
software fixture. This is a loaded-resource proof, not the earlier decoded-resource
adapter. The old procedural `TVortexAnimator` and its registration are inside the
commented `Ye Old Vortex` block in legacy/effect.cpp:12635–13413. Shipped
resources.rvr/spell.def selects `Teleporter` for the named `VortexM` variant.
These facts do not justify inventing animation for the literal Vortex type.

## Separate actual modern-policy defect

The same frozen binary with explicit modern lighting renders eight blank frames.
`SHelperMeshSubmit` starts with zero material arrays. Vortex previously never
copied either authored material, so the modern helper sees alpha0. Its software
RGB5 path ignores material, which concealed this bug in the approved fixture.
The adapter now retains each mesh's actual material and forwards diffuse,
ambient, specular, emissive and power to untextured submissions. No light,
texture, color multiplier, geometry, projection, timing or renderer default is
changed. The source-extracted ASan/UBSan test verifies both own descriptors and
34channels, including the zero-alpha negative. Existing strict legacy keys,
translated composition, cull and depth tests also pass.

## Inherited state remains a separate boundary

`vortex_loaded_context_probe.py` explicitly records its software constructor's
CULL_NONE boundary. It does not mislabel this as natural startup state. Full
original rendering changes the textured third object's state on its first
texture bind to CULL_NONE, ZWRITE0 and SRC_ALPHA/INV_SRC_ALPHA, revealing a blue
card. A separate original control resets scene0/cull2/depthwrite before each
frame: its first frame shows the card, while its second keeps the cached texture
and does not repeat the blend-state transition, suppressing that card again.
The two frames have identical authored geometry but different state history.
The first control incorrectly asserted equal pixels; that failed run is retained.
The corrected tool records the difference without an appearance assertion.

The old reviewed reference deliberately fixed cull2/depthwrite across its
three decoded-resource draws. Its scope remains valid, but does not establish
cold texture binding or arbitrary inherited scene state. No unconditional
card/culling fix follows from these controls. Natural caller/map state,
state-cache interactions, device behavior and exact pixels remain open.

Private evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/vortex-gallery-bug/`.
`gallery-auto-before`, `gallery-modern-before`, `native-loaded` (24frames),
`native-states`, `native-cull2-final`, `material-contract` and `vortex-tests.log`
retain the positive controls and declared limitations. Root owns review/approval.

The final private binary is
`fa22c58dadbb723759d940813f5e90cf8f42c2a14b1e46548ffc04f38c8f0e15`.
Modern output changes0/8→8/8 visible frames (139,567 pixels in frame1), using
the own reddish material. All eight exact-policy software gallery PNGs and all
eight native-domain isolated PNGs remain byte-identical to the frozen binary.
Four focused tests pass; the application exits cleanly for all six capture runs.
The combined private `review-manifest.json` SHA256 is
`dedcb1300da4bf99305cd41255a826385467c81f75f1fe3c15b2089bcb32bf8f`.
`material-contact.png` and `context-contact.png` show these separate findings.
This is a modern visibility fix, not a claim that the user's auto-mode static
cone or inherited-state behavior has been changed or accepted.

Root integrated build/four focused tests pass. Fresh root auto and modern
8-frame gallery captures repeat the candidate pixels exactly (139,567 nonblack
pixels in their first frames). Auto remains unchanged; modern recovers visibility.
Evidence: `research/vfx-vortex-gallery-root-20261011/{auto,modern}/manifest.json`.
The user's static/clipped auto-gallery complaint remains open and receives no
appearance approval from this modern-material correction.
