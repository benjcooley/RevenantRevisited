> Survey of the retail HUD code (subagent, 2026-10-07) for the HUD rebuild
> ([../HUD_REBUILD.md](../HUD_REBUILD.md)). Read from the decomp and the
> byte-identical asm; facts get confirmed or corrected by the emulator A/B as
> each pane is done. `$R` = the main checkout. Its §0 corrects older docs.

# Retail HUD paint-code map (Revenant.exe SHA 28bec273…) for the Unicorn emulation

`$R` = `/Users/benjamincooley/projects/RevenantRevisited/RevenantRevisited`. "asm L#" means a line in `$R/recon/retail_asm/baseline/sections/00001000_section_0.asm`. Confidence is H (high), M (medium) or L (low).

## 0. Fixes to the existing docs (these affect the emulation)

1. **`DAT_006680c8` is `NoTexOverlay`, not "Classic vs hi-res".** It is set by the INI key `NoTexOverlay` (string 0x5d8478, asm L544072) and by the command-line flags `NOTEXOVERLAYS` / `NOBLITTEXTURES` (asm L541186–541216). The retail `revenant.ini:126` says `NoTexOverlay = No`, so the value is 0.
   - **When it is 0 (the default):** the HUD loads `StatusBar.dat`, `SideBarTabs.dat`, `mpingametex.dat` and `createchartex.dat`, and draws with **textured 3D quads**. Source: `$R/recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:112-125` and asm 0x47a8f2–0x47a905.
   - **When it is non-zero:** it loads the `*NoTex.dat` files and draws with 2D DirectDraw blits.
   - `TPlyrStatusBar_SPEC.md §2` says the ==0 path uses `statusbarnotex.dat`. That is wrong. (H)
2. **`FUN_00414d70` is a method on the 3D scene object, not a bitmap blit.** It is `T3DScene::meth_0x414d70`, called with ECX = 0x65a57c (asm 0x54aa66). It draws one screen-space textured quad. It takes 14 stack arguments and returns with `ret 0x38`. Sources: `$R/recon/classes_readable/T3DScene.cpp:1013-1350`, `$R/tools/retail_runtime/trace_hud_bar.py`. (H)
3. **`FUN_00438d80` builds a draw-parameter struct; it does not set a shadow.** Its signature is `MakeDP(dp, dx, dy, sx, sy, w, h, drawmode)`, and the fields match `$R/src/graphics.h:58-94` exactly.
   - `FUN_00438d80(buf, 4, 4)` therefore means "draw the chip art at screen (4,4)".
   - Slot 7 confirms this: it calls `FUN_00414d70(4, 4, …this+0x70…)`. (H)
4. **The "BlitEffect" registry is the display's background-restore / dirty-rectangle system.** It matches `$R/legacy/display.cpp:466-945`: `MAXRESTOREBUFS 10` (`$R/legacy/RevDefs.h:244`), `MAXRESTORERECTS 128`, and the same UPDATE flag values 1/2/4/8/0x10.
   - 0x4aa850 = UseBackgroundArea
   - 0x4aa490 = FreeBackgroundArea
   - 0x4aa7c0 = set area rectangle
   - 0x4aacb0 = AddUpdateRect(x, y, w, h, flags)
   - 0x4aaeb0 = AddBackgroundUpdateRect(index, …)
   - 0x4aa280 = the display's blit override (vtable +0x5c), which runs the base blit and then flushes queued update rectangles. (H)
