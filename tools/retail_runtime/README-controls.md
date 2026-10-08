# Direct structured retail effect controls

`controls.py` provides reusable Python methods and JSON actions to drive known
retail effect functions directly. It does not type editor commands, click UI,
send keyboard input or use DOSBox in the normal test loop. Separate, targeted
DOSBox-X graphics-device diagnostics are permitted when investigating software
rendering, with coordinated guest ownership; they do not change these UI-free
controls. Each profile states exactly which operations
are supported. Unsupported operations fail explicitly.

The first concrete profile is **`authoredpartsys`**, a synthetic bounded
one-emitter/owner fixture. Creation allocates the known controller/particle
layouts; it is not a generic retail map object factory. `advance_ticks` executes
original retail Pulse `0x403760` and its spawn/update functions. Optional
`render=true` executes original pose/color instructions and stops before device
submission. Emitter/prototype transforms are identity fixtures, ground is zero,
and the random stream is the matched MSVC generator. State capture is available;
retail pixel capture and map placement are not yet supported by this profile.

The second profile, **`shared_missile_base`**, runs original GetAngle `0x4df070`,
Move `0x470920` and Pulse `0x510220`. Supplied source/destination positions,
target HP/enmity and ground/wall inputs drive its original launch, fixed-point
trajectory, collision/impact and KillRequest. It is not full Fireball leaf
animation, damage, rendered pixels or map reaping. Use that exact profile label
when reporting results rather than crediting all of Fireball as verified.

Pixel profiles **`flame`**, **`ripple_no_splash`**, **`fizzle`** and **`mistfog`** expose the
existing verified producers through the same typed lifecycle. They run original
Animate/Render, shipped mesh/atlas inputs, original projection and software
triangle raster `0x56d960` into independent RGB565/U16 depth surfaces. No VFX
equations are copied into the adapters. These captures are original software
reference pixels; comparison against Revisited's modern renderer remains a
separate gate.

MistFog runs the original looping 25-puff Initialize `0x4f2840`, Animate
`0x4f2950`, Render `0x4f2ae0` and CalcObjectMatrix `0x40a420`, observing the
unchanged raw/range RNG stream. Its validated fixture uses identity owner,
camera `[0,0,0]`/zdist 1925, a 384×384 viewport and full-white vertex color.
Position/source/camera/light changes are rejected in this profile. Puff
expiry/respawn and native RNG are part of the snapshot rather than being
reconstructed by host equations.

**`static_mesh`** selects one of thirteen verified constant single-object assets
by `params.profile` (for example `CampSign` or `Globe`) or `params.type_id`.
Its capability catalog lists the audited identities. It runs original key
decode/CalcObjectMatrix, matrix multiplication, point transform, projection and
software mesh raster. `set_sample_frame` selects verified frames 0, 50 or 99;
those captures remain identical. Owner positions `[0,0,0]` and `[16,-8,4]` are
supported and produce different visible pixels. Other translations and camera/
lighting changes are rejected until verified. This is constant asset frontend
sampling, not a claim that the complete Globe/sign effect executes or that a
native effect Animate was recovered. `advance_ticks` advances virtual time
without inventing motion or changing the selected sample.

**`fountain`** selects `CyanFont`, `RedFont`, `GreenFont` or `BlueFont` via
`profile`, friendly `color` (`cyan/red/green/blue` or integer 0–3), or exact
`type_id`. Multiple supplied selectors must match. The wrapper runs actual leaf
vtables and original Initialize `0x4e4080`, Animate and Render `0x4e41f0`; no
bubble equations or per-tick positions are supplied by the host. State includes
ten bubbles, native RNG and observations of actual delay/rise/shrink/respawn
transitions. Guest state, RNG observers and transition counters checkpoint
together, including midcycle resets. Camera/owner/lighting stay at the verified
identity/full-white inputs; natural map ownership and illumination are open.

**`colored_ribbon`** selects `RibbonB/G/O/P/R/W/Y` by audited name or type ID.
Both authored root parts, textures and local indices are composited in asset
order using original key/matrix/raster functions. Frame0 and the two verified
owner poses are supported; base Ribbon revive/combat is excluded. Reset also
restores the host texture/metadata binding when checkpointing after both parts.

