# FireSwarm, Faultfire, and Streamer: current rendered comparison

The three existing controller ports now have independent, full authored-normal
software references and current Metal panels. This is review evidence, not an
automatic all-gate or exact-pixel acceptance. Root owns the acceptance ledger.

## Causal source changes

- FireSwarm `0x582c1e78` and Faultfire `0x51753bce` use authored ARGB4444.
  Their quad submissions omitted `retail_argb4444`, bypassing the existing
  source RGB565-table/alpha-nibble-plus-one sampling policy. Set that format
  flag; keep the authored triangles, source alpha request, state, and UVs.
  Both native Render callers request scene mode `(2,1)`; Faultfire's request
  was also observed directly at `417d60` during this run.
- Streamer `0x482dfe82` submitted unlit white triangles. Its executed original
  Illuminate produces RGB5 `(29,29,29)` for the declared light descriptors.
  Submit its four unchanged authored meshes through the existing source-lit,
  additive helper path, retaining source face culling and no depth writes.
  The normal-only inverse of the common-world 1.5 Z bridge preserves native
  normal space without changing geometry. Mesh references are cached and
  released with their owner. Preview submission runs inside the world pass.
- All three support the existing explicit native-domain isolated-preview flag.
  The independently derived owner/projector is selected only for preview;
  runtime-owned map effects retain the ordinary projector and owner transform.

No renderer or shader file changed. No per-effect position, scale, phase, or
brightness fitting was used. The retained pre-change binary is a negative
control for the old unlit/format behavior.

## Frozen evidence

Private artifact root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`.
Current binary SHA-256:
`f19c510502e9225635ce425fb44e19d0b67d88e416a636ac777d74918778bb90`.
Original executable remains pinned to
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

Each `mesh-leaf-final/review-NAME/manifest.json` freezes 13 original samples,
103 baseline Metal frames, 103 current common frames, and 103 current
native-domain frames, image hashes, source-light logs, and six RNG observations.
Its `contact.png` displays original | current common | current native-domain.
All captures use origin0/facing0, camera0, 640x340 viewport, 24Hz, seed1,
warmup1, packed source ambient38/directional1/no point lights. Retained
capture i follows tick i+1; no temporal search or alignment was performed.

- FireSwarm manifest SHA-256:
  `06251459ea2fc0a69e123ef956844c6425198c0584539c3c4178a40070e11ad3`.
- Faultfire manifest SHA-256:
  `88fd615281476ac959407971913a16258d1e6d05439edef35026765096364125`.
- Streamer manifest SHA-256:
  `e434276fd6d1fc8b1e24d8c17b2499c011458aa7e2be8691ec88d134c128dc59`.

`mesh-leaf-current/lit-NAME/` contains original owner fragments
`40e470/40e740`, relative RenderObject carrier `40aad5`, and unmodified
`56eb30` transform/Illuminate/projector/raster output. Every selected native
image, depth buffer, and transformed color set repeats exactly. The original
controller fixtures supply their own matrices and mutable UVs; production
geometry and precomputed colors are not inputs to this reference.

Summed absolute RGB error (baseline / current common / current native-domain):
FireSwarm later phases 9,870,686 / 8,606,070 / 6,215,387;
Faultfire 817,732 / 834,277 / 602,757;
Streamer 4,429,997 / 3,080,429 / 490,549. These are diagnostics, not thresholds.
Faultfire's common metric does not improve; source alpha coverage restores its
faint upper region while ordinary projection and framebuffer differences remain.

Actual map evidence is `mesh-leaf-map/NAME/manifest.json` and the combined
`mesh-leaf-map/contact.png`. Three 124-frame FontRuntimeLab runs pass all 16
typed create/MOVE/absence checks. FireSwarm and Streamer naturally expire;
persistent Faultfire is explicitly deleted. The final 16 frames of each run
equal the initial clean floor. Every manifest retains exact command arguments,
log rows, frame hashes, and the same current binary hash. Ambient32 white,
camera `(110,10000,10000,16)`, independent SAVE directories, no DOSBox.

## Verification and remaining limits

- Full application build passes.
- FireSwarm's 79 native/controller states repeat exactly, including tick76 kill.
- Faultfire's 129 native theta/per-vertex U states and preview/map producers pass
  the existing independent test, including both phase wraps and 128 RNG calls.
- Streamer's 104 native/controller states and 11 selected shared-raster
  image/depth pairs pass twice with the new actual helper submission captured
  by the geometry boundary. This older white-vertex raster proof remains
  separate from the new full-normal original and actual Metal comparisons.

FireSwarm's original software raster rejects early triangles with edges above
640px X or 480px Y even in larger viewports (`56d9fd..56da7f -> 56dba7`).
The first selected native phases are therefore empty; Metal retains the
hardware-visible cylinder. Later cylinder/ring phases and natural expiry are
comparable, but this does not certify the early hardware/device appearance.
Native fixed-point triangle seams and destination RGB565 precision remain
visible in later frames. Streamer sound/poison-cure caller behavior and complete
spell contexts are still separate. None of these stationary effects is a
point-to-point projectile.

Reproduce original samples with `mesh_leaf_visual_reference.py`; reproduce
fixed-frame comparisons with `port_capture.py` and retained scenarios, then
freeze them with `mesh_leaf_visual_review.py`. Licensed images and manifests
stay private; only these probes, source changes, and this compact record ship.