5. **`FUN_004bd680` is `TSurface::Put(x, y, TBitmap*, drawmode, SColor*)`.** It is a member call whose ECX is the destination surface (asm 0x54a23a uses `mov ecx,[edi+0x6c]`). It does not draw to an "implicit current surface". (H)
6. **`FUN_0046d710` is always a by-name lookup.** ECX is the resource-archive blob and the name is pushed on the stack (asm 0x53cc3x pushes 0x5e470c/0x5e4710/0x5e4718 with ECX = `[0x65c5c8]`). There is no "iterator" form. (H)
7. **The cursor and `texthealthbar` come from `gamedata.dat`.** Both are looked up in the archive held at `DAT_0065abc0`, which is loaded from `gamedata.dat` (asm L548638 and L504533; texthealthbar at asm L1345460). The docs say `playscrn.dat`. (H)
8. **The bottom bar's live width is `display_w − sidebar width`, i.e. 452 at 640×480.** `Pulse` calls `FUN_0052da60(display_w − DAT_0066615c, …)`, which goes on to 0x52c930 (`TBottomBarPane::SetRect`). The 640 in the spec is only the constructor's template value. (M-H)
9. **`$R/docs/ui/RECON_UI_COVERAGE.md` is stale.** For example, 0x54dd40 is the software-renderer camera constructor (`$R/tools/retail_runtime/software_probe.py:55`), not `TEquipPane`.

## 1. Per-pane map

All rectangles are at 640×480 with the sidebar shown. The TPane field layout is: `+4/+8/+0xc/+0x10` = live x/y/w/h, `+0x14..+0x20` = template x/y/w/h (copied to live by 0x491900), `+0x40` = initialized flag, `+0x44` = skip-draw, `+0x50` = visible. Source: `$R/recon/discovered/cls_0x5a4494_TPane_Initialize_491900.cpp`.

I dumped every vtable override below directly from `.rdata` (`sections/001a3000_section_1.asm`). For classes with a 32-slot vtable, "slot 31" is actually the first entry of the next class's vtable.

