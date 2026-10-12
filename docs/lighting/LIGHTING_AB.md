# Lighting A/B across the game: Classic locked to retail, modern aligned

Status: Classic locked to retail at every location with a good retail shot;
the modern model fitted and its defaults chosen (2026-10-12). Results in §8;
open items at the end of §8.1.

Goal, from the author:

- **Classic** (`[Revisited] LightingMode = 0`, the default) draws every place
  in the game with the same light as retail: identical, not similar.
- **Modern** (`LightingMode = 1`, the deferred sun, AO and normals) looks good
  and is roughly as bright and as vivid as retail everywhere, tuned through
  the Revisited overlay (`revisited/resources/`) and not by changing Classic.

[LIGHTING_FIDELITY.md](../LIGHTING_FIDELITY.md) holds the forensics of both
retail paths and the one location checked so far (shot S1, the Keep's pit,
where Classic is within 3 of 255 on every static surface). This plan extends
that check to the whole game and adds the modern alignment.

## 1. What "retail" is

Retail lights the map one of two ways, picked by `[Options] RealTimeLight`
(LIGHTING_FIDELITY §1):

| `RealTimeLight` | Retail path | Who sees it | Classic today |
|---|---|---|---|
| No  | DLS light table, per pixel (MMX transfer) | the dosbox-x lab | reproduced (S1) |
| Yes | the light grid, per vertex, through the 3D device | **the shipped GOG INI** (with `GammaLevel = 4`, `EnhancedLighting = Yes`, `Software3D = No`) | not reproduced |

Both paths are retail. The port reads `RealTimeLight` (default Yes, as
retail), but Classic always draws the No path. So:

- Every location is captured in the lab with **both** settings. The No
  shots are Classic's exact target now. The Yes shots measure how far the
  shipped default look is from it, and are the reference the modern mode is
  judged against for brightness and vividness (what players remember).
- Whether Classic then also draws the grid when `RealTimeLight = Yes` (so
  that Classic is retail for any INI) is decided from the Yes shots and the
  grid forensics (§6.3). Until then a Classic run is configured with
  `RealTimeLight = No` for comparisons.

Lab settings (both configurations): `Software3D = Yes`, `GammaLevel = 3`,
`EnhancedLighting = No`, `MaxLights = 3`, `Ambient3D = 130`,
`LightRange3D = 180`, `LightMult3D = 250`; 640×480; CPU `pentium_mmx`.
The port runs with the same keys in its INI and `--resolution=640x480`.

## 2. Locations

