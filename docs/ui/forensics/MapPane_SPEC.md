# MapPane (Map / Automap sidebar content) — Reconstruction SPEC

Class: `cls_0x5a5658` (role label **MapSidebarPane**; candidate src class
`TAutoMap`). The automap pane shown in the lower sidebar region when the "Map"
tab is selected.

---

## §0 — Sources & status

**Panel class id / address:** `cls_0x5a5658`, global instance `DAT_0065b4f0`
(`0x65b4f0`). Direct `TPane` leaf, 32-slot vtable, instance size `0x132 = 306`
bytes (B_r11 Step-4 table; `recon/discovered/port_status/MapSidebarPane.md:5`).

**Recon files read (authoritative):**
- `recon/ghidra/cls_0x5a5658.cpp` — full decompile. Methods traced:
  - `virt_meth_0x529970` — **Initialize** (`:73-439`) — loads `AutoMap.dat`,
    builds mosaic surfaces, tile cache, scroll center.
  - `~cls_0x5a5658` @ `0x52a400` — **dtor** (`:445-486`).
  - `virt_meth_0x52a490` — **Close** (`:492-576`) — frees surfaces.
  - `virt_meth_0x52a5f0` — **Draw orchestrator** (`:582-592`).
  - `meth_0x52a830` — **map-body blit** (4-corner Amap tiling) (`:669-714`).
  - `meth_0x52a620` — **object/blip render** (`:598-663`).
  - `meth_0x52aa70` — **5×5 tile-cache loader** (`automaps\%d_%d_%d.bmp`)
    (`:722-914`).
  - `meth_0x52af00` — **heightfield/lighting mesh build** (`:922-1152`) —
    3D path, OUT-OF-SCOPE (see §14).
  - `meth_0x52b520` — **mesh submit/blit to display** (`:1158-1319`) — 3D path,
    OUT-OF-SCOPE.
  - `meth_0x52bdb0` — **chrome/overlay draw** (frame + zoom buttons)
    (`:1325-1362`).
- `tools/ui/dump_dat.py` on `data/resources_unzipped/automap.dat` (asset
  measurement, §2).
- `objdump` of the inline CRT-init thunk @ `0x4808a0` (viewport-rect globals,
  §3).

**Prior briefs:** `docs/ui/briefs/B_r11_sidebar_tab_cascade.md` (sidebar roster,
visibility model, dispatch case 0xc), `docs/ui/briefs/B_r3_playscreen_panes.md`.
Method behavior cited from `docs/ui/forensics/UI_METHOD_MAP.md`.

