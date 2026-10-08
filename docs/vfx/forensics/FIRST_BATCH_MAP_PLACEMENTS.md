# First-batch effects in their shipped map settings

Audited 2026-10-04 by reading the original compressed `data/Modules/Ahkuilon.rvm`. No guest input, rendering, map edits or source/data changes were performed. The complete placement/context inventory is [first-batch-map-placements-20261004.json](/Users/benjamincooley/RevenantRetailLab/first-batch-map-placements-20261004.json).

**Capture the actual BlueFont pair at the Labyrinth altar first.** It is the quickest new in-situ reference: two saved objects, real altar/walkway tiles, original colored lights and known area settings. The existing City torch setting is the next useful Flame reference. Neither recommendation constitutes rendered visibility or fidelity acceptance; inspect a native software screenshot before recording.

## Scan coverage and exact effect IDs

All **4,896** `Map/<level>_<sectorx>_<sectory>.DAT` members parsed to exact EOF with no errors: 4,684 v15, 139 v14, 35 v13, 27 v1, nine v12 and two v10 sectors. The first-batch serialized placements are:

- Base **Flame** `0x50ba373b`, `Magic\Flame.I3D`: **784** records across 19 levels. The JSON preserves every member, slot, position and serialized map index.
- **BlueFont** `0x557c4738`, `Misc\Sparkle.I3D`: **two** records, both in level 58, sector `(9,7)`.
- **CyanFont** `0x22491405`: **zero static records**, but a shipped forest script creates it at the Styxx chest.
- **RedFont** `0x335a2516`: **zero static records**, but shipped Sabu cinematics create it in levels 45 and 46.
- **GreenFont** `0x446b3627`, **Mist** `0x2093487a` and **Fizzle** `0xab8800dd`: **zero static records** and no exact-name references in the module's `.s`/`.def` text scan. Their natural runtime/cast use still needs tracing; this scan does not establish absence from the game.

The four fountain types share `Misc\Sparkle.I3D`; separate type IDs choose the color objects. Colored Flame asset aliases are outside this scan's base-Flame ID. Do not replace real scene lights or mounting with arbitrary ambient or placement calibration.

## Priority 1: BlueFont at the level-58 altar

Profile ID: `bluefont-labyrinth58-altar` in the JSON.

- Module **Ahkuilon**, area **The Labyrinth**, level **58**, member **`Map/58_9_7.dat`**, sector **`(9,7)`**.
- Saved **BlueFont** `0x557c4738`, slot **6**, position **`(10193,7859,489)`**, serialized map index **1444484948**.
- Saved **BlueFont** `0x557c4738`, slot **7**, position **`(10132,7858,489)`**, serialized map index **1712920419**.
- Both records have object version 0 and flags `0x4c009`. Preserve their saved positions/flags; they are not fresh editor spawns or synthetic fixture records.
- Original area ambient is **24**, RGB **`(250,150,250)`**. `area.def` also specifies background effect **`labback`** and `labyrinth.s`. Keep this scene context in both runs; a changing background effect may invalidate an assumption of an entirely static background.
- Initial paired camera suggestion: **`[58,10162,7858,416]`**. XY is the midpoint of the saved pair; Z is the actual nearby altar/walkway floor. This is a framing inference, not a calibrated projection offset or claimed original camera state.

The actual **LabAltar** `0x84110a39`, slot 24, is at **`(10153,7855,416)`**, map index **1913553995**, asset `Labyrnth\LabAltar.I2D`. Nearby **LabWalkWay** `0x84ff0516` tiles include `(10177,7905,416)`, `(10176,7808,416)`, `(10080,7904,416)` and `(10080,7808,416)`. This is an altar/walkway setting with the sparkles mounted 73 units above the floor, not empty ground.

Genuine nearby `Light` tile records (`0xa4dc0069`, `Town\TwnLight.I2D`) have RGB **`(10,150,200)`**, intensity **220**, multiplier **12**, zero local offset:

- Slot 23: **`(10077,7801,456)`**, map index **1371815687**.
- Slot 22: **`(10067,7921,456)`**, map index **1237597955**.
- Slot 25: **`(10151,7949,526)`**, map index **437159137**.
- Slot 21: **`(10170,7958,456)`**, map index **969162453**.

