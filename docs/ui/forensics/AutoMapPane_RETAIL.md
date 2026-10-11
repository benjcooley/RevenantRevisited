# TAutoMap (cls_0x5a5658) — retail forensics spec

> Measured 2026-10-10 from the disassembly (the HUD track's forensics agent)
> and confirmed in the emulator for the frame (`tools/retail_runtime/slots/hud/automap.py`,
> `hud_ab.py automap`). Supersedes `MapPane_SPEC.md`. The port is
> `src/automap.*` (docs/ui/HUD_REBUILD.md §6d).

Subject: the side-panel automap pane, global instance `0x0065b4f0`, vtable `0x5a5658`,
source fingerprint `d:\revenant\AutoMap.cpp` (strings `0x5e34a4…0x5e35c4`, used in the
render-state error reports of `0x52b520`).

**Method.** Everything below was read from capstone disassembly of
`recon/retail_asm/baseline/Revenant.rebuilt.exe` loaded through `HudWorld`; addresses are
instruction addresses. The Ghidra file `recon/ghidra/cls_0x5a5658.cpp` was used only as a
map; where it disagrees, the asm wins. Asset facts come from parsing
`$R/data/resources_unzipped/automap.dat` and the module tiles in
`$R/data/Modules/Ahkuilon_unzipped/Automaps/`. ($R = the data checkout, `RevenantRevisited/`.)

Confidence: **[C]** read directly in the asm / data; **[M]** inferred from strong evidence
(cited); anything weaker is in the UNCONFIRMED list at the end.

---

## 0. Background needed by every section

### 0.1 Object layout (fields used by this class)

| Field | Meaning | Evidence |
|---|---|---|
| +4/+8/+0xc/+0x10 | pane live x, y, w, h = 452, 306, 188, 174 (also read as globals `0x65b4f4/0x65b4f8/0x65b4fc/0x65b500`) | [C] used throughout |
| +0x50 | TPane **dirty** flag: slot 10 `0x444fb0` writes 1, slot 11 `0x444fc0` (`SetDirty(v)`) writes v | [C] `0x444fb0`, `0x444fc4` |
| +0x60 | `SAutoMapTravels*` = `&player->+0x314` (per-player discovery store, §3.2) | [C] `0x52a0d5`, `0x52baf0` |
| +0x64 | current level's visgrid (0x2084-byte block, §3.1) | [C] `0x52a201`, `0x52bd2e` |
| +0x68 | mosaic surface 320×320, flags `0x4000484` (CPU-side staging copy) | [C] `0x5299cb..0x529a12` |
| +0x6c | mosaic surface 320×320, flags `0x4000884` (the copy that is drawn) | [C] `0x529a5a..0x529ab3` |
| +0x70 | TDDSurface (Marker w×h, `0x488`) — blip marker texture | [C] `0x529cdb..0x529d11` |
| +0x74 | TDDSurface from bitmap "Marker" (`0x4a5ca0`, `0x288`) | [C] `0x529bf8..0x529c2d` |
| +0x78 | mosaic built from bitmap "Amap" (`0x4bb870`, `0x488`) | [C] `0x529c62..0x529c9c` |
| +0x7c | mosaic (Amap w×h, `0x888`) — chrome texture that is drawn | [C] `0x529d4b..0x529d89` |
| +0x80 | TDDSurface (PlusSel w×h, `0x288`) — scratch for the button art | [C] `0x529b92..0x529bc8` |
| +0x84 / +0x88 | TDDSurface (PlusSel w×h, `0x488`) — PlusSel / MinusSel textures | [C] `0x529dc8..0x529e89` |
| +0x8c/+0x90/+0x94/+0x98 | bitmaps "Marker", "Amap", "PlusSel", "MinusSel" from `AutoMap.dat` (blob `DAT_0066f330`) | [C] `0x529afc..0x529b4d` |
| +0x9c | "+" (zoom-in) held | [C] §2 |
| +0xa0 | "−" (zoom-out) held | [C] §2 |
| +0xa4 | free-scroll mode (set by a left press in the map circle, cleared by right-button up) | [C] §2 |
| +0xa8..+0x10b | `TBitmap* tile[5][5]`, index `[i*5+j]`, i = x, j = y | [C] `0x52aacb` (0x2a·4 = 0xa8) |
| +0x10c / +0x114 | tile base x / y at the previous RecordTravels | [C] `0x52ba3c`, `0x52ba30` |
| +0x110 / +0x118 | tile base x / y = scroll origin `>> 6` | [C] `0x52ba60`, `0x52ba4b` |
| +0x11c / +0x120 | scroll origin x / y, in **mosaic pixels** | [C] §1.1 |
| +0x12c | level id the tile cache was built for | [C] `0x52ae58` |
| +0x130 | zoom percent (100…300) | [C] §1.1 |
| +0x134 / +0x13c | player screen pos x / y at the previous RecordTravels | [C] `0x52ba69`, `0x52ba77` |
| +0x138 / +0x140 | player screen pos x / y (`GetScreenPos`) | [C] `0x52ba7d`, `0x52a218` |

Statics owned by the class: `DAT_0066f330` AutoMap.dat blob; `DAT_0066f334` pulse
sub-counter; `DAT_0066e148` blip animation frame 0..3; `DAT_0066e2dc` / `DAT_0066e2e0` last
mouse x / y (pane coords); `DAT_0066e14c[100]` float fog values (stored `[col*10+row]`);
`DAT_0066e2e4[486]` uint16 indices; `DAT_0066e6b0[100]` 32-byte vertices; `DAT_0066f338`
zoom of the last fog build; `DAT_005e32f0` / `DAT_005e32f4` snapped fog origin x / y
(initial value 0x7fffffff in .data); `DAT_005d7a6c` fog-of-war flag (initial 1).

Engine globals read: `DAT_00667fcc` player; MapPane `0x6668d8` fields +0x1c/+0x20 (newwidth/
newheight; written 640×480 or 0x1c4×0x12e at the asm-file lines 256934 / 257050),
+0x2c/+0x30 (current scroll x/y, copied into posx/posy at the end of MapPane Pulse
`0x454421..0x45443e`), +0x98 (`DAT_00666970`, the map pane's **level**, per
docs/gameflow/forensics/EXITS.md:186); TModuleMgr at `0x65a768`: +0x14 module pointer array
(`DAT_0065a77c`), +0x1c current module index (`DAT_0065a784`), +0x20 module directory path
(`DAT_0065a788`); module record +0x58 = module folder name (used by
TModuleMgr's find-by-name `stricmp(rec+0x58, name)` in `recon/ghidra/cls_0x4609f0.cpp:226-229`
and to build the path "<ModulesPath><name>.rvm" / "<ModulesPath><name>\" at `:82-131`).

### 0.2 Coordinate spaces

| Space | Unit | Relation |
|---|---|---|
| map-screen | isometric screen px of the main map | `TObjectInstance::GetScreenPos` `0x46eb40`: `sx = (int)(x − y)`, `sy = (int)(0.5x + 0.5y − 0.867z)` (constants at `0x5d4740..0x5d4768`), cached at obj+0xcc/+0xd0 [C] |
| mosaic px | 1 mosaic px = 8 map-screen px | `(sx >> 3)` in blips `0x52a73d`; scroll from `(…) >> 3` `0x52a38d` [C] |
| tile | 64×64 mosaic px = 512×512 map-screen px | `tilebase = origin >> 6` `0x52ba48/0x52ba57`; stamp at `tile*64` [C] |
| visgrid word | 512×512 map-screen px (one tile) | `x >> 9` in `0x529300/0x5293f0` [C] |
| visgrid bit | 128×128 map-screen px (4×4 per word) | `(x mod 512) >> 7` [C] |
| window | 144×144 screen px at pane+(0x14,0x12) = screen (472,324) | clip set to `(px+0x14, py+0x12, 0x90, 0x90)` in `0x52a62e..0x52a697`, `0x52a944..0x52a975`, `0x52b828..0x52b85f` [C] |

Screen px per mosaic px = `Z·0x90 / ((W−0x40)·100)` with `W` = mosaic width (`[+0x68]+4` =
320) and `Z` = +0x130, all integer arithmetic with signed truncating `idiv` [C]
(`0x52a2d4..0x52a2ed`, `0x52a733..0x52a74f`, `0x52a863..0x52a87f`). At Z = 100 the window
shows `(320−64)·100/100 = 256` mosaic px; at Z = 300 it shows 85 (`3686400/43200`).

Mosaic-px → screen mapping used by blips (and implied by the body and fog):
`screen_x = ((sx>>3) − [+0x11c])·Z·0x90 / ((W−0x40)·100) + paneX + 0x14`, same for y with
+0x120 and paneY + 0x12 [C] `0x52a739..0x52a776`.

### 0.3 Per-pulse order

TPlayScreen at `0x47c21d` calls the pane pulse pass `0x48fda0` (which calls every pane's slot
19), then at `0x47c224..0x47c23c`: `if (!(modeflags & 8)) AutoMap.RecordTravels()`
(`0x52b9f0`) [C]. Flag 8 is the modal "pulse only the top pane" bit
(docs/gameflow/forensics/INGAME_MENU.md:152), so under a modal both the automap's Pulse and
RecordTravels stop together. So in each pulse: **Pulse (slot 19) first, RecordTravels
second**; the draw (slot 7) happens later in the animate pass. The pulse rate is the screen
pulse (~24 Hz per INGAME_MENU.md:185, "5 pulses (~0.2 s)" — not re-measured here).

Consequences [C, from the field flow]: the tile base +0x110 used by the tile cache in Pulse
k was computed by RecordTravels k−1 from the scroll of Pulse k−1 (one-pulse lag), and the
fog's "player moved" test in Pulse k compares the positions stored by RecordTravels k−1.
The tile cache relies on exactly one RecordTravels between two Pulses (it never updates
+0x10c itself; see §4).

---

## 1. Slot 19 Pulse `0x52a280`, RecordTravels `0x52b9f0`, slots 20 / 21

### 1.1 Slot 19 — `Pulse()` `0x52a280` [C]

```c
void TAutoMap::Pulse()                                   // 0x52a280
{
    // blip animation: frame advances every 5 pulses, cycles 0..3
    if (++g_pulseSub /*0x66f334*/ > 4) {                 // 0x52a281..0x52a295
        g_pulseSub = 0;
        if (++g_blipFrame /*0x66e148*/ > 3) g_blipFrame = 0;   // 0x52a297..0x52a2af
    }

    int W = mosaic68->width;                              // [+0x68]+4
    int cx, cy;                                           // view centre, only valid in free-scroll
    if (freeScroll /*+0xa4*/) {                           // 0x52a2b5
        int half = trunc_div2(((W - 0x40) * 14400) / (zoom * 0x90));   // 0x52a2bf..0x52a2f2
        cx = scrollX /*+0x11c*/ + half;                   // 0x52a2f4
        cy = scrollY /*+0x120*/ + half;                   // 0x52a2f6
    }

    int oldZoom = zoom;                                   // edx, 0x52a310
    if (zoomInHeld /*+0x9c*/) {                           // 0x52a30e
        zoom = min(zoom + 10, 300);                       // 0x52a314 add 0xa; 0x52a317 cmp 0x12c; 0x52a31e
    } else if (zoomOutHeld /*+0xa0*/) {                   // 0x52a325
        zoom = max(zoom - 10, 100);                       // 0x52a32f add -0xa; 0x52a332 cmp 0x64; 0x52a337
    }                                                     // store 0x52a33c (only when a button is held)

    if (!freeScroll) {                                    // follow mode, 0x52a342..0x52a3a9
        int h = trunc_div2(((0x40 - W) * 14400) / (zoom * 0x90));  // negative half-span, NEW zoom
        scrollX = ((trunc_div2(MapPane.newwidth  /*0x6668f4*/) + MapPane.scrollx /*0x666904*/) >> 3) + h;
        scrollY = ((trunc_div2(MapPane.newheight /*0x6668f8*/) + MapPane.scrolly /*0x666908*/) >> 3) + h;
    } else if (zoom != oldZoom) {                         // 0x52a3ab..0x52a3e5: keep the view centre
        int h = trunc_div2(((0x40 - W) * 14400) / (zoom * 0x90));
        scrollX = cx + h;
        scrollY = cy + h;
    }

    RebuildTileCache();                                   // 0x52a3ed -> 0x52aa70 (does nothing unless dirty, §4)
    BuildFogMesh();                                       // 0x52a3f4 -> 0x52af00 (positions every pulse, values when dirty, §5)
}
// trunc_div2(v) = cdq; sub eax,edx; sar eax,1  -> v/2 rounded toward zero.
// ">> 3" is an arithmetic shift (floor).  14400 = 5*5*9*64 (the lea/shl chain).
```

- **Scroll rule.** In follow mode the automap is centred, every pulse and with no easing, on
  the **main map view's centre** (map pane width/2 + scroll x, height/2 + scroll y), divided by
  8 — i.e. on the camera, not directly on the player. Any smoothing comes from the map pane's
  own camera. (The 1998 snapshot's `MAX_AUTOMAP_SCROLL_SPEED` target-scroll easing,
  `$R/legacy/automap.h:26`, is not in retail.)
- **Zoom.** ±10 percentage points per pulse while a button is held; clamped to [100, 300];
  "+" wins if both flags are set.
- In free-scroll mode the scroll origin is left alone except to keep the centre fixed when
  the zoom changes.
- Tile cache and fog mesh are called every pulse; each has its own dirty test.

### 1.2 RecordTravels `0x52b9f0` (not a vtable slot; called from `0x47c23c`) [C]

```c
void TAutoMap::RecordTravels()                            // 0x52b9f0
{
    if (!Player /*0x667fcc*/) return;                     // 0x52ba15
    int level = MapPane.level;                            // 0x666970, 0x52ba1b

    prevTileX = tileX;  prevTileY = tileY;                // +0x10c = +0x110 (0x52ba3c); +0x114 = +0x118 (0x52ba30)
    tileY = scrollY >> 6;  tileX = scrollX >> 6;          // +0x118 (0x52ba4b), +0x110 (0x52ba60)
    prevPlayerX = playerX; prevPlayerY = playerY;         // +0x134 = +0x138 (0x52ba69); +0x13c = +0x140 (0x52ba77)
    Player->GetScreenPos(&playerX, &playerY);             // 0x52ba7d, writes +0x138, +0x140

    if (!travels /*+0x60*/) BindTravels();                // 0x52ba82..0x52bb57, same as Initialize §6
                                                          // (deletes records with 0x52c720)
    if (grid /*+0x64*/) {
        if (grid->level == level) goto mark;              // 0x52bb6b..0x52bb73
        // level changed: store the old level's grid in the player's record list
        SVisRecord *r = new SVisRecord;  r->level = grid->level;          // 0x52bb83..0x52bba9
        RleEncode(grid->words, 0x1040, &r->nwords, &r->data);             // 0x52bbc9 (0x5295e0) + copy
        for (i = 0; i < travels->records.count; ++i)                       // 0x52bc22..0x52bc3c
            if (travels->records[i]->level == grid->level) {               // first match only
                delete-record(travels->records[i]); travels->records.RemoveAt(i); break;   // 0x52bc3e..0x52bc72
            }
        travels->records.Add(r);                                          // 0x52bc7e (0x41c840)
        free(grid); grid = NULL;                                          // 0x52bc87..0x52bc8f
    }
    for (i = 0; i < travels->records.count; ++i)          // 0x52bc96..0x52bcfc (no break: last match wins,
        if (travels->records[i]->level == level)          //  an earlier match's grid leaks)
            grid = new SVisGrid(travels->records[i]);     // 0x529220
    if (!grid) { grid = calloc-like new SVisGrid; grid->level = level; }  // 0x52bd05..0x52bd2e (0x820 dwords zeroed)
mark:
    if (playerX != prevPlayerX || playerY != prevPlayerY)                 // 0x52bd31..0x52bd50
        for (dx = -0x200; dx <= 0x200; dx += 0x80)                       // 0x52bd52, 0x52bd86..0x52bd92
            for (dy = -0x200; dy <= 0x200; dy += 0x80)                   // 0x52bd57, 0x52bd78..0x52bd84
                SetVisBit(grid, playerX + dx, playerY + dy, 1);          // 0x52bd73 (0x5293f0)
}
```

Discovery is therefore **per pulse, only when the player's screen position changed**, and
reveals a 9×9 block of 128-px sub-cells (±512 map-screen px) around the player.

### 1.3 Slot 20 — `DrawBackground()` `0x52bf00` [C]

```c
void TAutoMap::DrawBackground()                            // 0x52bf00
{
    if (dirty /*+0x50*/) {                                  // 0x52bf03
        this->vtbl[21](Display /*PTR_DAT_005d79e0*/, 0x10 /*DM_BACKGROUND*/);   // 0x52bf0a..0x52bf17
        this->SetDirty(FALSE);                              // vtbl +0x2c, 0x52bf1c..0x52bf20
    }
}
```
Initialize ends with `SetDirty(TRUE)` (`0x52a25e..0x52a262`), so slot 21 runs once after
Initialize and again whenever something marks the pane dirty.

### 1.4 Slot 21 `0x52bf30(TSurface *dst, int drawmode)` — 2D background + texture refresh [C]

```c
if (dst != Display) dst->SetOrigin(0, 0x132);             // vtbl +0x40, 0x52bf41..0x52bf51
dst->Box(0, 0, 0xbc, 0xae, 0 /*color*/, 0xffff, 0, drawmode);   // vtbl +0x64, 0x52bf55..0x52bf71
dst->Put(0, 0, bmAmap, drawmode, NULL);                   // 0x4bd680, 0x52bf74..0x52bf81
scratch80->Put(0, 0, bmPlusSel,  0x80000000, NULL);       // 0x52bf86..0x52bf9b
tex84->ParamBlit(MakeDP(0,0,0,0,w80,h80,0x80000000), scratch80);   // 0x52bfa0..0x52bfdf
scratch80->Put(0, 0, bmMinusSel, 0x80000000, NULL);       // 0x52bff8..0x52c00d
tex88->ParamBlit(MakeDP(0,0,0,0,w80,h80,0x80000000), scratch80);   // 0x52c012..0x52c051
tex70->ParamBlit(MakeDP(0,0,0,0,w74,h74,0x80000000), surf74);       // marker, 0x52c06a..0x52c0a3
mosaic7c->ParamBlit({drawmode 0x80000000, d=(0,0,w78,h78), s=(0,0,w78,h78), intensity 0x1f}, mosaic78); // 0x52c0bc..0x52c134
mosaic6c->ParamBlit(MakeDP(0,0,0,0,320,320,0x80000000), mosaic68);  // map body, 0x52c14d..0x52c17d
if (dst != Display) dst->SetOrigin(0, 0);                 // 0x52c180..0x52c190
```
(Surface vtable identities: `+0x40` = SetOrigin (base `0x419e60` writes +0x14/+0x18),
`+0x44` = SetClipRect (base `0x419e80` writes +0x20..+0x2c), `+0x48` = SetClipMode (base
`0x419ea0` writes +0x1c), `+0x5c` = ParamBlit, `+0x64` = Box — [C] for the three setters
from their bodies; ParamBlit/Box per RETAIL_HUD_CODE_MAP.md §2.)

---

## 2. Mouse: slots 25 / 26

Button codes (retail, as in MAP_INPUT.md §4 and `src/revdefs.h:268-277`): 1 = left down,
2 = right down, 4 = left up, 5 = right up. `x, y` are pane-local.

Bitmap sizes from `automap.dat` [C]: Marker 16×16; Amap 188×174 (flags 0x10000); PlusSel
32×32 reg (−8,−3); MinusSel 32×32 reg (−144,−3). The reg points match the hit/draw offsets
(8,3) and (0x90,3).

### 2.1 Hit regions (inlined in slot 25; same code as the uncalled helper `0x52c340`) [C]

```c
int Region(x, y) {
    BM *p = bmPlusSel;                                      // +0x94 used for BOTH buttons
    if (x > 8    && x < 8    + p->w && y > 3 && y < 3 + p->h) return 1;   // "+" : 0x52c1b7..0x52c1da
    if (x > 0x90 && x < 0x90 + p->w && y > 3 && y < 3 + p->h) return 2;   // "-" : 0x52c1e1..0x52c20a
    int d2 = (y - 0x57)*(y - 0x57) + (x - 0x5a)*(x - 0x5a);               // 0x52c211..0x52c222
    return d2 < 0x1299 ? 3 : 0;                                           // 0x52c226..0x52c231 (0x1299 = 4761 = 69^2)
}
```
So "+" = x 9..39, y 4..34; "−" = x 145..175, y 4..34 (strict inequalities, 32×32 art); map
circle centre pane (90, 87) = screen (542, 393), radius² < 4761. (The circle centre is not the
window centre (92, 90).)

### 2.2 Slot 25 `MouseClick(button, x, y)` `0x52c1a0` [C]

```c
if (button == 1) {                                          // 0x52c1a5
    switch (Region(x, y)) {
    case 1: zoomInHeld  = 1; break;                         // +0x9c, 0x52c268
    case 2: zoomOutHeld = 1; break;                         // +0xa0, 0x52c259
    case 3: lastMouseY = y; lastMouseX = x;                 // 0x66e2e0 / 0x66e2dc, 0x52c23d/0x52c243
            freeScroll = 1; break;                          // +0xa4, 0x52c24a
    }                                                       // region 0: nothing
} else if (button == 4) {                                   // 0x52c276
    zoomOutHeld = 0; zoomInHeld = 0;                        // 0x52c27e, 0x52c284 (anywhere; freeScroll untouched)
    lastMouseX = x; lastMouseY = y;                         // 0x52c292, 0x52c297
} else if (button == 5) {                                   // 0x52c2a1
    freeScroll = 0;                                         // 0x52c2a6 -> next Pulse recentres on the camera
}
```

### 2.3 Slot 26 `MouseMove(button, x, y)` `0x52c2c0` [C]

```c
if (x >= 0 && y >= 0 && x < w /*+0xc*/ && y < h /*+0x10*/ && button == 1      // 0x52c2c5..0x52c2e1
    && (y-0x57)*(y-0x57) + (x-0x5a)*(x-0x5a) < 0x1299) {                   // 0x52c2e3..0x52c2fd
    scrollX += lastMouseX - x;  lastMouseX = x;             // 0x52c2ff..0x52c31b
    scrollY += lastMouseY - y;  lastMouseY = y;             // 0x52c315..0x52c330
}
```
- **Dragging scrolls the map**: one **mosaic** px per screen px of mouse travel, independent of
  zoom (so the map moves at 0.5625× the cursor speed at Z=100 and 1.6875× at Z=300), in the
  "grab" direction. MouseMove does not test +0xa4; the scroll only persists because the left
  press in the circle set +0xa4 (otherwise the next Pulse overwrites it).
- The zoom buttons are **hold-to-zoom**: a left press sets the flag; Pulse steps 10 per pulse;
  the left release clears both flags wherever it happens.
- Right-button release returns to follow mode. No wheel, keyboard or double-click handling
  exists in these two slots.

---

## 3. Discovery data

### 3.1 The visgrid (`+0x64`, 0x2084 bytes) [C]

```c
struct SVisGrid {            // 0x2084 bytes
    int32_t  level;          // +0     (MapPane level id)
    uint16_t word[65][64];   // +4     0x1040 words; word index = row*64 + col
};
```
Cell addressing (identical in `0x529300` and `0x5293f0`):

```c
// returns false if out of range
bool VisAddr(int x, int y, int *wordIdx, int *bit) {
    int cx = x >> 9, cy = y >> 9;                           // 0x52930e/0x529311 (arith)
    if (cx > 0x3f || cx < -0x3f || cy < 0 || cy > 0x3f) return false;   // 0x529314..0x529331
    if (cx < 0) {                                           // 0x529337
        cx += 0x41;                                         // rows 2..64
        cy = (cy <= 0x1f) ? cy + 0x20 : cy - 0x20;          // 0x52933e..0x529348
    }
    *wordIdx = cx * 64 + cy;                                // 0x52934b..0x52934e
    int bx = posmod(x, 512) >> 7, by = posmod(y, 512) >> 7; // 0x529359..0x52938d, 0x529394..0x5293c8
    *bit = by * 4 + bx;                                     // 0x5293cb..0x5293ce
    return true;
}
bool GetVisBit(grid, x, y)  /*0x529300, thiscall, ret 8*/  { ... return (grid->word[idx] >> bit) & 1; }  // 0x5293d5..0x5293d9; out of range -> 0
void SetVisBit(grid, x, y, on) /*0x5293f0, ret 0xc*/ { if (on) word |= 1<<bit; else word &= ~(1<<bit); }   // 0x5294b3..0x5294e2
```
- x, y are **map-screen** coordinates (the player's `GetScreenPos`), so one word = one 64×64
  automap tile and one bit = a 16×16 mosaic-px sub-cell. The module tiles agree: file names
  in `Ahkuilon_unzipped/Automaps/` use tile x −59..33 and tile y 1..49 (y never negative).
- Negative columns fold into rows 2..64 with cy shifted by 32, so negative and positive
  columns can alias the same word (e.g. cx = 2, cy = 40 and cx = −63, cy = 8). Retail quirk.

### 3.2 Per-player store at `player+0x314` [C unless marked]

```c
struct SAutoMapTravels {          // at player + 0x314
    uint32_t ???;                  // +0x00  never touched by automap code (UNCONFIRMED)
    char     moduleName[32];       // +0x04  (player+0x318); size bounded by +0x24 [M]
    TPtrArray<SVisRecord*> records;// +0x24  count at +0x24, data pointer at +0x34 (player+0x338/+0x348)
};
struct SVisRecord {               // 12 bytes, new'd
    int32_t   level;               // +0  (-1 after reset, 0x5295b0)
    int32_t   nwords;              // +4  length of the RLE stream in uint16 words
    uint16_t *data;                // +8  RLE stream (malloc'd, 0x482ef0)
};
```
- `moduleName` is the **current module's folder name** (TModuleMgr module record +0x58),
  copied when the store is (re)bound; it is how the automap notices that the player's saved
  discovery data belongs to another module (then all records are dropped). The compare in
  Initialize / RecordTravels / `0x52c3d0` is an inline case-sensitive `strcmp`.
- RLE stream (decoder `0x529150`): pairs `(ctrl, value)` of uint16; if `ctrl & 0x8000` emit
  `value` `(ctrl & 0x7fff) + 1` times, else emit it once; pairs are consumed while the pair's
  end offset ≤ `2*nwords` bytes (`0x52916d..0x529203`). `SVisGrid(rec)` (`0x529220`) decodes
  into a temp buffer, requires exactly 0x1040 words (else
  `"error decompressing level visgrid for level %d"`, `0x529287..0x5292ab`), copies 65 rows
  of 0x80 bytes (`0x5292b3..0x5292d2`). The encoder is `0x5295e0` (runs → `0x8000|n`, value;
  singles → `0`, value).
- Save `0x529830` (called from the player saver at `0x51c14c`): if `AutoMap.travels == this`
  first `AutoMap.FlushGrid()` (`0x52c5c0`); then write `moduleName` (`0x49ccc0`), int32 count,
  then per record int32 level, int32 nwords, nwords × uint16 (`0x52985b..0x529961`).
  Load `0x529770` (from `0x51bcf3`): the mirror image (`0x49ce00` for the name).
- `FlushGrid` `0x52c5c0`: RLE-encode `+0x64` into a new record, remove the first record with
  the same level (`0x52c69d..0x52c6ed`), append (`0x52c6fd`). The in-memory grid stays.
- `0x52c3d0(player)` (called at `0x51ef84` with 0 and `0x51f03e` with the new player): sets
  `+0x60 = 0`; if the argument is non-null, frees `+0x64` **without flushing**, rebinds to the
  global player's store (module-name check; mismatch → copy the module name, or `"Revenant"`
  (`0x5e3794`) when there is no current module, and delete every record via `0x52c720`), then
  loads the grid for `MapPane.level` or creates a zeroed one (`0x52c50a..0x52c599`).
- Record-list wipe loops (Initialize `0x52a12b..0x52a157`, RecordTravels `0x52bb3c..0x52bb57`,
  `0x52c4f0..0x52c507`) call `RemoveAt(i)` while incrementing `i`, so every second record is
  skipped (leaked), then force the count to 0. Retail bug; no visible effect.

### 3.3 `FUN_0052ae70` (called by Initialize) — fog vertex static init [C]

```c
for (col = 0; col < 10; ++col)                              // edx = 0x66e6b4 + col*0x20 .. 0x66e7f4
    for (row = 0; row < 10; ++row) {                        // eax += 0x140 per row
        V &v = vert[row*10 + col];                          // base 0x66e6b0, stride 0x20
        v.x   = (float)(paneX + 0x14 + col*0x10);            // 0x52ae9b..0x52aeaa
        v.y   = (float)(paneY + 0x12 + row*0x10);            // 0x52aead..0x52aebe
        v.z   = 0x3f7fbe77;  /* 0.999f */                    // 0x52aec4
        v.rhw = 1.0f;                                        // 0x52aeca
    }
```
x/y are overwritten by every fog build (§5); z and rhw are only written here. Diffuse is
written by the build; specular and u/v are never written by any code (no other references to
the `0x66e14c..0x66eab0` range exist in .text — scanned) and lie in the zero-initialized part of
.data, so they are 0.

---

## 4. Tile cache `0x52aa70` [C]

```c
void TAutoMap::RebuildTileCache()                           // 0x52aa70
{
    if (tileX == prevTileX && tileY == prevTileY && MapPane.level == cacheLevel) return;  // 0x52aa79..0x52aaa7

    TBitmap *keepBm[25] = {0}; int keep[25] = {0};          // 0x52aab0..0x52aad0
    for (i = 0; i < 5; ++i) for (j = 0; j < 5; ++j) {       // old slots; 0x52aadc..0x52ab7a
        unsigned ni = (prevTileX - tileX) + i;              // 0x52aadc..0x52aaf4
        unsigned nj = (prevTileY - tileY) + j;              // 0x52aaf6..0x52ab00
        if (cacheLevel == MapPane.level && ni < 5 && nj < 5) {     // 0x52ab02..0x52ab18
            keepBm[ni*5+nj] = tile[i][j]; keep[ni*5+nj] = 1; tile[i][j] = 0;   // 0x52ab1a..0x52ab34
        } else { if (tile[i][j]) free(tile[i][j]); tile[i][j] = 0; }          // 0x52ab41..0x52ab5b
    }
    memcpy(tile, keepBm, 25*4);                             // 0x52ab80..0x52ab91

    for (i = 0; i < 5; ++i) for (j = 0; j < 5; ++j) {       // 0x52ab93..0x52ae4a
        if (tile[i][j] || keep[i*5+j]) continue;            // 0x52aba6..0x52abb0
        int tx = tileX + i, ty = tileY + j;
        sprintf(path, "%sautomaps\\%d_%d_%d.bmp",           // fmt 0x5e3464, 0x52abb6..0x52abe2
                g_modulePath /*0x65a788*/, MapPane.level /*0x666970*/, tx, ty);
        if (FileExists(path, 0) /*0x4a1c00*/) {             // 0x52abee..0x52abfa
            TBitmap *bm = NewBitmap(64, 64, BmpFormat(path) /*0x4a30d0*/, 0);   // 0x4a1ec0, 0x52abfc..0x52ac12
            tile[i][j] = bm; bm->LoadBMP(path);             // 0x4a2ce0, 0x52ac25..0x52ac2a
        } else tile[i][j] = 0;                              // 0x52ac31..0x52ac35
        if (tile[i][j])
            mosaic68->Put(tx*64, ty*64, tile[i][j], 2 /*DM_WRAPCLIP*/, NULL);   // 0x52ac45..0x52ac66 (error 0x5e347c)
        else
            mosaic68->Box(tx*64, ty*64, 64, 64, 0, 0xffff, 0x7f7f, 0x80000000); // black, 0x52ad1c..0x52ad4d
        SDrawParam dp = {0};                                // 0x54-byte struct, src/graphics.h:58-94
        dp.drawmode = 6;  /* DM_WRAPCLIP|DM_WRAPCLIPSRC */  // 0x52acc1 / 0x52ada2
        dp.dx = dp.sx = tx*64;  dp.dy = dp.sy = ty*64;      // 0x52ac98..0x52acba
        dp.dwidth = dp.dheight = dp.swidth = dp.sheight = 64;
        dp.intensity = 0x1f;                                // 0x52ad07 / 0x52ae0c
        mosaic6c->ParamBlit(&dp, mosaic68, 0, 0);           // vtbl +0x5c, 0x52ae23
    }
    cacheLevel = MapPane.level;                             // +0x12c, 0x52ae50..0x52ae58
}
```
- **Path**: `"<module dir>\automaps\<level>_<tileX>_<tileY>.bmp"`. `g_modulePath` is
  TModuleMgr+0x20 = ModulesPath + module folder name + `"\"` (`cls_0x4609f0.cpp:82-131`,
  suffix string `0x5d0e18` = `"\"`), so in practice the file comes from the mounted module
  archive, e.g. `Ahkuilon.rvm` → `Automaps/0_-10_25.bmp` (64×64, 8-bit BMP — verified).
  Level = MapPane level; tile x/y = `(scrollOrigin >> 6) + i/j`, i, j = 0..4, i.e. the 5×5
  ring starts at the tile containing the view's top-left mosaic px.
- **Ring shift**: tiles still inside the new 5×5 window keep their bitmaps and are not
  re-stamped; the rest are freed. A missing-file slot that survives a shift keeps `keep = 1`
  with a NULL bitmap, so it is not retried.
- **Mosaic stamping**: both mosaics are put in wrap mode at Initialize
  (`SetClipMode(1 /*CLIP_WRAP*/)` + `SetClipRect(-1000000, -1000000, 2000000, 2000000)`,
  `0x529a09..0x529a31` for +0x68, `0x529a96..0x529ac0` for +0x6c; mosaic overrides
  `0x4bbd30` / `0x4bbcd0` store the mode/rect). Tile (tx, ty) therefore lands at
  `((tx*64) mod 320, (ty*64) mod 320)`: the 320×320 mosaic is a torus holding exactly the 5×5
  ring, each tile in its residue slot, so a shift only overwrites the departing tiles' slots.
  The Put uses drawmode 2 (wrap) explicitly; the Box relies on the surface's wrap mode [M].
- **Missing tile**: `Box(…, color 0, zpos 0xffff, normal 0x7f7f, DM_USEDEFAULT)` = black.
- **+0x6c from +0x68**: a straight 64×64 copy of the same region (drawmode 6 = wrap dest + wrap
  source, no effect bits, intensity 0x1f), per tile, right after stamping; plus a full 320×320
  copy at Initialize (`0x529ac3..0x529af3`) and in slot 21 (`0x52c14d..0x52c17d`).
  +0x68 (flags 0x4000484) is the CPU-written copy and +0x6c (0x4000884) the one handed to the
  quad drawer; the same 0x4xx → 0x8xx pairing is used for Amap (+0x78 → +0x7c) [M: role of
  the 0x400/0x800 bits per RETAIL_HUD_CODE_MAP §2].
- **The 0xfff0bdc0 values are not a clear colour**: they are the x/y of the
  `SetClipRect(-1e6, -1e6, 2e6, 2e6)` call (vtbl +0x44, 4 args, `0x529a1b..0x529a31`).

### 4.1 How +0x6c is drawn (body blit `0x52a830`, for reference) [C]

```c
int W = mosaic6c->w, H = mosaic6c->h, D = (mosaic68->w - 0x40) * 100;     // 0x52a84a..0x52a87c
int qw = zoom*W*0x90 / D, qh = zoom*H*0x90 / D;                            // 0x52a86d..0x52a895 (180 at Z=100)
int ox = paneX+0x14 - posmod(scrollX, W)*zoom*0x90 / D;                    // 0x52a899..0x52a8cc
int oy = paneY+0x12 - posmod(scrollY, H)*zoom*0x90 / D;                    // 0x52a8c6..0x52a8fb
int ox2 = ox + qw, oy2 = oy + qh;                                          // 0x52a8fd..0x52a90a (before the +1)
int drawH = DAT_0066818c ? qh + 1 : qh;                                    // 0x52a90e..0x52a91a (height only)
save clip; SetClip(paneX+0x14, paneY+0x12, 0x90, 0x90);                    // 0x52a91e..0x52a975
// 4 quads at (ox,oy) 0x52a9a3, (ox2,oy) 0x52a9d1, (ox,oy2) 0x52aa03, (ox2,oy2) 0x52aa31:
    Quad(qx, qy, 0, mosaic6c, 0, qw, drawH, 0xffffffff, 0, 0, W, H, NULL, 0); // 0x414d70
restore clip;
// posmod here = (v % W) + (v < 0 ? W : 0)  (a negative multiple of W gives W, not 0)
```

---

## 5. Fog mesh

### 5.1 Vertex layout (FVF 0x1c4 = XYZRHW | DIFFUSE | SPECULAR | TEX1, stride 0x20) [C]

| Offset | Field | Value |
|---|---|---|
| +0x00 | float x | screen x, every build (`0x52b4b2`) |
| +0x04 | float y | screen y, every build (`0x52b4bd`) |
| +0x08 | float z | 0.999 (`0x3f7fbe77`), init only |
| +0x0c | float rhw | 1.0, init only |
| +0x10 | DWORD diffuse | `((int)(fog*255.0f)) << 24`, RGB = 0 (`0x52b4b5..0x52b4cb`) |
| +0x14 | DWORD specular | 0 (never written) |
| +0x18/+0x1c | float u, v | 0 (never written; no texture bound) |

Vertices `vert[row*10 + col]` at `0x66e6b0`; fog values `fog[col*10 + row]` at `0x66e14c`.

### 5.2 Build `0x52af00` (called from Pulse every pulse) [C]

```c
void TAutoMap::BuildFogMesh()                                    // 0x52af00
{
    int Z = zoom, W = mosaic68->w;
    int span = ((W*5 - 0x140) * 2880) / (Z * 0x90);              // = (W-64)*14400/(Z*144), visible mosaic px; 0x52af1c..0x52af39
    int N = (((span*8 + 0x78) >> 7) + 7) / 8;                    // 0x52af3b..0x52af4e (toward-zero /8)
    int S = N << 7;                                              // map-screen px per vertex step; 0x52af5e
    int D = (W*5 - 0x140) * 5 * 4;                               // = 100*(W-64); 0x52af57..0x52af6c
    int P = ((S >> 3) * Z * 0x90) / D;                           // screen px per vertex step; 0x52af64..0x52af7c
    int X8 = scrollX << 3, Y8 = scrollY << 3;                    // 0x52af2c, 0x52af7e
    int mx = posmod(X8, S), my = posmod(Y8, S);                  // 0x52af93..0x52afb1
    int X0 = X8 - mx, Y0 = Y8 - my;                              // 0x52afb3..0x52afc5

    bool rebuild = !(Z == g_lastFogZoom /*0x66f338*/             // 0x52afc7
                  && (abs(g_fogX0 /*0x5e32f0*/ - X0) >> 7) < N   // 0x52afd3..0x52aff0
                  && (abs(g_fogY0 /*0x5e32f4*/ - Y0) >> 7) < N   // 0x52aff2..0x52b00b
                  && playerX == prevPlayerX && playerY == prevPlayerY);   // 0x52b00d..0x52b033
    // (X0 and the stored value are both multiples of S when Z is unchanged, so the two
    //  distance tests are equivalent to "snapped origin unchanged".)

    int offx = ((mx >> 3) * Z * 0x90) / D, offy = ((my >> 3) * Z * 0x90) / D;   // 0x52b03d..0x52b069
    for (row = 0; row < 10; ++row) {                             // outer, 0x52b0b3 .. 0x52b4f8
        int Y  = Y0 + row*S,  Ym = Y - 0x80;
        float sy = (float)(paneY + 0x12 - offy + row*P);
        for (col = 0; col < 10; ++col) {                         // inner, 0x52b10c .. 0x52b4e2
            int X  = X0 + col*S,  Xm = X - 0x80;
            float sx = (float)(paneX + 0x14 - offx + col*P);
            float &f = fog[col*10 + row];
            if (rebuild) {
                g_fogX0 = X0; g_fogY0 = Y0;                      // 0x52b139..0x52b14b (written per vertex)
                switch (N) {                                     // jump table 0x52b510
                case 1:  f = Vis(X, Y) ? 0.0f : 1.0f; break;     // 0x52b161 (0.0 @0x5a3530, 1.0 @0x5a34e4)
                case 2:  f = 0.25f * undiscovered of {(Xm,Ym),(X,Y),(Xm,Y),(X,Ym)}; break;          // 0x52b191, 0.25 @0x5a3568
                case 3:  f = (1/9.f) * undiscovered of {Xm,X,Xm+0x100} x {Ym,Y,Ym+0x100}; break;   // 0x52b200, @0x5b92b8
                case 4:  f = (1/16.f) * undiscovered of {Xm-0x80,Xm,X,Xm+0x100} x {Ym-0x80,Ym,Y,Ym+0x100}; break; // 0x52b2f0, @0x5ac628
                default: f = Vis(X, Y) ? 0.0f : 1.0f;            // 0x52b480
                }
            }
            vert[row*10+col].x = sx;  vert[row*10+col].y = sy;   // every call
            vert[row*10+col].diffuse = ((int)(f * 255.0f)) << 24;   // _ftol 0x58a840 (truncation), 255.0 @0x5a352c
        }
    }
    g_lastFogZoom = Z;                                           // 0x52b505
}
// Vis(x,y) = GetVisBit(+0x64, x, y) (0x529300); "undiscovered" counts samples with Vis == 0.
```

- **Colour rule**: black, alpha = fraction of undiscovered 128-px sub-cells sampled at the
  vertex × 255, truncated (1.0→255, 0.75→191, 0.5→127, 0.25→63, 0→0). Discovered → alpha 0
  (transparent), undiscovered → opaque black; Gouraud-interpolated between vertices.
- With W = 320 and Z ∈ [100, 300]: **N = 2 for Z = 100…190 (S = 256, P = 18…34)** and
  **N = 1 for Z = 200…300 (S = 128, P = 18…27)**. Cases 3 and 4 need Z < 100 and are
  unreachable in retail. With N = 1 a vertex samples only the sub-cell to its lower right
  (half-cell offset); with N = 2 it averages the four sub-cells meeting at the vertex.
- The mesh is 10×10 vertices spanning 9P ≥ 162 px, starting up to P px before the window's
  top-left; the clip rect trims it to the 144×144 window.
- Values are recomputed only on zoom change, snapped-origin change, or player movement (as
  recorded by RecordTravels); positions and diffuse are rewritten every pulse.

### 5.3 Submit `0x52b520` (from draw slot 7, only when `DAT_005d7a6c != 0`) [C]

```c
// indices: quad (r,c), v = r*10 + c, r,c = 0..8:  (v, v+1, v+10), (v+1, v+11, v+10)
//          -> 9*9*6 = 486 uint16 at 0x66e2e4 (0x52b526..0x52b589); triangles are clockwise on screen
BeginScene();                                       // 0x416f70, 0x52b590
save = GetRenderState(14,19,20,15,7,22,25);         // 0x417260 (device +0x54), 0x52b5a1..0x52b691
SetRenderState(D3DRENDERSTATE_ZWRITEENABLE   (14), FALSE);                 // 0x52b6ad..0x52b6b6
SetRenderState(D3DRENDERSTATE_SRCBLEND       (19), D3DBLEND_SRCALPHA  =5); // 0x52b6d2..0x52b6db
SetRenderState(D3DRENDERSTATE_DESTBLEND      (20), D3DBLEND_INVSRCALPHA=6);// 0x52b6f7..0x52b700
SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE(15), TRUE);                  // 0x52b71c..0x52b725
SetRenderState(D3DRENDERSTATE_ZENABLE        (7),  FALSE);                 // 0x52b741..0x52b74a
SetRenderState(D3DRENDERSTATE_CULLMODE       (22), D3DCULL_CCW =3);        // 0x52b766..0x52b76f (CW tris are kept)
SetRenderState(D3DRENDERSTATE_ALPHAFUNC      (25), D3DCMP_GREATER=5);      // 0x52b78b..0x52b794
SetTexture(0, NULL); SetTexture(1, NULL);           // 0x4172b0 (device +0x98), 0x52b7b0..0x52b7e2
save clip; SetClip(paneX+0x14, paneY+0x12, 0x90, 0x90);                    // 0x412250 / 0x412150, 0x52b7fe..0x52b85f
DrawIndexedPrimitive(D3DPT_TRIANGLELIST=4, 0x1c4, 0x66e6b0, 100, 0x66e2e4, 486, D3DDP_DONOTLIGHT=0x10); // 0x4174b0, 0x52b864..0x52b87f
SetRenderState(each of the 7, saved value);         // 0x52b89f..0x52b99b
restore clip;                                       // 0x52b9b7..0x52b9d6
EndScene();                                         // 0x416fb0, 0x52b9e0
```
- `0x417060` is `SetRenderState(state, value)` (it also clears the blend-mode cache
  `DAT_005e8790`, forcing the next quad draw to re-apply its blend setup); `0x417260` is
  `GetRenderState` [C, bodies].
- **Not set here**: ALPHABLENDENABLE (27), ALPHAREF (24), SHADEMODE, texture-stage ops.
  ALPHABLENDENABLE is set at 3D-scene init from `DAT_005c61a4` (= 1) at `0x41331b..0x413326`
  and never by the quad blend selector `0x417d60`; ALPHAREF is never set through
  `0x417060` anywhere in .text (scanned). See UNCONFIRMED.
- Draw order in slot 7 `0x52a5f0` [C]: body blit `0x52a830`; **if fow**: blips `0x52a620`
  then fog `0x52b520`; chrome `0x52bdb0`. Turning fog of war off hides the blips too.

---

## 6. Initialize after the art (`0x529ff6` → end) [C]

```c
travels = NULL; grid = NULL;                                  // +0x60, +0x64   0x529ff8, 0x529ffb
tileY = prevTileY = tileX = prevTileX = 0;                    // 0x529ffe..0x52a010
cacheLevel = 0; scrollY = scrollX = 0;                        // 0x52a016..0x52a022
zoom = 100;                                                   // 0x52a028
g_fow /*0x5d7a6c*/ = 1;                                       // 0x52a032
g_blipFrame = 0; g_lastMouseY = 0; g_lastMouseX = 0;          // 0x52a03c..0x52a048
zoomOutHeld = zoomInHeld = freeScroll = 0;                    // 0x52a04e..0x52a05a
FUN_0052ae70();                                               // fog vertex init, §3.3
if (Player) {                                                 // 0x52a065
    int mi = ModuleMgr.cur;                                   // 0x65a784
    if (mi >= 0 && ModuleMgr.mod[mi] && strcmp(Player+0x318, ModuleMgr.mod[mi]+0x58) == 0)
        travels = Player+0x314;                               // 0x52a073..0x52a0d5
    if (!travels) {                                           // 0x52a0d8
        travels = Player+0x314;
        strcpy(Player+0x318, (mi < 0 ? NULL : ModuleMgr.mod[mi]) + 0x58);   // 0x52a0ee..0x52a123
        for (i = 0; i < records.count; ++i) {                 // 0x52a125..0x52a153
            if (i >= 0 && records[i]) RecordDelete(records[i], 1);          // 0x520070 = reset 0x5295b0 + free
            records.RemoveAt(i);                                            // 0x41cb40 (skips every 2nd)
        }
        records.count = 0; records.+4 = 0;                    // 0x52a155..0x52a157
    }
    int level = MapPane.level;                                // 0x52a15d
    for (i = 0; i < records.count; ++i)                       // 0x52a16a..0x52a1d1
        if (records[i]->level == level) grid = new SVisGrid(records[i]);   // 0x2084 bytes, 0x529220
    if (!grid) { grid = new 0x2084 bytes; grid->level = level; zero 0x820 dwords; }  // 0x52a1d3..0x52a201
    Player->GetScreenPos(&playerX, &playerY);                 // 0x52a204..0x52a218
    for (dx = -0x200; dx <= 0x200; dx += 0x80)                // 0x52a21d..0x52a259
        for (dy = -0x200; dy <= 0x200; dy += 0x80)
            SetVisBit(grid, playerX + dx, playerY + dy, 1);   // 0x52a23a
}
SetDirty(TRUE);                                               // vtbl +0x2c(1), 0x52a25b..0x52a262
return 1;
```
- The "level-name compare" is a **module** name compare (player+0x318 vs the current
  module's folder name); a mismatch wipes the player's discovery records.
- The 0x2084-byte buffer is `SVisGrid` (§3.1): level id + 65×64 uint16 words.
- `+0x134/+0x13c`, `DAT_0066f338`, `DAT_005e32f0/4` and the fog value array are not reset.
- Latent crash: with no current module (`mi < 0`) the strcpy reads from address 0x58; only
  `0x52c3d0` handles that case (it uses `"Revenant"`).

---

## 7. Blips and chrome (draw-side, for completeness) [C]

- **Blips `0x52a620`**: clip to the window; iterate objects near the player
  (`0x44ceb0(&iter, &player->pos(+0x10..+0x18), 0x200, 0x60, 2, 0, -1)`); skip if
  `obj->vtbl[0x1c0]() <= 0` or `obj->0x472e30("invisible")`; map `GetScreenPos` with the §0.2
  formula; keep if `x ∈ [px+0x14, px+0xa4)` and `y ∈ [py+0x12, py+0xa2)`; draw
  `Quad(x, y, 0, tex70, 0, 4, 4, 0xffffffff, sx = g_blipFrame*4, sy, 4, 4, NULL, 4)` with
  `sy = 0` for the player, `8` if `obj->0x4c89c0(player)` (IsEnemy) is true, else `0xc`
  (`0x52a796..0x52a7ea`). The blip's top-left (not its centre) is at the mapped point.
- **Chrome `0x52bdb0`**: clip to the pane rect; `Quad(px, py, 0, tex7c, 0, pw, ph, −1, 0, 0,
  pw, ph, NULL, 0)`; if +0x9c: PlusSel texture +0x84 at `(px+8, py+3)`, 32×32; if +0xa0:
  MinusSel texture +0x88 at `(px+0x90, py+3)` (`0x52bebb add edi, 0x90`), 32×32. The button
  art is shown only **while held**.

---

## 8. Corrections to existing docs

`docs/ui/forensics/MapPane_SPEC.md`:
- :180 "+0x6c … cleared to 0xfff0bdc0" — wrong; it is `SetClipRect(-1e6,-1e6,2e6,2e6)` +
  `SetClipMode(CLIP_WRAP)` on both mosaics (§4).
- :182 "+0x78/0x7c PlusSel-derived zoom-in button surfaces" — wrong; +0x78/+0x7c are the Amap
  mosaics; PlusSel/MinusSel textures are +0x84/+0x88, scratch +0x80.
- :222-223 button positions (480,327) / (360,327) — wrong; they are pane+(8,3) = (460,309)
  and pane+(0x90,3) = (596,309); the chrome uses paneX/paneY, not +0x14/+0x12.
- :323 `sprintf(buf, …, area, cx, cy, level)` — wrong order; it is `(modulePath, level, tileX, tileY)`.
- :388-391, :413-421 zoom step / click handler UNCONFIRMED — now resolved (§1.1, §2).
- :472 "if (zoomCanIncrease) DrawBitmap(plusSel…)" — wrong; drawn while the button is held.

`docs/ui/HUD_REBUILD.md:470-476` (button positions (8,3)/(144,3), held-state gating) agrees
with this spec.

---

## UNCONFIRMED

1. **ALPHABLENDENABLE at fog time.** The fog never sets it; it relies on the state left by
   3D-scene init (`DAT_005c61a4` = 1). Only `0x5233e6` / `0x536f61` (variable values) and
   `0x45d362` / `0x491eea` / `0x52303e` (value 1) set it elsewhere. Very likely TRUE, not proven
   for every code path.
2. **ALPHAREF** is never set through `0x417060`; assumed the D3D device default (0), so the
   alpha test passes alpha > 0. Not checked whether it is set by another route (e.g. the
   software renderer `0x56d400`, or directly on the device).
3. **SHADEMODE and texture-stage colour/alpha ops** during the fog draw are inherited from
   the previous draw; with both stage textures NULL the output should be the vertex diffuse,
   but the stage setup was not traced.
4. Exact `SetClipRect` behaviour of `0x412150` on the hardware path (viewport vs. scissor) —
   only its argument shape `(x, y, w, h)` and the save/restore pairing are confirmed.
5. Whether `Box` on the mosaic honours the surface wrap mode with `DM_USEDEFAULT` (Put uses an
   explicit DM_WRAPCLIP). The ring logic only works if it does.
6. Roles of surface flag bits 0x400 vs 0x800 (staging vs drawn copy) — taken from
   RETAIL_HUD_CODE_MAP.md §2, which itself rates the DirectDraw caps mapping M.
7. Identity of `DAT_0066818c` (adds +1 to the body quad height; also remaps blend flags in
   `0x417d60`) — a renderer-compatibility flag; not identified.
8. `SAutoMapTravels +0x00` (player+0x314) — never touched by automap code; content unknown.
   `moduleName` size assumed 32 bytes from the layout (+0x04..+0x24).
9. Stream format of the name written/read by `0x49ccc0` / `0x49ce00` in the save/load of
   player+0x314 (needed for byte-exact save interop) — not decoded. The record payload
   layout (int32 level, int32 nwords, uint16[nwords]) is confirmed; exact run-splitting of the
   encoder `0x5295e0` at buffer edges was not traced.
10. Pulse rate: taken from INGAME_MENU.md ("5 pulses ≈ 0.2 s", ~24 Hz), not re-measured.
    Zoom speed and blip animation (frame every 5 pulses) are per pulse.
11. MouseMove's `button` argument is tested `== 1`; that this means "left held" follows the
    MB_ numbering and MAP_INPUT §5, not a trace of the screen's move dispatch. Whether a left
    release outside the pane still reaches slot 25 (to clear the zoom flags) depends on the
    screen's mouse capture — not traced.
12. `0x44ceb0` iterator arguments (0x200, 0x60, 2, 0, −1) and `0x472e30("invisible")` /
    `vtbl[0x1c0]` semantics in the blip filter — names inferred (Health per UI_METHOD_MAP §15b).
13. `FileExists` (`0x4a1c00`), `BmpFormat` (`0x4a30d0`: opens the file, checks 'BM' and the
    0x28-byte info header), `NewBitmap` (`0x4a1ec0`), `LoadBMP` (`0x4a2ce0`) — names from their
    first instructions only.
14. `0x52c340` (a copy of the click hit test) has no call sites or vtable references in the
    exe; dead code or reached through an unscanned pointer.
15. Initial contents of a freshly created mosaic (what the first full +0x68 → +0x6c copy at
    Initialize carries before any tile is stamped).
