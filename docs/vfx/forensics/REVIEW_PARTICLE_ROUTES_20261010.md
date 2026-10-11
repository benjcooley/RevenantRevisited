# Centered review particle-route audit — 2026-10-10

All fifteen requested runtime identities render and evolve in actual Metal review
captures. No missing production hook was found in this group, so this audit adds
no admission bypass, brightness adjustment, or replacement geometry.

The nine static/camera profiles are Regeneration, SwiftStrike, fspray, YEnergy,
YEnergyLose, YAbsorb, nullifier, Ogrestrength, and trollblood. The six other profiles
are Speed, Quicksilver, Fmastery, Might, Immortalmight, and goldeffect. The catalogue
selector is `goldeffect`, not the shorthand `Gold`.

Each film contains sixty 0.1-second samples at 1024×768, seed 1, one warmup frame,
`--vfx-review-first --vfx-review-speed=1`, normal bright review lighting, and the
unchanged fixture INI. Each selected type uses the normal map factory, animator,
controller Pulse, and maprenderer SubmitPartSys path. Logs confirm normal map
spawn and no preview fallback. Actual transient tiles remain resident; review
has ground drawing disabled. The fixed pixel-measurement rectangle
`(192,96)-(832,424)` excludes the title and effect-name label. Six-phase panels
were visually inspected for shape and evolution, as well as the pixel counts.

Twelve types have nonempty pixels in all sixty samples and sixty distinct ROI
images. YEnergyLose has 30 nonempty samples / 31 distinct ROI images; YAbsorb has
43 / 44; goldeffect has 49 / 49. Their visible decay, authored blank tails, and
subsequent respawn explain the empty samples. The small/dim fspray and trollblood
visuals are present and evolve; their labels are not the evidence.

This verifies centered review runtime visibility. It does not prove identical
retail framebuffers, synchronized isolated/reference timing, default-speed
scrolling visibility, or routes outside these fifteen identities. The earlier
native/isolated appearance records retain their own scope. Root's separate Warp
and scroll-controller review admission fixes are not credited to this audit.

## Frozen evidence

Private lab root:
`research/vfx-wide-20261010/hard-alpha/review-audit-centered/`.
`frozen-index.json` binds the fifteen commands, individual full-frame hashes,
logs, per-type evolution panels, pixel report, and contact sheet. Source is
private merge `63d8715`, equivalent to root `a6cdcbc`; the capture binary was frozen
throughout the batch. Later root guards and label suppression were merged only
as source after this binary was built.

- Frozen index SHA256: `09b5c77c2b313485a0dcc6b235c5b8423e319aa0b49bb0701dfe406932438432`.
- Capture binary SHA256: `e8a478ac427dae1c886383a29cee604447109a5d7acde6bc89502dd8155d6fbe`.
- 900/900 requested frames completed successfully across fifteen films.
- The actual review admission/dispatch ASan/UBSan regression
  `tools/retail_runtime/test_vfxreviewspawn.py` passes after merging root `09a1891`.

The initial `0.1` scroll argument was rejected by the old numeric-bound parser;
those zero-frame runs are excluded. Root `09a1891` fixes that parser independently.