The adjacent `Map/58_10_7.dat` contains additional walkway and lights; include neighboring sectors as the normal map loader does. The JSON retains the nearest eight lights for each effect. Do not hide `Light`: doing so removes the actual pools and changes the comparison.

Sector SHA-256: `ab1ee123c57467602d3a40fdf0af3b037b40f25f59085f0c866adc5ba55f4c25`. Both saved effect bodies, tile IDs and light definitions are retained in the JSON. Obtain the original native screenshot at this camera before asserting that walls, widgets or the background effect leave an unobstructed ROI.

### Actual capture evidence: diagnostic, acceptance open

The parent has now captured this shipped Labyrinth setting. Both the [native software manifest](/Users/benjamincooley/RevenantRetailLab/captures/runs/sw-insitu-01-bluefont-labyrinth/manifest.json) and [paired comparison manifest](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-insitu-bluefont-labyrinth-20261004/manifest.json) report **`complete_diagnostic`**. They use the proposed camera and original ambient/color. The [comparison image](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-insitu-bluefont-labyrinth-20261004/comparison.png) and [elapsed-time video](/Users/benjamincooley/RevenantRetailLab/captures/ab/sw-insitu-bluefont-labyrinth-20261004/comparison-elapsed.mp4) retain the full setting for inspection.

- [x] Native software recording acquired for the real altar setting over approximately **18 host seconds**; no scene objects were hidden and no map changes were saved, as recorded in the native manifest.
- [x] Port sector/runtime capture uses the original Ahkuilon module, camera `[58,10162,7858,416]`, ambient `[24,250,150,250]` and saved BlueFont placements. Each serialized map index **1444484948** and **1712920419** attaches **exactly once**, with `colorobj=3` and its original position in the retained log lines.
- [x] Port capture produced **150 images**, of which **121 whole-scene images differ**. This establishes changing scene output through actual sector loading; it is not an isolated proof of each sparkle's animation or fidelity.
- [x] Native diagnostic sampling retained **540 samples at 30Hz**, with **21 distinct whole-scene images / 20 changes**. These changes include actors, lights/widgets and background; they are not exact effect simulation ticks.
- [ ] Obtain a responsive, sufficiently paced native reference and repeat the protocol. Software rendering of the full scene is very slow: the native manifest records approximately **0.8 update/render FPS** and a **500ms window-probe timeout** during this run. Preserve the diagnostic designation; the changing video does not turn the failed readiness probe into a passed health gate.
- [ ] Verify guest archive/member bytes against the retained original sector hash. The paired manifest records the host module hash, but **no exact guest-byte verification is recorded for this capture**.
- [ ] Verify effect-specific animation, same runtime timing and complete lighting/projection/occlusion parity. The full-map paired output still exposes scene lighting, camera/projection and cadence differences; retail editor widgets remain visible.
- [ ] Accept full visual/runtime parity. Both manifests explicitly leave visual fidelity and final acceptance **false**.

Native AVI SHA-256: `c081c44deb7f6fda0d05fbbcc3821e4f98979a1aed840141f0fe42b40040bff1`. Paired port binary SHA-256: `517788209cd912c2556c27a142474b5f167d6238dda753b7a3359db4e23a04fc`. The comparison is elapsed-time only, with no phase search, RNG synchronization, position fitting or scale fitting. Its full-scene mean absolute error **26.80** is a diagnostic result, not an acceptance threshold or isolated BlueFont score.

## Priority 2: Flame mounted on City dungeon torches

Profile ID: `flame-city41-torches`. This reuses the previously exercised real scene coordinates, with a fresh software reference required.

- Module **Ahkuilon**, area **The City of The Children**, level **41**, member **`Map/41_10_10.dat`**, sector **`(10,10)`**.
- **Flame** `0x50ba373b`, slot **17**, saved position **`(10449,10475,98)`**, serialized map index **2137626654**.
- **Flame** `0x50ba373b`, slot **18**, saved position **`(10757,10479,98)`**, serialized map index **2138397532**.
- Paired camera **`[41,10603,10477,16]`**; original area ambient **24**, RGB **`(150,150,250)`**, script `dungeon.s`.

