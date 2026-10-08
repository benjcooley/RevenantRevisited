# Bounded retail RGB565 blend audit

Purpose: decide whether FF's orange-annulus versus filled-cloud mismatch and
Cone's brighter core justify changing software helper blending. Result:
**a blanket overwrite/color-key-zero replacement is not supported**. Actual
retail RGB565 rendering has an additive destination-read kernel and selects it
when the destination blend state is ONE. No brightness fitting is recommended.

## Actual retail proof

Original executable SHA256
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
The compatibility executable's audited instruction ranges are byte-identical.

1. Actual SetRenderState is `0x56d400`. Decoding its byte/jump tables maps
   enum 20 (DESTBLEND) to `0x56d44c`, which stores the supplied value at
   `0x675f10` (`0x56d452`). Enum 19 (SRCBLEND) has a different storage address.
2. Actual RGB dispatch at `0x56e2a3` loads **that exact destination variable**
   and compares it with 2, D3DBLEND_ONE. Equality selects the additive branch;
   non-equality selects the other raster wrappers. It is incorrect to assume
   the stored destination state is unused based on the older source wrapper.
3. With depth test on and depth writes off, the additive branch calls
   `0x54e3a0` at `0x56e58b`. Its RGB565 path reaches kernel `0x552c10`
   through `0x54e4ae` (with separate equal-light/format fast paths).
4. Inside that actual write-off pixel loop, source texture is sampled at
   `0x5533b7` and modulated via channel tables. The existing framebuffer word
   is explicitly read at `0x55343a`. Source and destination carry-table entries
   are combined at `0x553448..0x553453`, the result is mapped through the
   saturation table at `0x553455`, then stored back at `0x553460`. This is
   destination addition, not overwrite.
5. A separate direct RGB565 kernel `0x558400` exists. Its audited loop stores
   the source-table result at `0x558b5a` without reading destination color.
   Existence of that kernel does not establish its selection for effects
   whose renderer sets destination ONE.

The equal-light RGB565 write-off fast path is also checked: call `0x54e434`
reaches `0x5605b0`; it reads destination at `0x560d0b`, adds carry-table values
at `0x560d28`, and writes the result at `0x560d39` (with the corresponding
second scan span at `0x561176..0x5611a1`). Thus a planar quad's constant vertex
light is not evidence for bypassing addition either.

The audited inner loops contain no explicit zero-texture color-key branch
between source sample and output store. In the additive path, black-source
behavior follows the modulation/carry tables; it must not be replaced with a
guessed opaque-alpha/key-zero rule. In the direct path, the inspected store is
not guarded by a source-zero test. Complete table contents/rounding and all
other format/depth fast paths were not exhaustively emulated in this audit.

## Older library evidence is distinct

The named snapshot Release BlueRev.lib function
`DrawTextureAndModulationGoraudZbufferWOff` has direct output stores at offsets
`0x849` and `0xc64`, and no destination color read or explicit zero-key branch
in those inspected loops. Its code blocks do **not** byte-match the retail
executable. The actual retail destination-state selector and additive kernel
therefore take precedence over that older library association.

The source `legacy/blue/swscene.cpp` records SRC/DST states, while its snapshot
DrawIndexedPrimitive does not select the later retail blend wrappers in the
same way. Neither that source omission nor the nonmatching library can justify
changing the current reference-mode blend policy to overwrite.

## Action and remaining scope

Retain software helper mode one's sourced texture/vertex-light modulation.
For a caller with proven destination ONE, retain additive submission. Mode
zero/hardware material behavior and stored material inputs should remain
unchanged. A different explicit destination state would require its actual
retail branch to be mapped and tested before adding a distinct backend policy.

FF/Cone's outstanding appearance is not cleared by this audit. The actual
blend trace rules out the proposed blanket overwrite/key-zero correction; it
does not prove every LUT rounding, clipping/coverage, equal-light fast path or
captured device state. Preserve the failed comparisons and those concrete
raster/version/state gates rather than fit intensity, alpha, geometry or phase.

Executable/library hashes, decoded render-state table, exact address chain and
retained disassemblies are in
`/Users/benjamincooley/RevenantRetailLab/research/rgb565-blend-audit/audit.json`.
No engine edits, builds, render processes or guest input were performed.