**Status:** `forensics-partial`. The panel framing, chrome, asset roster,
viewport rect, scroll/zoom state, tile-cache load, blip placement, and chrome
overlay (frame + zoom buttons) are fully pinned. The **map-body pixel render is
a live mesh render through the 3D display surface** (`meth_0x52af00` +
`meth_0x52b520` via `cls_0x411eb0`); its internal triangle/lighting math is
flagged OUT-OF-SCOPE/UNCONFIRMED (§14 #1) — it belongs to the renderer path, not
the UI. The spec describes the render TARGET, how it is framed, and how the
chrome composites over it.

**Ghidra class-merge caveat (load-bearing):** `recon/ghidra/cls_0x5a5658.cpp`
contains, after `meth_0x52bdb0`, three methods that are **NOT this panel** —
`virt_meth_0x52c970_TBarInvPane_Initialize` (`:1382`), `virt_meth_0x52ca70`
(`:1410`), `virt_meth_0x52d6e0` (`:1527`). They reference the inventory bar
(`s_Pouch`, item stride `0x2d`, start `0xdc`, `DAT_0065b644`) — they are
**TBarInvPane** methods Ghidra merged into this file (AGENT_PROTOCOL Rule 2,
vtable-sharing/intrusion). Do not port them as automap. The automap methods are
the `0x529970-0x52bdb0` block only.

**Snapshot note (supplementary — superseded by retail).** A 1998/99 version
exists: `src/automap.{h,cpp}` (`TAutoMap`), `src/mappane.{h,cpp}`,
`src/mapscroll.{h,cpp}`. It is **superseded**: the snapshot draws the map by a
per-pixel CPU merge of a "before"/"after" bitmap pair (`MergePixel` /
`MergePixelToBlack`, `automap.cpp:234+`) into a single `DisplayMap`, then
`Display.Put(0,0,DisplayMap,ScrollX,ScrollY,…)` (`automap.cpp:180`), with a
1-pixel `0xFFFF` player marker (`DrawPlayerMarker`, `automap.cpp:204-213`) and a
168×128 pane (`MULTIPANE*`, `src/revdefs.h:142-145`). Retail replaced all of
that with a 5×5 64-px tile cache + a 3D heightfield/lighting mesh + a 16×16
`Marker` sprite, and a 188×174 pane. **Reusable only as field-name/intent
context** (the scroll-to-target-centered-on-player model and the
discovered-area concept survive — `automap.h:83-126`). Do NOT take any
coordinate, layout, or the per-pixel merge from the snapshot; retail is
definitive. `src/mapscroll.*` is unrelated (the `TMapScroll` game-object class).

---

## §1 — Overview

The Map pane is a small top-down minimap in the lower-right sidebar region. When
the "Map" lower-region tab is active (`DAT_0065d1bc == 1`; dispatcher case 0xc,
B_r11 §3) the pane renders: (a) a scrolled, zoomable rendering of the area's
discovered automap into the pane viewport, (b) 16×16 `Marker` blips for the
player and visible nearby creatures, and (c) a static chrome frame (`Amap`) plus
two zoom buttons (`PlusSel` / `MinusSel`).

Plain-language: it is a live, scrolling map window. The map content is not a
static bitmap — it is generated each frame as a lit, scrolled mesh from a cache
of 64×64 discovered-area tiles loaded from `Modules/<area>/Automaps/X_Y_Z.bmp`.
The map auto-centers on the player; the player and creatures appear as small
marker sprites; a chrome border with +/- zoom buttons frames it. Zoom is an
integer percent in `mbr_0x130` (initialized to **100**, `cls_0x5a5658.cpp:312`).

**Visibility:** shown when lower region == Map. The Draw orchestrator
(`virt_meth_0x52a5f0`) gates the map-body + mesh on `DAT_005d7a6c != 0`
(`:586`) — a global "automap enabled / data ready" flag set in Initialize
(`:313`). The chrome (`meth_0x52bdb0`) always draws when the pane paints.

---

## §2 — Asset roster

All from `data/resources_unzipped/automap.dat` (also `legacy/AUTOMAP.DAT` exists
but is a 1-entry 2084-byte stub — ignore). Measured via
`tools/ui/dump_dat.py automap.dat`. Bitmaps are 15-bit (RGB555); the dump
labels the depth flag `0x10000` "unsupported" but the entries decode at the
listed dims.

| asset | archive | entry | WxH (measured) | role | flags / kc | cite |
|---|---|---|---|---|---|---|
| Marker | automap.dat | `Marker` | **16×16** | player + creature blip sprite | `0x10000`, kc=0 | dump_dat.py output; `cls_0x5a5658.cpp:153` (mbr_0x8c) |
| Amap | automap.dat | `Amap` | **188×174** | chrome frame/border (defines viewport size) | `0x10000`, kc=0 | dump; `:156` (mbr_0x90); inline-init `0xbc×0xae` @ `0x4808cd` |
| PlusSel | automap.dat | `PlusSel` | **32×32** | zoom-in button (`+`) | `0x10400` (shadow) kc=0 | dump; `:159` (mbr_0x94) |
| MinusSel | automap.dat | `MinusSel` | **32×32** | zoom-out button (`−`) | `0x10400` (shadow) kc=0 | dump; `:163` (mbr_0x98) |

The four are fetched in load order in Initialize (`meth_0x46d710` iterator on
the opened `AutoMap.dat`, `cls_0x5a5658.cpp:153-165`) and stored:
`mbr_0x8c=Marker`, `mbr_0x90=Amap`, `mbr_0x94=PlusSel`, `mbr_0x98=MinusSel`.
(Order = archive order: Marker, Amap, PlusSel, MinusSel.)

**Per-cell map tiles (runtime, not in automap.dat):**
`Modules/<area>/Automaps/%d_%d_%d.bmp` — **64×64** indexed bitmaps (measured:
`Ahkuilon/Automaps/0_-10_25.bmp` = 64×64 mode P). Format string
`s__sautomaps__d__d__d_bmp` (`%s\automaps\%d_%d_%d.bmp`,
`cls_0x5a5658.cpp:828`), loaded by `meth_0x52aa70` into a 5×5 tile ring
(`mbr_0xa8..` array, 25 slots). Each is one discovered map cell; the args are
(area-path, cell-x, cell-y, level).

There is **no static "map content" bitmap** — the body is composed from these
64-px tiles via the mesh path (§5/§6). A reconstruction must load the per-area
tiles, not draw a stand-in.

---

## §3 — Coordinate frames & surfaces

**Pane rect (the viewport on screen).** The CRT inline-init thunk @ `0x4808a0`
writes the pane geometry into module globals (decoded from objdump):

| global | value | meaning | cite |
|---|---|---|---|
| `DAT_0065b4f4` | `0x1c4 = 452` | viewport screen **X** | objdump `0x4808aa` |
| `DAT_0065b4f8` | `0x132 = 306` | viewport screen **Y** | objdump `0x4808c3` |
| `DAT_0065b4fc` | `0xbc = 188`  | viewport **width**  (= Amap w) | objdump `0x4808d2` |
| `DAT_0065b500` | `0xae = 174`  | viewport **height** (= Amap h) | objdump `0x4808e1` |

(`0x65b504/8/c`, `0x65b510` are a second "default/restore" copy of the same four
values, written at `0x4808af/c8/d7/e6`.) So the **automap viewport is the screen
rect (452, 306) 188×174** — note this is module-global state, NOT a `this+0xc`
pane field; the draw methods read the globals directly. (The TPane base
`this->mbr_0xc/0x10` also hold w/h `188/174` for hit-testing, `:1541`.)

**Frame table (mandatory):**

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| screen | framebuffer | root | (0,0) | final pixels | — |
| **map-viewport** | screen | TL (constant) | `(DAT_0065b4f4, DAT_0065b4f8)` = **(452, 306)**; size 188×174 | the whole pane: chrome + map body + blips | objdump @ `0x4808a0`; `cls_0x5a5658.cpp:686,689` |
| map-body | map-viewport | TL+(0x14,0x12) | screen `(452+0x14, 306+0x12)` = **(472, 324)** | scrolled map mesh + blips draw here | `:686` (`DAT_0065b4f4+0x14`), `:689` (`DAT_0065b4f8+0x12`) |
| tile-mosaic-surf | (offscreen) | own TL | a 320×320 (`0x140²`) scratch surface (`mbr_0x68`) | the assembled 5×5 64-px tile field, source for the mesh | `:124,140` (`meth_0x4bb5c0(...,0x140,0x140)`) |
| source-rect | bitmap | own TL | per-asset | Marker/Amap/PlusSel/MinusSel sub-rects | §2 |

**Composition to screen:** the map body and all blips compose as
`screen = map-viewport_origin + (0x14, 0x12) + body_local`, i.e. the map
content is inset **(20, 18)** inside the Amap chrome frame. The chrome itself
(`Amap`, `meth_0x52bdb0`) draws at the map-viewport origin `(452, 306)` (its
`(char)DAT_0065b4f4, (char)DAT_0065b4f8`, `:1345-1348`).

**Anchor verification.** map-viewport x is a bare constant `DAT_0065b4f4=452`
(not `pane_w − k`), so it is **TL-anchored** at the fixed 4:3 / 640×480 screen
position (452,306) — the lower-right sidebar slot. The body inset (0x14,0x12) is
the constant gap between the chrome's outer edge and the map window inside the
`Amap` border art. (188−20−? the right/bottom inset is implied by the clip set
to width 0x90=144 in `meth_0x412150(...,0x90)` at `:702` — the visible map
window is ~144 px wide inside the 188-px chrome.)

**Surfaces allocated in Initialize** (`FUN_00482fb0` + `cls_0x5a68d8` ctor):

| field | size | role | cite |
|---|---|---|---|
| `mbr_0x68` | 320×320 (`0x140²`) | **primary tile mosaic** — 5×5 of 64-px tiles | `:115-132` |
| `mbr_0x6c` | 320×320 | secondary scratch (cleared to `0xfff0bdc0`) | `:133-148` |
| `mbr_0x70/0x74` | Marker-derived | rotated Marker variants | `:166-220` |
| `mbr_0x78/0x7c` | PlusSel-derived | zoom-in button surfaces | `:221-235` |
| `mbr_0x80/0x84/0x88` | PlusSel/MinusSel-derived | button + overlay surfaces | `:236-302` |

> The port need not mirror these 1:1 (NOMENCLATURE §3 scratch-surface rule). It
> must preserve the contract: assemble the tile field into one RT, render the
> framed/scrolled map into the body inset, then draw chrome + blips on top.

**Layout diagram (screen space):**

```
map-viewport TL = screen (452, 306)              size 188 x 174  (= Amap chrome)
┌──────────────────────────────────────────────┐  306
│  Amap chrome frame (188x174)                   │
│   ┌──────────────────────────────┐            │
│   │ map body / mesh window         │  body TL = │
│   │   inset (0x14,0x12)=(20,18)    │  (472,324) │
│   │   clip width 0x90 = 144        │            │
│   │   · player Marker blip (16x16) │            │
│   │   · creature Marker blips      │            │
│   └──────────────────────────────┘            │
│  [MinusSel 32x32]            [PlusSel 32x32]   │
│   at body-TL +(-0x70,+3)      at body-TL+(8,+3)│
└──────────────────────────────────────────────┘  306+174=480
452                                            452+188=640
```

(The +/- buttons sit at the chrome's bottom corners; see §4 for exact composed
coords.)

---

## §4 — Static element layout

Single-instance panel (no player/target mirror). All coords FINAL composed
**screen** space. Let `VX = 452`, `VY = 306` (map-viewport origin).

| element | space | (x,y) | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| Amap chrome frame | screen | **(452, 306)** = (VX, VY) | 188×174 | full Amap | `:1345-1348` (`cVar4=(char)DAT_0065b4f4`, `cVar5=(char)DAT_0065b4f8`, blit `mbr_0x7c`) |
| map body window | screen | **(472, 324)** = (VX+0x14, VY+0x12) | clip 0x90=144 wide | mesh (no static rect) | `:686,689,702` |
| PlusSel (zoom +) | screen | **(480, 327)** = (VX+0x14+8, VY+0x12+3) | 32×32 | full PlusSel | `:1350-1352` (`cVar4+8, cVar5+3`, blit `mbr_0x84`), gated `mbr_0x9c!=0` |
| MinusSel (zoom −) | screen | **(360, 327)** = (VX+0x14−0x70, VY+0x12+3) | 32×32 | full MinusSel | `:1354-1357` (`cVar4-0x70, cVar5+3`, blit `mbr_0x88`), gated `mbr_0xa0!=0` |

> Note the +/- button coords are computed from `cVar4 = (char)DAT_0065b4f4` and
> `cVar5 = (char)DAT_0065b4f8` (the **map-viewport** origin truncated to a byte
> for the blit dst). `(char)452 = 0xC4 = 196` and `(char)306 = 0x32 = 50` are
> the low-byte display-surface dst coords used by `cls_0x411eb0::meth_0x414d70`
> on the windowed display (the display surface is already offset to the pane
> region). For reconstruction, treat the buttons as anchored to the
> map-viewport origin at the offsets in the table: PlusSel = origin+(8,3),
> MinusSel = origin+(−0x70,3) = origin+(−112,3). They straddle the bottom of
> the chrome (bottom-anchored within the frame; the −0x70 puts MinusSel near the
> left, PlusSel near the right). UNCONFIRMED #3: whether the buttons are
> always-visible or only when zoom is non-default (`mbr_0x9c`/`mbr_0xa0`
> gating).

**Player / creature blips (dynamic — see §6 for the placement formula):**
drawn by `meth_0x52a620` into the **map-body** frame, clipped to the body
window. Blip = the `Marker` 16×16 sprite (`mbr_0x70`), `meth_0x414d70(... iVar8,
iVar7, 0, mbr_0x70, 0)` (`:651-653`).

---

## §5 — Draw order / composition

The vtable Draw slot is `virt_meth_0x52a5f0` (`:582`). Order:

1. `meth_0x52a830(this)` — **map-body base blit** (`:585`): the assembled
   `mbr_0x6c` map mosaic blitted into the body window via 4 corner-anchored
   `meth_0x414d70` calls (4 quadrant tiles of the scrolled field) at
   `(cVar3,cVar4)`, `(cVar8,cVar4)`, `(cVar3,cVar9)`, `(cVar8,cVar9)`
   (`:703-710`). Clip set to body origin `(VX+0x14, VY+0x12)` width 0x90 first
   (`:702`), restored after (`:711`).
2. **If `DAT_005d7a6c != 0`** (`:586`):
   a. `meth_0x52a620(this)` — **object/creature + player blips** (`:587`).
   b. `meth_0x52b520(this)` — **mesh submit** to the display (the lit/scrolled
      automap mesh; 3D path; OUT-OF-SCOPE internals) (`:588`).
3. `meth_0x52bdb0(this)` — **chrome overlay** (`:590`): Amap frame, then
   conditionally PlusSel / MinusSel zoom buttons.

> The 5×5 tile-cache load (`meth_0x52aa70`) and the heightfield rebuild
> (`meth_0x52af00`) are NOT in the per-frame Draw orchestrator — they run when
> the scroll cell / zoom / discovered-area changes (dirty check
> `mbr_0x110/0x114/0x12c` in `meth_0x52aa70:777`; `bVar3` change-detect in
> `meth_0x52af00`). They are the data-prep that feeds steps 1-2.

---

## §6 — Algorithms

### 6a. Scroll center (Initialize + per-tick)
Player world position is read via `TPlayScreen::meth_0x46eb40(DAT_00667fcc,
&mbr_0x138)` → fills `mbr_0x138` (scroll-center X) and `mbr_0x140` (Y)
(`cls_0x5a5658.cpp:425`). The scroll origin used by the draw math is
`mbr_0x11c` (X) / `mbr_0x120` (Y) (`:692,695`), with `mbr_0x134/0x13c` holding
the previous values for the change-detect. Snapshot intent (superseded but
clarifying): `TargetScroll = PlayerPos − Pane/2`, clamped to map bounds, eased
toward at ≤16 px/tick (`automap.cpp:1138-1177`). Retail replaces the eased
scroll with the mesh re-projection but the centre-on-player concept holds.

### 6b. Tile→viewport projection (the map-body blit, `meth_0x52a830`)
Per-corner source/dest math (`:686-698`), `iVar7 = (mbr_0x68_w + (−0x40))*100`
is the zoom denominator; `mbr_0x130` is the zoom percent (numerator). For each
of the 4 quadrants the dst byte coords are:
```
zoom   = mbr_0x130            # percent, default 100
denom  = (tileMosaicW - 0x40) * 100
sx     = mbr_0x6c.width  ; sy = mbr_0x6c.height
base_x = (DAT_0065b4f4 + 0x14)   # body origin x = 472
base_y = (DAT_0065b4f8 + 0x12)   # body origin y = 324
cVar3 = base_x - ((wrap(mbr_0x11c, sx)) * zoom * 0x90) / denom
cVar4 = base_y - ((wrap(mbr_0x120, sy)) * zoom * 0x90) / denom
cVar8 = cVar3 + (zoom * sx * 0x90) / denom
cVar9 = cVar4 + (zoom * sy * 0x90) / denom
```
`0x90 = 144` is the body window width in display units; the 4 blits tile the
wrapped scrolled mosaic so the seam scrolls smoothly. (The exact `wrap()` is the
signed-modulo expression at `:691-696`.) UNCONFIRMED #2: the precise
display-unit→screen-pixel scaling of `0x90` (display surface is pre-scaled).

### 6c. Blip placement (`meth_0x52a620`)
For each visible character within range (iterates an entity list via
`cls_0x44ceb0` spatial query, radius `0x200`, `:635-657`):
```
denom  = (tileMosaicW - 0x40) * 100          # :643
blip_x = ((entityWorldX >> 3) - mbr_0x11c) * zoom * 0x90 / denom + (DAT_0065b4f4 + 0x14)
blip_y = ((entityWorldY >> 3) - mbr_0x120) * zoom * 0x90 / denom + (DAT_0065b4f8 + 0x12)
if blip_x in [base_x, DAT_0065b4f4+0xa4) and blip_y in [base_y, DAT_0065b4f8+0xa2):
    meth_0x414d70(display, blip_x, blip_y, 0, mbr_0x70 /*Marker*/, 0)   # :651-653
```
(`:644-653`). World coords are `>>3` (world-units → map-units). The clip bounds
`+0xa4 = +164` (x) and `+0xa2 = +162` (y) define the visible blip window inside
the chrome. The player (`DAT_00667fcc`) gets the same Marker; enemies route
through `meth_0x4c89c0_IsEnemy` (`:649`) — likely selecting a different Marker
tint/variant (UNCONFIRMED #4 — only one Marker asset measured; the enemy/ally
color distinction source is not pinned).

### 6d. 5×5 tile-cache load (`meth_0x52aa70`)
On scroll-cell or area change (`mbr_0x110!=mbr_0x10c || mbr_0x118!=mbr_0x114 ||
_DAT_00666970!=mbr_0x12c`, `:777`): shift the existing 5×5 ring (`mbr_0xa8..`,
25 slots) by the cell delta (keep overlap, free the rest, `:789-813`); for each
now-empty slot, `sprintf(buf, "%s\automaps\%d_%d_%d.bmp", area, cx, cy, level)`
(`:828`), load the 64×64 indexed bitmap (`FUN_004a1c00`/`FUN_004a1ec0`/
`FUN_004a2ce0`, `:829-838`), and `meth_0x4bd680(mbr_0x68, (sx+mbr_0x110)*0x40,
(mbr_0x118+sy)*0x40, tile, 2)` — stamp it at its 64-px grid cell into the mosaic
(`:871-873`). Missing tile → fill the cell black via `Box(...,0,0xffff,0x7f7f)`
(`:841-843`).

### 6e. Heightfield/lighting mesh (`meth_0x52af00`) — OUT OF SCOPE
Builds a vertex/lighting grid by sampling the mosaic (`FUN_00529300` reads
mosaic pixels) at LOD `iVar20` (1..4 super-sample taps) and writes a float
vertex buffer at `&DAT_0066e14c`/`&UNK_0066e2fc`. This is the 3D render data; its
triangle topology + lighting constants (`_DAT_005a3530` etc.) are renderer
internals. See §14 #1.

---

## §7 — Effects & shadows

- The map-body and tile blits use plain opaque/`DM_BACKGROUND` copies
  (drawmode `0x80000000` in Initialize stamps, `2` = `DM_TRANSPARENT`-ish for
  tile stamps `:873`). No drop shadow on the map body.
- `PlusSel` / `MinusSel` carry font/shadow flag `0x10400` (the `0x400`
  drop-shadow bit, §2) — their sprite art includes a baked shadow edge; blit
  them as-is (no extra shadow pass).
- Marker blips: `meth_0x414d70(... 0, Marker, 0)` — opaque/keyed copy, no
  shadow.
- Chroma key: assets declare `kc=0`; per UI_METHOD_MAP §16 the global magenta
  key `0x7C1F` applies to 15-bit sprites that use it (Marker/Amap edges). Use
  real alpha where the asset carries it; do not punch black.

No text in this panel (see §8), so no 3-pass text shadow and **no pink-halo
risk** here.

---

## §8 — Text rendering

**None in the automap pane.** The automap methods (`0x529970-0x52bdb0`) render
zero strings — there is no coordinate readout and no level label drawn by this
panel. The only `FUN_0058b100` (sprintf) call is the **filename** format
`"%s\automaps\%d_%d_%d.bmp"` (`:828`), not on-screen text. (The
`meth_0x4be2b0_CompositeBuffer` text calls in this decomp file at `:1491,:1506`
belong to the merged-in **TBarInvPane** methods, NOT the automap — §0 caveat.)

> If a level/area label is desired in the Revisited build it would be a NEW
> addition, not a retail element — out of scope for a faithful port. Marked
> N/A, not UNCONFIRMED.

---

## §9 — Animation & dynamic behavior

- **Scroll** (tween, intent from snapshot — retail re-projects each tick):
  ```
  tween scroll (mbr_0x11c/0x120):
    from   current scroll
    to     player-centered target (Player - viewport/2, clamped)
    over   ≤16 px/tick (snapshot MAX_AUTOMAP_SCROLL_SPEED)
    easing linear, dominant-axis capped (snapshot automap.cpp:1160-1175)
    drive  player movement; mbr_0x134/0x13c hold previous for change-detect
    on-end hold when scroll == target
    cite   automap.cpp:1133-1178 (intent); cls_0x5a5658.cpp:1026 (change-detect)
  ```
  Retail's mesh rebuild is gated on `bVar3` (scroll/zoom changed,
  `meth_0x52af00:1016-1030`) — only re-projects when the view actually moved.