| Pane | Class / vtable · global | Initialize | Paint entry points (slot: address) | Resources | Rect | Key globals | Conf. / source |
|---|---|---|---|---|---|---|---|
| **TPlayScreen** | cls_0x5a5320; ctor 0x47a620 (asm L502919) | 0x47a660 | 3 Pulse 0x47b4d0 (layout) · 4 0x47bd20 · 5 0x47b4a0 · 6 0x47c2c0 · 7 0x490110 · 8 0x47c400 | loads all archives (§3) | full screen | `DAT_00667fcc` player, `DAT_006680c8` | H; Initialize file above |
| **TPlyrStatusBar** (both chips in one pane) | cls_0x5a54e4 · 0x65a8c0 | 0x549740 | **7 0x54ab80**: texture path, chip art from +0x70 at (4,4), target at (pane_w−0x88,4), then bars `FUN_0054a5d0` → 0x414d70 · 19 0x549da0: fade tick · **20 0x549e60**: rebuild chip art via 0x54a0a0 / 0x54a310; in texture mode also builds "bars" textures with 0x4a5ca0 · **23 0x54af20**: text into +0x64 scratch via 0x4be2b0; in NoTex mode also chip art and bars as 2D; text cells blitted with display +0x5c; then AddUpdateRect · helper 0x54ae10 builds the name/level string | `DAT_0065a9d0` = StatusBar.dat / StatusBarNoTex.dat. Names "bars", "BackPanel", "Ring", "HealthIcon", "ManaIcon", "FatigueIcon" (0x5e571c–0x5e57a8). Formats "%s\nLevel %d" (0x5e57dc) and DM variant (0x5e57bc). Font "Small" (`DAT_0065abc4`) | `Pulse` sets w = display_w − `DAT_0066615c` (=452), h = 0x70 (writes 0x65a8dc / 0x65a8e0). Target chip origin = pane_w − 0xc1 | `DAT_00667fcc`; target = player[0x38], if its type is 3 or 0x19 then [0x11], else falls back to 0x452690; `DAT_005d7a18`; character getters +0x130 (portrait), +0x1c0 / +0x1c8 / +0x1d0 (HP/FT/MP), +0x1d8 / +0x1e0 / +0x1e8 (bar descriptors), +0x354 (level) | H; `$R/docs/ui/forensics/TPlyrStatusBar_SPEC.md`, `$R/recon/discovered/cls_0x5a54e4_*` |
| Bar kernel (helper) | — | — | `FUN_0054a5d0`, thiscall, 13 arguments. Texture mode: 4× 0x414d70, args `(x, y, 1, this+0x68, 0, w, h, (fade*255/6)<<24 \| 0xffffff, sx, sy, w, h, 0, 4)`. NoTex: 4× display +0x5c | — | — | `this+0xd4` / `+0xdc` fade counters | H; `$R/recon/discovered/FUN_0054a5d0_*.cpp` |
| **TSideTabsPane** | cls_0x5a5750 · 0x65be50 (built on TButtonPane) | 0x53cc30 | **7 0x53d420**: texture path · 19 0x53d3a0: hover ramp · 20 0x53d400: not extracted · **23 0x53d540**: NoTex path via 0x438df0 / +0x5c, then AddUpdateRect · 25 0x53d6e0 | `DAT_0065c5c8` = SideBarTabs(NoTex).dat; names "Up" / "Down" / "Select" (0x5e470c / 0x5e4710 / 0x5e4718) | x = display_w − `DAT_0066614c` − 52 = 400; y = `DAT_00667c60` − 232 = 188; 52×232 | `DAT_0065d1b8` (upper region), `DAT_0065d1bc` (lower region); command dispatcher 0x47cf40 cases 7–0xc; region setters 0x53cab0 / 0x53cb40 | H; `$R/docs/ui/forensics/TSideTabsPane_SPEC.md` |
| **TSidePane** (container) | cls_0x5a53ec · 0x666140 | 0x53c8c0 (trivial) | 3 0x53c970: CreateBackgroundBuffers (allocates a DirectDraw surface, then UseBackgroundArea) · 4 0x53ca30: free · 18 0x53ca60 · 20 0x53c900: reads `DAT_0065d194` / `DAT_0065d198`, then AddUpdateRect | none | template w = 0xbc (asm L506256); full height | `DAT_0065d190` / `194` / `198` | M (my disassembly) |
| **TBottomPane** (container) | cls_0x5a5468 · 0x667c58 | 0x52d8a0 | 3 0x52d910 / 4 0x52d9d0: background area · **8 0x52da60: resize → `TBottomBarPane::SetRect` 0x52c930** · 18 0x52da00 · 20 0x52d8c0 | none | y = `DAT_00667c70`, h = `DAT_00667c78` | — | M |
| **TBottomBarPane** | cls_0x5a5808 · 0x65b638; ctor 0x487da0 | 0x52c780 | **20 0x52c800**: if visible, slot 21; then BarInv 0x52ca70; then QuickSpell 0x5444c0; then AddUpdateRect · **21 0x52c880**: stretched Put 0x4bd5e0 of "UtilityBar", Put 0x4bd680 of "BarEndCap" at (w−10, 0) | `DAT_0065a570` = BottomBar.dat | (0, 420, 452 live / 640 template, 60) | — | H; `BottomBarPane_SPEC.md` |
| **TBarInvPane** (potion shelf) | cls_0x5a56d4 · 0x65b028 (Ghidra mixed its methods into cls_0x5a5658) | 0x52c970 | 20 0x52ca50: dirty flag · **21 0x52ca70: draw** · 22 0x52cd80: hover | BottomBar.dat "BarInvBox" (42×42); item icons come from the item objects | slots at pane-local (220, 10), pitch 45, count = (w − 220)/45 | `DAT_0065b688` dirty; this+0x60 player; fonts 0x65b010 / 0x65a9cc | H; `BarInvPane_SPEC.md` |
| **TQuickSpellPane** | cls_0x5a5a30 · 0x65c6f8; ctor 0x488580 | 0x544160 | 20 0x5444a0: not extracted · **21 0x5444c0: draw** | `DAT_0065bc3c` = SpellIcons.dat (RingU/D/G plus spell icons); BottomBar.dat Arrow* | (0, 420, ·, 60); ring x positions from table 0x5e4fe8 | `DAT_0065b688`, `DAT_0065d0d0`; quick-spell names at player+0x2cc | H; `QuickSpellPane_SPEC.md` |
| **TTextBar** (game log) | cls_0x5a5560 · 0x65c5d0; ctor 0x480725 | 0x54bf70 | **7 0x54c600**: texture path · 19 0x54c460 · 20 0x54c440 · **23 0x54c780**: NoTex path · helpers: compose 0x54cd40, health bar 0x54cb00 (PutHue 0x4bd8c0), text 0x438ed0 | gamedata.dat "texthealthbar" (200×11); font "Small" | `Pulse`: x 0, y = `DAT_00667c70` − h, w = display_w − 52 − 188 = 400; lines drawn at (4, 9) | `DAT_006668dc..e8` viewport | H; `TTextBar_SPEC.md` |
| Equip (upper sidebar) | cls_0x5a55dc · 0x65b7e0 | 0x536360 | 7 0x5370b0 · 19 0x5368c0 · 20 0x5366a0 · **21 0x5366d0: paint, body not extracted** · 22 0x536970 | `DAT_0065dde4` = EquipPane.dat ("Equip" 188×306 plus 11 slot icons) | (452, 0, 188, 306) | `DAT_0065d1b8 == 0` | M; `EquipPane_SPEC.md` |
| Stats (TStatPane) | cls_0x5a5ba0 · 0x65b140; ctor 0x488910 | 0x546b50 | 20 0x549010 · **21 0x5491c0**: statpane.def interpreter 0x5475e0, WriteText 0x546de0 (TextOutA), WriteField 0x546f40 (DrawTextA) | `DAT_0065c6f4` = StatsPane.dat "Stats"; SpellIcons "RingT" | (452, 0, 188, 306) | `== 1` | H; `CharacterStatsPane_SPEC.md` |
| Spellbook ("Book" tab) | cls_0x5a5ae8 · 0x65a9d8; ctor 0x488620 | 0x5449e0 | 19 0x546620 · 20 0x544eb0 · **21 0x5452d0** | `DAT_00666640` = SpellScroll.dat; SpellIcons.dat | 188×306; origin is open in the spec | `== 2` | M-H; `SpellbookPane_SPEC.md` |
| Inventory | cls_0x5a58c0 · 0x65d4f8; ctor 0x487f50 | 0x537650 | 20 0x537a30 · **21 0x537a70** grid · 22 0x537f90 | `DAT_0065c130` = Inventory.dat; medgold font | (452, 306, 188, 174) | `DAT_0065d1bc == 0` | H; `InventoryPane_SPEC.md` |
| Map (automap) | cls_0x5a5658 · 0x65b4f0 | 0x529970 (loads AutoMap.dat itself) | **7 0x52a5f0** → 0x52a620 / 0x52a830 / 0x52bdb0 (all use 0x414d70) · 19 0x52a280 · 20 0x52bf00 · 21 0x52bf30 | AutoMap.dat; tiles `automaps\%d_%d_%d.bmp` | globals 0x65b4f4..0x65b500 = (452, 306, 188, 174) | `== 1` | M; `MapPane_SPEC.md` |
| Spell-creation tab (TSpellPane) | cls_0x5a5978 · 0x6661b0; ctor 0x488460 | 0x5432a0 | 20 0x5435c0 · **21 0x5435f0: not extracted** | `DAT_0065a574` = SpellPane.dat "spellconstr" | (452, 306, 188, 174) | `== 2` | M. Its vtable inherits TButtonPane slots (0x434f30, …), which contradicts B_r11's "plain TPane leaf" |
| **Cursor** (not a pane) | free functions 0x43a020–0x43a930; SetMouseBitmap 0x43a020; DrawMouseCursor 0x43a480–0x43a59a | — | Put with drawmode 0x21100 / 0x20100 | gamedata.dat "cursor" / "handcursor" → `DAT_0065a28c` / `DAT_0065a284` | at the mouse position | 0x6563a0–0x6563d4; mouse x/y `DAT_00668510` / `14` | H; `MouseCursor_SPEC.md` |

