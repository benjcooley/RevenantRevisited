# Genuine Regeneration spell owner controls

The prior generic effect Pulse handled animation completion but omitted the
spell-owned follow, delayed-start, light ramp and timed blinking branches.
The original `TEffect::SetSpell` at `4de7d0` binds `+d8` and resets the light
counters; it does **not** bind the followed object at `+e4`.

The original loaded SpellList has 74 blocks / 99 variants. Among the audited
Speed, Quicksilver, Fmastery, Might, Immortalmight, Ogrestrength, trollblood,
SwiftStrike and Regeneration names, only Regeneration has a shipped variant
binding. This change admits only its exact `Strike` control: one owner,
duration 2880, blink 24, WAIT 0, direct FOLLOW, zero position offset, and no
bone, pattern, damage, poison, sound or imagery override. Other Strike controls
retain the previous base Timer/Kill and empty Pulse behavior. Null-spell
animation completion remains unchanged.

The genuine reference executes the Acolyte constructors, Fighting target,
SpellList, Strike creator `542290`, resident empty sectors, Strike Pulse
`540d70` / Timer `540eb0`, and owner NextFrame / Pulse `4de800`. The owner is
created at step 5. Moving the invoker at step 8 moves the owner; moving the
separate Fighting target does not. The light ramps 32, 64, 96, 128, 160. The
24-step blink follows the original integer phase/remainder rules. The spell
expires at step 2879 and removes its owner with six owner ticks remaining,
rather than waiting for the owner's independent counter to reach zero.

Light activation uses the existing RealTimeLight option (`5d7a18`). Original
flag changes call OnScreen/OffScreen, including normal animator creation and
scene light registration. That callback is restored locally to this selected
owner path. The original light setters early-return while OF_LIGHT is clear;
their call order before first activation is deliberately preserved. SetSpell
resets the counters again when Strike Kill detaches the owner.

## Validation

- Independent original replays: all 2895 rows and all 99 variant records equal.
- Actual extracted source Timer/Pulse/Kill and generic owner Pulse: 2895 steps,
  31,614 active owner fields/flags compared from independently evolved source
  state. Birth, movement, counters, blinking and removal match.
- Separate literal original controls exercise delayed start, timed/follow
  expiry, fade-out and RealTimeLight off/on. Their explicit field overrides
  are diagnostic controls, not additional shipped variants. Six compare all
  fields. The seventh deliberately clears OF_PULSE before light reactivation:
  the original animator creation restores it, while the existing source
  initializer does not. The fixture records this exact 0x8000 boundary instead
  of manufacturing the bit. The selected natural lifetime retains OF_PULSE
  throughout and its full flags compare exactly.
- Actual production lifecycle extraction retains 27 null/unselected scenarios.
- The source probe rejects unsupported control fields and a Speed name, checks
  one creation/RNG call, and checks nonresident Pulse/Move dispatch.

The extracted source harness substitutes actor/map/scene boundaries. Its
OnScreen stub models the independently observed normal animator MOVING flag.
It is not a whole-engine cast, renderer, controller, damage or natural-map
acceptance. Existing fixed-owner appearance approvals do not become moving
spell approvals from this change. A real source map cast remains the next
integration check. General bone/polar following, poison/ranged damage,
unselected Strike controls, and delayed map reaping during standalone light
fade-out remain open. No Speed or Quicksilver spell variant is invented.

## Reproduction

Use the retail-asm Python environment and `PYTHONPATH=tools/retail_runtime:tools/retail_asm`.
The native executable and source-matched boot image are private prerequisites;
no binary/assets are committed.

```
python tools/retail_runtime/generic_effect_owner_probe.py --boot-image BOOT --output NATIVE
python tools/retail_runtime/generic_effect_owner_branch_probe.py --boot-image BOOT --output BRANCHES
python tools/retail_runtime/generic_effect_owner_source_probe.py --native NATIVE/regeneration-public-full.json --branches BRANCHES/branch-controls.json --output source-comparison.json
python tools/retail_runtime/test_effect_lifecycle.py
```

Private candidate evidence lives under
`research/vfx-wide-20261010/hard-alpha/generic-effect-pulse/` in the retail lab.
The root's fresh full native replay is under
`research/vfx-generic-owner-root-20261011/native/`.

Rootintegratedactualenginefixture passes all2880schedulersteps/31614ownerfields against theoriginal local-origin control: actualresidentAcolytes/BeginFighting/shippedStrikecreator, productionTimer/Pulse/Kill/genericowner/SubmitPartSys, invokerSetPos andrealspellkill2879. The3002framecapture exitscleanly andproducesnonemptyimages. Source/core2895stepcomparison and27unselectedlifecyclecases also pass. Evidence `research/vfx-generic-owner-root-20261011/engine-comparison.json`. This closes theboundedRegenerationbinding integrationcheck; naturalaffordablecasting/AI/broadStrike/bone/polar/lightedge boundaries remainopen.
