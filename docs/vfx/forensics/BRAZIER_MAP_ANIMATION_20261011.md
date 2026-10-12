# Nakranoth chamber brazier animation

User confirmed the named corrected reviewer works on2026-10-11. Root's
30-frame standalone check has15 distinct pictures and a clean exit. The final
integrated build also passes the unchanged chamber20-frame/15-picture capture
and initializes the174-entry normal reviewer, including TwnBrazier.

The tall yellow flame the user identified is `TILE/TwnBrazier`, ID
`0x70dc006a`, imagery `Town/TwnBrazier.I2D`. It is outside the176 shipped
EFFECT rows. The original chamber pairs these with static
`DunCrnTorch` tiles (`0x7d8505e5`). This is separate from both base `Flame`
and the legacy custom `Fire` effect.

The original saved map instance at (9678,9614,259), map index1019174967,
has15 animation frames and frame increment1. The proper SOFTWARE3D original
scene capture runs20 original screen ticks, records each real owner frame,
and confirms increment1 with wrapping modulo15. It draws the tall flame in
the actual chamber. Original bitmap asset SHA256:
`a6cb922e8d4b16c01bcc752fe7ce65d37203a24f8cbb1801b9e01bd1fe53f73c`.

## Defect and repair

The port map renderer sampled animation frames only for EFFECT objects.
Ordinary TILE records always sampled the state's still bitmap. The source
`TAnimImagery::DrawLit/DrawUnlit` draws the still, while
`TAnimAnimator::Animate(draw)` draws the authored current animation separately,
using the animation registration minus each frame bitmap registration. Retail
owner `GetStillImage` (vtable+0x12c, original46f160) remains a state-still query;
replacing every tile still with an animation frame would remove static bases.

The repair adds a separate animation-layer draw record for animated I2D
objects. It preserves the original still/base record, selects the real current
frame, and uses the authored per-frame registration. The layer is self-lit,
as the original animation draws after background lighting; existing tile
submission preserves bitmap alpha and depth. It samples frames without advancing
them. Inactive, missing-animator and completed ordinary animation layers do
not draw, matching the original `draw && !complete` gate. EFFECT handling keeps
its existing independent path.

Both layers retain their own cached bitmap reference. Movement/bin refreshes
preserve the previous sampled tile state/frame until texture refresh runs, so
moving an object cannot hide a changed animation frame behind a stale cache
key. Changing to a state without a still/animation clears the corresponding
asset index. Empty animation records are safe in bounds, depth and submit paths.
A readonly `TObjectAnimator::IsComplete()` accessor supplies the completion gate.

`SetReviewVisibleTileTypes()` allows explicitly selected TILE IDs through the
reviewer's hidden-ground filter; the class check precedes the ID check. Normal
map tile visibility is unchanged. The root agent added the named TwnBrazier
catalogue row and exact original TILE factory dispatch separately.

## Verification

The real unchanged Nakranoth map was captured at camera(49,9678,9614,259),
with the normal map factory, no spawns/asset replacements/frame edits, and
24Hz map ticks. Before the repair all20 PNG pixel buffers were identical and
the tall animated flames were absent. Afterward there are15 distinct pictures;
each of the five samples separated by15 ticks is pixel-identical. Readonly
trace records prove both layers exist for the exact original instance, its
frame advances once per legacy tick, and the static base asset remains fixed.
The still registration is (20,105,318); the animation registration is
(13,204,56). Normal original factory creation also works in the black
scrolling reviewer, with its visible TwnBrazier label and original stone base.

The ASan/UBSan source contract executes the actual bitmap selection, cache
refresh, layer visibility, reference accounting and transfer refresh functions
for45 ticks. It checks three loops, static base preservation, per-frame
registration, cache reuse, completed/inactive layers, missing animation states,
restoration, and movement without falsely marking an old bitmap current.
The private Revenant build and this contract pass.

Private evidence under `RevenantRetailLab/research`:

- `vfx-natural-software3d-root-20261011/brazier-cycle/manifest.json`: original
  unchanged chamber20-tick reference, SHA256
  `3305fcaa8b2e026a25cf6221179491e41bdb177c3418d070e99be30083117228`.
- `vfx-brazier-animation-agent-20261011/proof-final.json`: source/native/runtime
  evidence summary, SHA256
  `7a99637e9bf346e420a6d359027122782b93be3de01b56b8bc903df8d0178ccd`.
- `vfx-brazier-animation-agent-20261011/final-verified/manifest.json`: fixed
  actual map capture, SHA256
  `744d0a2fe6ab2da7ea53d5a8172ab3ef30fa80bab18fc6c1067689d5592a1390`.
- `vfx-brazier-animation-agent-20261011/reviewer/`: named black reviewer20-frame
  capture. Frame006 shows the full tall flame and stone base.

The frozen fixed executable SHA256 is
`cd49b654ffc1882dfc67560cc89cdde4bdd210a549a9ee543dcbfcedd7385d3a`.
The baseline executable is the earlier frozen source endpoint build, SHA256
`9875275ea0a1c820e1de8f63c1655dd7d259af70635714f8f8b8001bc3ed808a`.
All licensed assets, executables and rendered evidence remain outside Git.

## Reproduction

Use the runtime Python environment with
`PYTHONPATH=tools/retail_runtime:tools/retail_asm`:

```sh
python tools/retail_runtime/brazier_map_capture.py \
  --binary PRIVATE/Revenant-brazier-final --data-root data \
  --output PRIVATE/new-map-capture --expect-animation yes
python -m unittest tools.retail_runtime.test_brazier_map_animation
```

The capture tool runs only its owned Mac process, checks the typed original
map instance, records exact executable/asset/source/log/PNG hashes, and verifies
both layers,15-frame picture repetition and the unchanged still registration.
For an archived pre-fix executable, use `--expect-animation no` in a fresh output
directory. `working_tree_map_renderer_sha256` identifies the source observed at
capture time; the frozen executable hash remains authoritative for the code
actually running.

```sh
Revenant --test=vfx-review --vfx-review-effects=TwnBrazier --vfx-review-first
```

This proves recovery of the missing original map animation and its authored
frame progression. Full appearance/pixel, device, audio and real wall-clock
acceptance remain separate. Original and port captures use different explicit
cameras, so no fitted screen alignment or framebuffer equality is claimed.
The original emulator delivered its41ms WinMM ticks; the port uses its intended
24Hz legacy clock. This does not resolve the separate Ogrok natural-cadence
question or certify every possible finite-animation state transition.