Every AREA in the module's area.def gets key points (`tools/lighting_ab/
areas.py` writes `locations.json`, which the other tools read):

- **Landmarks** where the area has one, found with the map search
  (`find.py`: instance or type name over the module's sectors): the
  Keep's pit (shot S1) and Tendrick's hall, Gow's council, Harowen,
  Ollihoot, the Solifuge, the Recluse, Andria, the Blue Druhg, the Ogrok
  Torturer and Chef, the Ogrok arena (Slagg, Mort, Urgg), Shegra and the
  Druhg workers, Jhaga, Nahkranoth, the Labyrinth's markers, Yhagoro,
  Lucas's puppet show, Gus's bar, Navarro, and the surf (Wave effects) on
  the forest coast, Arakna Island and the Ruins. The player stands 32
  units in front of it.
- **The best-lit floor spot** (`scan_lights.py`) where there is none
  (Kragg Mountain, the Infested Swamp, the Underground Pyramid, the
  Nahkranoth Lair level 47).
- A key point must be in its area: on its level, inside its RECTs and
  outside every later area's (the last match wins).
- Areas with a night (the Forest, the Ruins, Misthaven) are shot at noon
  and at 01:00.

39 locations cover all 30 areas with a map; six AREA blocks (levels 73,
83, 100-102, 205) have no map in the Ahkuilon module.

## 3. Slots

`tools/lighting_ab/make_slots.py` copies the base slot (the `postopen` New
Game slot: the opening done, control on) once per location and patches its
`game.sav` with `tools/savefmt/revsave.py`'s field offsets:

- `player[0].pos.x/y/z` and `player[0].level`: the location;
- `header.gametime`: noon (`40000 + k·80000`, timeofday 720) or 01:00
  (`3333 + k·80000`); the day is `DAYLENGTH 80000` hundredths of a second, so
  the flat day and night stretches last minutes of play, far longer than a
  capture takes.

Everything else, the map's `CurMap` sectors included, is the base slot's. A
location's sectors aren't in it, so both games read them from the module.
The lab gets the base slot uploaded once (its `CurMap`); after that only a
location's `game.sav` (~2 KB) goes over the serial link.

## 4. Capture

**Retail** (`tools/lighting_ab/retail_capture.py`, through
`tools/retaillab/retail.py`): `start` with the configuration's INI keys,
then per location: upload `game.sav`, load the slot from the title screen,
wait for the map to settle (~6 s), capture. The title-screen load avoids
retail's in-game-load ambient bug (LIGHTING_FIDELITY §2.5). Never press OK
in Options during a run. `restore` at the end.

**Port** (`tools/lighting_ab/port_capture.py`): per location and lighting
mode, `--headless --resolution=640x480 --quickstart=<slot>`, a snapshot
after the same settle time. Modern runs use `--revisited` with
`REVENANT_REVISITED_PATH` pointing at the overlay under test, so the
committed overlay and a tuning candidate can be run side by side.

Shots and a manifest (settings, commit, timestamps) go in
`~/RevenantRevisited-lightab/<run>/`; selected pairs are committed under
`docs/lighting/ab/`.

## 5. Comparison

`tools/lighting_ab/compare.py` aligns each port shot to its retail shot
(integer offset search, ±16 px, on the map pane), masks the HUD (side panel,
bottom bar, status corner), and reports:

- **Classic vs retail (No):** per-pixel |Δ| on the map pane: the share of
  pixels within 3 of 255 per channel, the median and the 95th percentile;
  a diff heat map. Characters, effects and animated objects show up as
  compact blobs and are reviewed by eye, not counted against lighting.
  **Pass:** ≥ 95 % of static pixels within 3.
- **Modern vs retail (Yes and No):** mean luminance, the 10th/50th/90th
  luminance percentiles, mean chroma (saturation), and the mean hue of
  pixels above the 75th luminance percentile (the light pools). **Pass:**
  mean luminance and the 90th percentile within ±15 % of retail Yes, chroma
  within ±20 %, pool hue within 15°; plus an eyeball check of each
  side-by-side, because the modern mode adds form (N·L, AO, shadows) that
  retail never had.

Each run writes a markdown table (one row per location) and side-by-side
strips (retail No | Classic | retail Yes | modern).

## 6. Work

### 6.1 Already known wrong in both modes (fix first)

The day clock. Both are retail-confirmed and change outdoor light:

1. **Time of day.** Retail `TPlayScreen::Animate` (`0x0047c2c0`,
   `0x0047c3a5`–`0x0047c3de`): `timeofday = (gametime·1440 / daylength) mod
   1440`, `day = gametime / daylength` (`+0x694`), with `DAYLENGTH 80000`
   (an 800 s day). The port's `TimeOfDayMinutes` makes one game minute per
   real second (a 1440 s day, the wrong phase for any saved time).
2. **Daylight.** Retail `TPlayScreen::Daylight` (`0x0047ea00`) is the 1998
   function: 255 by day, 0 by night, linear ramps of `TWILIGHT` (60) minutes
   centred on 06:00 and 18:00; with `RealTimeLight` off the ramp moves in
   `twilightsteps` (8) steps, with it on smoothly. The port uses a cosine
   (`TPlayScreen::Daylight`), so most of the day is neither full day nor
   full night, and `DayTimeFlag` uses invented hour buckets.

### 6.2 Classic lock-in

Run every location with retail No. For each failing location, find the
cause (a missing object draw, a light the port doesn't submit, the 16-light
cap, an ambient path) in the forensics before changing code, then re-run
all locations. Classic changes must keep S1 passing.

### 6.3 The light grid (`RealTimeLight = Yes`)

Decided after the Yes shots: if the grid look differs from the No look by
more than the modern tolerance anywhere, Classic should draw the grid when
the INI asks for it. Its forensics are half done (LIGHTING_FIDELITY §3.4:
`FUN_0045bd50` builds the grid with the DLS falloff, the 1.375 reach and
the `K` gains; the draw `FUN_0045c8a0` wasn't closed). The emulator
(`tools/retail_runtime`) can run the grid build on a fixture and the lab
shots fix the output scale. It is a separate piece of work with its own
plan.

### 6.4 The modern model

The first modern model (an ambient of `AMBLIGHT * Ambient3D / 20`, authored
lights at `LightMult3D / 10000` per unit of multiplier over 1.8x their
reach, a 1.5 ceiling) read dim and flat against retail: the torch pools'
hue was off by 80-280 degrees and saturation down by a third. The model now
starts from retail's own levels and adds modern form on top; every term is
a `[Revisited]` setting (`SRevisitedSettings`, applied to the map renderer,
which re-sends its lighting state to TRenderer every frame -- the earlier
INI never reached it beyond the lighting mode).

**Tiles.** Ambient: retail's tile ambient gain (AMBLIGHT with the gamma
offset, AMBCOLOR scaled to a largest channel of 1, before the light table's
byte truncation) times `AmbientScale`, times AO. Authored lights: retail's
gain per unit of multiplier (`10 (1 - A/255) / 20`), retail's reach and
falloff (the iso metric and pow curve), each light's own colour (no
four-colour limit), shaped by N.L (`NormalHardness`). By day the sun takes
`SunShare` of the ambient over and gives it back on faces turned to it.
Tiles saturate per channel as retail's do, through a shared tone curve
(`ToneShoulder`: linear to the knee, then a soft roll-off to 1).

**One scale for tiles and 3D objects.** A tile pixel is its surface's
reflectance times the light baked into the art (the author: the tiles were
rendered with baked light near mid range, to keep their dynamic range and
colour). Dividing that level out (`TileBakedLevel`) and expressing light
as retail's tile gain times it leaves every tile exactly at retail's level
and gives 3D objects the same light in the same units.

**3D objects.** Retail lit them with a different model (the 3D scene's
ambient and a strong white key light, no map lights under `RealTimeLight =
No`), which is why its characters stand out from the dark rooms in natural
colours. Modern gives a 3D object:

- the light the tile under it gets (the ambient, the sun and the map's
  lights at its base -- its height above its base rides in the G-buffer
  normal's alpha -- summed without N.L), shaped by retail's key-light
  direction with the total conserved (`CharacterKeyLight`: faces toward it
  get up to 1 + k, faces away 1 - k), its colour partly neutralised at
  equal luminance (`CharacterLightSaturation`), scaled by
  `CharacterLightBoost`;
- plus retail's own 3D lighting at `CharacterRetailLight` of its strength.

Effects (EFFECT-class objects and effect meshes) cast no sun shadow
(`kObjFlagNoShadow`); they still receive it.

**Variants tried** (2026-10-11, all 31 locations with a good retail shot;
ratios to retail RealTimeLight=No, geometric mean over the set):

| | Settings | Luma | p90 | Chroma | Characters, by eye |
|---|---|---|---|---|---|
| ref | the first model rebased on retail levels, no rescale, hard clip | 1.01 | 0.97 | 0.90 | overlit near torches (pink, white) |
| a | `TileBakedLevel` 0.5, shoulder 0.8 | 0.96 | 0.93 | 0.90 | still hot |
| b | a + an added key light on 3D objects | 0.99 | 0.96 | 0.90 | closest overall, too bright by torches |
| c / c2 | a key light on everything (c2 rebalanced) | 1.30 / 1.10 | | 0.84 / 0.85 | flattens the tiles |
| e / f | the tile's light at the object (f: at its base), key form 0.5 | 0.98 / 1.05 | | 0.90 / 0.87 | brighter than b |
| f25 | f, `TileBakedLevel` 0.25 | 0.95 | 0.93 | 0.90 | right level, solid orange in torch light |
| g35 | f, baked 0.35, half the light's colour | 0.96 | 0.94 | 0.90 | natural skin, the author's pick |
| h1 | g35 + retail's own 3D lighting, half the local light | 1.01 | 0.97 | 0.89 | retail-like, warmest by torches |
| **h2** | **g35 + retail's own 3D lighting, a quarter of the local light** | **0.99** | **0.95** | **0.89** | **closest to retail; the default** |
| h3 | g35 + 0.75 of retail's 3D lighting, half the local light | 0.99 | 0.95 | 0.89 | slightly dimmer, warmer |

h2 is the default: `revisited/resources/Revenant.ini` (read with
`--revisited`) sets `LightingMode = 1` and its values, and
`SRevisitedSettings` carries the same numbers. Why the characters needed
retail's own model: retail lit them with a near-full white key light
(`min(1, level x 0.85 x 3)` saturates in most areas) whatever the local
light, so they stand out from dark rooms in natural colours; lighting them
only with the floor's light makes them darker and tinted. `LightCeiling`
was removed (the tone curve replaced the mesh cap).

The side-by-side images are in `~/RevenantRevisited-lightab/variants/`
(`tools/lighting_ab/sheet.py`).

### 6.5 Deliverables

- `tools/lighting_ab/` (slots, both captures, compare) and the run recipe
  in this doc.
- The day clock fixes, with tests.
- Classic passing at every location against retail No; the S1 table and a
  per-location table in §8.
- The tuned modern overlay (INI and area.def), with the side-by-side strips.
- The light-grid decision (§6.3) with the numbers behind it.

## 7. Open questions for the author

None yet. Questions about how a place should look are answered with lab
shots, not memory.

## 8. Results

### 8.1 Classic against retail (RealTimeLight=No), 2026-10-11

`tools/lighting_ab/compare.py`, run v3 (merged main). "Blocks": 16x16
blocks of the map whose mean colour is within 4 of 255 of retail's in every
channel. Characters, monsters and effects stand where they stand in each
capture, so 100% isn't reachable; the static scene is.

| Location | Blocks | Luma retail / Classic | Chroma retail / Classic |
|---|---|---|---|
| keep-pit (S1) | 86% | 48 / 50 | 129 / 129 |
| keep-hall | 79% | 52 / 54 | 166 / 166 |
| keep-gow | 87% | 33 / 34 | 124 / 123 |
| tower-harowen | 85% | 37 / 39 | 133 / 137 |
| arakna-gus | 90% | 42 / 44 | 208 / 205 |
| caverns-ollihoot | 92% | 48 / 49 | 85 / 84 |
| kragg-mountain-32 | 89% | 39 / 40 | 93 / 93 |
| infested-swamp-39 | 73% | 48 / 50 | 86 / 85 |
| crypt-andria | 90% | 54 / 56 | 112 / 112 |
| pit-jhaga | 84% | 26 / 27 | 136 / 136 |
| children-druhg | 92% | 29 / 30 | 105 / 105 |
| horrors-torturer | 90% | 27 / 28 | 116 / 116 |
| interiors-chef | 88% | 33 / 35 | 131 / 131 |
| sabu-ogrok-arena | 79% | 36 / 38 | 119 / 117 |
| slavecamp-shegra / workers | 88% / 90% | 42 / 44, 36 / 37 | 125 / 124, 120 / 120 |
| nahkranoth-lair-47 | 94% | 14 / 15 | 179 / 178 |
| lab-green / blue / yhagoro | 93-98% | 7-21 | within 2 |
| misthaven-bar | 88% | 15 / 16 | 63 / 66 |
| forest-waves day / night | 83% / 82% | 114 / 114, 54 / 58 | 115 / 113, 198 / 187 |
| ruins-waves-night | 80% | 56 / 60 | 196 / 186 |

What the Classic fixes were (all retail-confirmed):

- **The day clock** (§6.1): time of day from DAYLENGTH, retail's stepped
  Daylight and DayTimeFlag, SetGameTime setting the minute at once (a noon
  save had entered the Forest at night).
- **Four light colours** (LIGHTING_FIDELITY.md §2.6): lights take one of the
  light table's four ids in retail's draw order and a pixel shows the
  colour of the last light over it. The Caverns went from 49% to 92% of
  blocks and their pool hue from orange to retail's lilac.
- **128 lights** in the light pass (was 16; caves put up to 80 in view).

Open: the Labyrinth's red, yellow and purple levels draw the void without
the area's background effect in this build (chroma 86 / 135 / 106 against
125 / 171 / 133); the effect is reported working on main since. The Ruins
by day, Misthaven and the Labyrinth's orange level need retail re-shots
(the lab hung loading them). The light grid (`RealTimeLight = Yes`) is
not yet captured.

