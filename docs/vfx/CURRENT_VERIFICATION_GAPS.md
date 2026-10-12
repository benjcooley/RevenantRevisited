# VFX verification after the scoped appearance milestone

Updated 2026-10-11. All176 shipped EFFECT type rows have a current scoped
visual review. A local audit verifies every review's evidence hash, including
older nested rechecks. This does not prove every caller, state or rendering
configuration. The active conversion/verification goal remains open.

Use [the JSON ledger](EFFECT_BURNDOWN.json) for each approved configuration and
[the emulator handoff](../RETAIL_AB_TESTING.md) for new comparisons. All new VFX
validation uses the in-process emulator; DOSBox-X is excluded. Generated films,
licensed resources and private evidence stay outside Git.

## Active behavior and context work

- [ ] Generic effect owner and real Strike spell binding. Current
  `TEffect::Pulse` lacks retail follow-object, start-delay, attachment countdown,
  light and timed-life branches. `TStrikeSpell` also lacks the original Timer,
  Pulse and Kill overrides that configure them. A genuine shipped Regeneration
  creator is the first reference: actual Follow binds the invoker, wait5,
  lifetime/attachment2880 and blink24. Compare real invoker movement, start,
  light progression, expiration and cleanup before implementing the branches.
  `SetSpell` alone does not establish a follow binding.
- [ ] WaterFlft in the saved Pit scene. Genuine retail startup now loads the
  exact saved WaterFlft and two WaterClft neighbors and executes their default
  controller renders. Match the measured452×420 viewport and world camera,
  correlate original particle packets with the saved owners, then compare
  bounded phases with unchanged surrounding geometry/lights. Native wall
  lighting noise still needs a causal check; residency alone is not appearance.
- [ ] Halo raster control. Both genuine public configurations have original
  and actual GPU2×2 integer-grid/source-contribution controls. Any source change
  needs strict admission, unchanged ordinary-mode controls and fresh visual
  review. Remaining fixed-point/one-pixel differences remain explicit.

## Concrete next behavior gaps

- [ ] CombatFlash states1–17. The current production PartSys admission accepts
  only state0/start1. Execute the original state selector and public combat
  caller, then compare each supported state; the start1 pass does not cover them.
- [ ] Gold full-cycle terrain/collision cadence. Its scoped spray appearance
  passes, but retained native last42/43 versus port45 timing and sectorless
  floor0 controls do not establish the complete ground/bounce/end sequence.
  Compare the actual loaded factory, emitter-cache startup, terrain queries and
  natural completion before changing timing.

## Retained qualifications

Three Watchers, Rift, Labyrinth and restored Sandswirl use explicit modified
retail references; Dust and static Fire use qualified semantic devices. These
eight primary reviews do not establish unchanged-device fidelity. Sandswirl's
own Sand binding is evidenced in legacy source and its shipped asset is
byte-identical, but no enabled natural caller was found in the finite module,
script and spell audit. Its stationary and moved-target controls remain explicit
qualified configurations.

Natural triggers, actor attachments, audio, additional parameters and exact
fixed-point raster/device behavior remain separate per-row gaps. The earlier
all-gates acceptance count is0; do not promote it from visual reviews or use it
to erase the176 scoped appearance passes. Reopen the affected current review
after any behavior-changing fix until current port output is compared again.