**`fireball_head`** requires two supplied distinct endpoints and runs actual
missile movement plus original head/glow pixels in the same VM. Explicit
`render_inputs` select frame, integer rotation, scale, glow and face; changing
them never sets missile positions. Full animator waveforms, trail/burst/sparks,
damage/map removal and modern GPU acceptance are separate.

**`streamer`** runs original four-stream Init/Animate/Render with exact authored
meshes/textures, identity owner, face0 and the producer's declared frontend
culling boundary. Native frame101 frees its arrays and requests removal;
checkpoint/reset can restore an earlier active state and image. It has no RNG.
Spell/poison-cure/audio, real map/device culling and lighting remain open.

Flame additionally selects `variant=base|blue|green` or exact type ID
`0x50ba373b/c/d`. Explicit unknown or mismatched selectors fail; they never
fall back to base. Each variant loads its own shipped asset and pinned SHA.
**`mist`** is distinct from MistFog: original persistent fifty-drop native
Init/Animate/Render, actual death/respawn and range-RNG inputs, fixed face0/
identity/white inputs. At96ticks the tested fixture consumes1380 native RNG
inputs with227 deaths and226 respawns; an active48tick checkpoint replays it
exactly. Movement/face/map lighting remain separate interfaces.

**`pixie`** runs the original persistent 25-particle swarm in the verified
empty-nearby-character context. The native two range queries per tick and
5,000 range-RNG inputs over96ticks are preserved; both authored texture/object
selections render normally. Translation/face/character parameters are rejected
until those contexts are separately verified.

**`symglow`** preserves native UV mutation during Render. State inspection and
`advance_ticks` do not render. The first pixel capture at a simulation tick/
native timer performs one actual Render; repeat captures reuse exact cached
color/depth without scrolling UV again. Advancing to a new timer permits the
next native Render. Checkpoints include cache timer, buffers, UVs, native RNG
and observer state, so explicit stride3 capture schedules replay exactly.

**`waterfall`** is the literal type `0xa907dabf`, not WCap/WFall variants.
Creation executes unchanged native Init and its internal100tick warmup. State
exposes all100 records, native time/delay/reset fields, RNG and observed
landings/respawns; a48tick checkpoint restores the entire pool and pixels.
The earlier producer's half-pool inspection was corrected and reverified with
native/port cardinality assertions:9,700records/87,300float32fields across97
samples. Controlled-white vertices, original indices and unused copied-lighting
callback omission remain explicit; natural map/normal illumination/modern
triangulation/blending/culling are not certified.

**`sparks`** exposes exact type `0x14db0f2e`. Select `profile: "editor"`
(10 particles, native fallback parameters) or `"controlled_bounce_trail"`
(20 particles, bounce and two trail samples). Creation calls actual
InitParticles explicitly: tick zero is birth, with no hidden first Animate.
Ticks execute original ballistics and completion, preserving range-call and
raw CRT RNG counts independently. Capture uses four shipped photon quads,
actual Render/CalcObjectMatrix and original software raster. Native POS1
flags apply translation; the stored `.01` scale is not a geometric scale.
Checkpoint restores the complete pool, done state, RNG and observer state.
Seeking, arbitrary parameters, translated owner, combat/map and GPU remain
outside this profile. Example:

```json
{"op":"create_effect","id":"spark","type":"sparks","params":{"profile":"editor"}}
```

## Supported actions

- `create_effect`: `id`, `type`, optional profile-specific `params`. Partsys
  supports `shape` 0–3, `face` 0–255, `render`, unsigned `seed` and integer
  `owner` coordinates. Pixel profiles support `archive`, explicit camera and
  `rgba5`; Flame/Ripple additionally accept fixture `position`, and Ripple
  accepts no-splash `length` 4, 20 or 24. Projectiles use `spawn_effect`.
  MistFog accepts only `archive`, retaining its explicitly fixed inputs.
  Static mesh accepts `profile`/`type_id`, `archive`, a verified `position`, and
  initial `frame` 0/50/99. Unknown or mismatched profile/name IDs fail.
  Fountain accepts `profile`/`color`/`type_id` and `archive` only.
  Colored Ribbon accepts `profile`/`type_id`, `archive`, a verified owner
  `position` and optional frame0. Streamer accepts only `archive`. FireBall head
  uses `spawn_effect` with missile launch parameters and explicit `render_inputs`.
  Flame supports exact `variant`/`type_id` selectors; Mist accepts `archive`
  only, retaining verified fixed inputs.
