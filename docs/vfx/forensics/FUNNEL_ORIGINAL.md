# Funnel: original moving owner and authored controllers

Exact type `funnel` / `0xad92bc17`, `Imagery/Magic/funnel.i3d`, SHA-256
`8efb1a165196d79a648b1cf9bdf4635088709eec181d072067516b24599ab341`.
`Funnel_ORIGINAL` preserves its three authored states (10/30/10 frames), four
normal-lit base meshes, two particle controllers, actual movement and owner
kill. The existing bespoke preview remains available.

Original registration `4fd3d0` and lookup `40dca0` select builder `66cf70`,
factory `500690`, animator vtable `5ae87c`. Owner Initialize/Pulse are
`4fd3f0/4fd400`; animator initialization, Pulse, Animate and full Render execute
`4fd7e0`, `40e2e0`, `40e460`, `4fd800` and the default `40e8d0`. Original
NextFrame, SetState, RenderObject, normal lighting and software raster execute.
The pinned rebuilt executable SHA-256 is
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.

The declared fixture supplies an invoker facing 64, no live damage target,
source `(-180,0,78)`, and ground height one before X=180, zero beyond it. Owner
movement is the original four units per Pulse. Its terrain-impact branch
**requires spell data**: the original dereferences the spell unconditionally
there. The fixture provides explicit zero damage descriptors and records the
actual `4de3c0` call; it does not claim damage or natural actor behavior.

The original reaches X=180 on pulse100, calls the terrain damage boundary on
pulse101, and sets its counter to300. It continues moving until its current
30-frame cycle completes, enters shrink on pulse130 at X=296, and executes the
real kill on pulse140. The adapter follows these branches; it does not replace
this cycle boundary with immediate impact or an invented lifetime. Its
`is_alive` registration observes the owner's kill flag. `Spawn(source,target)`
uses the target to orient the explicit zero-height terrain boundary. This is
a controlled world fixture, not arbitrary terrain or natural spell dispatch.

Strict admission checks all eight objects, three states, keys, faces, normals,
UVs, five materials, five RGB565 texture formats/dimensions and seven literal
tags. The source key/matrix proof checks 3,600 key fields and 6,304 matrix fields,
with maximum errors below `2.5e-7`; invalid profile mutations reject. Existing
non-inclusive key behavior remains unchanged. Both emitter poses use the last
Animator Animate frame even across multiple Pulses without a Render.

The independent production PartSys comparison checks 108,490 fields in the
145-tick full-render sequence and 108,612 fields with every-third-tick rendering.
All RNG seeds match; maximum numeric difference is below `6.1e-5`. The original
full Render's sampled particle matrices and packed colors also agree with the
production RenderSample and camera-matrix path. The actual production owner
Advance and terrain-query body matches every declared owner state, frame,
position, counter and kill result. Source simulation inputs and emitted render
matrices have separate proofs; no fabricated color carrier feeds the native
reference.

Native draw tracing selects ONE/ONE, CULL_NONE and no depth writes for the four
base meshes. Reversing all original base triangles produces the same 3,581
nonzero pixels and identical native image, confirming the existing cull-none
pipeline. The exact RGB565 software particle policy is admitted only for this
validated type. No renderer, shader or generic parser behavior changes.
Four stable helper mesh keys avoid per-life resource growth.

The matched comparison uses 1280×960, center640,640, ambient38white plus
directional1 in original software, and ambient32white under the existing port
software-light policy. Native-domain depth is0..65535. Both actual Metal films
render every24Hz tick and save37 images at four-tick intervals, with clean exit.
Fourteen logged RNG milestones agree. All declared original/native-domain
sample bounds lie inside the complete viewport; the fixed detail crop is only
supplemental.

CSV timing is authoritative: one-based screenshot N occurs at pulse `4N-2`,
matching native record `4N-3`. An early quick panel used incorrect filename
arithmetic and is retained as rejected evidence. Correcting the pairing
resolved its apparent core-brightness and moving-wisp differences without any
source color, timing, geometry or scale change.

A fresh native run reproduces selected21/101/133 PNGs byte for byte and352,172
state/pool checks. Two particle resource-pointer words (`+d8/+dc`) shift16 bytes
because the explicit registry lookup adds a name allocation; only those pointer
identities are excluded. All numeric pool bytes, colors, matrices, RNG and
owner fields remain exact.

Private evidence is under RevenantRetailLab
`research/vfx-wide-20261010/hard-alpha/funnel-*`, with the matched review in
`funnel-review-matched`. Frozen Metal binary SHA-256:
`95b78799c9d97e893a5a30c11aca1dae75c978a490f5d231174011801b631b35`.
Decoded resource/COM construction, base animator initialization, silent audio,
map/script notifications and damage are explicit boundaries. Natural callers,
arbitrary actor/terrain inputs, device behavior, texture sampling, destination
blend quantization and exact framebuffer equality remain open.

Root independently approved the scoped native-domain appearance after reviewing
all 19 matched phases and actual-size phases21/65/101/133: moving coil, wisps,
sparks, declared terrain transition, shrink and the real pulse140 kill/blank
tail agree. This approval uses source(-180,0,78), target void-plane180 and the
explicit spell/ground1→0 fixture; it does not extend the caller/device/pixel
claims above. Correct CSV pairing and the rejected early phase panel remain
explicit controls. The private `funnel-review-matched/root-approval.json` pins
the cases, pair manifest, reviewed panels and captured binary. Approval SHA-256:
`38ab6a80ecdc8387fbd2da7fdec8ed10a59248eb8a0e3bbfceb0e585b9c3a60c`. Source `d678cfe8` was integrated by root as `134b72fb`.