Paint spec files are under `$R/docs/ui/forensics/`. Briefs are `$R/docs/ui/briefs/B_r3_playscreen_panes.md` and `$R/docs/ui/briefs/B_r11_sidebar_tab_cascade.md`.

## 2. Drawing primitives

| Address | Identity | Call shape / notes |
|---|---|---|
| **0x414d70** | Textured-quad draw on the 3D scene object | Member call, ECX = 0x65a57c. Arguments: `(dstX, dstY, z, srcTex, src2 (0), w, h, ARGB tint, sx, sy, sw, sh, float* matrix (0), flags)`. If the source's surface type is 0x10 (tiled surface), it loops over the tiles. Then it calls 0x414550, which builds 4 vertices with FVF 0x1c4 (pre-transformed position + diffuse + specular + 1 texture), starts the scene if needed, sets texture stages 0/1, picks blending from flag bits 2/4/8/0x10 (0x417d60) and render states (0x417060), and calls 0x4174b0 = DrawIndexedPrimitive (triangle list, 4 vertices, 6 indices). If flags & 0x400 or 0x800 it calls AddUpdateRect on the bounding box. (H) |
| 0x416f70 / 0x416fb0 | BeginScene / EndScene | Sets `DAT_005e8850`. Hardware: Direct3D device `DAT_00668f14` +0x24 / +0x28. Software (`DAT_005d7a28`=1): 0x56d260 / 0x56d330 with ECX = 0x5e88b8. |
| 0x4174b0 | DrawIndexedPrimitive wrapper (`ret 0x1c`) | Device +0x74, then clip status +0x7c. Software path: 0x56eb30. SetTexture is device +0x98 or software 0x56d3a0. |
| 0x4bd680 | `TSurface::Put` | ECX = destination. Drawmode 0x80000000 = opaque, 0x2000 = alpha. |
| 0x4bd5e0 | Sub-rectangle / stretched Put | 7 arguments; used by the bottom bar. |
| 0x4bd8c0 | `PutHue` | Used by the text bar's health bar. |
| 0x438df0 | Blit wrapper | ECX = destination. Arguments `(dx, dy, srcSurf, sx, sy, w, h, drawmode, p, p)`; builds the draw-parameter struct, then calls vtable +0x5c. |
| 0x438d80 | Draw-parameter struct builder | `MakeDP(dp, dx, dy, sx, sy, w, h, drawmode)`; struct is 0x54 bytes; intensity = 0x1f. |
| vtable +0x5c | Blit (`ParamBlit`) | Base surface 0x4bd490 (validates, then uses source +0x54/+0x60 or own +0x50). Display override 0x4aa280. Tiled surface 0x4bbed0. |
| vtable +0x64 | `Box` (fill) | `(dx, dy, w, h, color, 0xffff, 0x7f7f, drawmode)`. Base 0x4bde60; DirectDraw surface 0x4a6930. |
| **0x4be2b0** | GDI text renderer | ECX = destination. Arguments `(x, y, w, h, text, color*, fontId \| 0x400 (shadow), lineRect, format flags, drawmode)`. Gets a DC from the DirectDraw surface (+0x44 GetDC at 0x4be753, +0x68 ReleaseDC at 0x4bea20) and uses GDI SelectObject / SetTextColor / SetBkMode / DrawTextA (IAT 0x5a3030 / 28 / 44 / 3324). Wrappers 0x438ed0 and 0x4be110. Font manager object at 0x65b010; `FUN_004acb30(name)` returns a font index; called at startup (0x485dac–0x485e38) for "System" → `DAT_0065bc40`, "Small" → `DAT_0065abc4`, plus Med / Large / Dialog. Font record table at 0x65b020. (H for the API calls, M for the font record layout) |
| 0x4aa850 / 4aa7c0 / 4aa490 / 4aacb0 / 4aaeb0 | Background-restore system (§0 item 4) | Tables at 0x669af0 (10 × 0x4c bytes); pending callback rectangles at 0x669df4 with count at 0x66a8fc; pause flag 0x5e91bc. |