- `remove_effect`: remove a fixture ID. This does not claim execution of a retail
  map object's destructor. Reset can restore a fixture saved in a checkpoint.
- `advance_ticks`: `ticks`, optional effect `id`. Executes original functions,
  then advances virtual time at 24 Hz. Multiple IDs are independent fixtures,
  not interacting objects on one simulated map.
- `inspect_state`: optional `id`; returns pool state hash, tick/row trace,
  RNG/state/pose/color fields and original/external call counts.
- `checkpoint` / `reset`: preserve/restore guest and host fixture state, effect
  IDs, entity input handles and bindings. Every scenario starts from the saved
  checkpoint. Ordinary action requests continue the current state.
- `capture`: effect `id`, `kind=state|pixels`, optional `output` and
  `depth_output`. State captures produce canonical JSON. Pixel captures hash
  actual RGB565/U16 buffers and save PNG when `output` ends in `.png`, otherwise
  raw RGB565. `depth_output` saves raw little-endian U16 depth. Metadata records
  dimensions, stride, state/packet, actor inputs and exact named-build provenance.
  Partsys/shared missile state profiles do not support pixel capture.
- `validate_execution_contract`: effect `id`. A projectile must have observed
  movement from original Move calls; a primary two-endpoint profile must have
  supplied distinct endpoints. This validates execution scope, not full visual/
  damage/map acceptance.
- `set_sample_frame`: effect `id`, `frame` 0/50/99; supported by static mesh
  sampling only. Captures call the original pose/key functions with that frame.
  Colored Ribbon accepts frame0 only.
- `set_render_inputs`: FireBall head `id`, explicit renderer `inputs` with
  `frame`, `rotation`, `scale`, `glow`, `face`. This does not simulate its animator.

Call `capabilities()` or JSON `{"command":"status"}` before choosing a profile.
`verified_retail_types` lists all 37 audited retail name/ID pairs currently
available through bounded pixel adapters (four original leaves, thirteen
static assets, four Fountain leaves, seven colored Ribbons, FireBall head and
Streamer, blue/green Flame, Mist, Pixie, SymGlow, literal Waterfall and Sparks). Partsys and shared-missile state
fixtures do not invent registered leaf IDs. This catalog does not claim full
game acceptance.
Profiles use the verified retail baseline SHA
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
Flame and shared-missile profiles additionally accept explicit named builds
through `build_contract.verify_build`: matching actual SHA, baseline ancestry,
fixed PE layout/file length and any declared modified spans. They do not bypass
the baseline guard by merely replacing a constant. Ripple/Fizzle/MistFog/static mesh/Fountain and Partsys
remain baseline-only until their producer constructors expose the named-build
contract. Check each profile's `named_build_contract_supported` capability.

## Source/destination and moving effects

Fixture entity handles provide an explicit future source/target schema:

- `create_entity`: `id`, integer `position=[x,y,z]`, optional byte `facing`,
  `stats` object and `attachments` object.
- `spawn_effect`: `id`, `type`, optional `source_id`, `destination_id`,
  `launch_params` object.
- `set_owner`: effect `id`, `owner` entity handle. Authoredpartsys binds the
  synthetic owner position fields at offsets 16/20/24 and facing byte 54.
- `set_entity_position`: entity `id`, integer `position`, optional `facing`.
  Updates bound synthetic owner fields before the next original Pulse/pose.
- `inspect_entities` / `remove_entity`: inspect/remove input handles. Removing a
  handle still bound to an effect is rejected.
- `set_entity_motion`: entity `id`, `frames` array with unique `tick`, `position`
  and optional `facing`. Before each original effect tick, apply due SOURCE/
  TARGET actor fixture inputs and record them. Frames are explicit sampled
  positions; no interpolation or projectile repositioning is invented.
- `inspect_actor_inputs`: return control timeline tick, motion schedules and the
  exact delivered actor input stream. Checkpoints/reset preserve all three.

When actor scripts are active, `advance_ticks` must advance all active effects
on the same control timeline. Advancing one while leaving another behind is
rejected. Two effect fixtures can share source inputs while retaining separate
guest states. Their original Pulse functions still compute particle/effect
motion. Recorded target inputs alone do not imply a connected missile adapter.

