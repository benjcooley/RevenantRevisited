# TPlyrStatusBar — Reconstruction SPEC

> Forensic reconstruction contract for the player + current-target stat-bar HUD
> panel. Produced per [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md). The proven text/bar coordinate truths are
> imported from [WORKED_EXAMPLE_text_coords.md](WORKED_EXAMPLE_text_coords.md).
>
> All paths are relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`.
> Every number states its coordinate space and cites `file:line` + `hex (= dec)`.

---

## §0 — Sources & status

**Class:** `TPlyrStatusBar` = `cls_0x5a54e4`, vtable @ `0x5a54e4`. Single global
instance @ `0x65a8c0` (`DAT_0065a8c0`); **no second instance** — the panel draws
BOTH sides in one paint (confirmed B.r6 / B.r8, see §1).

**Paint method:** vtable slot **23** = `FUN_0054af20` @ `0x0054af20` (size 3909).
This is the full two-pass paint. Slot 7 (`FUN_0054ab80`) is a partial bar-only
redraw that shares the same bar code.

**Recon files read:**
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp` — paint (slot 23)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot7_partialdraw_54ab80.cpp` — partial bar redraw (slot 7)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot19_state_549da0.cpp` — animation-counter tick (slot 19)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot20_Animate_549e60.cpp` — portrait surface mgmt + side helpers (slot 20)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp` — ctor/surfaces (slot 0)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_Close_549d40.cpp` — surface free (slot 1)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PlayerSide_54a0a0.cpp` — `FUN_0054a0a0` chrome+icons (player)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_TargetSide_54a310.cpp` — `FUN_0054a310` chrome+icons (target)
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PortraitDraw_54ae10.cpp` — `FUN_0054ae10` name/level string builder
- `recon/discovered/FUN_0054a5d0_TPlyrStatusBar_BarRender_Helper.cpp` — `FUN_0054a5d0` 4-section bar fill
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_vtable.txt` — vtable dump
- `recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp` — pane position/size driver

**Prior briefs / notes consulted:**
- `docs/ui/briefs/B_r3_playscreen_panes.md` (class id), `B_r6_target_charpane_and_base.md`
  (single-instance two-pass confirmation), `recon/discovered/port_status/TPlyrStatusBar.md`
  (object-field map).
- `docs/ui/forensics/WORKED_EXAMPLE_text_coords.md` (authoritative text/bar coords).

**Existing port shell:** `src/uiplyrstatusbartest.cpp` (`--test=ui-plyrstatusbar`).

### Snapshot note (supplementary — superseded by retail)

The 1998 snapshot `src/statusbar.{h,cpp}` (`TStatusBar`/`THealthBar`/
`TStaminaBar`) is an **earlier, superseded** design — standalone **vertical
"tube"** bars at the screen edges, not the retail horizontal three-bar chip
this spec documents. It is supplementary context only; the reconstruction agent
builds from the retail sections (§2–§14). The only carry-over *intent* worth
noting: the level→target lerp ramp (`src/statusbar.cpp:32-66`,
`HEALTH_INCREMENT`) and the per-bar hue maps (`src/statusbar.cpp:81-90`,
`:105-108`). Both the layout and the hue-rotation mechanism are superseded by
retail (horizontal `Bars`-atlas bright/dim bands, §6b). Do NOT use the tube
coordinates (`src/revdefs.h:185-193`).

**Status:** `forensics-complete` for the Classic (640×480, `DAT_006680c8 == 0`)
software-composite path — every coordinate, asset, helper, text cell, shadow,
and animation pinned. UNCONFIRMED items (§14) are: the exact char-object
bar-source getter return semantics (vtable `+0x1d8/+0x1e0/+0x1e8`), the real
Locke portrait (`chars\locke.i3d`, I3D decoder deferred), and the `+0x40` of
target-name scratch column derivation (decoded from disassembly in the worked
example, not re-disassembled here). The hi-res (`DAT_006680c8 != 0`) path uses
the same coordinates through the mosaic vtable blits and is documented inline.

**2026-05-19 corrections (frame-graph hardening):** this revision adds the
mandatory §3 **Frame table** (NOMENCLATURE §1 / FORENSICS_PROTOCOL §3),
recomposes the target chrome / Ring / icon coordinates that were previously
printed in *chrome-surface-local* numbers under a "chip-local" label (the
dropped chrome-origin link that put the right panel `0x41` off — errors #5/#6),
and adds a **v-align** column to §8 with the multi-line name/level stack
clarified as TOP-aligned (errors #7). Player values were already correct
(player chrome origin = chip-local 0) and are unchanged.

### Errors to close (current `src/uiplyrstatusbartest.cpp` vs this spec)

1. **Text Y position.** The harness draws stat values top-anchored at
   `kTextY[3] = {0x07,0x17,0x24}` (`uiplyrstatusbartest.cpp:517`) which IS the
   recovered cell Y — correct. But it converts top→baseline via measured glyph
   bounds (`DrawStringTTFTopShadowed`, `:477-490`). Retail draws into a
   `0x32×0x0e` cell with GDI vertical metrics, not glyph-bbox-derived baselines.
   The spec mandates: place the cell at the recovered `(x,y,0x32,0x0e)` and let
   the text renderer top-align inside it (no `&0x20` vcenter bit set → top). See §8.
2. **Name/level centering.** The harness centers each line independently in a
   64px box with a hand-picked 2-line layout (`kLineTop[2]={0,14}`,
   `:560`). Retail composes a **single** `"%s\nLevel %d"` string with
   `font_id 0x402` (center) into a `0x40×0x40` cell — GDI `DrawTextA` does the
   centering and the `\n` line break. The spec gives the cell + format; the
   port must center the whole 2-line block in `0x40×0x40`, not 2 separate
   single-line centerings. See §8.
3. **Bar four-slice fill.** The harness reimplements the 4-segment math
   (`DrawBar`, `:248-306`) with its own bright/dim selection heuristics. The
   spec gives the exact `FUN_0054a5d0` algorithm + the full per-bar literal-arg
   table for all 3 bars × both sides (§6). The harness's `dir`-based seg layout
   is approximately right but the cap/tail widths and the bright/dim split
   thresholds must follow the helper exactly.
4. **Shadow direction.** The harness uses a fixed `+4,+4` screen→chrome
   normalization for both sides (`kBarScreenToChromeX/Y = -4,-4`, `:93-94`).
   Retail player side casts `(+4,+4)`; **target side casts `(pane_w-0x88, +4)`
   i.e. a mirrored down-LEFT shadow** (`FUN_00438d80(buf, pane_w-0x88, 4)` at
   slot23 `:641`). The per-side shadow offset differs — see §7.
5. **Target right panel placed `0x41` too far left.** A prior reconstruction
   read the target chrome/Ring/icon coords as if they were chip-local, but they
   are **chrome-surface-local** (the helper draws into the `+0x6c` chrome mosaic;
   that mosaic composites at chip-local `0x41 = 0xc1 − 0x80`). The dropped
   `0x41` chrome-origin term shifted the whole right card. The spec now gives
   target chrome/Ring/icons composed to chip-local AND final screen (§3 Frame
   table + §4). (TargetSide `FUN_0054a310` `:18,28,33-36` chrome-local; chrome
   origin chip-local `0x41` from WORKED_EXAMPLE "BackPanel local x = 0x41".)
6. **Target Ring/icon coords mislabeled as chip-local.** Same root cause as #5,
   specific to the §4 element rows — fixed there.
7. **Name/level vertical centering.** The harness implies the 2-line label is
   vertically centered in the `0x40×0x40` cell. Retail uses GDI `DrawTextA`
   `DT_TOP` (no `DT_VCENTER`): the two lines stack DOWN from the cell top by
   `lineHeight`; the lower part of the 64×64 cell stays EMPTY. `0x402` is a
   HORIZONTAL center only. §8 now carries an explicit v-align column (= TOP).

---

## §1 — Overview

`TPlyrStatusBar` is the top-of-screen HUD panel that shows two character
"status chips": the **player** chip anchored to the left edge and the
**current target** chip anchored to the right edge. Each chip shows a circular
portrait in a gold `Ring`, three horizontal stat bars (Health / Mana /
Fatigue) with their numeric values, three stat icons, and a centered
name + level label.

It is **one pane, one paint, two passes** (B.r6/B.r8 confirmed; no target
instance exists in the binary —
`docs/ui/briefs/B_r6_target_charpane_and_base.md:13`). Character binding is
implicit through globals, not an API:
- **Player** = `DAT_00667fcc` (the active player character object).
- **Target** = `DAT_00667fcc[0x38]` → if that object's type is `3` or `0x19`,
  its `[0x11]` field is the bound target character (`piStack_e8`)
  (slot23 `:64-71`). If null, the target side falls back to
  `FUN_00452690_TMapPane_GetInstance` (slot23 `:623-628`).

**The pane (full chip area):**
- Position/size: full display width × `0x70 (= 112)` tall, anchored to screen
  top. `TPlayScreen::Pulse` sets the pane height field `_DAT_0065a8e0 = 0x70`
  and the width field `_DAT_0065a8dc = display_width − DAT_0066615c`
  (`cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:278-281`). `DAT_0065a900` is the
  pane's visible flag.

**Visibility:**
- Whole panel gated on the player existing: slot 23 returns immediately if
  `DAT_00667fcc == 0` (`:61-63`); slots 19 and 20 also early-return on null
  player.
- **Target chip** is only drawn when a target character is resolved
  (`piStack_e8 != 0`, slot23 `:222`); else the target half is skipped.
- The whole pane is shown/hidden by `TPlayScreen` (HUD up only in play mode).

**Plain-language description:** Two small character cards sit in the top
corners. Each has a round gold-framed face on the left, three colored
horizontal gauges (red health, purple mana, orange fatigue) stacked to the
right of the face with a small icon at the left of each gauge and the current
value printed in white over the gauge, and the character's name + level
centered under the face. The left card is the player; the right card mirrors
the layout and shows whatever you are currently targeting; it fades in/out as
the target changes.

---

## §2 — Asset roster

All from `statusbarnotex.dat` (Classic / no-texture build). Dump command:
`python3 tools/ui/dump_dat.py <…>/data/resources_unzipped/statusbarnotex.dat`.
Measured dims from the dump header (the bitmap's own width/height words).

| asset | archive | entry | WxH (measured) | flags | role | source rects | cite |
|---|---|---|---|---|---|---|---|
| `BackPanel` | statusbarnotex.dat | `BackPanel` | **128×64** | `0x2` (BM_15BIT) | chip chrome/backdrop | full 0,0,128,64 | dump `[0]`; PlayerSide `:20,25` |
| `Bars` | statusbarnotex.dat | `Bars` | **128×128** | `0x2` | bar-fill atlas (6 bands + decals) | see source-rect map | dump `[1]`; slot23 `:484` etc. |
| `HealthIcon` | statusbarnotex.dat | `HealthIcon` | **24×24** | `0x102` (BM_15BIT+ALPHA) | red heart icon | full | dump `[2]`; PlayerSide `:22,33` |
| `ManaIcon` | statusbarnotex.dat | `ManaIcon` | **24×24** | `0x102` | blue/purple gem icon | full | dump `[3]`; PlayerSide `:23,32` |
| `FatigueIcon` | statusbarnotex.dat | `FatigueIcon` | **24×24** | `0x102` | orange/yellow chevron icon | full | dump `[4]`; PlayerSide `:24,31` |
| `Ring` | statusbarnotex.dat | `Ring` | **44×44** | `0x102` | gold portrait frame | full | dump `[5]`; PlayerSide `:21,30` |
| portrait | chars\locke.i3d | (live face) | (3D render → `+0x130` surface) | — | the real portrait | — | PlayerSide `:16-18` |
| `LockeFace` | portraits.dat | `LockeFace` | **30×30** | `0x2` | **placeholder** portrait only | full | portraits dump `[0]` |

> **Portrait note (mandatory, per task):** the real portrait is the **live 3D
> head render** obtained from the character object's vtable `+0x130`
> (`(**(code**)(*char + 0x130))()`, PlayerSide `:16`; TargetSide `:19`) and
> stamped opaque at chip-local `(0,0)` (PlayerSide `:18`). It comes from
> `chars\locke.i3d` → **needs the I3D decoder (deferred)**. `portraits.dat`
> `LockeFace` (30×30) is a stand-in for the harness only; do not treat it as
> the shipped asset.

**Icons carry alpha** (`flags 0x102` = BM_15BIT|`0x100`), and they are blitted
with `drawmode 0x2000` (`DM_ALPHA`), not magenta key (PlayerSide `:30-33`). The
Ring (`0x102`) likewise blits alpha. `BackPanel`/`Bars` (`flags 0x2`,
`kc=0x0`) are 15-bit; magenta `0x7C1F` is the global key for any keyed pixels
(UI_METHOD_MAP §16) but these blit opaque (`0x80000000`).

### Source-rect map — `Bars` atlas (128×128), measured

Pixel-band scan of `Bars` (Python/Pillow over the dumped PNG) cross-checked
against the `srcY` literals in the bar call sites (slot23 `:484,499,511`):

```
Bars atlas 128x128 (sx counts from left; bars start near x=2..3)
 y  ~3..10  HP  BRIGHT  red    (sample (192,112,88)..(120,40,16))   ← srcY 0x01 = 1
 y ~21..24  MP  BRIGHT  purple (sample (184,128,248)..(88,24,168))  ← srcY 0x13 = 19
 y ~36..39  FT  BRIGHT  orange (sample (232,176,128)..(128,72,16))  ← srcY 0x22 = 34
 y ~50..57  HP  DIM     gray   (sample (48,48,48)..(24,24,24))      ← dimY 0x31 = 49
 y ~68..71  MP  DIM     gray                                         ← dimY 0x42 = 66
 y ~83..86  FT  DIM     gray                                         ← dimY 0x51 = 81
 y ~99..125 decals: red + green "splat" + tiny dots  ← UNUSED by stat bars