**Object layouts**

- **TSurface** (vtable 0x5a68d8, constructor 0x4bcb00, source file `Surface.cpp`).
  - Fields: +4 width, +8 height, +0x30 clear/key colour (= `DAT_006668d0`), +0x38 pixel-format flags (mask 0x3001f; 0x30000 = alpha).
  - Vtable: +8 surface type (0x10 = tiled), +0x14 is-video-surface, +0x18 bits per pixel (0xf or 0x10), +0x24 reset clip/origin, +0x2c Lock, +0x30 Unlock, +0x38, +0x4c, +0x50, +0x54, +0x5c, +0x60, +0x64 as in `UI_METHOD_MAP §15a`.
  - The pre-release source `$R/legacy/surface.h` / `surface.cpp` has the same method order.
- **TDDSurface** (vtable 0x5a3980, `DDSurface.cpp`, size 0x78).
  - Initialize 0x4a5740(w, h, flags, …): flags 0x200 / 0x400 / 0x800 select memory type; 0x488 and 0x4000888 are the texture variants. Exact DirectDraw caps mapping is M.
  - Create-from-bitmap: 0x4a5ca0(bitmap, flags).
  - +0x68 holds the `IDirectDrawSurface*`.
  - Lock 0x4a60f0 → COM +0x64 (restores on IsLost +0x60 / Restore +0x6c). Unlock 0x4a6270 → COM +0x80. (H)