These handles **do not spawn original actors**. Stats/attachments are recorded
fixture inputs and explicitly remain unmapped in authoredpartsys. The shared
missile profile consumes destination `stats.hp` (int32, default 100) and
`stats.enemy` (boolean, default true), plus source/destination positions. Other
stats, facing and attachments are not mapped in that profile. Capability
descriptors list each profile's consumed `input_fields`. Emitter
transforms remain the declared identity fixture; changing an owner coordinate
does not establish complete in-map attachment or emitter behavior.

Authoredpartsys accepts a source owner. `shared_missile_base` accepts two supplied
endpoints and launch parameters `speed` (integer 1–120, default 8), `wall_x`,
`ground_height` (default 1), `launch_ready` (default true), and
`animator_present` (default false). It records tick position, fixed velocity,
accumulator, state, range, flags, events and original calls. Source/target role
setters update actor fixture fields only; they never rewrite missile positions.
Unregistered Fireball leaf/other missile profiles fail. A moving-effect adapter must run the
original launch and Pulse functions, record position/velocity/collision/impact/
reap, and bind any scripted source/target actor movement as matching fixture
inputs in retail and Revisited. Repositioning a missile each tick is not an
acceptable substitute for its original movement. `set_position`, `set_camera`
and `set_light` fail for state-only profiles. In pixel profiles, camera inputs
are fed to original world/view/projection and `set_light` accepts only
`{"rgba5":[r,g,b,a]}` (four integers 0–31), explicitly selected upstream vertex
modulation, not original map ambient/light collection. Flame/Ripple position
uses an explicit upstream world matrix. Fizzle retains local-Z `abs_pos=0` and
rejects owner translation rather than inventing an owner-height correction.
MistFog also rejects camera/light changes; its full-white/identity-owner policy
is fixed until additional inputs are verified by its producer.

Every profile declares `kind`: `stationary`, `projectile`, or `point_to_point`.
Every projectile acceptance run must show movement produced by **original
simulation ticks**, including position/velocity and relevant collision/impact/
reap trajectories. Every point-to-point primary run must receive **two distinct
supplied source/destination endpoints**, including their per-tick trajectories
if actors move. A projectile can additionally require a destination. A
stationary preview cannot earn acceptance for either moving or two-endpoint
effects. Zero-length/equal-endpoint cases are separate explicitly labeled
degenerate tests and cannot replace the distinct-endpoint primary fixture.
The controller refuses nonstationary profiles unless they declare a validated
`launch` adapter; adding profile-kind metadata alone does not enable execution.
The current shared missile probe rejects zero-length endpoints even when the
control request correctly labels a degenerate case; that original fixture edge
case still needs its own verified adapter policy.

## Run an example

From the inner repository:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/controls.py \
  recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --scenario tools/retail_runtime/scenarios/partsys_controls.json \
  --output /tmp/partsys-controls.json
```

Python usage (add `tools/retail_runtime` to your import path):

```python
from controls import Controller

lab = Controller(executable, build=build_record)
lab.create_entity("source", [10, 20, 30], facing=64)
lab.spawn_effect("particles", "authoredpartsys", source_id="source",
                 launch_params={"shape": 2, "render": True, "seed": 7})
lab.checkpoint()
lab.advance_ticks(10, "particles")
first = lab.capture("particles")
lab.reset()
lab.advance_ticks(10, "particles")
assert lab.capture("particles")["sha256"] == first["sha256"]
lab.remove_effect("particles")
```

For actual original projectile motion:

```python
lab.create_entity("caster", [0, 0, 128])
lab.create_entity("target", [320, 0, 128], stats={"hp": 100, "enemy": True})
lab.spawn_effect("missile", "shared_missile_base", source_id="caster",
                 destination_id="target", launch_params={"speed": 8})