```

Notes:
- The HP band is taller (`h = 0x11 = 17`) than MP/FT (`h = 0x0c = 12`) — see the
  per-bar `h` arg in §6. The scan shows HP pixels y3–10 (the visible stroke);
  the helper reads a 17-tall slice starting at the band top.
- `srcX = 2` for every bar (slot23 `:484` arg5). Caps are read from the band's
  left/right ends; the interior is read from `srcX + capW` onward (§6).
- The y≈99–125 decals (splats / tiny digit-like dots) are **not** used by the
  stat-value text (those use the `Small` TTF font, §8) and not by the bars —
  do not sample them.

---

## §3 — Coordinate system & surfaces

### Pane on screen
- Pane rect (**screen**): `x=0, y=0, w=pane_w, h=0x70 (=112)`.
  `pane_w` is the live width field at **`this+0xc`** (NOMENCLATURE §1), set by
  `TPlayScreen::Pulse` to `display_width − DAT_0066615c`
  (`…Pulse_47b4d0.cpp:280`). Slot 23 reads it as `*(int*)(param_1 + 0xc)` for
  all target-side x math (e.g. `:641,657,672,687,692`).

### The chip model (NOMENCLATURE §1)
- **Chip width = `0xc1 = 193` px** (WORKED_EXAMPLE "Whole-Chip Coordinates";
  derived from the target HP bar x = `pane_w − 0xc1`, slot23 `:657`). The chip
  is **wider than the 128px `BackPanel` chrome** because the bars extend right
  of the chrome.
- **Player chip origin (screen) = `0`.** Player coords are used directly as
  pane-local.
- **Target chip origin (screen) = `pane_w − 0xc1`.** A target chip-local x
  converts to screen as `pane_w − 0xc1 + chip_x`. Concretely the target
  destinations are written as `pane_w − offset`:
  - HP bar x = `pane_w − 0xc1` (chip-local `0x00`)  (`:657`)
  - MP bar x = `pane_w − 0x91` (chip-local `0x30`)  (`:672`)
  - FT bar x = `pane_w − 0x79` (chip-local `0x48`)  (`:687`)
  - value cell x = `pane_w − 0x80` (chip-local `0x41`)  (`:534`/text decode, WORKED_EXAMPLE)
  - name cell x = `pane_w − 0x44` (chip-local `0x7d`)  (WORKED_EXAMPLE name decode)
  - chrome (BackPanel) x = `pane_w − 0x80` → chip-local `0x41 = 0xc1 − 0x80` (WORKED_EXAMPLE)

> Forgetting the chip is `0xc1` wide and only the chrome is `0x80` makes the
> target bars land left of where they belong (NOMENCLATURE chip model warning).

### Frame table (mandatory — NOMENCLATURE §1, FORENSICS_PROTOCOL §3)

Every coordinate in this spec belongs to exactly ONE of these frames. The
backbone of the right-panel fix is the **chrome-surface frame**: the chrome/icon
helpers draw into the `+0x6c` mosaic at *chrome-surface-local* coords, and that
mosaic composites at chip-local `0x41` on the target (`0` on the player). A
chrome-local number is NOT a chip-local number — applying (or, on the player,
not needing) the `0x41` term is mandatory.

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | — (root) | — | framebuffer TL `(0,0)` | final pixel positions; `pane_w = this+0xc` (live) | Pulse `:280` |
| **pane** | screen | TL | screen `(0,0)`; size `pane_w × 0x70` | the whole HUD pane rect | Pulse `:278-281`; slot23 reads `this+0xc` `:641` |
| **player-chip** | pane | **TL** | pane-local `0` (constant) | the whole left card (width `0xc1=193`) | WORKED_EXAMPLE "player chip origin = 0" |
| **target-chip** | pane | **TR** | `pane_w − 0xc1` (= `pane_w − 193`, scales with width) | the whole right card (width `0xc1=193`) | slot23 `:657` (HP x = `pane_w−0xc1`); WORKED_EXAMPLE |
| **player-chrome surface** (`+0x6c`→`+0x70`) | player-chip | **TL** | chip-local `0` (constant; player chip origin is screen 0, so chrome-local = chip-local = screen) | BackPanel, live portrait, Ring, 3 icons | PlayerSide `:14,25,30-33`; Init `:69-74` |
| **target-chrome surface** (`+0x6c`→`+0x74`) | target-chip | **TL** | **chip-local `0x41` (= `0xc1 − 0x80`)** ← the dropped link | BackPanel, live portrait, Ring, 3 icons | TargetSide `:18,28,33-36`; WORKED_EXAMPLE "BackPanel local x = 0x41" |
| **text scratch surface** (`+0x64`) | (its own 0x80×0x80) | — | source rows per cell (player `0,0x0e,0x1c`; target `0x2a,0x38,0x46`; names `+0x40` col) | the GDI text-composite cells before blit to pane | Init `:39,48-49`; WORKED_EXAMPLE "Stat Value Cells" |
| **source-rect (Bars / BackPanel / icons / Ring)** | the source bitmap | — | source bitmap TL `(0,0)` | `(sx,sy,sw,sh)` sub-rects copied OUT of the atlases | §2; slot23 `:484` etc. |

**Anchor detection (NOMENCLATURE §2 rule):** the target HP bar x is written
`pane_w − 0xc1` in the decomp (slot23 `:657`) → `parent_w − k` ⇒ **right-anchored
(TR)**, offset `0xc1`. Every target value cell (`pane_w−0x80`), name
(`pane_w−0x44`), MP/FT bar (`pane_w−0x91`/`pane_w−0x79`) is likewise `pane_w − k`
⇒ TR-anchored. The player side reads bare constants (`0x44`, `0x47`, `0`) ⇒
**TL-anchored**, offset = the constant.

**Composition formula to the FINAL frame:**

```
screen_x = chip_origin_in_screen  +  chrome_origin_in_chip  +  x_in_chrome
```

- **Player:** `chip_origin = 0`, `chrome_origin = 0` → `screen_x = x_in_chrome`
  (chrome-local == chip-local == screen; the player numbers were already correct).
- **Target:** `chip_origin = pane_w − 0xc1`, `chrome_origin = 0x41` →
  `screen_x = (pane_w − 0xc1) + 0x41 + x_in_chrome = pane_w − 0x80 + x_in_chrome`.
  (Cross-check: BackPanel chrome-local `0` → screen `pane_w − 0x80`, matching the
  target value-cell x `pane_w − 0x80`, WORKED_EXAMPLE.)

Y has no chrome offset (chrome origin chip-local-y = `0`), so `screen_y = y_in_chrome` for both sides.

### Scratch / mosaic surfaces (from Initialize, Classic branch)
Allocated in `FUN_00549740_…Initialize` (`:28-116`):

| field | size | type | holds | cite |
|---|---|---|---|---|
| `this+0x64` (`+100`) | `0x80×0x80` sprite | scratch text surface | the GDI `DrawTextA` text-composite cell (value + name strings) | Init `:39,48-49`; slot23 text path |
| `this+0x60` | `0x28×0x28` sprite | small scratch (portrait corner / unused in paint) | Init `:58-61` |
| `this+0x6c` | `0x80×0x40` mosaic | **chrome+icons compose buffer** (player & target chip art) | Init `:69-74`; PlayerSide `:14-35` |
| `this+0x70` | `0x80×0x40` mosaic | **player effect/bar buffer** | Init `:82-87`; slot23 `:460` |
| `this+0x74` | `0x80×0x40` mosaic | **target effect/bar buffer** | Init `:95-100`; slot23 `:635` |
| `this+0x68` | bar-atlas-sized sprite | the **`Bars` source bitmap** loaded by name `DAT_005e5724` | Init `:101-115`; bar helper reads `*(this+0x68)` as the blit source `:98` |

**Two-stage blit (NOMENCLATURE §3):** chrome/icons are composed into the
`+0x6c` mosaic (PlayerSide `:14-33`), then that mosaic is blitted onto the
chip's display surface (`+0x70` player / `+0x74` target) via `(*+0x5c)`
(PlayerSide `:34-35`, TargetSide `:37-38`). Text is drawn into the `+0x64`
scratch then blitted to the live pane via `(*+0x5c)` (slot23 `:548-575`). The
final destination is always the stage-(b) `SDrawParam.dx/dy`, NOT the
stage-(a) draw position (the worked-example trap).

**Direct-renderer contract for the port (NOMENCLATURE §3):** compose the whole
chip (chrome + ring + portrait + icons + bars + text) into ONE fixed
render target, then blit that surface during the HUD swapchain pass. Do not
mix a bitmap path and a glyph path to the swapchain.

### Layout diagram (chip-local; player chip, origin = screen 0)

```
chip-local x:  0x00            0x2b   0x41(chrome L)         0x80(chrome R)        0xc1
               ├──────┬─────────┼──────┼──────────────────────────┼────────────────┤
   y=0  ┌──────┤ Ring  44x44    │      │ BackPanel chrome 128x64   │  (bars overhang)│
        │ face │ ctr(0x1a,0x1f) │ icon │  drawn at chrome-local 0  │                 │
   y=3  │ live │ tl ≈ (4,9)     │ HP   │  HP bar  dst(0x44,0x0f) w 0x77 h 0x11        │
   y=7  │ I3D  │                │ 24²  │  HP value cell (0x47,0x07) 0x32x0x0e  white  │
  y=0x11│ at   │                │ MP   │  MP bar  dst(0x44,0x1f) w 0x45 h 0x0c        │
  y=0x17│ (0,0)│                │ 24²  │  MP value cell (0x47,0x17) 0x32x0x0e  white  │
  y=0x20│      │                │ FT   │  FT bar  dst(0x44,0x2c) w 0x2d h 0x0c        │
  y=0x24│      │                │ 24²  │  FT value cell (0x47,0x24) 0x32x0x0e  white  │
  y=0x36│      │  name+level cell (0x00,0x36) 0x40x0x40, centered, white              │
        └──────┴────────────────┴──────┴──────────────────────────────────────────────┘

