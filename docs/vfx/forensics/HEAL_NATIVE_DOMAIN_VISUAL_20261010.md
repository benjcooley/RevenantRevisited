# Heal authored meshes and lifecycle

`heal:0xad92bc1d` now submits its exact authored heal mesh (12 vertices,
12 triangles) and glow cylinder (174 vertices, 192 triangles), retaining the
original particle order followed by two opposite glow rotations. Slot0 is
also a rendered heal mesh. The old preview substituted tinted billboards,
used only the first four vertices, and omitted that central mesh. All eight
shipped textures are ARGB4444; the existing source alpha policy ignores vertex
color. The preview now uses that policy with positive-face culling and retains
the original ordered alpha composite. No shared renderer or camera changed.

The original leaf was resolved by executing all93 animator registrations,
lookup and the real factory: builder `0x66cd38`, factory `0x4f4c60`, vtable
`0x5a8a20`, Init `0x4df680`, Animate `0x4df820`, Render `0x4dfa60`. The historical
M03 note's `0x5a8f20` label is Cure, independently confirmed by the same lookup;
it is not the reference used here.

The fixed configuration is spell level1, caster/owner position0, facing0,
seed1 and 24Hz. Init relocation, every Animate/Render call, glow TRS
`0x40a420`, owner/relative world composition and original FVF_VERTEX/ARGB4444
software execute in the x86 fixture. Asset/resource residency and complete
RenderObject dispatch are explicit boundaries. Scene blend2 is executed;
cull2, depth testing and no depth writes are declared renderer inputs.
At zero glow scale the original TRS rejects the draw; the port does too.

Production now uses exact `1000.0/24.0` ticks and the original float rise
constant `1.3333333730697632f`. Compiling current initialization and Advance
unchanged reproduces all91 ticks, 27,300 float fields, active/frame counters,
rotation, 284 RNG calls and death at tick78. RNG ranges/order are checked
against original outputs. The actual preview logs independently pin seven
RNG checkpoints and source ambient38/directional1. Mesh submission runs in the
world callback, so the gallery uses the same actual animated meshes.

The native-domain option is an isolated fixture with cameraZ0. Common-world
captures retain the shared projector residual. Neither configuration changes
gameplay camera/terrain/input. Pixel and framebuffer precision differ;
natural spell attachment, player HP/audio, levels2–4, complete resource
dispatch and retail full-map/device behavior remain open.

A preliminary contact incorrectly paired capture70 with original tick70.
One warmup means capture1 is tick2; capture70 is tick71, one shrink step later.
Final review uses capture=tick−1, checks logged RNG against the prior native
state, and includes the complete decay/blank tail. No production timing or
scale was fitted to an image.

Reproducible tools are `heal_probe.py`, `heal_state_compare.py`,
`heal_visual_review.py`, `port_capture.py` and `review_capture_survey.py`.
Private evidence lives under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/heal-visual`.
Generated artifacts and licensed assets are not committed.

Aura was investigated first and deferred: executed lookup resolves factory
`0x4f57c0`, vtable `0x5a9900`, Init/Animate/Render at
`0x4e39f0/0x4e3b30/0x4e3f40`. Animate requires an actual character animator's
bone matrices; its bone-count input is not supplied by the observed leaf Init.
The current Aura preview's fixed billboards/tint and missing bone sampling
are not a full native appearance implementation. No invented bone poses or
Aura acceptance are included in this change.

Frozen evidence (2026-10-10):

- `native-final2/manifest.json`: SHA256 `690ba0f5f3cca4493a93a6de277925c098734417e3ad7192e0237aa3a4026ca3`.
- `state-final/manifest.json`: SHA256 `19a225c9b436940729533aa9554a32f906de120355c94ae2233b6f4b31231fef`.
- `metal-frozen-common/manifest.json`: SHA256 `2cb45afa3012011abc9fcdaf76acbf2e429bea20e208531e0191a489e33c332a`.
- `metal-domain-repeat/manifest.json`: SHA256 `c899b0f3713509dcd6a47fc3fb0eb96101a3eb504a3f32b14a043be997e11056`.
- `review-final/manifest.json`: SHA256 `ed15330af92eb1af70c7908e3ce3a8b0c9273da99d5c49854366baa32f8445b8`.
- `map-frozen/manifest.json`: SHA256 `5e4c2b8629eb4e9d46cbf615fc74b384f2a4e699adf0398b88ae70184e3767d0`.
- Metal binary: SHA256 `39d34378401d06f5095c956c52317d1f75cc88182de4b0dca9b4b2314a3fd10e`.

The native reference reproduces all92 states, render packets and software
pixel hashes on a second warm replay. Review freezes90 three-way frames after
warmup, including75 visible frames and the exhausted tail. The final gallery
run exits0 with38/40 visible and39 distinct frames, logging the actual Heal
preview route; this is runtime smoke, not a native-map visual comparison.
A separate Metal recording (`metal-frozen-domain`) repeated the preceding
readback once at capture29 and is excluded as a capture diagnostic. The full
`metal-domain-repeat` recording has no live duplicate and matches all90
earlier complete domain image pixels; no frame-by-frame splicing was used.

Root independently reviewed the final contact/detail and approved the scoped
native-domain level1/facing0 cylinder, rising points, phase progression and
empty tail. Approval record `root-approval.json`: SHA256 `903daefb82e29696d6383408b943acc0fa82e691f4cd8a2af0fd3d264f9c5d1d`.
It pins all90 phase triples, the final binary, RNG/light logs and separate
state/gallery proofs. This grants one scoped appearance pass only.
