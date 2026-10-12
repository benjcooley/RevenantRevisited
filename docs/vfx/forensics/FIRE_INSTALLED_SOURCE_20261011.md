# Fire: exact installed static path

The shipped `Fire` type `0x119f01e3` uses the default animator, one authored old-format quad/key and texture selector0. Its33 resident textures do not establish a running atlas clock. The older procedural scatter previews remain available by name for historical work; the retail effect reviewer now routes this exact ID to `Fire_INSTALLED_FACE0`, marked partial.

The new strict `firelegacykeys.h` body guard admits only the unchanged277,496-byte `Misc/fire.i3d` body, including all texture frames. The loader copies its actual8-byte key (rotationX=pi/2, translationY=5); previously this resource received the generic zero-key fallback. All other old-format keys retain their existing admission rules. The source factory uses the real class builder/owner, original geometry/material/texture0, actual NextFrame/Pulse and persistent lifecycle. Four named cardinal construction controls use0/64/128/192 before factory construction. There is no invented texture timer, geometry scale or brightness.

Root ASan/UBSan profile/key-copy checks pass551 rejects and9 pose fields against original409950. The logged actual source GPU matrices match all192 original cardinal matrix channels within1e-6. Existing Ribbon, Vortex, Smoke and Barrier loader contracts pass after their extraction fixtures declare the new Fire opt-in false. Build and31 review-admission/compiled lifecycle checks pass.

Four cardinal3-frame captures clean-exit with frame0 and zero RNG. Original0/128 are blank,64/192 each show one stationary sample. Source integer-grid0/64/128 are blank;192 shows one sample at the correct647/469 location with a one-bit channel residual. The earlier unsnapped64 sample differs in position and color. These are retained raster diagnostics, not visual approval. The integer grid is the already-audited original truncate-to-integer primitive policy used by CombatFlash; ordinary rendering is unchanged.

Private source checkpoint: `recon/retail_asm/runtime/effects/current-fire-installed-source-20261011/source-checkpoint.json`. Evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-fire-root-20261011/installed`.

Natural caller, animated-fire expectations, full original span/UV/alpha raster parity and exact pixels remain open. Scoped visual count stays171/176.

Final exact-size guard build repeats the three facing192 images byte-identically, with clean exit.

## Original cardinal texel observation

`fire_cardinal_raster_probe.py` replays each unchanged original draw through the complete transform/illumination/integer-span/ARGB4444 raster. Read-only texture hooks reproduce every pixel: facing64 reads the nonzero word0x4410 at texel38/6, yieldingRGB5650x1020 at631/462; facing192 reads0x4420 at35/7, yielding0x1040 at647/469. Facing0/128 sample only zero texels. The source uses the same vertices, matrices, endpoints and literal first texture; its fixed-point span versus GPU interpolation difference remains visible at this tiny scale. Changing brightness, texture coordinates, size or atlas timing would conceal that mismatch, so these inputs remain authored. No appearance credit is added.

Root private manifest: `/Users/benjamincooley/RevenantRetailLab/research/vfx-fire-root-20261011/cardinal-raster/manifest.json`.

The scrolling reviewer also exposes `Fire (legacy animated)` using the existing
TFireEffect scatter implementation and labels the installed path
`Fire (retail default)`. The legacy entry is supplementary and has no unchanged
retail appearance approval. It restores visibility of the animated conversion
for inspection without replacing the installed static/default reference. Filter
`--vfx-review-effects=Fire` includes both; `Fire (legacy animated)` selects the
animated recipe alone, while the exact retail ID selects the installed path.

User review2026-10-11 rejects the animated legacy viewer output asbroken. It was not a verifiedretailmatch and is excludedfrom the fullreviewer until corrected. Explicitdiagnostic selection remainsavailablewithanunverifiedlabel. PrimaryFireverification isreopened; oldstaticquad/texture0deviceproof is supporting evidence only, not completeanimatedFireacceptance.