Player bars cast shadow DOWN-RIGHT (+4,+4).  (icon x = 0x2b = 43 on player side;
player chrome origin = chip-local 0, so chrome-local == chip-local == screen.)

Target chip = x-mirror: origin screen = pane_w-0xc1; chrome surface composites at
chip-local 0x41. Same Y rows. Bars cast shadow DOWN-LEFT. The TargetSide helper's
chrome-LOCAL icon x = 0x3a (TargetSide :34-36) composes to chip-local 0x41+0x3a =
0x7b (= 123), screen pane_w-0x46. Ring chrome-local center 0x62 → chip-local 0xa3,
screen pane_w-0x1e. Do NOT use the chrome-local 0x3a/0x62 as chip-local (the §4 fix).
```

---

## §4 — Static element layout

**Every (x,y) below is a FINAL, COMPOSED, single-space coordinate.** Player
chrome/ring/icons: chip-local == screen (player chip origin = 0), shown as
chip-local. Target chrome/ring/icons: the recon value is *chrome-surface-local*
and the chrome composites at chip-local `0x41`; both the **composed chip-local**
(`0x41 + chrome_local`) AND the **final screen** (`pane_w − 0x80 + chrome_local`,
per §3 formula) are given. Bars and text cells are already screen / pane-local in
the recon (player coords ARE pane-local; target coords are `pane_w − offset`).

| element | space | player (x,y) | (w,h) | target — chip-local | target — screen | source rect | cite |
|---|---|---|---|---|---|---|---|
| live portrait (I3D) | chip-local→screen | `(0,0)` | sprite dims | `(0x41,0)` | `(pane_w−0x80,0)` | from `+0x130` | PlayerSide `:18`; TargetSide `:21` |
| `BackPanel` chrome | chip-local→screen | `(0,0)` | 128×64 | `(0x41,0)` | `(pane_w−0x80,0)` | full 128×64 | PlayerSide `:25`; TargetSide `:28`; WORKED_EXAMPLE "BackPanel local x = 0x41" |
| `Ring` (centered) | chip-local→screen | center `(0x1a,0x1f)`=(26,31) → tl `(0x04,0x09)`=(4,9) | 44×44 | center `(0xa3,0x1f)`=(163,31) → tl `(0x8d,0x09)`=(141,9) | center `(pane_w−0x1e,0x1f)` → tl `(pane_w−0x34,0x09)` | full 44×44 | PlayerSide `:30`; TargetSide `:33` |
| `HealthIcon` | chip-local→screen | `(0x2b,0x03)`=(43,3) | 24×24 | `(0x7b,0x03)`=(123,3) | `(pane_w−0x46,0x03)` | full | PlayerSide `:33`; TargetSide `:36` |
| `ManaIcon` | chip-local→screen | `(0x2b,0x11)`=(43,17) | 24×24 | `(0x7b,0x11)`=(123,17) | `(pane_w−0x46,0x11)` | full | PlayerSide `:32`; TargetSide `:35` |
| `FatigueIcon` | chip-local→screen | `(0x2b,0x20)`=(43,32) | 24×24 | `(0x7b,0x20)`=(123,32) | `(pane_w−0x46,0x20)` | full | PlayerSide `:31`; TargetSide `:34` |
| HP bar | screen | `(0x44,0x0f)`=(68,15) | w `0x77`, h `0x11` | chip-local `0x00` | `(pane_w−0xc1,0x0f)` | Bars (§6) | slot23 `:484` / `:657` |
| MP bar | screen | `(0x44,0x1f)`=(68,31) | w `0x45`, h `0x0c` | chip-local `0x30` | `(pane_w−0x91,0x1f)` | Bars (§6) | slot23 `:499` / `:672` |
| FT bar | screen | `(0x44,0x2c)`=(68,44) | w `0x2d`, h `0x0c` | chip-local `0x48` | `(pane_w−0x79,0x2c)` | Bars (§6) | slot23 `:511` / `:687` |
| HP value | pane-local→screen | cell `(0x47,0x07)` | `0x32×0x0e` | chip-local `0x41` | cell `(pane_w−0x80,0x07)` | scratch src `(0,0)` / `(0,0x2a)` | WORKED_EXAMPLE "Stat Value Cells" |
| MP value | pane-local→screen | cell `(0x47,0x17)` | `0x32×0x0e` | chip-local `0x41` | cell `(pane_w−0x80,0x17)` | scratch src `(0,0x0e)` / `(0,0x38)` | WORKED_EXAMPLE |
| FT value | pane-local→screen | cell `(0x47,0x24)` | `0x32×0x0e` | chip-local `0x41` | cell `(pane_w−0x80,0x24)` | scratch src `(0,0x1c)` / `(0,0x46)` | WORKED_EXAMPLE |
| name+level | pane-local→screen | cell `(0x00,0x36)` | `0x40×0x40` | chip-local `0x7d` | cell `(pane_w−0x44,0x36)` | scratch src `(0x40,0)` / `(0x40,0x40)` | WORKED_EXAMPLE "Name/Level" |

> **Target chrome/Ring/icon recomposition (the §5/§6 fix — these were the
> mislabeled values).** The TargetSide helper `FUN_0054a310` draws into the
> `+0x6c` chrome mosaic in *chrome-surface-local* coords: BackPanel `(0,0)`
> (`:28`), Ring center `0x62` (`:33`), icons `0x3a` (`:34-36`). The mosaic is
> blitted into the `+0x74` target display surface (`:37-38`); that surface is the
> target chip whose origin is screen `pane_w − 0xc1`, and the chrome composites
> at chip-local `0x41`. Per §3 (`screen_x = pane_w − 0x80 + x_in_chrome`):
> - BackPanel:  chrome `0x00` → chip-local `0x41` → screen `pane_w − 0x80`.
> - Ring center: chrome `0x62` → chip-local `0x41+0x62 = 0xa3` → screen `pane_w − 0x1e`; tl = center − (44/2=`0x16`) → chip-local `0x8d`, screen `pane_w − 0x34`.
> - Icons:      chrome `0x3a` → chip-local `0x41+0x3a = 0x7b` → screen `pane_w − 0x46`.
>
> The earlier spec printed `0x62`/`0x3a`/`0` under a "chip-local" column — those
> are chrome-local; the `0x41` chrome origin was dropped, shifting the whole
> right card left by `0x41`. Player needs no such term (player chrome origin = 0,
> so its chrome-local values ARE chip-local/screen — they were correct).

**Text alignment (inside the cell):** HP/MP/FT values — **player left**
(`font_id 0x401`), **target right** (`font_id 0x404`); name/level — **center**
(`font_id 0x402`), both sides. See §8. Target value is NOT the player `0x47`
mirrored inside the 128-wide chrome — it sits at `pane_w−0x80` (the left edge of
the right chrome) and right-aligns inside the cell (WORKED_EXAMPLE).

---

## §5 — Draw order / composition

Two vtable slots cooperate (per the port_status map):

- **Slot 20 `FUN_0054a0a0`/`FUN_0054a310` (Animate / chrome build)** — rebuilds
  the chip art when the bound character changes: clear `+0x6c`, stamp live
  portrait, `BackPanel`, `Ring` (centered), 3 icons; blit `+0x6c` → display
  (`+0x70` player / `+0x74` target). See §6.
- **Slot 23 `FUN_0054af20` (paint)** — the per-frame composite below.
- **Slot 7 `FUN_0054ab80`** — partial bar-only redraw (same bar code, used when
  only the gauge fill changed). Not the full paint.
- **Slot 19 `FUN_0054 9da0`** — animation-counter tick only, no drawing (§9).

**Slot 23 ordered sequence** (Classic, `DAT_006680c8 == 0`):

1. Resolve target char `piStack_e8` from `DAT_00667fcc[0x38][0x11]` (`:64-71`).
2. **Player value cells** (each only if its cached stat changed): for HP
   (`+0x1c0`), MP (`+0x1d0`), FT (`+0x1c8`): `sprintf("%d")` into scratch,
   `Box`-clear the scratch cell (`(*+0x64)(0,rowY,0x32,…)`), pack white
   `(0xff,0xff,0xff)` → `FUN_004be2b0` draws the string into the `+0x64`
   scratch (`:73-152`).
3. **Player name/level**: if changed, `FUN_0054ae10` builds `"%s\nLevel %d"`,
   clear scratch `(0x40,0,0x40,…)`, draw centered into scratch `(0x40,0,…)`
   (`:153-204`).
4. Cache current player stats into `param_1+0x98/0x9c/0xa4/0xa0` (`:205-221`).
5. **Target value cells** (if `piStack_e8 != 0`): same as step 2 but stat
   getters on `piStack_e8`, scratch rows `0x2a/0x38/0x46`, font `0x404`
   (`:222-303`).
6. **Target name/level**: same as step 3, scratch `(0x40,0x40,…)` (`:304-439`).
7. **Player bars** (if fade counter `+0xd4 != 0`): set per-side shadow via
   `FUN_00438d80(buf, 4, 4)`, blit player effect buf `+0x70` via `(*+0x5c)`,
   then 3× `FUN_0054a5d0` for HP/MP/FT, then blit the 3 value scratch cells +
   name cell from `+0x64` to the pane via `(*+0x5c)` (`:455-620`).
8. **Target bars** (if fade counter `+0xdc != 0`): same with shadow
   `FUN_00438d80(buf, pane_w−0x88, 4)`, target effect buf `+0x74`, x mirrored,
   then target value/name cell blits (`:630-797`).
9. Final `FUN_004aacb0_BlitEffect_Iterate` to flush the registered shadow
   effects onto the live display (`:798-814`).

> The text is composed in steps 2/3/5/6 into the `+0x64` scratch and only
> *blitted to the pane* in steps 7/8 (the `(*+0x5c)` calls with the
> `SDrawParam` dst cells in §4). This is the two-coordinate-space trap.

---

## §6 — Algorithms

### 6a. Chrome + icon compose — `FUN_0054a0a0` (player) / `FUN_0054a310` (target)

Signature: `void FUN_0054a0a0(this)`. Both helpers do the same shape; target
adds the target-presence guard and shifts the Ring/icon x.

```
buf6c = this[0x6c]                          # the chrome mosaic
buf6c.Box(0,0, buf6c.w, buf6c.h, color=_DAT_006668d0, 0xffff,0x7f7f, DM_USEDEFAULT)   # clear  (PlayerSide :15)
face = char->vtable[0x130]()                # live portrait render surface  (:16/:19)
if face: stamp(0,0, face, opaque)           # FUN_004bd680(0,0,face,0x80000000,0)  (:18/:21)
backPanel = ResByName("BackPanel")          # FUN_0046d710(s_BackPanel_…)  (:20/:23)
ring      = ResByName("Ring")               # &DAT_005e5748 / &DAT_005e5780 (:21/:24)
hpI = ResByName("HealthIcon"); mpI=…; ftI=… # (:22-24 / :25-27)
stamp(0,0, backPanel, opaque)               # FUN_004bd680(0,0,bp,0x80000000,0)  (:25/:28)
if face: blit buf6c -> this[0x60] via (*+0x5c)     # commit portrait corner   (:27/:30)
# All x below are CHROME-SURFACE-LOCAL (drawn into the +0x6c mosaic). They are
# NOT final coords — the mosaic composites at chip-local 0 (player) / 0x41
# (target); compose per §3/§4 to get chip-local & screen.
# Ring centered on its anchor (chrome-local) using its own w/h:
stamp(anchorX - ring.w/2, 0x1f - ring.h/2, ring, ALPHA)   # CHROME-LOCAL anchorX: player 0x1a, target 0x62  (:30/:33)
stamp(iconX, 0x20, ftI, ALPHA)              # CHROME-LOCAL iconX = 0x2b player / 0x3a target   (:31/:34)
stamp(iconX, 0x11, mpI, ALPHA)              # (:32/:35)
stamp(iconX, 0x03, hpI, ALPHA)              # (:33/:36)
blit buf6c -> display (this[0x70] player / this[0x74] target) via (*+0x5c)   # (:34/:37)
```

- `stamp(...)` = `FUN_004bd680(x,y,res,drawmode,0)` (UI_METHOD_MAP §4); opaque =
  `0x80000000`, alpha = `0x2000`.
- Ring centering reads the bitmap header `ring[0]`=w, `ring[1]`=h
  (`anchorX - (ring.w>>1)`, `0x1f - (ring.h>>1)`). With Ring 44×44:
  - **player** chrome-local tl `(0x1a−0x16, 0x1f−0x16) = (0x04,0x09) = (4,9)` —
    player chrome origin = chip-local 0, so this IS chip-local/screen.
  - **target** chrome-local tl `(0x62−0x16, 0x1f−0x16) = (0x4c,0x09) = (76,9)` —
    this is CHROME-local; compose +0x41 → chip-local tl `(0x8d,0x09)`, screen
    `(pane_w−0x34,0x09)` (the §4 composed value). Do NOT use `(76,9)` as the
    target's chip/screen position.
- `Box` clear color `_DAT_006668d0` is the panel transparent/key fill.

### 6b. Bar fill — `FUN_0054a5d0` (the 4-section kernel)

Signature (recon `:3-6`):
`FUN_0054a5d0(this, value, max, dstX, dstY, srcX, srcXbright, capX2, srcDimY, fullW, drawmode, capW, sectW, dir)`
— but **the 14 formal params do not map 1:1 to the 10 call-site literals**; the
caller pushes the bar-source getter return as the first two args and then a
literal block. The decoded per-call literals (the bar geometry) are tabulated
below; treat them as the contract, not the formal names (UNCONFIRMED-D, §14).

**Behavior (Classic, `DAT_006680c8 == 0`, `:96-112`):**
```
value = clamp(value, 0, max)                                    # :40-48
animCtr = (dir < 1) ? this[0xdc] : this[0xd4]                   # target vs player fade  :49-54
alpha   = (animCtr * 0xff) / 6                                  # 0..255 over 6 ticks
packed  = (alpha << 24) | 0xffffff                              # white tint, ramped alpha  :97
fillW   = value * fullW / max                                   # filled pixels  :55
# split into 4 source slices from the Bars band; the fill point picks bright vs dim per slice:
seg1 = left cap   (width capW)            -> bright if fillW >= capW/2 else dim
seg2 = interior A (bright portion)        -> width = clamp(fillW - capW, 0, fullW-2*capW)
seg3 = interior B (dim/empty portion)     -> width = (fullW-2*capW) - seg2
seg4 = right cap/tail (width = (sectW - fullW) + capW)  -> bright if fillW >= fullW-capW else dim
# each slice blitted with FUN_00414d70(dstX', dstY, 1, this[0x68]=Bars, 0, sliceW, h,
#                                       packed, srcX', srcY(bright|dim), sliceW, h, 0, 4)
```
- `dir = +1` (player) grows fill left→right; `dir = −1` (target) drains
  right→left (the bright/dim slices swap), so target gauges deplete from the
  inner edge toward the chrome. (`:57-95` is the dir-mirrored slice setup.)
- The bright srcY vs dim srcY are the two band rows from §2; the helper chooses
  per slice by comparing `fillW` to the slice boundary.
- Hi-res branch (`:113-192`) does the identical 4 slices via 4× `(*+0x5c)`
  mosaic blits on `PTR_DAT_005d79e0` — same coordinates.

**Per-bar call-site literals (the 10 pushed literals), all 3 bars × both sides**
(args as pushed, slot23):

| side | bar | getter | dstX | dstY | srcX | brightY | (a) | dimY | tailX | h | fullW | capW | dir | cite |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| player | HP | `+0x1d8` | `0x44` | `0x0f` | `2` | `1` | `2` | `0x31` | `0x7d` | `0x11` | `0x77` | `6` | `+1` | `:484-486` |
| player | MP | `+0x1e8` | `0x44` | `0x1f` | `2` | `0x13` | `2` | `0x42` | `0x4d` | `0x0c` | `0x45` | `4` | `+1` | `:499-501` |
| player | FT | `+0x1e0` | `0x44` | `0x2c` | `2` | `0x22` | `2` | `0x51` | `0x35` | `0x0c` | `0x2d` | `4` | `+1` | `:511-513` |
| target | HP | `+0x1d8` | `pane_w−0xc1` | `0x0f` | `2` | `1` | `2` | `0x31` | `0x7d` | `0x11` | `0x77` | `6` | `−1` | `:657-661` |
| target | MP | `+0x1e8` | `pane_w−0x91` | `0x1f` | `2` | `0x13` | `2` | `0x42` | `0x4d` | `0x0c` | `0x45` | `4` | `−1` | `:672-676` |
| target | FT | `+0x1e0` | `pane_w−0x79` | `0x2c` | `2` | `0x22` | `2` | `0x51` | `0x35` | `0x0c` | `0x2d` | `4` | `−1` | `:686-689` |

- `dstX/dstY` are **raw screen** bar destinations (`FUN_00414d70` arg1/2 =
  `SDrawParam.dx/dy`; helper `:98`, WORKED_EXAMPLE "Bar Helper Reconstruction").
- The slot-7 partial-redraw uses the IDENTICAL literals (slot7 `:46,61,76` /
  `:100,115,130`) — same table.
- The first two helper args are `value` and `max`: the caller calls the
  getter `(*char + 0x1d8)(…geometry…)` to get a packed bar descriptor, then
  `(*char + 0x1c0)(descriptor)` (HP current) / `(*+0x1d0)` (MP) / `(*+0x1c8)`
  (FT) to get the value, and passes both into `FUN_0054a5d0` (slot23 `:484-486`).

> **Shadow/normalization trap (WORKED_EXAMPLE):** in retail the bar dst is the
> raw screen coord BECAUSE the chrome/effect buffer was first blitted offset by
> the `FUN_00438d80(…,4,4)` shadow setup. If the port composes the chrome at
> chip-local `(0,0)` (not via the +4,+4 two-stage path), subtract `(4,4)` from
> the player bar dst (and apply the mirrored offset for target — §7) so bars
> don't land `+4,+4` off the chip.

> cf. snapshot `src/statusbar.cpp:56` (`PutHue` of one `"tube"` bitmap masked by
> `176*level/1000`) + the `GetHue` ramps (`:81-90`, `:105-108`) — but retail
> supersedes this: it composites pre-authored bright/dim bands from the `Bars`
> atlas (§2/§6b), it does NOT hue-rotate a single bitmap.

---

## §7 — Effects & shadows

### Per-side bar/chrome drop shadow (`FUN_00438d80`, UI_METHOD_MAP §8.1)
Semantics: `FUN_00438d80(buf, dx, dy)` sets where the SHADOW lands; the
content blits at `dst = recon_coord − (dx,dy)` (NOMENCLATURE §4,
[[project-retail-shadow-semantics]]).

| side | shadow setup call | (dx,dy) | shadow direction | cite |
|---|---|---|---|---|
| player | `FUN_00438d80(acStack_e4, 4, 4)` | `(+4,+4)` | down-RIGHT | slot23 `:466` |
| target | `FUN_00438d80(acStack_e4, pane_w−0x88, 4)` | `(pane_w−0x88, +4)` | the right chip's effect-buffer origin shifts the shadow; net visual = down-LEFT mirror of the player cast | slot23 `:641` |

The player's `dx=4` is a simple +4 cast. The target's `dx = pane_w−0x88` is the
chip's screen origin pre-bias (the effect buffer `+0x74` is 0x80 wide; the chip
sits at `pane_w−0xc1`), so after the `dst = recon − dx` rule the target content
lands at its mirrored chip position and the shadow falls toward the chip
interior (down-left relative to the right card). **The two sides are NOT the
same offset** — this is error #4 to close.

### Text 3-pass shadow (`FUN_004be2b0`, UI_METHOD_MAP §5 step 8)
Every text element uses a font id with bit `0x400` set
(`0x401`/`0x402`/`0x404`), so `FUN_004be2b0` runs the 3-pass black drop shadow
(`param_8 & 0x400`, UI_METHOD_MAP §5):
- `SetTextColor(black)`, `SetBkMode(TRANSPARENT)`.
- Pass 1 base, pass 2 `+1x`, pass 3 `+1y` — all black.
- Then the colored pass at base (white, see §8).
- Net: a 1px black shadow on the right + bottom, white glyphs on top.

### Chroma key
- Chrome/Bars (`flags 0x2`, `kc=0x0`): 15-bit, blit opaque; magenta `0x7C1F`
  is the global transparent key for any keyed pixels (UI_METHOD_MAP §16).
- Icons + Ring (`flags 0x102`): carry **alpha**, blitted with `DM_ALPHA`
  (`0x2000`) — use the source alpha, do NOT apply magenta key (would punch
  black pixels).

---

## §8 — Text rendering

The text path: `FUN_004be2b0_CompositeBuffer(scratch, x, y, w, h, str, colorPtr,
fontId, lineRect=DAT_0065abc4? , flags, drawmode)` draws into the `+0x64`
scratch; later the scratch cell is blitted to the pane (§5 steps 7/8).

**`h-align` is HORIZONTAL only (the DrawTextA `&1/&2/&4` bits — NOMENCLATURE §2).
`v-align` is SEPARATE and is `TOP` for every cell (GDI `DrawTextA` `DT_TOP`, no
`DT_VCENTER` bit anywhere on this path). "center" (`0x402`) means horizontally
centered, NOT vertically centered.** Do NOT specify a baseline/ascent here — the
in-cell vertical baseline (GDI DT_TOP internal-leading, ~2px) is corrected once
in `font.cpp` `DrawText*ToTarget`, shared by every panel (NOMENCLATURE §2
"baseline").

| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| player HP value | pane `(0x47,0x07,0x32,0x0e)` | `Small` (`DAT_0065abc4`) | ~12 | **white** `0xffffff` | left | TOP | yes (0x400) | `"%d"` (`DAT_005e57e8`) | `(*player+0x1c0)()` HP | slot23 `:73-97` |
| player MP value | pane `(0x47,0x17,0x32,0x0e)` | `Small` | ~12 | white | left | TOP | yes | `"%d"` (`DAT_005e57ec`) | `(*player+0x1d0)()` MP | `:100-124` |
| player FT value | pane `(0x47,0x24,0x32,0x0e)` | `Small` | ~12 | white | left | TOP | yes | `"%d"` (`DAT_005e57f0`) | `(*player+0x1c8)()` FT | `:127-151` |
| player name/level | pane `(0x00,0x36,0x40,0x40)` | `Small` | ~12 | white | center | TOP | yes | `"%s\nLevel %d"` (`DAT_005e57dc`) or DM `"%s\nLv:%d P:%d M:%d"` (`DAT_005e57bc`) | name + `(*player+0x354)` level (+ DM kills/MP) | `:153-204`, helper `:32-41` |
| target HP value | pane `(pane_w−0x80,0x07,0x32,0x0e)` | `Small` | ~12 | white | **right** | TOP | yes | `"%d"` (`DAT_005e57f4`) | `(*target+0x1c0)()` HP | `:222-248` |
| target MP value | pane `(pane_w−0x80,0x17,0x32,0x0e)` | `Small` | ~12 | white | right | TOP | yes | `"%d"` (`DAT_005e57f8`) | `(*target+0x1d0)()` MP | `:250-275` |
| target FT value | pane `(pane_w−0x80,0x24,0x32,0x0e)` | `Small` | ~12 | white | right | TOP | yes | `"%d"` (`DAT_005e57fc`) | `(*target+0x1c8)()` FT | `:277-302` |
| target name/level | pane `(pane_w−0x44,0x36,0x40,0x40)` | `Small` | ~12 | white | center | TOP | yes | same as player name | target name+level | `:304-434` |

> **Name/level vertical layout (load-bearing, NOMENCLATURE §2 v-align).** The
> name/level is ONE string `"%s\nLevel %d"` (two lines via the `\n`) drawn into
> the `0x40×0x40` (64×64) cell with `font_id 0x402`. `0x402` = HORIZONTAL center
> only. The two lines are **TOP-aligned and stack DOWNWARD from the cell top by
> `lineHeight`** (GDI `DrawTextA` `DT_TOP`); the lower portion of the 64-tall
> cell is left EMPTY. Do NOT vertically center the 2-line block in the cell, and
> do NOT hand-place each line with picked top offsets — emit the single `\n`
> string and let the cell renderer top-stack the lines. (`FUN_0054ae10` builds
> the one string; `FUN_004be2b0` draws it with no `DT_VCENTER` bit.)

**Font flag decode (WORKED_EXAMPLE "Font Flags", UI_METHOD_MAP §5):**
- `0x401` = HDC index 1 + shadow(`0x400`); align passed **left** (player values).
- `0x402` = HDC index 2 + shadow; align **center** (name/level).
- `0x404` = HDC index 4 + shadow; align **right** (target values).
- The font pointer after the flags is `DAT_0065abc4` = **FONT.DEF "Small"**
  (static init `0x485db1..0x485dc5`), NOT FONT.DEF "Numbers".

**White color SOURCE (mandatory):** the stat-value/name color is **packed at
the call site, NOT taken from FONT.DEF "Numbers" red**. Each text call site
pushes `0xff, 0xff, 0xff` through `FUN_00429950_BlitEffect_PackRGB` and copies
the packed value with `FUN_00419dd0_BlitEffect_SetField` immediately before
`FUN_004be2b0` (slot23 `:88-92`, `:117-119`, `:144-146`, `:193-196`,
`:240-243`, `:266-269`, `:294-297`, `:427-429`). So the intended top pass is
**white** (`0xffffff`); the FONT.DEF `WINFONT "Numbers" … COLOR 255 0 0` red is
NOT on this path. If the port renders gray/tinted text, debug the atlas/blend
(coverage in alpha, white RGB), do not change the color (WORKED_EXAMPLE).

**Name/level format detail (`FUN_0054ae10`):** if multiplayer/DM is active
(`DAT_0066829c && DAT_00676828 && (DAT_006767fc & 0x80)` and the pane is the
0xb-tagged variant), it uses `"%s\nLv:%d P:%d M:%d"` with kills/MP from
`char[0x194]/[0x196]`; otherwise `"%s\nLevel %d"` with `(*char+0x354)` level
(helper `:15-43`). The string is also run through a localization lookup
(`FUN_0049d6d0(s_STATBARFMT…)` / `s_STATBARDMFMT…`) before formatting.

---

## §9 — Animation & dynamic behavior

### Fade counters (the smooth show/hide) — slot 19 `FUN_0054 9da0`
Two integer ramps, ticked once per call (24Hz sim gate):

```
ramp this[0xd4]  (player fade)
  range   0..6
  step    +1 per tick toward target, -1 away
  target  this[0xd8] = 6 (player always present once DAT_00667fcc != 0)   (:25-33)
  maps-to bar alpha = (this[0xd4] * 0xff) / 6   (helper :97)
  cite    slot19 :24-33 ; helper FUN_0054a5d0 :97