lab.checkpoint()
lab.advance_ticks(40, "missile")
lab.validate_execution_contract("missile")
trace = lab.capture("missile")
# Original fixture impacts at tick 37 at [288, 0, 110], KillRequest at tick 38.
```

The standalone example is `scenarios/missile_controls.json`, usable with the
same `controls.py --scenario` command as the partsys example. Source/target
motion can be supplied with `set_entity_motion`; original missile updates still
compute the flight. Record both the effect trace and `inspect_actor_inputs`
for retail/port replay. Capture results include the actor input stream and
binding provenance in addition to effect state.

For an actual animated reference PNG:

```python
lab.create_effect("torch", "flame")
lab.checkpoint()
lab.advance_ticks(3, "torch")      # Calls actual retail Animate 0x4e4ef0.
image = lab.capture("torch", "pixels", output="/tmp/torch.png",
                    depth_output="/tmp/torch.depth-u16")
lab.reset()
lab.advance_ticks(3, "torch")
assert lab.capture("torch", "pixels")["sha256"] == image["sha256"]
```

The original Flame counter wraps at 18 ticks, with eight distinct atlas cells;
some adjacent ticks intentionally produce identical pixels. The Ripple profile
executes native Initialize/Animate and respects native kill flags; killed
fixtures stop animation and capture a cleared surface. Fizzle snapshots actual
90-slot arrays, native RNG inputs, software texture selection and host observer
counters alongside guest memory, so warm replays preserve the particle image.

Omit `--scenario` for persistent JSONL. `--setup` is a JSON array of one-time
actions followed by an automatic checkpoint. `--build` supplies a named
`build.json` whose SHA must match the executable. Request IDs are echoed:

```json
{"id":"create","command":"action","action":{"op":"create_effect","id":"p","type":"authoredpartsys","params":{"seed":7}}}
{"id":"save","command":"checkpoint"}
{"id":"step","command":"action","action":{"op":"advance_ticks","id":"p","ticks":10}}
{"id":"state","command":"action","action":{"op":"inspect_state","id":"p"}}
{"id":"reset","command":"restore"}
```

`command=execute` takes a scenario with an `operations` array and optional exact
`executable_sha256`. It resets before execution and restores the saved checkpoint
on failure. An explicit `checkpoint` operation changes the reset point; avoid
it inside a scenario when testing rollback to the original checkpoint.

## Parallel agent routing

[The parallel pool](README-parallel.md) supports a different runner per named
session. Set `runner` to `controls.py` and `pass_build_record=true` to pass exact
variant provenance into the control process:

```python
from parallel import Pool

sessions = [{"name": name, "build": build_record,
             "runner": "/absolute/path/tools/retail_runtime/controls.py",
             "pass_build_record": True,
             "setup": [{"op": "create_effect", "id": "p",
                        "type": "authoredpartsys", "params": {"seed": 42}}]}
            for name in ("agent-a", "agent-b")]
scenario = {"operations": [{"op": "advance_ticks", "id": "p", "ticks": 10},
                           {"op": "capture", "id": "p", "output": "state.json"}]}
with Pool(sessions=sessions, artifact_root="/tmp/effect-captures") as pool:
    results = pool.execute([{"session": name, "scenario": scenario}
                            for name in ("agent-a", "agent-b")])
```

Each session retains its own loaded PE and fixture checkpoint across requests.
Captures go into separate per-job directories. Different agents may select
different named builds, provided their profile is explicitly validated for each
build. Profiles supporting named-build contracts may use guarded experimental
images; other profiles require the unchanged retail image.
Pool `reload(session,new_build)` restarts that control session with
the new build record and leaves sibling sessions alone.

The checked-in moving-missile manifest runs an impact case and a dead-target
flight case in separate warm agent sessions with different actor health inputs:

```sh
"$PY" tools/retail_runtime/parallel.py \
  --jobs tools/retail_runtime/scenarios/parallel_missile_controls.json \
  --artifacts /tmp/retail-moving-controls \
  --output /tmp/retail-moving-controls-result.json
```

Both advance original missile simulation with distinct source/destination
endpoints. One impacts; the dead-target case remains in flight at tick 40.
Independent captures and exact build/input provenance are retained for each.

The pixel-control sample runs Flame and no-splash Ripple in separate warm
sessions and writes their actual reference PNGs and depth:

```sh
"$PY" tools/retail_runtime/parallel.py \
  --jobs tools/retail_runtime/scenarios/parallel_pixel_controls.json \
  --artifacts /tmp/retail-pixel-controls \
  --output /tmp/retail-pixel-controls-result.json
