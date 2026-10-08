# Retail Revenant A/B testing with the thin emulator

**Agent entry point. Overall goal: convert and verify all 176 shipped VFX rows.**
Improve the test system when it enables faster VFX work, then use it to find and
fix port defects. This document is the handoff for agents using the tool.

Updated 2026-10-07. **The in-process retail emulator is the primary tool for
VFX validation.** Use DOSBox-X with the emulated 3D device only for targeted
checks of software-rendering issues. Most state, animation, geometry, motion,
instrumentation and A/B work stays in the fast emulator. Coordinate ownership
before operating a DOSBox guest used by another project.

Run the thin comparison first. Escalate a specific case to the 3D guest when
matching state, geometry and materials still leave a suspected software-device
artifact. Keep that capture as a separate diagnostic with its device and scene
settings recorded; it does not become a dependency for routine batches.

Use the original retail software renderer. Prioritize fast, repeatable scenarios
and real assets. Known software-renderer limits are recorded separately from
VFX simulation, geometry, animation, material and integration defects. Rendering
perfection is not a prerequisite for progressing through the effects catalogue.

## Locations

- Repository: `/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited`.
- CPU/API emulator: [`tools/retail_runtime/runtime.py`](../tools/retail_runtime/runtime.py).
- Persistent scenario commands: [`tools/retail_runtime/run.py`](../tools/retail_runtime/run.py).
- Direct effect/entity controls: [instructions](../tools/retail_runtime/README-controls.md).
- Exercised VFX comparisons: [instructions](../tools/retail_runtime/README-effects.md).
- Original software renderer fixture: [`tools/retail_runtime/software_probe.py`](../tools/retail_runtime/software_probe.py).
- Parallel/private named builds: [instructions](../tools/retail_runtime/README-parallel.md).
- Byte-identical retail assembly baseline: [`recon/retail_asm/baseline`](../recon/retail_asm/baseline).
- Original assets: `/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT`.
- Port executable: `/Users/benjamincooley/projects/RevenantRevisited/build/Revenant`.
- Current thin-emulator evidence: [`recon/retail_asm/runtime`](../recon/retail_asm/runtime).
- Gameflow/retail-trace fixtures are integrated from main. See
  [the gameflow trace guide](gameflow/RETAIL_TRACE.md) for their entry point;
  these use the shared runtime alongside the VFX fixtures.