ramp this[0xdc]  (target fade)
  range   0..6
  step    +1/-1 per tick toward target
  target  this[0xe0] = (target present ? 6 : 0)   (:35-44)
  maps-to bar alpha = (this[0xdc] * 0xff) / 6
  cite    slot19 :35-48 ; helper :97
```

When `DAT_00667fcc == 0`, slot 19 zeroes all four fields (`:10-15`).

> cf. snapshot level-ramp in `src/statusbar.cpp:32-66` (lerp `level` toward
> `targetlevel` by `HEALTH_INCREMENT=10`) — same lerp intent, but retail
> supersedes it (this fade ramp is a 0..6 *show/hide alpha*, not a value lerp;
> the value itself comes live from the char getters, §6/§8).

In slot 23 the alpha is also gated for the chrome/effect blit: the effect
buffer is only drawn when `iVar4 = (counter * 0xff)/6 > 0x80` for the hi-res
path / `!= 0` for the bar fill (slot23 `:456-459`, `:631-634`).

### Temporal diagram — target fade-in on target acquire

```
target fade counter this[0xdc] (0..6)  →  bar alpha (0..255 = ctr*0xff/6)
  6 ┤            ●────────────●          (target held)
    │          ╱
    │        ╱   +1/tick, linear, alpha = ctr*255/6
  0 ┤●──────╱                  ╲──────●  (target lost → ramp down -1/tick)
    └──────────────────────────────────── sim ticks @24Hz
     no target │ acquire (ramp up 6t) │ held │ lose (ramp down 6t)
