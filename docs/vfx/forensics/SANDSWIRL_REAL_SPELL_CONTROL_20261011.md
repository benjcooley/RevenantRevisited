# Sandswirl real spell/actor control and source parent contract

The original SpellList parser loads99 variants. None has a name or effect binding
containing Sandswirl. This probe therefore explicitly uses the real named
**Quicksand** variant as a public `TEffect::SetSpell` control. It does not invent a
Sandswirl variant or claim that Quicksand naturally invokes Sandswirl. The prior
qualified `sand`→`sparks` guest-code patch is still required and remains recorded;
missing intended child imagery is not solved.

Two Acolytes are constructed through original MapPane.NewObject in a genuine
active sector window at `(3185,3185,16)` and `(3281,3185,16)`. Original
GetInstance returns them, BeginFighting establishes the target, and actual
SpellList/creator `542200` constructs the named spell. The only binding write is
the original inline SetSpell assignment. No actor, query, damage, particle or
spell method is replaced. Character simulation and the global spell scheduler
are outside this declared effect tick fixture.

## Complete qualified parent lifecycle

All310 ticks execute the original surviving owners and full Render paths:

- Init creates85 Sparks particles through original allocation and parameters.
  The stack-derived `seekz` field is still41 with the real caller; it is retained,
  not treated as an initialized Boolean contract. Direction now comes from the
  real invoker/target angle.
- At75, actual FindObjectsInRange returns the two recorded actor map IDs. The
  parent excludes the caster, creates one original Quicksand owner, and resets the
  particle target to `(96,0,-32)` with speed7.5 and hit range10.
- The retail code creates/initializes that child but never calls the older
  source's `InitQuicksand(target,delay)`. Retail `4edb30` is itself a `ret8` stub.
  There is no original victim-disable assignment to port from the old snapshot.
- The Sparks child drains/kills at97. The spawned null-spell Quicksand is blank,
  advances56 pulses, and kills at global131. New children first pulse on the
  following fixture tick. Contrary to an early hypothesis, null spell does not
  cause immediate Quicksand death.
- Sandswirl kills at300 and remains dead through310. Both genuine actors retain
  health102, positions and flags; no invented restoration is needed. This is
  owner completion, not a claim about global map destruction/unregistration.

Two independent executions have identical complete pool/parameter/RNG records,
ordered draw hashes and framebuffer hashes for all310 ticks. All18 selected PNGs
are byte-identical. Every original Render/raster operation executes; PNGs alone
are sampled for storage. The unchanged missing-child negative still faults at
tick1 in the same active-window setup.

## Source controller boundary

`src/effects/sandswirlcontroller.h` implements the independently compiled parent
schedule and target selection: the74-frame speed ramp, tick75 real-query inputs,
caster/null/dead filtering, maximum five targets, relative target positions,
spawn requests and tick300 completion. It chooses no asset, performs no particle
simulation and has **no default runtime admission**. Start refuses an unverified
child. Missing caller at75 stops cleanly rather than reproducing the original
null dereference. Integrators must provide the verified child and map interfaces;
this header does not authorize using Sparks as sand.

`sandswirl_controller_probe.py` compiles that header against the frozen genuine
caller trace:4,340 fields across310 ticks pass exactly, including float bit
patterns. `test_sandswirl_controller.cpp` additionally checks missing child/caller,
empty/dead/caster filtering, the five-target limit and no post-kill advance.
Source particle allocation/RNG/seeking/render parity and the undefined `seekz`
contract remain open. The existing approximate partial preview is unchanged.

## Reproduce and evidence

Run `sandswirl_qualified_child_probe.py` with `--boot-image`, a fresh `--output`,
and both `--substitute-sparks --quicksand-spell-control`. With neither flag it
retains the original missing-child negative. The flags explicitly identify both
diagnostic substitutions. Run `sandswirl_controller_probe.py --trace <manifest>
--output <fresh-directory>` for the compiled source and guard checks.

Private paths below are relative to
`research/vfx-wide-20261010/hard-astra/`:

- `sandswirl-spell-control-final/manifest.json`:
  `c9e022878153e80a6621d7aacd7d5c9a18fa35e8e3e92527c1dc813c587f64e3`.
- `sandswirl-controller-proof/manifest.json`:
  `1e227ef61dc973ba1bd6368bf66c94ff87aacf55a94760b887e1907f3da4a24d`.
- `sandswirl-active-negative-current/manifest.json`:
  `a17991e6b20f478bf16f0ecdce08082e5e03415348bf7693ccc13e94a3f4ed28`.
- `sandswirl-spell-control-repeat.json` records both trace hashes and all310
  unchanged framebuffer/draw/state comparisons.

No original Sandswirl appearance approval, natural named caller, modified retail
binary distribution, engine asset alias, renderer change or ledger update is
claimed. The first spell-control attempt is retained as a failed vtable assertion;
the final tool verifies the actual owner Pulse slot `0x110` before any ticking.