```

To run every current fixture family in a single batch, including two state
sessions and seventeen pixel sessions (base/blue/green Flame, Ripple, Fizzle,
MistFog/Mist, a sign, Globe, CyanFont, colored Ribbon, moving FireBall head,
Streamer, empty-context Pixie, render-cadence SymGlow, literal Waterfall and Sparks):

```sh
"$PY" tools/retail_runtime/parallel.py \
  --jobs tools/retail_runtime/scenarios/current_profiles_batch.json --repeat 2 \
  --artifacts /tmp/retail-current-profiles \
  --output /tmp/current-profiles-result.json
```

All nineteen sessions initialize once. Both batches restore their warm checkpoints;
each job writes separate state JSON or PNG/depth artifacts. The measured
mixed batches run in 1.61 and 1.80 seconds after setup (during concurrent verification), with exactly matching
captures and unchanged PIDs. See `current-profiles-batch-verification.json` for
current measured times. These are bounded fixture timings, not full-map performance.

`current_profiles_batch.json` is also input for Python `Pool`:

```python
from parallel import Pool, load_manifest
manifest = load_manifest("tools/retail_runtime/scenarios/current_profiles_batch.json")
with Pool(sessions=manifest["sessions"], artifact_root="/tmp/effect-batch") as pool:
    first = pool.execute(manifest["jobs"])
    second = pool.execute(manifest["jobs"])
```

`load_manifest` resolves build/scenario/runner paths once. Change a session's
`build` to a private workspace
record for profiles that support the named-build contract. Do not weaken a
baseline-only profile's guard to run a modified image.

## Add a profile

`Controller.register_profile(name, Profile(factory, description, actions,
required_sha256, kind="stationary", requires_destination=False))` registers an explicit adapter factory. The factory receives
the pinned executable and parameters. The adapter provides supported methods
such as `advance_ticks`, `inspect_state`, `checkpoint`, `reset`, `capture` and
optional setters. Declare only actions that execute validated original code or
apply verified fixture inputs. Supplying a registry does not imply a shared game
world. The pixel adapters return `PixelFrame` with actual RGB565 and U16 depth
bytes; the controller saves and hashes them. Add new profiles only after their
original lifecycle and render boundaries are connected and verified.

## Verify

```sh
"$PY" tools/retail_runtime/test_controls.py \
  --output recon/retail_asm/runtime/controls-verification.json
```

Twenty-five tests exercise actual original Pulse/pose instructions through concrete
create/advance/state/reset/remove controls, source-owner field binding,
checkpoint replay, unsupported-feature rejection, failure rollback, JSONL named
build pinning, two parallel control processes and multi-effect source/target
motion-input replay across checkpoint/remove/create/reset. They also run the
original moving missile through impact/KillRequest, reject missing/equal primary
endpoints, prove source/target setters do not teleport the missile, and replay
a scripted moving target exactly. Two parallel missile workers keep different
target-health states, produce different flight/impact traces, and warm-replay
those traces without restarting. The tests also cover Flame counter wrap,
changing native UV/pixels, PNG/depth export,
guarded named variants, Ripple lifespan and Fizzle particle/RNG pixel replay.
Parallel Flame/Ripple workers produce separate matching warm PNG/depth captures.
MistFog additionally runs 240 original ticks through puff respawns, checkpoints
midcycle at tick 120 and repeats exactly with its native RNG and pixels.
The static control test samples CampSign and Globe at all three frames, verifies
owner translation moves visible pixels, and restores the exact original image.
All four Fountain leaves advance 96 native ticks through delay/rise/shrink/
respawn, checkpoint at 48 ticks and replay exact RNG/state/pixels. Their four
captures have different colors, and invalid/mismatched selectors fail.
`current-profiles-batch-verification.json` records matching captures across all
nineteen sessions' repeated requests without any worker restart.
The coordinated PartsysProbe
refactor retained the production A/B: 40 cases, 19,800 checked fields, zero
differences in `recon/retail_asm/runtime/partsys-controls-refactor-ab.json`.
That report verifies bounded state and pose/color, not VFX pixels or map fidelity.

[The exact-ID coverage map](README-coverage.md) keeps exposed controls, bounded
state/pixels, actual runtime/context, modern GPU and full acceptance separate.
Regenerate it after registry/ledger updates; current exposed/rendered counts37/36
do not change the full acceptance count0.
