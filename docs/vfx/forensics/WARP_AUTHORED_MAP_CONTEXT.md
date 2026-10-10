# Saved Warp doors and barriers in their original maps

The read-only original Ahkuilon module scan locates 27 saved colored Warp doors,
eight LabGate barriers and140 Speaker records across4,896 sectors. Serialized
positions/flags do not establish rendering or trigger acceptance.

`map_effect_placements.py` is the reusable locator. Select exact retail IDs:

```sh
python tools/retail_runtime/map_effect_placements.py \
  --type-id 0xad92bc28 --type-id 0xad92bc37 --type-id 0xad92bc38 \
  --output /tmp/new-warp-placement-inventory.json
```

This parses candidate top-level EFFECT records, preserves member/slot/position,
map index and flags, and records module/sector hashes. It never writes module
data. The selected blue/barrier inventory contains11 records with no errors.

## Actual saved blue door capture

- Ahkuilon, The Labyrinth level53, `Map/53_9_9.dat`, slot28.
- `TeleportDoorInsideB`, type `0xad92bc28`, saved map index1473492260.
- Saved position `(9954,10004,559)`, flags `0x0004c001`.
- Sector-mode camera `(53,9954,10004,559)`, original area ambient40,
  RGB `(0,250,125)`.

The actual port loaded that saved identity once and passed the typed position
observation. Its 24-frame capture has16 distinct scene images and a clean exit.
Visual inspection shows the animated blue triangular portal in its authored
gate, with original terrain, mounting and surrounding lights retained. No
objects were spawned, moved, removed or saved for this capture. Writes were
isolated under a private save path.

Local evidence:
`research/vfx-next-20261009/warp-blue-authored-sector/manifest.json` in the retail
lab. This is an authored saved-map smoke, separate from the original-software
leaf comparisons. Full original-scene A/B, background/story execution and the
natural teleport trigger remain unverified; no new frontend/full acceptance
credit follows from this context capture.

## Hidden originals matter

All eight saved LabGate barrier records have `OF_INVISIBLE` (`0x80`). Their
untextured red editor geometry proof is useful, but normal-game visibility must
follow the saved flags. The level57 hub also contains invisible/paused portal
companions alongside visible doors. Preserve these relationships when building
an in-situ scenario; do not replace them with freshly spawned visible copies
and call that the original scene.

Level57 `Map/57_10_8.dat` contains visible yellow/orange/blue/green/purple doors
and nearby hidden barriers. `Map/57_10_9.dat` contains white/red doors and hidden
barriers. These are practical next contextual references once the isolated
controller and current port checks are complete.
