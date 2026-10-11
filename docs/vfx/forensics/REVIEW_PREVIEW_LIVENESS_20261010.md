# Review preview mapping and lifetime audit — 2026-10-10

Source fix: `2d882a4`, on the integrated `09a1891` review runtime. This is a gallery runtime/visibility correction, not new retail visual acceptance.

The exact source registry supplied eleven mappings missing from stale ledger `preview_candidates`: Ogrestrength, trollblood, Regeneration, SwiftStrike, fspray, YEnergy, YEnergyLose, YAbsorb, nullifier, Speed, and Quicksilver. The generator records the exact retail IDs and factory IDs; it does not infer associations from names/assets. The audited map has 100 candidate rows, 91 unique retail IDs, and four explicitly blocked endpoint-dependent IDs (87 unblocked mapped identities, subject to actual catalogue/API availability). Placeholder exclusions, including Blast, remain in force.

Twenty-three registration sites now expose real liveness: thirteen opaque owner adapters test the retained owner identity and `OF_KILL`, and FireSwarm, Streamer, and MeteorStorm report their implementation's `IsAlive()`. The bridge recreates a completed owner before the next submission. Unknown liveness retains the existing browser cadence; persistent live Static entries are not reset. This supplies no invented lifetime and does not change the isolated browser's one-shot behavior.

A 20-selection centered actual gallery survey used `--headless --test=vfx-review --vfx-review-first --vfx-review-speed=0.1 --resolution=640x480 --vfx-no-ui --filmstrip=40,0.1 --snapstep=0.1 --snapwarmup=1 --snapseed=1`, private SAVE and the fountain-command-20261004 data fixture. Raw full-frame pixels, not labelled filmstrip composites, were measured. All runs exited cleanly. Nineteen implemented selections produced effect pixels; Blast remained exactly black in all 40 frames. Per selection, nonempty frames / distinct raw frames:

- TeleportDoorInsideB 40/16; speaker 40/1; LabGateBarrierS 40/1.
- OlihootSign 40/1; RibbonB 40/1; Aura 40/40; CyanFont 40/40.
- Fizzle 32/33; FireSwarm 39/33; Streamer 36/36; FireFlash 29/29; FireCone 37/38.
- MeteorStorm 40/40; MPAppear 39/38; Speed 40/38; Quicksilver 40/39.
- Regeneration 40/40; SwiftStrike 40/40; fspray 40/40; Blast 0/1.

The Warp, speaker and barrier restoration belongs to root's separate verified map-controller admission fix. Their prior centered captures had no effect pixels. Static assets correctly have one distinct image at this near-stationary camera rate; animation is not inferred from nonemptiness.

A normal approach control omitted `--vfx-review-first`, used speed 48, and captured 40 frames at 0.5-second intervals with 0.1-second simulation steps. Before the fix, MeteorStorm exhausted its actual particle state near the center and MPAppear completed before reaching it. Counting pixels with `max(R,G,B)-min(R,G,B)>32` excludes the old baseline's white labels: the last ten MeteorStorm samples changed from 0 to 9 colored frames, and MPAppear from 0 to 10. Early colored samples match before the first restart. The corrected label-free complete runs contain 17 and 32 nonempty frames respectively, including initial offscreen padding.

Six fresh isolated controls use the same exact registered implementations: OlihootSign, RibbonB, Aura, MeteorStorm, MPAppear, and Speed. Their visible forms and actual animation are present in both isolated and gallery captures. These are presence/route controls, not pixel-equivalence claims: the gallery uses map projection/lighting, and the isolated Aura browser additionally supplies its character rig. No rig or attachment is invented by the gallery bridge.

Validation: complete private Revenant build; `tools/vfx/test_review_preview_map.py` (four tests); standalone `tools/test_vfx_review_preview.cpp` linked with bridge and time implementation. The finite-owner test verifies recreation only after reported death, while existing tests retain unknown Static instances and reject placeholders/unsupported endpoint/attachment APIs.

Private evidence root: `/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/`. Frozen binary SHA-256: `6595bf6223ec3c64148ffcadabf412cde01d6943579f8097770fd38d7ca4b7b9`. Per-run commands, routes, hashes and metrics are in `review-survey-final`, `review-isolated-final`, and `review-leadin-{baseline,final}`. Aggregate `review-runtime-audit.json` SHA-256: `ae24a7d3d156458009f9bec1268aa2459b8ecb0e1285d5e4137095fbbdf8276e`. Review panel: `review-isolated-vs-gallery.png`. Earlier mixed centered/approach directories for MPAppear/PunchAndJudy/Shadowfist/Warriorborn are excluded; the separate frozen lead-in baseline directories used here contain exactly 40 frames each. Assets, binaries, captures and generated reports remain outside source control.
