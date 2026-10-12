# Ordered Metal reference captures

The old synchronous and asynchronous readbacks created separate Metal command
queues. Waiting for the copy command buffer did not establish a dependency on
the renderer's writes. Several earlier VFX films retain a rare previous-frame
image as an explicitly disclosed capture residual; they were not phase-aligned
to conceal it.

Readback now uses the renderer's borrowed command queue. Sokol enqueues each
render command buffer when it is created, so a later asynchronous copy cannot
overtake a frame awaiting commit. Synchronous reads reject an uncommitted render
frame instead of blocking behind it. Successful reads require completed Metal
commands; asynchronous failures release their slots without exposing data.
BGRA-to-RGBA conversion and complete output-buffer checks are retained.

This follows Metal's documented ordering: command buffers execute in their
enqueue order within one queue. See Apple's
[command submission model](https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Cmd-Submiss/Cmd-Submiss.html)
and [resource synchronization](https://developer.apple.com/documentation/metal/resource-synchronization).
No VFX simulation, seed, geometry, lighting, shader, or phase-selection policy
changes.

`tools/test_metal_readback.mm` exercises the actual Sokol renderer and production
readback implementation on the GPU. A shared-event gate deliberately holds each
render while reads are submitted. All256 changing-color frames pass synchronous
pixel/rectangle and asynchronous checks, including pending-frame rejection,
busy-slot rejection, small-buffer rejection and slot reuse. A separate BGRA
target verifies all three read APIs. The old readback implementation, compiled
against the same delayed-render harness, fails the expected-pixel assertion.

Run on macOS:

```sh
clang++ -std=c++17 -fobjc-arc -I thirdparty/sokol \
  tools/test_metal_readback.mm src/platform/macosx/renderer_readback.mm \
  -framework Metal -framework Foundation -framework QuartzCore \
  -o /tmp/revenant-metal-readback-test
/tmp/revenant-metal-readback-test
```

Private runtime replay evidence is under
`RevenantRetailLab/research/vfx-readback-20261011`. Repeated captures compare
identical frame indices; a mismatch is a failure, never a request to search
another phase. This infrastructure correction grants no effect appearance
credit by itself.

The rebuilt game passes three140-frame EnergySpray native-domain runs with
identical hashes at every index (420frames total). Existing three-frame prelit
and three-frame default diagnostics are byte-identical to their prior movies.
`repeat-proof.json` records this result on binary
`194b06023e01e9cfb29579fd04e37408d6165844754040880d5fc2e485df8a7b`.
The GPU harness and negative-control outputs remain alongside the private
runtime logs; source and this record are the repository deliverables.