For the first flame, the actual **DunTorch4** `0x8447068b`, asset `Dungeon\DunTorch4.I2D`, slot 255, is at **`(10471,10497,16)`**, map index **1622448082**. Nearby column/wall records include DunColS `(10432,10480,16)` and DunWallColS `(10432,10480,128)`. Preserve their occlusion and mounting relationships.

Closest original lights have RGB **`(255,100,10)`**, multiplier **10**:

- `(10477,10509,102)`, slot 93, map index **809260990**, intensity **250**.
- `(10450,10606,102)`, slot 16, map index **2121676019**, intensity **250**.
- `(10736,10448,64)`, slot 95, map index **1077696496**, intensity **220**, adjacent to the second flame.

Sector SHA-256: `75752638016aca201bf2ed968d6c63d71d1ab9134b5e6c076a2d16ed0621b1b7`. The retained [map-torches_city.png](/Users/benjamincooley/RevenantRetailLab/fixtures/map-torches_city.png) is useful for recognizing the scene; its older hardware rendering and editor widgets make it unsuitable as an accurate accepted reference. Preserve lighting and acquire a fresh software clip.

## Priority 3: CyanFont at the original Styxx chest script location

Profile ID: `cyanfont-styxxchest-script-context`. This is a genuine shipped script setting, not a saved fountain object.

- Module **Ahkuilon**, area **The Forest**, level **0**, sector **`(4,22)`**, member **`Map/0_4_22.dat`**.
- **ForStyxxTomb** `0xcef50cde`, `Forest\ForStyxxTomb.I2D`, saved position **`(4465,23078,39)`**, serialized owner map index **177155495**.
- **STYXXCHEST**, type RunChestE, saved position **`(4268,23080,39)`**, serialized chest map index **177156133**.
- `forest.s`, `OBJECT "FORSTYXXTOMB"`, lines **237–256**: its unlocked `ALWAYS` branch reveals the chest, consumes the rose, runs **`ADDAT 4268 23080 105 CYANFONT`**, and pauses the owner.
- Original day ambient **30**, RGB **`(250,250,250)`**. Actual **Forgggg2** forest-floor tiles nearby are at **Z32**. Initial camera suggestion **`[0,4268,23080,32]`** is based on those tiles.

**Retail ADDAT's third number is the amount, not Z.** The parent traced the script dispatch through `TScript` to the same command-table row `0x5c6ec0` and handler `0x421770`: `105` feeds `SetAmount`; one CyanFont object is created, with Z inherited from the live `MapPane.centerZ`. Record the actual camera/spawn Z in the native capture. There is no serialized CyanFont map index to invent.

Original nearby blue lights include `(4279,23111,70)`, `(4394,23106,80)` and `(4266,23256,80)`: RGB **`(0,0,255)`**, intensity **255**, multiplier **12**, zero local offset. Those colors matter for the scene-lit sparkle. Triggering the actual unlocked script is stronger evidence than manually adding the effect; if the exact shipped command sequence is replayed for a reference, label it as a scripted-context fixture until the natural trigger is verified.

## Priority 4: RedFont in the Sabu cinematics

No static RedFont record exists. Preserve the real caller, camera and short lifecycle rather than treating it as a persistent map fountain.

- **Sabu5**, level **45**, area **The Chase of Sabu**, member **`Map/45_6_8.dat`**: original owner position **`(7152,8656,0)`**, map index **302906579**. `dungeon.s:763–808` triggers when the player enters the scripted cube, centers on Sabu5, then at lines **782–785** adds RedFont, moves it **+100 Z**, waits **24 ticks**, and deletes it. The owner-relative spawn follows the current cinematic context; do not assume the initial serialized owner position remains its position at the spawn frame.
- **Sabu6**, level **46**, area **The Slave Camp**, member **`Map/46_9_9.dat`**: original owner position **`(10001,9458,145)`**, map index **571417564**. `dungeon.s:903–906` runs **`ADDAT 10001 9458 144 redfont`**, moves it **+100 Z**, waits **48 ticks**, and deletes it. Here **144 is amount**; the spawned Z inherits the live camera center, and final Z is that value plus 100. There is no saved RedFont map index.

