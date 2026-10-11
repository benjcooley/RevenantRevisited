# Gallery runtime audit, 2026-10-10

The full 176-entry actual Metal sweep produced 108 visible entries, 64 unavailable
entries and 4 empty short captures, with 0 capture failures. Every 81 scoped visual
pass was admitted; none was unavailable. These are presence diagnostics, not
proof that every visible entry has correct animation, motion or retail fidelity.

Six captures per entry cover the first 21 simulation ticks, with a private save
folder per process, 640 x 480, 24 Hz, seed 1, warmup 1 and labels suppressed. The binary
was copied before the sweep, so subsequent builds did not modify the tested
executable. The original immutable manifest pins its binary and frame hashes.

The empty entries were Drip, Fog, jtele and jteled. A 40-capture follow-up spans 158
ticks: Drip emits in 8 frames; Fog, jtele and jteled remain empty. Source inspection
finds Fog's unconverted preview collapses its 36-vertex grid into one averaged
billboard and resolves no texture. Its fallback is now excluded. The two jtele
assets combined scrolling texture admission with an unsupported particle
controller. The review gate now rejects that incomplete controller instead of
presenting its geometry as a working effect. All three retain their named slots.

Integrated authored water repeats produce WaterFlft 40/40, WaterFrt 40/40,
WaterClft 40/40 andWaterCrt 39/40 visible frames. Real Photon runs charge, moving
flight, target collision and decay across repeated cycles (31/40 visible); Heal
runs its authored bubble/cylinder lifecycle (38/40 visible). Separate original
software comparisons approve their recorded native-domain configurations.

Private manifests, indexed with frozen hashes in EVIDENCE_INDEX.json:

- `recon/retail_asm/runtime/effects/current-gallery-runtime-20261010/full-catalogue-current.json`
- `recon/retail_asm/runtime/effects/current-gallery-runtime-20261010/water-and-delayed-current.json`
- `recon/retail_asm/runtime/effects/current-gallery-runtime-20261010/teleport-delayed-current.json`

Reproduce a presence sweep with `tools/vfx/review_capture_survey.py`; pass the
actual binary, data root and a new private output directory. Use 40 frames for
slow births or finite effects. Review full animation and an original retail
comparison before granting visual credit. Images, assets and binaries stay
outside the source repository.