```

### Dirty / redraw model
Slot 23 caches the last-drawn values and only re-runs each text composite when
its value changed:

| cache field | guards | cite |
|---|---|---|
| `param_1+0x98` | player HP value | `:74` |
| `param_1+0x9c` | player MP value | `:101` |
| `param_1+0xa0` | player FT value | `:128` |
| `param_1+0xa4` | player level | `:156` |
| `param_1+0xbc` | player name ptr (`char[0xe]`) | `:157,203` |
| `param_1+0x94` | bound player object ptr | `:153,206` |
| `param_1+0xac/0xb0/0xb4` | target HP/MP/FT values | `:225,252,279` |
| `param_1+0xc0` | target name ptr (`target[0xe]`) | `:304` |
| `param_1+0xa8/0xb8` | bound target object / id | `:304,442-443` |
| `param_1+0xc4..0xd0` | DM kills/score fields | `:166-178` |

The bars are redrawn every frame the fade counter is nonzero (no value cache —
the gauge is cheap and the fill animates). Recommend a monotonic version
counter per cell in the port instead of value-equality bools
([[feedback-versions-over-flags]]).

### State machine
No multi-phase widget beyond the two fade ramps. Player chip: present/absent
(binary, no fade — player counter targets 6 immediately). Target chip:
absent → ramping-in → held → ramping-out → absent.

---

## §10 — Input & dispatch

**Non-interactive.** TPlyrStatusBar is a pure HUD readout — no hit rects, no
command ids, no click handler in the vtable overrides (slots 7/19/20/23 are all
draw/animate; no MouseDown/Up override). Skipped.

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo around text** (the load-bearing one;
   [[project-retail-pink-halo-bug]], NOMENCLATURE §4, UI_METHOD_MAP §16).
   - **What it looks like:** a faint magenta/pink fringe around the white stat
     numbers and name/level text.
   - **Why:** `FUN_004be2b0` renders text into the `+0x64` scratch which was
     `Box`-cleared to a backing color; the antialiased edges of the 3-pass
     black shadow get partial-alpha pixels that sample the magenta-cleared
     cell. When the cell is blitted with magenta `0x7C1F` keyed out, the
     surviving partial-alpha edge pixels carry the magenta tint → pink halo.
   - **Correct intended visual:** white glyphs with a crisp 1px black drop
     shadow (right+bottom), no colored halo. Render text as a white-RGB
     coverage atlas (coverage in alpha) and composite with real alpha; do the
     3 black passes + white pass; never magenta-key the text cell.
2. **`+4,+4` bar offset** (WORKED_EXAMPLE). Not a retail bug per se but the
   classic port mis-port: the raw retail bar dst (`0x44,0x0f`) is correct ONLY
   when the chrome/effect buffer is first drawn via the `FUN_00438d80(…,4,4)`
   two-stage path. A port that composes the chrome at chip-local `(0,0)` must
   subtract `(4,4)` (player) / apply the mirrored offset (target). Reproduce
   the intent (bars aligned to the chip), not a literal `+4,+4` paste.

---

## §12 — Reconstruction pseudocode

```
# ---- once at init (mirror FUN_00549740 Classic branch) ----
load statusbarnotex.dat -> BackPanel, Bars, HealthIcon, ManaIcon, FatigueIcon, Ring
allocate chip render target (the §3 direct-renderer contract)
font = TTF "Small" (~12px) ; whiteText = (1,1,1)