The Sabu6 setting has real raised **Dunffff** floor tiles at **Z144**, including `(10000,9424,144)`. Initial camera suggestion **`[46,10001,9458,144]`** reflects that floor; record the original cinematic camera when verifying the natural sequence. Original area ambient is **24**, RGB **`(150,150,250)`**. Nearby actual red lights are `(10000,9450,241)`, RGB `(200,10,10)`, intensity **255**, multiplier **10**, and `(9977,9535,57)` / `(10028,9562,221)` with the same color/multiplier and intensity **100**.

Sabu character records use two nested base-version bytes before the common object fields (`LOAD_BASE(TComplexObject)` then `LOAD_BASE(TObjectInstance)`). Their high-bit name bytes are retained in JSON and normalized to low seven bits for script-owner lookup; the actual record bytes and positions are preserved. Skipping those base-version bytes prevents the nonsensical coordinates that a universal body-offset parser would produce.

## Proposed next environmental map batch

These are **offline placement proposals only**. No new native capture, port render or acceptance claim accompanies this scan. Use original saved records and surrounding geometry/lights; choose the initial screenshot before deciding which scene is practical under the current slow full-map software renderer.

1. **Existing Caverns Drip regression setting first**: Ahkuilon level **30**, `Map/30_7_9.dat`, area The Caverns, original ambient **12 RGB `(155,155,210)`**. Saved **Drip** `0x5975abde`, slot **229**, position **`(8159,9591,102)`**, serialized map index **1693528533**. Reuse the previously exercised camera **`[30,8159,9591,229]`** and source parameters `(20,200,35)`; a fresh software actual-setting clip would close the remaining native-reference gap for the repaired Drip/Ripple path. The sector has 152 occupied records and includes repeated fgeyser objects elsewhere, so whole-scene changes alone cannot establish the drip/ripple lifecycle.
2. **Drip plus saved Ripple pool setting**: level **31**, `Map/31_1_11.dat`, The Spiral Arm, original ambient **15 RGB `(155,155,210)`**. Saved Drip `0x5975abde`, slot **414**, position **`(1469,11667,209)`**, map index **1102777057**. Three saved Ripples `0x12309867` surround it: slot **10** `(1469,11667,209)` / **1589734810**; slot **48** `(1463,11672,209)` / **1589735707**; slot **63** `(1472,11663,209)` / **1589724874**. Real pool tiles include CavFlrPoolBot2 `(1475,11661,152)` and CavFlrP03 `(1447,11689,148)`. Initial camera **`[31,1469,11667,152]`** is a framing inference from the pool tile, not a correction to the saved effect Z. This sector also contains FlowWater, StillWater, BendWater and geysers; preserve those relationships and use effect-specific ROIs.
3. **Canal/fall composition with several variants in one original setting**: level **48**, `Map/48_18_19.dat`, The Pit, original ambient **26 RGB `(150,150,250)`**. WaterClft `0xd0c0f038`, slots **15/16**, positions **`(19074,19978,148)`** / **`(19168,19978,148)`**, map indices **224724730 / 225837827**; WaterFlft `0xd0c0f036`, slot **20**, **`(19299,20013,124)`**, index **231055555**; MistFog `0x180674ba`, slot **293**, **`(19350,20044,16)`**, index **2032347848**. The actual DunCanalEW tiles are at Z16, with dungeon stairs and transition tiles; this tests mounting/occlusion rather than an empty-plane particle preview. Suggested wide initial camera **`[48,19212,20013,16]`** is inferred from the saved composition. This scene contains 159 occupied records.
4. **Small oil-geyser source setting**: level **30**, `Map/30_7_6.dat`, The Caverns, ambient **12 RGB `(155,155,210)`**. fgeyser `0xad92bd31`, slot **22**, **`(8144,7148,170)`**, index **878222926**, sits at actual CavOil03 `(8150,7152,164)`. Initial camera **`[30,8144,7148,164]`** is inferred from that tile. The sector has only **33 occupied records**, but **11 separate saved fgeyser records** occur there; retain the original repeated layers and their unique map indices rather than deduplicating them into one emitter. This is a promising simpler setting for testing retail-layer behavior, with speed and neighboring-sector cost still unmeasured.
5. **Literal Waterfall type mapping check**: the only saved Waterfall `0xa907dabf` is level **31**, `Map/31_4_9.dat`, slot **273**, **`(5041,9503,-96)`**, index **95962744**. Actual CavWater01 tiles are at `(5040,9488,-128)` and `(5024,9488,-128)`; The Spiral Arm ambient is **15 RGB `(155,155,210)`**. Suggested camera **`[31,5041,9503,-128]`** comes from those tiles. Its retail asset is **`misc\Water.i3d`**, so resolve its actual animator/asset mapping before treating a WFall harness alias as the same effect. This is a denser 307-object scene with many nearby geyser layers, making it a lower priority until capture performance improves.