- **Tiled ("mosaic") surface** (vtable 0x5a3e7c, size 0x74, init 0x4bb5c0(w, h, flags)).
  - [0x1a] tile count, [0x1b] tile rectangles (16 bytes each: x0, y0, x1, y1), [0x1c] tile surfaces.
- **TDisplay** (vtable 0x5a67e8, `Display.cpp`, size 0x98, derives from TDDSurface). Global pointer `PTR_DAT_005d79e0`.
  - +4 / +8 = display width / height; +0x80 / +0x8c are the front / back buffers that get swapped.
  - Page flip is not pinned.
- **TBitmap** (the resource bitmap format): `$R/src/bitmapdata.h:34-60`, 0x48-byte header. Flag values: BM_15BIT = 2, BM_ALPHA = 0x100, and 0x10000 = ARGB4444 (per the MapPane row in `$R/docs/ui/forensics/README.md`).
- **How DirectDraw is reached:** the EXE only imports `DirectDrawCreate` and `DirectDrawEnumerateA` (from my parse of the PE import table). Everything else goes through COM vtables: `IDirectDrawSurface` Lock +0x64, Unlock +0x80, GetDC +0x44, ReleaseDC +0x68; `IDirect3DDevice3` at `DAT_00668f14`.
- **Existing software-renderer support:** the Unicorn runtime already drives the software 3D path (0x56d3a0 SetTexture with fake surface Lock / Unlock / GetPixelFormat; 0x56d960 rasteriser into RGB565) in `$R/tools/retail_runtime/software_probe.py`.

## 3. Resource loading

