# Createfood dynamic angle-lookup boundary

Exact `Createfood` `0x838cffba`, `Magic\\Createfood.I3D`, resolves the original
animator factory `4f4d00`, Init `4e0040`, Animate `4e0260`, Render `4e0650`.
The null-spell branch is valid. Actual owner Init `4dfdf0` / Pulse `4dfe30`
creates one FOOD request at60 and the animator kills the owner at80.
The independently parsed shipped FOOD catalogue has19 entries; original RNG
selects the requested type. Object creation and text are observed service
boundaries, not a claim of a spawned gameplay item.

The complete original lifecycle/full RenderObject probe found283 Render
vector calls spilling into an adjacent dynamic table. The first is tick24,
angle385/distance61, reading Y from `0x635146`. Original `46db20` directly
indexes DistX `634d44` and DistY `634f44` without masking the growing angle.
After the two256-entry arrays, `635144..635543` is MMXLightTable.

This is not immutable padding. Complete original MakeColorTables `41dff0`
executes with only Display::BitsPerPixel supplied as16. Actual SetLightColor
`41da10` then rebuilds the adjacent MMX table under three declared ambient
levels, all four white lights and multiplier28. The literal original
angle600/distance50 result changes from(953,2359,0) at ambient0 to
(1355,2559,0) at32 and(2459,3162,0) at128. The valid angle255 control remains
(-1,-50,0) in all three cases. Source `colortable.cpp` independently identifies
the same mutable MMX lighting producer.

No production code or global ConvertToVector behavior is changed. A future
exact Createfood adapter needs explicit initialized scene/light-table context
for this original lookup quirk; silently wrapping or using zero-filled adjacent
memory is not a faithful reference. Earlier partial-initialization images are
historical diagnostics. Appearance credit is held; no actual Metal comparison
or natural caster/inventory/device acceptance is claimed.

Reproduce with `tools/retail_runtime/createfood_native_preflight.py` and the
pinned original executable. The tool checks actual85 lifecycle records, the
food request, kill80/blank tail, Render state/RNG invariance, all original
vector access addresses and the changing/unchanged controls. Resource/COM,
base bookkeeping, silent audio, owner script/flag notifications and food/text
services remain explicit boundaries.

Private manifest: `research/vfx-wide-20261010/hard-astra/`
`createfood-angle-boundary-final/manifest.json`, SHA256
`6554efea3993f9a036ea4f45fa29ee6bbc360f0acaa8884f99fcd5a27adba531`.