The same complete archive pass found **22 Drip**, **14 Ripple**, **one Waterfall**, **nine WaterFlft**, **eight WaterFrt**, **two WaterClft**, **11 WaterCrt**, **three MistFog**, **1,433 sgeyser** and **3,536 fgeyser** saved records. These are record counts, including intentional or historical same-position layers, not unique visible emitters or accepted effect variants. They demonstrate that genuine environmental settings are available without inventing test-map placements.

## Capture and A/B checklist for these settings

- [ ] Use one healthy retail process and software rendering, with the parent retaining the only guest controller.
- [ ] Verify the guest consumes the same original compressed member bytes as the port. Save hashes for the module/member and any save-path sector that could shadow it.
- [ ] Load the exact module/level/camera and original area ambient/color. Preserve tiles, actor state, all lights and any `BGEFFECT`; inspect a native screenshot to confirm the expected setting and occlusion.
- [ ] For saved Flame/BlueFont, compare the serialized objects themselves; do not replace them with `SpawnForTest`, move them or save editor changes.
- [ ] For Cyan/Red scripted usage, record actual caller state, command arguments, inherited camera Z and lifecycle. Do not interpret ADDAT amount as height.
- [ ] Acquire the native software clip first. Retain visible widget provenance until a widget-only hiding method is proved; `Light.toggle invisible on` invalidates the light comparison.
- [ ] Capture the port's actual sector/runtime path with identical saved placement/camera/area settings. Verify the exact type ID and serialized map index attach and animate through normal loading.
- [ ] Compare correct geometry, material, color, timing, light interaction, mounting and foreground/background occlusion. Separate capture completion, runtime regression and visual parity acceptance.

## Serialization and reproducibility evidence

The scan uses genuine headers and body fields documented in [../../../recon/docs/SECTOR_FILE_FORMAT.md](../../../recon/docs/SECTOR_FILE_FORMAT.md), [../../../src/sector.cpp](../../../src/sector.cpp), [../../../src/object.cpp](../../../src/object.cpp), [../../../src/object.h](../../../src/object.h), [../../../src/character.cpp](../../../src/character.cpp), [../../../src/complexobj.cpp](../../../src/complexobj.cpp) and [../../../src/lightdef.h](../../../src/lightdef.h). The existing lab helper `fountain_runtime_fixture.py` supplies the verified v15 record/body layout; the scan extends version gates instead of byte-searching for effect IDs.

For v14+, the header includes the state hash; each object's `blocksize` covers body plus inventory, and `invblocksize` carves the inventory tail out of that total. The scanner skips that total exactly once. v10/12/13 omit the hash/inventory-tail field. The 27 v1 sectors contain tile/exit records without block sizes; their genuine old base-body and exit-tail layouts also reached exact EOF. Names are not used as record-boundary guesses.

Nearby context uses the same level's surrounding 3×3 sectors, actual tile/light classes and decoded serialized light definitions. It does not infer light radius, exposure or visible pixels. Full positions, type IDs, signed map indices, body/member hashes, nearby tiles/lights, area settings, script-owner records and ranked capture profiles are retained in JSON.

Original archive SHA-256: `557e0307e0c619846b29d783d044e92994fdcaa9297171fdd7f3ff55ad5a4b2c`. The scan establishes exact placement provenance, not rendered visibility or completed A/B acceptance.