- **Loader:** `FUN_0047f670(name, id = -1, 0)` returns a pointer to the loaded archive blob. (H, my disassembly)
  - **Path:** ResourcePath (`0x65dde8`) + name, with ".dat" appended if there is no extension; if `id ≥ 0` the format is `"%s%s.%03d"`.
  - **Opening:** 0x4a13f0 tries RunPath (`0x65d254`), then an alternate/CD path (`0x6666cc`), then calls 0x4a1240. That function takes mutex 0x65b8ac, uses a file-slot table at 0x668b10, and tries CRT fopen (0x58b5db) and/or the ZIP packs at 0x6687f8 / 0x668808 (ZIP reader class cls_0x49ead0, methods 0x4a0380 / 0x4a06a0 / 0x4a0890). The order of those attempts is M.
  - **Reading:** read 0x4a15a0, seek 0x4a16c0, close 0x4a1540.
  - **Header check:** magic 'CGSR' (0x52534743). Header layout is in `$R/docs/ASSET_SYSTEM.md:105-114`.
  - **Allocation:** malloc 0x482ef0(objsize).
  - **In-place colour conversion** based on the display's bits per pixel (display vtable +0x18):
    - 16 bpp display: 15-bit bitmaps (flags & 2) → 0x4b8820; 8-bit (flags & 1) → 0x4b8b30.
    - 15 bpp display: 16-bit bitmaps (flags & 4) → 0x4b93a0; 8-bit → 0x4b9690.
    - Skipped when flags & 0x40000.
    - **The display object must exist before any archive is loaded.**
- **Port equivalent:** `$R/src/resource.cpp:45-150`.
- **Name lookup:** `FUN_0046d710` (ECX = archive blob, stack argument = name).
  - Blob layout: `{int count; int nameOff[256]; int dataOff[256] @+0x404}`, with offsets relative to each slot.
  - Case-insensitive compare via 0x59a530.
  - A miss is fatal: "Unable to find '%s' in multiresource" (0x5d4718). Source: `$R/recon/ghidra/cls_0x46d6b0.cpp`.
- **Where the files are:** `revenant.ini` `[Paths] ResourcePath = ".\Resources"` (`/Users/benjamincooley/RevenantRetailLab/retail-cd/REVENANT/revenant.ini:4`). On the CD the .dat files are inside `resources.rvr`, a ZIP that must be uncompressed (it fails with "Compressed files found in resource file RESOURCE.RVR", string 0x5d8c60). The `Resources/` folder on disk holds only .def and .s files. Extracted copies are in `$R/data/resources_unzipped/`.
- **HUD archives loaded in TPlayScreen::Initialize:**

  | Global | File |
  |---|---|
  | `DAT_0065dde4` | EquipPane |
  | `DAT_0065a574` | SpellPane |
  | `DAT_0065bc3c` | SpellIcons |
  | `DAT_0065a9d0` | StatusBar[NoTex] |
  | `DAT_0065c5c8` | SideBarTabs[NoTex] |
  | `DAT_0065c130` | Inventory |
  | `DAT_0065a570` | BottomBar |
  | `DAT_00666640` | SpellScroll |
  | `DAT_00666444` | Dialog |
  | `DAT_0065a9d4` | Portraits |
  | `DAT_0065c6f4` | StatsPane |
  | `DAT_00667fc4` / `DAT_0065c890` | mpingame / createchar, tex or notex |

  Loaded earlier at startup (asm L548600): `DAT_0065abc0` = gamedata.dat, `DAT_0065bb10` = widgetsalpha.dat, then widgets tex/notex.
- **Decompression:** none on the HUD path. No HUD bitmap has `BM_COMPRESSED` (0x4000) or `BM_CHUNKED` (0x8000), and the packs are stored-only. ChunkDecompress is only used for map/imagery data; its retail address is not pinned (only weak candidates 0x48e630 / 0x48e670 from `$R/recon/classes_readable/TChunkCache.cpp`).
- **Texture upload in texture mode:** bitmap → 0x4a5ca0 (DirectDraw surface), then a blit into a texture/tiled surface (status bar slot 20, `$R/recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot20_Animate_549e60.cpp:62-117`).

## 4. How TPlayScreen creates and positions the panes