- **Zoom:** integer percent `mbr_0x130`, init 100 (`:312`). Changed by the
  +/- buttons (PlusSel/MinusSel). The render denom `(W-0x40)*100` scales with
  it. UNCONFIRMED #3: zoom step size + min/max bounds (the click handler that
  mutates `mbr_0x130` is in the TBarInvPane-merged region or a slot not in this
  file).
- **Tile-cache dirty model:** redraw/reload when `mbr_0x110/0x114` (cell) or
  `_DAT_00666970` (area, → `mbr_0x12c`) change (`:777`). The mesh dirty when
  scroll/zoom change (`:1016-1027`).
- `DAT_005d7a6c` — automap-enabled gate (set 1 in Initialize `:313`); when 0 the
  body + mesh are skipped, only chrome draws.

---

## §10 — Input & dispatch

**Pane selection** is via the sidebar (B_r11): TSideTabsPane "Map" button →
TPlayScreen `FUN_0047cf40` case 0xc → sets `DAT_0065d1bc = 1`, presents
`DAT_0065b4f0`.

**In-pane interaction:** the pane is interactive (drag-to-scroll + zoom
buttons). Hit handling: TPane base `mbr_0xc/0x10` = 188/174 hit rect. The drag
handler lives in the merged-region method `virt_meth_0x52d6e0` shape but that
specific one is TBarInvPane (§0); the automap's own MouseClick/MouseMove are
snapshot-confirmed (`automap.h:57-58` `MouseClick`/`MouseMove`,
`DraggingMap`/`DragScroll*` fields) but the **retail in-pane click handler
address is UNCONFIRMED** (#5) — not isolated in this decomp file. Zoom buttons:
PlusSel/MinusSel hit rects = their 32×32 dst rects (§4); they mutate
`mbr_0x130`.

| control | hit rect (screen) | action | cite |
|---|---|---|---|
| map body (drag) | (472,324) 144×~144 | drag-scroll | snapshot `automap.cpp:1133+`; retail addr UNCONFIRMED #5 |
| PlusSel | (480,327) 32×32 | zoom in (mbr_0x130↑) | §4; step UNCONFIRMED #3 |
| MinusSel | (360,327) 32×32 | zoom out (mbr_0x130↓) | §4 |

---

## §11 — Retail bugs NOT to reproduce

- No text → no pink-halo bug in this panel.
- The merged TBarInvPane methods in the decomp file are a Ghidra artifact, not a
  bug — but do not port them here.
- Snapshot's single-pixel `0xFFFF` marker (`automap.cpp:213`) is the SUPERSEDED
  design; retail uses the 16×16 `Marker` sprite. Do not reproduce the pixel
  marker.

---

## §12 — Reconstruction pseudocode

```cpp
// Viewport (screen): VX=452, VY=306, W=188, H=174 (= Amap chrome).
// Body inset (20,18); body window clip width 144.

void MapPane::Initialize() {
  archive = OpenArchive("AutoMap.dat");
  marker  = archive.Next();   // 16x16
  amap    = archive.Next();   // 188x174 chrome
  plusSel = archive.Next();   // 32x32
  minusSel= archive.Next();   // 32x32
  tileMosaic = NewSurface(320,320);   // 5x5 of 64px tiles
  zoomPct = 100;
  ReadPlayerScrollCenter(&scrollX, &scrollY);
  LoadTiles5x5();             // §6d
  automapEnabled = true;
}

void MapPane::Update() {           // 24Hz gate
  if (scrollCellChanged() || areaChanged()) LoadTiles5x5();      // §6d
  if (scrollChanged() || zoomChanged())     RebuildMesh();       // §6e (renderer)
  EaseScrollTowardPlayerCenter();                                // §6a / §9
}

void MapPane::Draw() {                                  // virt_meth_0x52a5f0
  // 1. map-body: blit the scrolled mosaic (4 quadrants) into the body window
  PushClip(VX+0x14, VY+0x12, /*w*/0x90);
  DrawMapBodyQuadrants(zoomPct, scrollX, scrollY);      // §6b (meth_0x52a830)
  if (automapEnabled) {
    DrawBlips();                                         // §6c (meth_0x52a620)
    SubmitMapMesh();                                     // §6e (meth_0x52b520) renderer
  }
  PopClip();
  // 2. chrome overlay
  DrawBitmap(amap, VX, VY);                              // meth_0x52bdb0
  if (zoomCanIncrease) DrawBitmap(plusSel,  VX+0x14+8,    VY+0x12+3);
  if (zoomCanDecrease) DrawBitmap(minusSel, VX+0x14-0x70, VY+0x12+3);
}

void MapPane::DrawBlips() {                              // §6c
  for (e : creaturesWithin(player, 0x200)) {
    int bx = ((e.worldX>>3) - scrollX) * zoomPct * 0x90 / ((tileMosaicW-0x40)*100) + VX+0x14;
    int by = ((e.worldY>>3) - scrollY) * zoomPct * 0x90 / ((tileMosaicW-0x40)*100) + VY+0x12;
    if (inBlipWindow(bx,by)) DrawBitmap(marker, bx, by); // Marker 16x16
  }
}
```

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `meth_0x414d70` (= `FUN_00414d70`) tile/Amap/Marker blit | `DrawBitmapToTarget` / `DrawBitmapSubrectToTarget` | renderer |
| `meth_0x4bd680` tile stamp into mosaic | `DrawBitmapToTarget` (opaque) into the mosaic RT | renderer |
| `(*+0x64)` Box fill (missing tile → black) | `FillRectToTarget` / `Box` | surface/renderer |
| `meth_0x412150` / `meth_0x412250` clip set/get | `PushClip` / `GetClip` on the target | renderer |
| mosaic surface (`cls_0x5a68d8`, 320×320) | one offscreen `TSurface` tile RT | surface |
| heightfield mesh (`meth_0x52af00/0x52b520`) | the deferred map-mesh renderer (NEW; see §14 #1) | renderer (3D) |
| `meth_0x46d710` archive iterator | asset-cache `LoadArchive` + sequential `GetEntry` | assets |

Compose-to-target contract: assemble tiles → tile RT; render the framed scrolled
map into the pane RT body inset; draw Amap + buttons + Marker blips on the pane
RT; `DrawSurface` the pane RT into the HUD.

**Missing primitive:** the lit/scrolled automap mesh render (§6e) has no port
equivalent yet — needs a dedicated map-mesh path or a simplified flat scrolled
tile blit (the §6b 4-quadrant blit alone may suffice for a faithful-enough
minimap if the lighting mesh is deemed out-of-scope).

---

## §14 — UNCONFIRMED / open questions

1. **Map-body mesh internals (`meth_0x52af00` + `meth_0x52b520`)** — the
   per-vertex heightfield/lighting math, triangle topology, and lighting
   constants (`_DAT_005a3530/005a3568/005b92b8/005ac628`) are the **3D render
   path**, not UI. *Tried:* read both bodies; they build/submit a float vertex
   grid via `cls_0x411eb0` (the display/3D surface). *Impact if wrong:* the map
   body would render flat instead of lit, but framing/blips/chrome are
   unaffected. *Resolve:* treat as renderer scope; for a first port use the §6b
   flat 4-quadrant tile blit and skip the mesh, OR `DecompileAddr` the
   `cls_0x411eb0::meth_0x4174b0` mesh-submit kernel.

2. **Display-unit `0x90` (=144) → screen-pixel scaling** in §6b/§6c. *Tried:*
   the body clip is set to width `0x90` and dst byte coords derive from it; the
   `cls_0x411eb0` display surface appears pre-offset/pre-scaled to the pane
   region. *Impact:* blip/body could be a few px off if the display surface
   origin is not exactly the pane TL. *Resolve:* trace `cls_0x411eb0`'s window
   origin (the `&DAT_0065a578+4` display object) — confirm its (0,0) == pane TL.

3. **Zoom button gating + step.** PlusSel draws only if `mbr_0x9c!=0`, MinusSel
   only if `mbr_0xa0!=0` (`:1349,1354`); the zoom step / min-max applied to
   `mbr_0x130` and what sets `mbr_0x9c/0xa0` are not in this file. *Impact:*
   buttons might be always-on or hidden at zoom extremes. *Resolve:*
   FindImmRefs `mbr_0x130`/`mbr_0x9c`/`mbr_0xa0` writers; likely the in-pane
   MouseClick (see #5).

4. **Enemy vs ally blip distinction.** `meth_0x52a620` calls
   `meth_0x4c89c0_IsEnemy` (`:649`) before drawing, but only one `Marker` asset
   was measured. *Impact:* all blips might render identically. *Resolve:* check
   whether `mbr_0x70` (used) vs `mbr_0x74` (rotated Marker variant built in
   Initialize `:184-191`) is selected by IsEnemy, or a color tint is applied.

5. **In-pane MouseClick/MouseMove (drag-scroll) retail address.** Snapshot has
   `MouseClick`/`MouseMove` + `DraggingMap`/`DragScroll*` (`automap.h:57,103`),
   but the retail handler is not isolated in `cls_0x5a5658.cpp` (the
   `virt_meth_0x52d6e0` present is the merged TBarInvPane one). *Impact:*
   drag-to-scroll + button clicks unported. *Resolve:* `DumpVtable.java
   0x5a5658` to enumerate the real 32 slots and find the click slot
   (`0x444fb0`-family) address.

6. **`DAT_005d7a6c` exact semantics** (automap-enabled gate). Set 1 in
   Initialize; if cleared elsewhere the body hides. *Impact:* low. *Resolve:*
   FindImmRefs `0x5d7a6c`.
