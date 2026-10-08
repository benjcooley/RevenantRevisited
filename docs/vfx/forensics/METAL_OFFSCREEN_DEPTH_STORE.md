# Metal offscreen depth preservation and read-only native probe

2026-10-05 checkpoint. **Root corrected a real backend contract defect: offscreen depth/stencil attachments were not explicitly stored for later LOAD passes. Build and teleportation runtime/pair complete. Root final isolated actual-map reviews pass teleportation, Warriorborn and Shadowfist. Native exact phase/cadence, natural spell/character/story/audio and full acceptance remain separate. Current Cure/Gold protection both finish240rawframes each, exact post-teleportation baselines; no full acceptance credit.**

## Exact backend change

`thirdparty/sokol/sokol_gfx.h` adds exactly three lines: one explanatory comment, offscreen `depthAttachment.storeAction = MTLStoreActionStore`, and matching stencilStore inside the existing depth-stencil conditional. Default/swapchain pass is untouched. Removing those lines reconstructs the preserved pristine baseline byte-for-byte. No shader/camera/effect geometry/controller/layout or retail raster approximation changes.

[CPU descriptor proof](/Users/benjamincooley/RevenantRetailLab/research/shared-projection-depth-20261005/metal-depth-store-proof.json) allocates a Foundation/Metal render-pass descriptor and reads SDK defaults:depthStore0/DontCare, stencilStore0, colorStore1. It creates no MTLDevice/encoder/commandbuffer and performs no GPU draw. Missing offscreenStore is source-contract evidence; actual GPU regression is separate below.

Baseline source SHA3194f5ce…/binary6987d828… and post-fix source00825564… are retained. [Build result](/Users/benjamincooley/RevenantRetailLab/research/metal-depth-store-20261005/build-result.json) exits0, binary `ab3e9405df46de1384446a13a7b0a67e8aa293f6fa4997d3e9f4ebc44e21f1bf`.

## Actual teleportation regression scope

[Loaded-floor runtime](/Users/benjamincooley/RevenantRetailLab/captures/runtime-fixtures/teleportation-depth-store-20261005/command-capture/manifest.json) completes400frames/10rows/50exactgroundtail. All50groundtail hashes match the prior6987run exactly;300activeimages change. [New own A/B](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-fps-teleportation-depth-store-actual-map-20261005/manifest.json) adopts240unaltered linked sourceframes60..299 after fixed2-second warmup, each side background0 against its own unchanged ground. No commonfloor/background substitution or phase/pose/scale/color fit.

Root final rawzoom/visual review now passes the isolated cylinder/rings and expected ground occlusion; the extra white base disappears. The older MISMATCH remains preserved. The [actual retail signedMAX per-triangle depth fact](RETAIL_SW_PROJECTION_DEPTH_LIMITS.md) stays valid as a software-reference limitation, but it is not asserted as the cause of this corrected Metal base. This correction restores the modern backend's own depth-preservation contract; it does not approximate software rasterization or establish full projector/cadence/nativefloor/context parity.

## Tiny read-only guest probe

A separate6144-byte `RETREAD.EXE` PE32/i386 OS/subsystem4.0 tool imports Kernel32 only and uses ReadProcessMemory for game-memory access. Root installed/runs it through the existing Lua/MCP guest runner without restarting native game/GUESTCTL.27host parser boundary checks and executable/source hashes are retained; guest execution is a separate successful result.

Initial job260 produced complete JSON but Lua's signed launchPID did not match unsigned helperPID, so observation timed out. The unsigned-normalized mandatory correlation fix retains that failed wrapper; jobs261/262 finish successfully. The EXE remains unchanged. [Probe instructions and limits](/Users/benjamincooley/RevenantRetailLab/research/retail-live-probe-20261005/README.md).

[Decoded initial globals](/Users/benjamincooley/RevenantRetailLab/research/retail-live-probe-20261005/live-initial-decoded.json) retain pid4294843377, softwareflag1, actual VIEW/PROJECTION, screen center320/240, rasterpointer0xd34670 and Zwrite0/Zenable1. Two32-byte reads at409430 and56e86b independently match unmodified original PE code. Reads are sequential/non-atomic. WORLD/TRANS after deletion may retain the last object: they are not a live-owner or sampled-floor-depth claim. No memory writes, input/focus, game restart or visual acceptance are implied.

[Pre-review integrity](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/metal-depth-store-checkpoint/integrity-pre-review.json) checks exact source delta/build,400runtimeframes,240linkedpairframes/media and probe rawcode/hash provenance. All three family rechecks/reviews and current-binary Cure/Gold protection are now terminal; current hashes follow. All176 rows remain mandatory unfinished in the [ledger](../EFFECT_BURNDOWN.md).

## Final isolated map review and exact before/after pixels

Current ab3e9405… builds/render passes retain identical authored keys, controllers, camera, geometry and floor. Root reviews **PASS** for teleportation, Warriorborn and Shadowfist across six raw samples each: overall form/scale/color/additive lobe envelope and real-floor occlusion, without phase/position/scale/brightness fitting. Native exact logical phase/cadence and natural spell/character/story/audio/full acceptance remain open.

[Final family hash verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/metal-depth-store-checkpoint/final-family-review-integrity.json) independently checks1200runtimeframes/30rows and720unaltered selectedpairframes/media/reviews. Each400-frame fixture passes10rows/50groundtail and each side backdrop is0 against its **own floor**. Compared with the retained prior400-frame captures, Teleportation changes300activeimages, Warriorborn330, **Shadowfist0**; all50groundtail hashes match exactly for each. Teleport's whitebase and Warrior's lowerextra rays are removed by restoring depth preservation. Shadow's pixels were already comparable: its pass is a scoped review of unchanged output, not a claimed image delta.

These actual integration results isolate the source/backendStore change and observed occlusion correction beyond the CPU SDK-default proof. They do not establish every native depth/raster behavior or justify replacing the modern default with retail's signedMAX triangle approximation. The historical pre-fix mismatch/inconclusive reviews remain intact.

[Additional read-only probe integrity](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/metal-depth-store-checkpoint/probe-additional-integrity.json) retains successful engine/floor rawJSON. Only the first64proven raster-header bytes have field provenance; engine+0x100 is adjacent heap, not established fields. Header-derived FloorRaw pixel-address meaning has not been confirmed against footpix, so its raw samples are not physical floor-depth proof. WORLD after deletion remains cached rather than a live-owner claim.

## Final terminal protection and acceptance scope

[Root final integration](/Users/benjamincooley/RevenantRetailLab/research/metal-depth-store-20261005/final-integration.json) and [protection](/Users/benjamincooley/RevenantRetailLab/research/metal-depth-store-20261005/regression-protection.json) are complete. Both Cure240rawframes and Gold240rawframes exactly match post-teleportation baselines on currentab3e. [Final immutable verification](/Users/benjamincooley/RevenantRetailLab/research/ledger-reconciliation-20261005/metal-depth-store-checkpoint/integrity-final.json) includes1200runtimeframes/30rows,720selected linkedpairframes and480protectionframes/media plus current reviews/contact hashes. Pre-review report is retained historically; its older pair metadata hashes are superseded by this final current report after contacts/reviews were added.

These three scoped appearance passes bring isolated visual coverage10→13. Same effects already had source/runtime/reference/pair rows, so those counts do not increase. Full0/176 remains: native exact phases/cadence/fullcycles, source/fractional requirements and natural spell/character/story/audio/context gates remain independent. The shared contract fix supplies observed port occlusion causality without proving retail software/modern raster pixel identity.
