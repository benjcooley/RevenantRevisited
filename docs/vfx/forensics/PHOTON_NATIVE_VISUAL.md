# Photon moving leaf: original software and actual Metal

The former stationary preview used a timed state switch, a guessed billboard,
synthetic point light, and five horizontal cells of one texture. The replacement
uses the shared integer missile controller (speed 16), the original 32-slot
animator, the authored 4-vertex/2-face quad, and all five distinct 64x64 texture
frames. Its existing strict ARGB4444 software policy is unchanged. No renderer,
shader, asset, geometry, tint, or camera fitting was introduced.

`TPhotonEffect_AUTHORED_FLIGHT` uses Initialize's actual `ready=1`, then Pulse
before Animate. `TPhotonEffect_AUTHORED_CHARGE` explicitly performs one Animate
before its first Pulse, clearing ready and allowing the charge to complete.
Both aim from the supplied origin toward X+1000 and run through range impact.
`SetProjectileEndpoints(source,destination)` exposes the same control before
launch. `Pulse` uses the same controller with actual map collision callbacks;
the builder is registered, but natural animator draw dispatch is not certified.

The native fixture executes original leaf Initialize 510400, Pulse 510450,
animator Initialize 5104c0, Animate 510600, Render 5109c0, RenderObject 40a8f0,
normal Illuminate and software raster 56eb30. It supplies explicit COM texture,
world, audio and command-reaper boundaries. Texture frame changes switch real
ARGB4444 surface bytes. The reference never substitutes white vertex lighting.
Original owner 40e470/40e740 supplies world transforms. Source flags 0x48 replace
authored animation keys; the legacy-key loader is unchanged.

Validation:

- `photon_state_probe.py`: 53,244 exact scalar/binary32 checks across both
  101-tick sequences, including 32 complete slots, launch readiness and CRT RNG.
- `photon_geometry_probe.py`: 4,264 checks, 328 original draw packets; maximum
  world-coordinate difference 1.53e-5. Original vertex/UV/index bytes and all
  selected five-frame texture choices match the production quad helper.
- Four actual Metal films: 100 frames each at 24 Hz, one warmup frame. Flight has
  55 nonempty frames and charge 89. All 28 logged RNG controls match the original.
- Two 14-phase panel sets include charge/flight, impact, decay and empty tail.
  The same 512-square original-owner crop is applied to every image. Common
  world and explicit native-projector results are both retained.
- Private full build and Python compilation pass. No shared GPU policy changes.

Evidence root:
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha/`

- `photon-state/manifest.json`, `photon-geometry/manifest.json`
- `photon-native-{flight,charge}/trace.json`
- `photon-metal-freeze/{flight,charge}-{common,native}/manifest.json`
- `photon-review-final/{flight,charge}/{pair-manifest.json,whole-life.png,detail.png}`

Metal binary SHA256:
`50db29a5e1ebe5f1b6d233d3cc7ff84ef8b2b20c37d5f3cc40ca3900bc1f5a55`.
Photon asset SHA256:
`c06262a0ab7029ab8580ae71edfd31f0dcb98691269076cf2654447634516a11`.
Native rebuilt executable SHA256:
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

Limits: explicit source(-240,0,128), destination(760,0,128), no live character,
range impact, face 0, camera(0,0,0), ambient 38/255 and directional 1 native;
Metal scene ambient 32 white. Natural spell scheduling, damage/audio, original
command-reaper behavior, exact device framebuffer and arbitrary caller order
remain open. The collapsed controller retains positive-scale visual particles
until drained; original command-done writes are observed through an intercepted
boundary. Modern common-world/native projection and backend quantization
remain separate. This is a scoped leaf appearance proof, not natural-game or
full-pixel acceptance.
