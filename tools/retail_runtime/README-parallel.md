# Independent retail builds and parallel scenarios

Agents can run **different modified retail executables at the same time**. Each
named session has its own persistent Mac process, guest memory, virtual files,
clock and warm checkpoint. Each agent owns a private workspace and can publish
many named builds. There is no shared mutable emulator or global current binary.
The thin runtime is the primary VFX test loop. Normal state, geometry, motion
and structured-control tests do not start DOSBox-X or use editor UI. DOSBox-X
with real emulated 3D may be used separately for targeted graphics-device or
software-rendering diagnostics, with coordinated guest ownership. It is not
an automatic fallback when a required thin-runtime capability is missing;
record that capability and implement it or test another reproducible effect.

The original retail instructions run in Unicorn. The runtime does not boot
Windows. Concurrent processes and binary instrumentation are working; full
retail startup remains incomplete. Bounded original software VFX pixels are
available through [typed controls](README-controls.md). The first sample below
validates render-state functions and worker isolation. Full modern-renderer and
map acceptance remain separate [runtime capability gates](README.md).

## Private workspaces and many named builds

Run these from the inner `RevenantRevisited` repository:

```sh
PY=/Users/benjamincooley/RevenantRetailLab/research/retail-asm/venv/bin/python
"$PY" tools/retail_runtime/variants.py create /tmp/revenant-agent-flame \
  --baseline recon/retail_asm/baseline/Revenant.rebuilt.exe \
  --source recon/retail_asm/baseline
```

Choose a different, empty workspace for each agent. Creation copies the PE and
all `.asm`/`.inc` files into independent storage, never writable hardlinks. Edit
`/tmp/revenant-agent-flame/source/retail.asm` or its included sections. Do not
edit the repository baseline or a published executable. Optional source copying
can be omitted when an agent will import a separately instrumented PE instead.

Write a JSON patch manifest identifying addresses, changes and purpose, then:

```sh
"$PY" tools/retail_runtime/variants.py build /tmp/revenant-agent-flame \
  --name trace-flame --patch-manifest /tmp/flame-patches.json \
  --nasm /opt/homebrew/bin/nasm
"$PY" tools/retail_runtime/variants.py build /tmp/revenant-agent-flame \
  --name no-collision --patch-manifest /tmp/collision-patches.json \
  --nasm /opt/homebrew/bin/nasm
"$PY" tools/retail_runtime/variants.py list /tmp/revenant-agent-flame
```

The outputs are `builds/<build-id>/Revenant.trace-flame.exe` and
`builds/<build-id>/Revenant.no-collision.exe`. Rebuilding `trace-flame` creates a
**new build ID and directory**, preserving earlier artifacts. Assembly failure
publishes nothing and preserves every successful build. These experimental
builds intentionally do not use the baseline's byte-identity promotion guard.
Published files are read-only; edits belong in source and produce new builds.

Every `build.json` records the build name/ID, actual executable SHA256, parent
retail baseline SHA256 and patch manifest. Assembly builds also record source
hashes. The unchanged baseline SHA is
`28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5`.
An ancestry record identifies the source baseline; it does not itself verify
that a behavioral patch is correct. Verify changes against the recorded patch
manifest and original code.

To publish a modified PE produced by the existing hook tools:

```sh
"$PY" tools/retail_runtime/variants.py import /tmp/revenant-agent-flame \
  --name trace-flame --executable /tmp/Revenant.instrumented.exe \
  --patch-manifest /tmp/flame-patches.json
```

## Parallel jobs from JSON

The checked-in sample runs two sessions against the same unchanged retail image:

```sh
"$PY" tools/retail_runtime/parallel.py \
  --jobs tools/retail_runtime/scenarios/parallel_render_state.json \
  --artifacts /tmp/retail-parallel \
  --output /tmp/retail-parallel-result.json
```

A manifest has `sessions` and `jobs`. Each session has a unique `name`, one
`build` record or path to `build.json`, and its own one-time `setup` array. Each
job has a `session`, optional `name`, and a `scenario` object or JSON-file path.
Paths to scenario/build files and runners are relative to the job manifest.
Relative executables are resolved against their build JSON file's directory,
or the job manifest for inline records. Python callers can use `load_manifest`
to apply the same resolution. For different private builds, use this structure:

```json
{
  "sessions": [
    {"name": "flame", "build": "/tmp/revenant-agent-flame/builds/BUILD_ID/build.json", "setup": []},
    {"name": "sparks", "build": "/tmp/revenant-agent-sparks/builds/BUILD_ID/build.json", "setup": []}
  ],
  "jobs": [
    {"name": "flame-frame-001", "session": "flame", "scenario": "/tmp/flame-scenario.json"},
    {"name": "sparks-frame-001", "session": "sparks", "scenario": "/tmp/sparks-scenario.json"}
  ]
}
```

The runner loads each image once, verifies its SHA in the worker's handshake,
then restores that worker's checkpoint before every scenario. Embedded scenario
`setup`, if present, must exactly match the session setup. A scenario's pinned
`executable_sha256` must match its selected build. A baseline-pinned scenario
cannot silently run against a patched variant; explicitly review its addresses
and update the pin. If the scenario omits the pin, the pool pins it to the
selected build before transmission.

Use `--repeat N` to replay the full job batch while retaining every worker and
checkpoint. Request IDs and artifact directories continue across batches;
the result report records each batch's time and status. The mixed
`scenarios/current_profiles_batch.json` runs all current typed state/pixel
profiles; see [the direct controls instructions](README-controls.md).

Jobs for different sessions run concurrently. Jobs for the same session execute
in input order. Results return in original input order with deterministic IDs
`job-000000`, `job-000001`, etc. IDs continue across batches in a retained pool.
Each result identifies session, PID, build ID, SHA, ancestry, patch manifest and
artifact directory. Each pool creates a unique session directory, so different
agents cannot overwrite each other's captures. Relative capture `output` paths
are resolved beneath each job's directory; absolute paths and `..` are rejected.
The root `result.json` path is reserved for job metadata.

Ordinary scenario errors return a failing result and roll back without killing
the worker. Other jobs continue, and the same worker accepts later requests.
`--timeout` sets the wall deadline per worker request (default 30 seconds). A
stalled or broken worker is killed; its next job starts a new process with the
same pinned build/setup. A crashed worker is also replaced independently.
Process cleanup occurs when the batch CLI exits or the Python pool closes.
Requests never invoke guest OS boot or DOSBox controls.

## Retain workers across batches and reload one build

The Python API can retain the pool between batches. Import from
`tools/retail_runtime` using the same venv as `run.py`:

```python
import json
from pathlib import Path
from parallel import Pool

flame = json.loads(Path("/tmp/flame-build.json").read_text())
sparks = json.loads(Path("/tmp/sparks-build.json").read_text())
with Pool(sessions=[{"name": "flame", "build": flame},
                    {"name": "sparks", "build": sparks}],
          artifact_root="/tmp/retail-captures") as pool:
    first = pool.execute([{ "session": "flame", "scenario": flame_scenario },
                          { "session": "sparks", "scenario": sparks_scenario }])
    # Publish a new named build first; replacing one session is explicit.
    new_flame = json.loads(Path("/tmp/new-flame-build.json").read_text())
    pool.reload("flame", new_flame)
    flame_scenario["executable_sha256"] = new_flame["executable_sha256"]
    second = pool.execute([{ "session": "flame", "scenario": flame_scenario }])
```

`reload` creates a fresh checkpoint in the selected session; every other session
keeps its process/state. Old captured results retain their exact build IDs and
hashes. Each explicit reload gets a separate load directory for build metadata,
setup and setup captures, preserving earlier setup artifacts too.
Modifying a loaded published file instead of publishing a new build is
detected by pre/post-batch hashing and rejected.

For one shared unchanged image, the shorter API is
`Pool(executable, workers=2, setup=setup_operations)`. Unrouted jobs are assigned
round-robin. Such unregistered images have no asserted baseline ancestry;
use workspace build records when tracking modified reference binaries.

## Verification

```sh
"$PY" tools/retail_runtime/test_parallel.py \
  --output recon/retail_asm/runtime/parallel-verification.json
```

Seven tests cover two warm simultaneous workers executing original render-state
calls, deterministic results, failed-job recovery, capture isolation, a worker
crash, wall deadline cleanup, private source copies, failed assembly preserving
old artifacts, and different named modified images coexisting. Test-only
variants force DESTBLEND to 6 versus 2; reloading one forces 4 while the other
remains 2. They are instrumentation examples, not VFX fixes.
Build-file-relative executable paths are resolved and exercised through a
private copied baseline without relying on the process current directory.
Temporary test workspaces are deleted afterward; their hashes and observations remain in the
verification report. These isolation tests do not establish VFX visual parity;
the typed pixel profiles and their separate comparisons supply bounded original
frames. Full modern-renderer/map acceptance remains a further gate.