# ---- per sim tick (24Hz) — mirror slot 19 ----
tick_ramp(playerFade, target = player ? 6 : 0)         # §9
tick_ramp(targetFade, target = curTarget ? 6 : 0)

# ---- per frame paint — mirror slot 23 ----
def paint(pane_w):
    if not player: return
    target = resolve_target(player)                    # player[0x38][0x11] or MapPane fallback
    clear chip RT
    draw_side(player, isRight=False, originX=0,            fade=playerFade)
    if target:
        draw_side(target, isRight=True, originX=pane_w-0xC1, fade=targetFade)
    blit chip RT to HUD swapchain

def draw_side(ch, isRight, originX, fade):
    # --- chrome (mirror FUN_0054a0a0 / FUN_0054a310) ---
    # Retail draws chrome into a chrome surface, which composites at
    #   chip-local 0 (player) / 0x41 (target).  All coords below are CHIP-LOCAL
    #   (the §4 composed values); chromeOff = isRight ? 0x41 : 0
    chromeOff = isRight ? 0x41 : 0
    draw portrait(ch) at chip(chromeOff+0, 0)           # I3D live render (deferred → placeholder)
    draw BackPanel    at chip(chromeOff+0, 0)           # 128x64 opaque
    draw Ring centered at chip(chromeOff + (isRight?0x62:0x1a), 0x1f)  # 44x44 alpha
    iconX = chromeOff + (isRight ? 0x3a : 0x2b)         # player 0x2b ; target 0x7b
    draw FatigueIcon (iconX,0x20); ManaIcon(iconX,0x11); HealthIcon(iconX,0x03)  # alpha
    # then composite chip RT onto screen at originX (player 0 / target pane_w-0xc1)

    # --- bars (mirror FUN_0054a5d0), only if fade != 0 ---
    if fade != 0:
        alpha = fade*255/6
        for bar in [HP, MP, FT]:   # use §6 table row for (isRight, bar)
            dstX = bar.dstX_player if !isRight else (pane_w - bar.targetOff)
            draw_bar_4slice(Bars, dstX, bar.dstY, srcX=2, brightY=bar.brightY,
                            dimY=bar.dimY, fullW=bar.fullW, capW=bar.capW,
                            tailX=bar.tailX, h=bar.h, value=get_stat(ch,bar),
                            max=get_max(ch,bar), dir=isRight?-1:+1, tint=(white,alpha))
            # shadow: player (+4,+4); target mirrored (see §7)

    # --- text cells (mirror the §8 table); top-aligned in cell, GDI-style ---
    for bar in [HP, MP, FT]:
        cellX = isRight ? pane_w-0x80 : 0x47
        draw_text_cell(str(get_stat(ch,bar)), cellX, bar.textY, 0x32, 0x0e,
                       align = isRight ? RIGHT : LEFT, color=white, shadow3pass=True)
    nameCellX = isRight ? pane_w-0x44 : 0x00
    draw_text_cell(format("%s\nLevel %d", ch.name, ch.level),
                   nameCellX, 0x36, 0x40, 0x40, align=CENTER, color=white, shadow3pass=True)