- Checkpoint restore tracks written pages natively (`dirtypages.c`, built with `cc` on first use; 2026-10-07): 3x on store-heavy fixtures, same pages and results. [Details and timings](../tools/retail_runtime/README.md#checkpoint-restore-native-dirty-pages).
- Complete effect ledger: [176 retail rows](vfx/EFFECT_BURNDOWN.md).

The unchanged retail SHA-256 is
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Experimental binaries have their own names, hashes, parent baseline and patch
manifests. Each agent owns a private runtime process and checkpoints; each may
modify a private assembly/EXE workspace and keep many named builds. Publishing
another build preserves earlier artifacts. Never edit the shared retail baseline.

## Start with exercised commands

From the repository root:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/run.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --scenario tools/retail_runtime/scenarios/render_state.json --repeat 100
"$PY" tools/retail_runtime/effect_probe.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/effects/partsys-state-ab.json --repeat 3
"$PY" tools/retail_runtime/software_probe.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --output recon/retail_asm/runtime/software --repeat 100
```

These demonstrate warm original-code calls, original particle-system state vs
compiled production code, and actual textured software raster output into RAM.
They do not yet constitute a complete rendered effect or map-context match.
Do not turn a state-only or renderer-only pass into an effect sign-off.

`run.py` without `--scenario` keeps one process open and accepts JSONL commands.
Its `execute` command restores the session checkpoint before each scenario.
`parallel.py` runs independent persistent processes; a job selects its named
session/build. Use [the parallel guide](../tools/retail_runtime/README-parallel.md)
for workspace creation, build publication, reload and isolated artifact paths.

## Direct test control: no editor commands

[`controls.py`](../tools/retail_runtime/controls.py) exposes Python methods and
JSON actions. The working `authoredpartsys` profile supports creating/removing
effect fixtures, source entity inputs, owner position/facing, original simulation
ticks, state inspection, checkpoint/reset and state capture. It calls original
retail functions directly. Its current source entity is a bounded fixture,
not a fully loaded map actor; stats/attachments are not mapped by that adapter.

The checked-in [example](../tools/retail_runtime/scenarios/partsys_controls.json)
creates a source, spawns its effect, advances it, captures state and removes it:

```sh
"$PY" tools/retail_runtime/controls.py recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --scenario tools/retail_runtime/scenarios/partsys_controls.json \
  --output recon/retail_asm/runtime/partsys-controls-example.json
```

This command launches a structured scenario; it does not type an editor console
command. Without `--scenario`, the process accepts JSONL action/execute requests.
Use the [control guide](../tools/retail_runtime/README-controls.md) for supported
actions and the [parallel guide](../tools/retail_runtime/README-parallel.md) to
route control sessions by agent and named build. Query capabilities first.
Unsupported actor, camera, lighting, movement or pixel operations fail explicitly.

The [current mixed batch](../tools/retail_runtime/scenarios/current_profiles_batch.json)
keeps six independent sessions warm: Partsys and shared-missile state, plus
Flame, Ripple, Fizzle and MistFog pixels. Two repeated batches retained their
worker PIDs and identical state/color/depth hashes, taking roughly 0.26–0.28
seconds per warm batch. See [the control guide](../tools/retail_runtime/README-controls.md)
for the `parallel.py --repeat` command and
[batch verification](../recon/retail_asm/runtime/current-profiles-batch-verification.json).
MistFog currently requires the validated identity/full-white-light fixture;
unsupported input overrides remain explicit.

**Every projectile must move under original update code. Every point-to-point
effect must have two supplied endpoints.** Provide source/destination positions,
stats, attachments and launch parameters as required by the specific effect.
Record trajectories, velocity, collisions/impact and cleanup at fixed ticks.
Scripted actor movement supplies the same external inputs to both versions;
moving a missile manually each frame cannot replace its original simulation.
Stationary and one-point stand-ins cannot satisfy these acceptance gates.

Classify by actual behavior rather than the effect's name. For example, the
current IceBolt source audit describes a two-endpoint freeze beam derived from
`TEffect`, not a flying missile. Its fixture needs both endpoints and beam
length/orientation/lifecycle checks. Retail particle-count and controller
differences remain unverified; do not infer them from the snapshot alone.

## Current VFX evidence

- [Flame](../recon/retail_asm/runtime/effects/flame-frontend-ab/manifest.json):
  18 frames, eight distinct images, no differing RGB565 pixels. Original Animate
  wrap, Render transforms, authored atlas and compiled production component.
- [Ripple](../recon/retail_asm/runtime/effects/ripple-frontend-ab/manifest.json):
  57 no-splash cases, birth through kill and post-death, matching state, geometry,
  UVs and RGB565 pixels. Splash/RNG/children remain separate checks.
- [Partsys](../recon/retail_asm/runtime/effects/partsys-state-ab.json): 19,800
  passing state/pose comparisons and exact warm reset; this profile's device
  submission is not executed.
- [Fizzle](../recon/retail_asm/runtime/effects/fizzle-frontend-ab/manifest.json):
  complete 40-frame burst, all geometry inside the viewport, exact particle
  state/RNG/lifetime and matching RGB565 output. Original death occurs at tick 29.
- [Moving missile controls](../recon/retail_asm/runtime/controls-verification.json):
  original launch/trajectory/impact/KillRequest, source/destination inputs,
  scripted actor motion and parallel replay. This is shared missile-base
  behavior, not full Fireball visuals, damage or map reaping.
- [Colored Flame variants](../recon/retail_asm/runtime/effects/flame-variants-runtime/README.md):
  blue and green each match 18 original-raster frames; their modern preview IDs
  share the verified authored producer and clock. Use `flame_probe.py --profile
  blue` or `--profile green` with the same executable/output arguments.
- [Mist](../recon/retail_asm/runtime/effects/mist-frontend-ab/manifest.json):
  97 lifecycle states, 1,380 strict native RNG calls and eight repeated image/depth
  samples across respawns. Actual preview/map producer methods are compiled;
  natural map binding and lighting remain separate.
- [Pixie](../recon/retail_asm/runtime/effects/pixie-frontend-ab/manifest.json):
  97 swarm ticks, 5,000 strict native RNG calls and eight repeated image/depth
  samples. The empty nearby-character fixture does not verify interaction.
- [SymGlow](../recon/retail_asm/runtime/effects/symglow-current-frontend-ab/README.md):
  162 matching breathing/UV states, 14 repeated image/depth samples and a
  skipped-render regression. Native Render changes UV state; capture adapters
  cache it once for the current animation tick.
- [Literal Waterfall](../recon/retail_asm/runtime/effects/waterfall-frontend-ab/README.md):
  actual 100-tick warmup, 97 checked ticks and eight repeated image/depth samples.
  White vertex inputs and original indices are explicit; this does not verify
  other WCap/WFall controllers, modern triangulation or natural lighting.
- [Standalone Sparks](../recon/retail_asm/runtime/effects/sparks-editor-current-ab/README.md):
  original ten-particle fallback and controlled twenty-particle bounce/trail
  cases, exact initialization/ballistics and 22 repeated image/depth pairs.
  Actual blocked-combat caller/audio/owner integration remains separate.
- [Drip](../recon/retail_asm/runtime/effects/drip-frontend-ab/README.md):
  falling head, emitter state and child requests; full child rendering and
  audio/global RNG parity are not covered by this head-only proof.
- [Yellow FireBall](../recon/retail_asm/runtime/effects/yfireball-handoff.json):
  actual Y kernels and green asset through shared moving code. Bridge-relative
  software frames do not grant independent retail device/Metal acceptance.
- [Cure null-spell](../recon/retail_asm/runtime/effects/cure-nullspell-frontend-ab/README.md):
  all 85 records through natural kill, exact native state and 14 repeated
  image/depth pairs. Real spell/target behavior and actual additive/material
  lighting/helper sorting are separate gates.

Flame and Ripple compare the actual retail and compiled production effect
frontends through the same original software rasterizer. These scoped results
do not grant full Revisited GPU-backend or natural-map/context acceptance.
Use the current ledger for unfinished gates and the next easy effect.

The [actual Metal comparison](../recon/retail_asm/runtime/effects/flame-metal-capture/README.md)
also runs the real Revisited renderer. `port_capture.py` launches an explicit
scenario without editor commands; `compare_backend.py` reads a hash-pinned frame
mapping without fitting phase, position, scale or brightness. Current Flame
output looks similar but differs at measured pixels, so this is retained as a
backend diagnostic. This separate gate prevents a shared-raster frontend pass
from being mistaken for full renderer parity.

The capture wrapper defaults to one explicit warmup frame: zero-warmup startup
can capture solid magenta before the effect is drawn. Warmup advances simulation;
use the recorded absolute simulation times when mapping retail ticks. In the
24 Hz Flame check, the first valid capture is at 0.083333333 seconds, and its
first 17 frames equal the preceding run's frames 2–18. This is a recorded
one-tick offset, not phase fitting. Headless captures now exit through normal
cleanup; see [verification](../recon/retail_asm/runtime/shutdown-fix-verification.json).

## Fast VFX loop

1. Pick an easy unverified effect from the current ledger. Defer expensive
   fixture reconstruction while easier effects remain.
2. Prepare actual retail assets and recorded owner, camera, light, ground and
   effect parameters in a thin-emulator fixture. For map effects, prefer their
   actual placement/context once supported. Synthetic floor fixtures remain
   explicitly labelled.
3. Checkpoint initialized memory, CPU/thread states, RNG, files and virtual time.
   Reuse the process and restore the checkpoint between runs.
4. Execute original update/render functions at explicit ticks. Capture state and
   RGB565/depth buffers through the original software renderer. Record any
   environment boundaries adapted or bypassed.
5. Run the corresponding Revisited scenario with the same inputs. Compare state,
   authored geometry/material/animation, visible frames and lifecycle behavior.
6. Fix causal port defects, rerun quickly, retain evidence, and update only the
   acceptance gates actually demonstrated. Move to the next easy row.

Keep simulation comparisons strict where the input/RNG contract is controlled.
Independent random seeds can still support visual acceptance when appearance
and behavior match; they do not support an exact state/pixel identity claim.

Keep renderer limits visible but separate. For example, the original ARGB4444
software lookup path may quantize full red differently from RGB565/modern GPU
output. Do not tune effect colors or geometry to hide a renderer discrepancy.
A documented rendering limit need not halt other state/animation tests or the
whole VFX queue. Missing thin-emulator support stays open or gets implemented;
it does not send routine work back through the DOSBox/editor capture workflow.
When a discrepancy may specifically come from software rendering, a focused
DOSBox-X/3D-device comparison is allowed. Record the device, fixture and result,
then bring that finding back into the thin-emulator tests.

## Instrumenting an effect when needed

Use an agent's private named retail build for logging or behavior-isolation
experiments. The [assembly laboratory guide](../recon/retail_asm/README.md)
describes the byte-identical baseline and freestanding C/assembly hooks.
[`function_hook.py`](../tools/retail_asm/function_hook.py) can add an entry trace
that logs through the original Win32 file imports, preserves entry state,
replays complete stolen instructions and rejoins the original function. It
rejects prologues requiring unsupported relocation. Replacement/skip modes
require an explicit ABI and are experiments rather than unchanged references.

Publish the instrumented image through the [named-build workflow](../tools/retail_runtime/README-parallel.md)
and select/reload only the owning session. Logs are virtual files and can be
captured with the scenario state. Keep the unmodified control alongside it.
Effect fixtures pin the exact baseline hash by default. Flame also accepts
`--build build.json` for an explicit named image with unchanged fixed-address PE
layout, verified actual SHA and baseline ancestry. Declared patch spans are
checked when present; behavioral equivalence is still tested. The exercised
[Flame logging proof](../recon/retail_asm/runtime/instrumentation/flame-trace-control-verification.json)
records a compiled C trace at original Render `0x4e4f20`, logging every frame
while all 18 color/depth outputs equal the unmodified control. Other fixture
adapters require their own supported build contract. Record all patch addresses
and changed behavior in the build manifest.

## Required evidence

Every comparison records the exact build name/hash/parent/patch manifest, scenario
and asset hashes, fixture assumptions, timing/RNG policy, port source/build hash,
state trace, frames/depth where available, and known renderer limitations.
Instrumented variants distinguish changed behavior from unchanged-retail
references. Show the change and retain its log; preserve the unmodified control.

Warm replay reliability, CPU-state parity, rendered appearance, map/combat/spell
integration and full acceptance are distinct gates. Preserve all 176 type IDs.
Existing DOSBox-era references and prior visual reviews remain research evidence.
New validation primarily uses this emulator; targeted 3D-device diagnostics may
clarify software-renderer differences without replacing the fast VFX workflow.

Full retail startup is still incomplete; targeted original subsystem fixtures
already work. Current API coverage and remaining gaps are in the
[runtime README](../tools/retail_runtime/README.md). See the
[archived DOSBox instructions](retail-history/RETAIL_AB_DOSBOX_2026-10-06.md) only
for historical provenance or unrelated guest work.
