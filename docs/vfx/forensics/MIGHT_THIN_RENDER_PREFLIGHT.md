# Might rendered thin-runtime preflight

2026-10-08. The bounded readiness check found a missing adapter boundary,
not a graphics-device defect. No rendered A/B or new acceptance is claimed.

The current reusable PartsysProbe executes native Pulse and particle Render
pose sampling, but stops at `4028e2`. Continuing that original Render call
reproduces `UC_ERR_READ_UNMAPPED` at precisely that address: the synthetic
owner has no vtable for its virtual `+24` animator accessor. Consequently
native `RenderObject40a8f0` and the prototype's actual geometry submission are
not reached. The synthetic prototype also reserves only `200` bytes while
native Render writes its blend mode at `+348`; a full renderer adapter needs
the real `34c`-byte object layout.

The ten-slot synthetic controller is not Might's literal authored controller.
Retained Might proofs cover directly seeded 31-slot or measured quality1
seven-slot pools, native curves and pose/color sampling. They explicitly omit
the full retail parser/controller initialization and prototype/world raster.
The production SubmitPartSys path has a raw-center/local-model Z bridge for
the modern renderer. A rendered comparison must state and verify that domain
handoff rather than feed modern world coordinates into a different projector.

Next implementation step: bind a correctly sized prototype, owner/animator
accessor and imagery interface; execute actual native prototype matrices for
the exact shipped `#rect`, and compile the production SubmitPartSys packet
body with the same upstream inputs. Then connect the existing software raster.
Device fidelity is a separate later gate.

Reproduce the immediate boundary with `tools/retail_runtime/might_render_preflight.py`
using a local verified retail EXE, imagery archive and a unique `--output` JSON.
Source paths are repository-relative; no external research scripts are imported.

An actionable simpler alternative is literal **Water `0x1903abcd`**, distinct
from Waterfall. Its registered native builder is `66ce98`, installed by
`4f32aa`, with builder vtable `5ad4e0`. The existing Waterfall fixture can guide
its direct native drop Init/Animate/Render adapter. Its source drop machine
uses delay, translation and respawn rather than authored curve parsing;
native cardinality and actual leaf functions still require verification.
