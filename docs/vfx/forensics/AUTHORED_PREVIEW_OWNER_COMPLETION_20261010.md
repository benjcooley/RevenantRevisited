# Stop authored particles when the owner completes

Quick, Restorelife, Fairy, Labback, Dexterity and Charm now check the real owner
Alive flag immediately after NextFrame/Pulse, before advancing PartSys.
Antimagic already carries this guard. Original effect Pulse `4de800` sets kill
before the controller advance; executing that final simulation step consumed
unwanted RNG after completion. Quick and Restorelife can reach this through
their nonlooping default states; looping adapters also need the guard if their
owner dies during Pulse.

The active lifetime, authored pose, render order and appearance inputs are
unchanged. `test_authored_preview_lifecycle.py` extracts and compiles all seven
actual Advance functions with ASan/UBSan. It checks the live, kill and subsequent
ticks, including particle-step and RNG-call counts. Seven negative controls
remove only the new post-Pulse guard and reproduce the extra step/RNG call.
This test covers scheduling at the owner boundary, not the already-tested
particle simulator or a new natural-caller/appearance acceptance.

Validation: `python tools/retail_runtime/test_authored_preview_lifecycle.py`
passes. No shared renderer, parser, simulator or viewer change is included.
