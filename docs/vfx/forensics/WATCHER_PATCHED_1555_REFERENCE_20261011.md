# Watchers: explicitly patched original ARGB1555 reference

The three installed Watcher assets contain correct ARGB1555 textures. The
unchanged original software renderer misroutes them to an ARGB4444 decoder;
its rainbow output is retained as a failed reference. The previous qualified
1555→4444 upload adaptation also remains diagnostic: it loses one RGB bit, and
the original conversion maps opaque alpha to 8. Neither is used to tune the port.

`watcher_retail_1555_patch.py` builds a separate, explicitly modified retail PE
from the pinned local original. It checks every original instruction and adds
one `.w1555` section. A patch after original SetTexture's GetPixelFormat records
whether all five bit-count/channel fields are exactly 16/7c00/03e0/001f/8000.
Only then do the two scan halves of the actual Watcher 54ff60 leaf decode 1555.
Other formats execute the displaced original 4444 decode instructions.

RGB5 red/blue remain exact. Green5 expands to green6 by high-bit replication,
equivalent to rounding its full normalized range. Binary alpha supplies exact
zero or one to the existing integer blend arithmetic. There is no texture
rewrite or RGB4 reduction. The actual original loaded resources, frame
selection, matrices, geometry, UV selection, scan conversion and remaining
blend loop execute unchanged. This is a semantic-format repair, not a fitted
color adjustment or a claim about the unmodified device.

The qualified executable is local-only, SHA256
`e3e5fa7ef5841e70dc6855e492595ca47cda03cc414a832eb4c1c6be697f76fa`.
Its added section SHA256 is
`7f4e2a3ca557d3437101a4bb0afd4b7c183f9acbe45ce3c91bb795ccd40e0a9c`.
No executable, assembly dump or licensed image is committed. The source recipe
writes its exact assembly and complete patch manifest beside each local build.

`watcher_patched_reference.py` restores the source-matched unmodified startup
cache, then maps the exact patched PE bytes and added section before scene or
resource calls. This declares the startup replay boundary; it is not an
independent whole-game boot test of the modified PE. Full original loaded
rendering thereafter produces all 24 frames with unchanged 1555 descriptors.
A fresh independent run produces identical 24 PNGs. No host pixel conversion,
replacement vertex color, material tint, frame alignment or geometry is used.

`watcher_1555_patch_proof.py` executes both assembled x86 decode branches for
all 65,536 words under both format policies: 524,288 channel assertions pass.
Five independently changed format fields reject the opt-in. Eight literal
texels run the actual original depth-tested raster: transparent words preserve
the destination, opaque words preserve full RGB5. Actual original RGB565 and
ARGB4444 color/depth images are byte-identical before/after patch. The proof
explicitly invalidates Unicorn's translated code cache when replacing already
executed instructions. A retained unittest rebuilds twice and runs these
controls. The earlier literal test without depth testing reached a different,
unpatched raster and is retained as a rejected configuration diagnostic.

Three fresh actual Metal films use the unchanged correct source decoder and
native-owner/projector preview: each has 8/8 visible frames and four authored
phases, clean exit. Every source frame is identical to its earlier unmodified
native-owner film. Current private source binary SHA256 is
`afdf4e3e35e8602b4a2af9bec0014ed970eb5bb2ff89ff7157da6d500d8ea739`.
The fixed 640x480, camera 320/240, origin0, ambient 38/directional 1 comparison is
frozen under `hard-alpha/watcher-patched-review-final`. All eight raw phases
and whole/detail panels are pinned (66 file hashes verified); detail uses the same crop 250/70/390/260
on both sides and nearest 2x display only. Pair manifest SHA256:
`2bd540dabc473c1acbca1ffe71af60ab39b9f5f71e2cf4f7afff9a15b05f0db9`.

Remaining limits: approximately 1–2 pixel sampling/edge differences; original
scan conversion versus Metal filtering/framebuffer precision; generic owner
world Pulse and natural map placement; independent whole-game patched boot;
other primitive/blend raster branches. Unchanged-retail/device-identical and
exact-pixel claims remain false. This package requests independent approval
only for the explicitly modified-original semantic1555 reference configuration.
No engine source or decoder modification was required.

## Reproduce

Use the laboratory Python environment and `PYTHONPATH=tools/retail_runtime:tools/retail_asm`.
Build with `watcher_retail_1555_patch.py --executable ORIGINAL_EXE --output NEW_BUILD`.
Run `watcher_1555_patch_proof.py --executable ORIGINAL_EXE --patch NEW_BUILD/patch-manifest.json --output NEW_PROOF`.
Run `watcher_patched_reference.py --boot-image ORIGINAL_CACHE --patch NEW_BUILD/patch-manifest.json --output NEW_REFERENCE`.
The whole comparison collector is `watcher_patched_visual_review.py --root LAB_HARD_ALPHA --output NEW_REVIEW`.
The local build, proof and reference folders are respectively
`watcher-patched-build-v2`, `watcher-patch-proof-qualified-ztest`, and
`watcher-patched-reference-v2` / `watcher-patched-reference-repeat` under
`/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-alpha`.