- **Initialization order** (Initialize file lines 175–240):
  1. Pane inits: map 0x44d5c0 → side 0x53c8c0 → bottom 0x52d8a0 → side tabs 0x53cc30 → automap 0x529970 → inventory 0x537650 → quick spell 0x544160 → bar-inventory 0x52c970 → text bar 0x54bf70 → dialog 0x534fd0 → bottom bar 0x52c780 → equip 0x536360 → spell 0x5432a0 → stats 0x546b50 → spellbook 0x5449e0 → status bar 0x549740.
  2. Panes registered on the screen with `FUN_0048ed90(&pane, -1)`: 0x6668d8 (map), 0x667cc8 (dialog), 0x666140, 0x667c58, 0x65be50, 0x65c5d0, 0x65a8c0.
  3. Cursor set up, then the region setters 0x53cab0(0) / 0x53cb40(0).
  4. On mode changes, `Pulse` (0x47b4d0) also registers the bottom bar, bar-inventory and quick-spell panes (0x65b638 / 0x65b028 / 0x65c6f8), and the sidebar content panes are registered/removed by dispatcher 0x47cf40.
- **Layout:** done in `Pulse`, `$R/recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:263-296`. The formulas are in the §1 rect column. Sidebar width `DAT_0066615c` = 0xbc (188) when shown, 0 when hidden. Map pane = `(display_w − 188) × (display_h − DAT_00667c78)`.
- **Per-frame pane calls.** Each screen-level method loops over the registered panes, skips any that are hidden (+0x3c) or have +0x44 set, sets `DAT_00666644` to the current pane, calls pane +0x24 and then the slot below. TPlayScreen vtable entries match the TScreen base vtable 0x5a5ed4:

  | Screen slot | Method | Calls on each pane |
  |---|---|---|
  | 4 | 0x48fda0 | slot 19 (+0x4c) |
  | 5 | 0x48ff00 | slot 20 (+0x50) — this is where the bottom bar and sidebar content panes paint |
  | 6 | 0x490030 | slot 22 (+0x58) |
  | **7** | **0x490110** | **slot 7 (+0x1c) — the texture pass** |
  | **8** | **0x4901e0** | **slot 23 (+0x5c) — the 2D pass**, reached through TPlayScreen 0x47c400 |

  The frame driver at **0x48f560** wraps screen slot 7 between BeginScene (0x416f70) and EndScene (0x416fb0) with ECX = 0x65a57c. The overall order across the driver functions 0x48f340 / 0x48f450 / 0x48f560 is M.

## 5. Gaps and low-confidence items

1. **Not extracted:** Equip slot 21 paint 0x5366d0; spell-creation tab slot 21 0x5435f0; quick-spell slot 20 0x5444a0; side-tabs slot 20 0x53d400; TSidePane slot 20 (0x53c900); the main loop that calls the frame drivers (0x48f340 / 0x48f450 / 0x48f560).
2. **Text needs a GDI stand-in.** Every HUD number and label goes through `IDirectDrawSurface::GetDC` plus GDI `CreateFontA`, `DrawTextA`, `TextOutA` and `GetTextMetricsA`. The emulator must rasterise Windows Arial-12, or capture text separately. The font manager (0x65b010) and the FONT.DEF parsing are not traced.
3. **Portrait:** character vtable +0x130 renders a live 3D head (I3D). It needs the 3D pipeline or a stubbed surface.
4. **Status-bar bar descriptors** (character +0x1d8 / +0x1e0 / +0x1e8) are unverified. Status-bar slot-7 vs slot-23 ownership in NoTex mode is M.
5. **0x414550 inner path:** the meaning of z, the second texture, and the blend selected by flag 4 (via 0x417d60) has not been read in detail. The hardware path needs an `IDirect3DDevice3` shim; the software path (`DAT_005d7a28` = 1) reuses runtime support that already works.
6. **Spellbook pane origin is unknown.** TSidePane / TBottomPane absolute rectangles are only partially decoded.
7. **The ZIP/pack lookup order in 0x4a1240 is M.** The runtime's `mount_file` at the expected `.\Resources\X.dat` path may get around it.
8. **Command-line parsing of NOTEXOVERLAYS / NOBLITTEXTURES** also sets `DAT_006680cc` and `DAT_005db0b8`; what those two do is unknown.