def draw_bar_4slice(...):   # §6b
    value = clamp(value,0,max); fillW = value*fullW/max
    inner = fullW - 2*capW
    seg2W = clamp(fillW-capW, 0, inner); seg3W = inner - seg2W
    tailW = (tailX - fullW) + capW
    if dir<0: swap which slices read bright vs dim (drain from inner edge)
    blit cap(capW), interiorA(seg2W bright), interiorB(seg3W dim), tail(tailW)
        from Bars at (srcX, brightY|dimY) with tint (white, alpha)

def draw_text_cell(str, x, y, w, h, align, color, shadow3pass):
    # GDI DrawTextA model: v-align = TOP always (no DT_VCENTER), h-align per
    # `align` (left/center/right HORIZONTAL only), clipped to (x,y,w,h).
    # Multi-line strings ("%s\nLevel %d") stack DOWN from the cell top by
    # lineHeight — do NOT vertically center the block in the 0x40-tall name cell.
    # Do NOT bake a baseline/ascent here — font.cpp handles DT_TOP leading.
    # 3 black passes (base,+1x,+1y) then color pass.
```

Numbers come from the §4 / §6 / §8 tables — do not re-state them in code.

---

## §13 — Port mapping notes

| retail call | port primitive | cite |
|---|---|---|
| `FUN_004bd680(x,y,res,0x80000000,0)` (opaque stamp) | `Renderer->DrawBitmap[ToTarget]` | UI_METHOD_MAP §4 |
| `FUN_004bd680(x,y,res,0x2000,0)` (alpha stamp) | `Renderer->DrawBitmapSubrect[ToTarget]` (source alpha) | UI_METHOD_MAP §4 |
| `FUN_00414d70(...)` / `(*+0x5c)` bar slice blit | `Renderer->DrawBitmapSubrectTinted[ToTarget]` (alpha packed in tint) | UI_METHOD_MAP §2,§11 |
| `(*surface+0x64)(Box)` clear | `TSurface::Box` / `StartPass` clear | UI_METHOD_MAP §15a |
| `(*surface+0x5c)` blit-with-effects | `Renderer->DrawSurface` / `DrawBitmap…Shadowed` | UI_METHOD_MAP §8.4,§15a |
| `FUN_00438d80(buf,dx,dy)` shadow setup | `Renderer->DrawBitmap…Shadowed(off_x,off_y)` | UI_METHOD_MAP §8.1 |
| `FUN_004be2b0` text + 3-pass shadow | `Renderer->CompositeTinted` over a TTF coverage atlas (white RGB, alpha coverage); 3 black + 1 white pass | UI_METHOD_MAP §5 |
| `FUN_00429950` / `FUN_00419dd0` color pack | inline tint `(1,1,1)` | UI_METHOD_MAP §6,§7 |
| `FUN_0046d710(name)` resource lookup | asset-cache lookup → `PTBitmap` at init | UI_METHOD_MAP §9 |
| `FUN_0058b100(buf,fmt,…)` | `snprintf` | UI_METHOD_MAP §10 |
| live portrait `(*char+0x130)` | **needs new primitive:** I3D head render → texture (deferred) | §2 |

**Missing primitives to flag:**
- I3D portrait decode/render (`chars\locke.i3d`) — deferred; harness uses
  `LockeFace` placeholder.
- A "draw text into a fixed cell with GDI-style top/center/right alignment +
  clip" helper that matches the `0x32×0x0e` / `0x40×0x40` cell model (the
  current harness derives baselines from glyph bounds — error #1/#2).

---

## §14 — UNCONFIRMED / open questions

1. **Char-object bar-source getters `+0x1d8` (HP) / `+0x1e8` (MP) / `+0x1e0`
   (FT)** — these return the packed bar descriptor consumed by `FUN_0054a5d0`'s
   first two args (with `+0x1c0/+0x1d0/+0x1c8` giving the current value). The
   exact return layout and how `value`/`max` are split is inferred from the
   call shape, not the getter bodies. **Impact if wrong:** fill ratio could be
   off (wrong max). **Resolve:** `DecompileAddr.java` on the char-object
   methods at object-vtable `+0x1d8/+0x1e0/+0x1e8` (UI_METHOD_MAP §15b
   UNCONFIRMED-8).
2. **`FUN_0054a5d0` param↔atlas mapping (UNCONFIRMED-D)** — the 14 formal params
   vs 10 call-site literals; cap/full-width and the bright/dim slice split are
   matched by width + the §2 color scan, but the precise boundary at which each
   slice flips bright↔dim is read from the dir-mirrored branch, not bit-exact
   verified per slice. **Impact:** a 1–2px seam between bright/dim at the fill
   edge. **Resolve:** trace the `:57-95` branch arithmetic against a known
   fill, or visual-verify at several fill levels.
3. **Real Locke portrait** — `chars\locke.i3d` needs the I3D decoder
   (deferred). **Impact:** placeholder face until decoder lands. **Resolve:**
   I3D decode task; then render the head to the `+0x130`-equivalent surface.
4. **Target shadow `dx = pane_w − 0x88` exact net visual** — decoded from the
   slot23 literal (`:641`); the down-left mirror interpretation follows the
   shadow semantics + chip origin, but I did not re-run the
   `FUN_004aacb0`/`FUN_004aaeb0` iterate math for the target buffer to pixel-
   confirm the landing. **Impact:** target shadow could be a few px off.
   **Resolve:** `objdump` the `0x54bb00..0x54bb30` region or visual-verify the
   target card shadow direction.
5. **Target name scratch column `0x40`** — the name/level scratch cells live at
   scratch x `0x40` (player src `(0x40,0)`, target `(0x40,0x40)`); the final
   blit dst `(pane_w−0x44,0x36)` is from the worked example's disassembly
   decode, trusted but not re-disassembled here. **Impact:** target name x off.
   **Resolve:** re-run the WORKED_EXAMPLE objdump on `0x54bbf0..0x54be15`.
6. **DM/multiplayer name format gate** — `DAT_0066829c / DAT_00676828 /
   DAT_006767fc & 0x80` select the `"%s\nLv:%d P:%d M:%d"` format. Single-player
   uses `"%s\nLevel %d"`. The exact meaning of each global (DM mode? netgame?)
   is inferred from the format string. **Impact:** none for SP. **Resolve:**
   trace the DM globals if multiplayer is in scope.
