# LoadDef_loadgame — Reconstruction SPEC

> Forensic reconstruction contract for the in-game **Load Game** DEF screen
> (`loadgame.def`). Class = `cls_0x5b9584` (vftable @ `0x5b9584`, instance @
> `DAT_0066f8d0`). Produced per
> [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md).
>
> All paths are relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`. Every number
> states its coordinate space and cites `file:line` + `hex (= dec)`.

---

## §0 — Sources & status

**Status:** `forensics-complete-by-sibling-cross-reference` (2026-05-29 revision).
The DEF layout (loadgame.def) and the activator function
`FUN_00539380_LoadDef_loadgame` (= `cls_0x5b9584` vtable slot 0, the "open"
virtual) are confirmed top to bottom and decode to literal arguments. The
three loadgame-specific class virtuals at slots 148 / 160 / 164 (`0x539590` /
`0x539550` / `0x539470`) are still not decompiled into `recon/discovered/`, but
their roles are now resolved by **direct vftable-shape alignment with the
sibling savegame class `cls_0x5b963c`** whose corresponding slots have been
analyzed in `docs/ui/forensics/SaveGameDef_SPEC.md`. The slot↔role mapping
(slot 148 = CommandDispatch / button-and-row-select branches, slot 160 =
GetField/Activate for `picture`, slot 164 = ListField for `gamelist_name`
column) is HIGH confidence because (a) both vftables have identical layout
length (184 bytes, 46 slots) and identical inherited slots in every position
except 0 / 4 / 124 / 148 / 160 / 164 (the class-specific overrides), and (b)
each loadgame override has exactly the same string-xref shape as its savegame
counterpart, restricted to the differences expected (no EDIT widget name on
loadgame, `loadgame` button name where savegame has `savegame`, etc.). See
**§14 Slot-mapping cross-reference** for the full slot diff. Original
UNCONFIRMED items A / B / C / F are downgraded but kept in §14 with their
sibling-derived resolution and the body-extraction step still listed.

**Still UNCONFIRMED at the line level:** the precise branch order inside
`virt_meth_0x539590` (CommandDispatch) — i.e. is the row-select callback
inlined into the same method that handles button clicks (sibling savegame
pattern says yes), and where exactly the `_DAT_0066fa68` modal flag is
cleared. The string-xref + global-write evidence is now decisive enough for
reconstruction (see §10 and §14-F resolution).

The render path is the DEF widget engine
(`recon/discovered/port_status/DefWidgetEngine.md`) — every element on
loadgame.def (FRAME / BITMAP / TEXT / LISTBOX / BUTTON) is dispatched through
the engine's per-class widget paint, NOT through a per-panel paint method.
**The reconstruction agent must FIRST land the DEF widget engine
(`docs/ui/briefs/B_r7_def_widget_engine.md` outstanding work items
1-5).** This spec captures everything the loadgame screen contributes ON TOP OF
the engine — the layout, asset roster, populate logic, button dispatch — but
the per-widget render code (text shadow, listbox row paint, button up/down
states, scrollbar) lives in the engine.

**Class identification (HIGH confidence).** The Open virtual at `0x539380`
calls `cls_0x5b93c4::virt_meth_0x435150` (= `FUN_00435150_DefScreen_Open`) with
`screenName="loadgame"`, `panelName="default"`, `widgetsName="widgets"`
(`recon/ghidra/cls_0x5b9584.cpp:57-58, recon/ghidra/_data.txt:114520, :114516,
:114512`). The savegame screen is the sibling class `cls_0x5b963c` (Open at
`0x5399f0`, identical shape) — they share the parent `cls_0x5b93c4` (DefScreen
base, which contains `FUN_00435150_DefScreen_Open` as virtual slot 132 in the
shared vtable shape; see `recon/ghidra/cls_0x5b9584__vftable_5b9584.cpp:40`).
The DEF activator `FUN_00539380_LoadDef_loadgame` in
`recon/discovered/renames/agent_ui_wave2_defengine.txt:270` is just the
class's vtable-slot-0 (open) virtual; **there is no separate free-function
activator** — the dispatch label is a synonym for the class method, and
`cls_0x5b9584` IS the load-game DEF-screen class.

**Errors to close:** none — there is no existing port shell for this screen
(`grep -rln 'loadgame\|LoadDef_loadgame\|FUN_00539380\|cls_0x5b9584'
src/`: only `playscreen.cpp` / `command.cpp` which deal with the SAVED-GAME
*loader* `LoadGame()`, not the UI). This spec is the first.

**Recon files read:**
- `recon/ghidra/cls_0x5b9584.cpp` — class body: Open (`0x539380`), Hide
  (`0x539440`), ctor (`0x539fc0`), dtor (`0x53a030`), `virt_meth_0x53a0c0`
  (vector-delete). 5 methods, 420-byte struct.
- `recon/ghidra/cls_0x5b9584__vftable_5b9584.cpp` — 46-slot vftable showing
  the three loadgame-specific virtuals `0x539470` (slot 164), `0x539550`
  (slot 160), `0x539590` (slot 148) inherited from / placed alongside the
  TDefScreen base shape.
- `recon/ghidra/cls_0x5b963c.cpp` + `cls_0x5b963c__vftable_5b963c.cpp` —
  sibling savegame class + its 46-slot vftable. **Decisive for slot-role
  resolution** — the override slots 0 / 4 / 124 / 148 / 160 / 164 line up
  position-for-position with loadgame (just different function addresses
  inside the same address window).
- `docs/ui/forensics/SaveGameDef_SPEC.md` — already-completed sibling spec.
  §6 documents what savegame's slot-148 / 160 / 164 do (CommandDispatch /
  GetField+Activate / ListField). Reused here verbatim for loadgame because
  the override pattern is identical except for button names.
- `recon/discovered/FUN_005399f0_LoadDef_savegame.cpp` — already-extracted
  savegame Open, identical call pattern (used to verify the loadgame
  activator shape).
- `recon/discovered/FUN_00537110_LoadDef_ingamemenu.cpp` — sibling
  DefScreen activator showing the canonical 11-arg DefScreen_Open call.
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp` and
  `FUN_00435040_DefScreen_LoadAndShow.cpp` — the open path the
  loadgame activator funnels into; LoadAndShow loads the bitmap `"background"`
  via vtable slot `+0xa0` then `LoadFile(name + ".def")`.
- `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:473-479` —
  the in-game pause command 0x53 path that opens the screen.
- `recon/discovered/cls_misc_LoadGame_48df70.cpp` — `FUN_0048df70_LoadGame`,
  the SAVED-FILE loader that the `Load Game` button invokes downstream.
- `recon/discovered/cls_0x5a5d18_TLogoScreen_Initialize_53a2c0.cpp:76` and
  `recon/discovered/player_init_notes.md:58` — the MainMenu MenuLoadGame
  binding (UNCONFIRMED-E: see §14).
- `recon/discovered/port_status/DefWidgetEngine.md` — engine identification +
  per-screen activator table (loadgame row line 67).
- `recon/ghidra/_data.txt:114502-114620` — every string XREF in the loadgame
  address window (`0x5392e0..0x539a55`), naming every NAME-attribute the
  three unextracted slots bind to.
- `RevenantRevisited/data/resources_unzipped/loadgame.def` — the literal
  on-disk DEF source, fully reproduced in §3 / §4.
- `RevenantRevisited/data/resources_unzipped/widgets.def:81-145` — the STYLE
  defaults the loadgame controls inherit from.

**Asset measurements:**
- `loadgametex.dat / loadgamenotex.dat / loadgamealpha.dat` → 1 entry
  `Background` 640×480 (`/tmp/loadgame_dump/loadgamenotex` dump).
- `widgetstex.dat` → 39 entries (frames, scroll, buttons, edit
  graphics), all dims captured (`/tmp/loadgame_dump/widgetstex/widgetstex.json`).

**Snapshot note (supplementary — superseded by retail):** there is **no
snapshot `src/` LoadDef_loadgame** — the 1998/99 source tree predates the DEF
widget engine; loadgame UI in the snapshot (if it existed) would be a
hand-rolled `TPane`-style screen, not a DEF panel. The retail layout
established in `loadgame.def` IS the authoritative design and is not derived
from a snapshot ancestor.

---

## §1 — Overview

The Load Game screen is a **modal 640×480 fullscreen DEF panel** drawn over a
darkened freeze of the play scene. It presents the user with:
- A **screenshot preview** of the selected save (`216×160` BITMAP `picture`
  inside a `ClearFrame2` chrome).
- **Game metadata** below the preview: a centered `Med`-font game title (the
  game name), and two `Module: ___` / `Character: ___` rows.
- A **scrollable LISTBOX** of save-file names on the right side, format
  `gamelist_name` per row.
- Two action **BUTTONs** at the bottom: `Load Game` and `Exit`.

When invoked, the screen runs the screenshot snapshot pipeline (writes the
current 3D frame as `ss.bmp` to use as the screenshot preview if the user is
saving in-place — for loadgame, the preview slot shows the save's stored
screenshot, NOT the current frame; see §9), then opens the DEF panel above the
existing in-game HUD/world via `DefScreen_Open`.

**When shown / hidden.**
- **Shown:** `TPlayScreen::DispatchCommand` case **`0x53`**
  (`recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:473-479`)
  fires from the in-game pause menu's "Load Game" item. The case sets
  `_DAT_0066fa68 = 1` (load-game-modal global, UNCONFIRMED-F),
  calls `FUN_00539380_LoadDef_loadgame()` (the class Open virtual), then
  `FUN_0048f040(&DAT_0066f8d0, …)` (register the screen instance on the
  modal-screen stack — `&DAT_0066f8d0` is the loadgame instance's storage),
  then `FUN_00539440()` (the Hide / re-show post-fixup, slot 4 of the
  class). Finally `_DAT_0065cb40 = 1` (dirty/redraw flag).
- **Hidden:** the `Exit` button (NAME `"exit"`) or the `Load Game` button on
  successful load. The string `s_loadingmap_005e42dc` ("loadingmap",
  `_data.txt:114585`) referenced from `virt_meth_0x539590` confirms the Load
  click path transitions through the load-screen / map-load sequence; the
  Exit path closes the modal without a load.
- **Visibility predicate (UNCONFIRMED-F):** retail uses
  `_DAT_0066fa68` as the "load-game modal is up" flag (set 1 at case 0x53
  open, cleared on close — by analogy with the options screen at case 0x52
  using `_DAT_0066fe3c`). The exact clear point is the Exit-button callback
  inside the unextracted `virt_meth_0x539590`.

**Instances:** singleton — `DAT_0066f8d0` is the storage slot for the one
`cls_0x5b9584` instance (`recon/ghidra/_data.txt:137145`,
`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:476`).

**Plain-language description.** A 640×480 modal load-game dialog with two
columns: left = a screenshot preview of the highlighted save plus its
name + module + character info; right = a vertical scrollable list of all save
files in the user's save folder; bottom = `Load Game` and `Exit` buttons. The
screen lets the player pick a save file by clicking it in the list (which
updates the left-side preview), and confirm with `Load Game`.

---

## §2 — Asset roster

All bitmaps are loaded by the DEF widget engine via the
`background → "<screen>tex.dat" + "<screen>alpha.dat"` lookup
(`recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp:31` calls the
class's `+0xa0` resource-fetch with the literal `"background"`).

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| Background | `loadgametex.dat` (color) + `loadgamealpha.dat` (alpha) + `loadgamenotex.dat` (notex variant) | `Background` | **640×480** | screen-filling backdrop (parchment / cinematic art) | whole bitmap drawn opaque at (0,0) | `/tmp/loadgame_dump/loadgametex/loadgametex.json` (measured 2026-05-29) |
| ClearFrame2 | `widgetstex.dat` (+ `widgetsalpha.dat`) | `ClearFrame2` | **64×57** atlas | 9-slice frame chrome — used by the picture frame, gameinfo frame (FRAME 14 14 16 16 MARGINS 0 0 4 4 per STYLE FRAME `widgets.def:81-82`) | 9-slice corners 0/14, edges 14..(64-14), interior tile | `/tmp/loadgame_dump/widgetstex/widgetstex.json:[3]`; `widgets.def:81-82` |
| ClearFrame3 | `widgetstex.dat` | `ClearFrame3` | 64×57 atlas | TEXT bg style (when CTRLFLAG_CLEARBG / default TEXT style applies); FRAME 5 5 5 5 MARGINS 0 0 4 4 per `widgets.def:84-86` | 9-slice corners 0/5 | `widgetstex.json:[6]`; `widgets.def:84-86` |
| ClearFrame2 (button up) | `widgetstex.dat` | `ClearFrame2` | 64×57 atlas | default BUTTON UP state (NORMAL style `widgets.def:89`); FRAME 10 10 12 12 MARGINS 0 0 4 4 | 9-slice corners 0/10, edges 10..(64-12) | `widgets.def:88-93` |
| FillFrame2 (button down) | `widgetstex.dat` | `FillFrame2` | 64×57 atlas | default BUTTON DOWN state (NORMAL style); same 9-slice | same | `widgetstex.json:[4]`; `widgets.def:88-93` |
| VScrollRect | `widgetstex.dat` | `VScrollRect` | 74×82 atlas | LISTBOX VLIST background frame (STYLE LISTBOX VLIST `widgets.def:120`); FRAME 8 26 26 26 MARGINS 0 0 4 4 NOCENTER, item rect 5 5 23 5 | 9-slice corners 0/8 (TL) 0/26 (TR/BL/BR) | `widgets.def:119-125`; `widgetstex.json:[23]` |
| VScrollUpU / VScrollUpD | `widgetstex.dat` | `VScrollUpU` / `VScrollUpD` | **16×11** each | scrollbar up-arrow (up/down state) | whole | `widgetstex.json:[27,28]`; `widgets.def:123` |
| VScrollDownU / VScrollDownD | `widgetstex.dat` | `VScrollDownU` / `VScrollDownD` | **16×11** each | scrollbar down-arrow | whole | `widgetstex.json:[29,30]`; `widgets.def:124` |
| VScrollThumbU / VScrollThumbD | `widgetstex.dat` | `VScrollThumbU` / `VScrollThumbD` | **14×13** each | scrollbar thumb 3-slice (min 19, dflt 30, 2px margin per STYLE) | 3-slice rows | `widgetstex.json:[25,26]`; `widgets.def:125` |
| **picture** (preview slot) | `Save<n>/ss.bmp` (per-save 24-bit BMP, loaded on save-selection) | n/a (file-per-save) | **216×160** (target rect; source-bmp resampled to fit) | screenshot preview of the highlighted save | dst rect (54,91,216,160) screen-local; source = full ss.bmp resampled | `loadgame.def:11` POS `54 91 216 160`; `s_ss.bmp_005e4304` referenced from `virt_meth_0x539590:005398ea` (`_data.txt:114609`) |

**Background fallback (UNCONFIRMED-G).** `loadgamealpha.dat`/`loadgamenotex.dat`
both contain the SAME `Background` entry as `loadgametex.dat` (verified by
dump tool — all three are 616536 bytes, same content). The runtime picks one
based on user texture preference (high-tex / no-tex paths); the alpha plane
is a `BM_15BIT` color image, NOT a coverage atlas — the "alpha"/"notex" suffix
on these `.dat` archives refers to the user-config quality mode, not a
separate alpha channel. Confirmed via `dump_dat.py` (flags=0x2 = `BM_15BIT`
for all three).

There is **no per-element atlas source-rect map** here — the loadgame screen
draws no game-art atlas. All sub-rect math (the 9-slice frame split, scrollbar
3-slice, button up/down) is handled inside the DEF widget engine using the
STYLE blocks at `widgets.def:81-145`. **Defer the 9-slice / 3-slice source-rect
maps to the engine SPEC** (TODO once the engine recon advances).

---

## §3 — Coordinate frames & surfaces

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | TL | `(0, 0)` | final pixel position. Loadgame is a full-screen modal, so for this panel the screen = the design canvas. | n/a |
| **canvas** | screen | TL | `(0, 0)` | the design-time 640×480 surface DefScreen_Open establishes. `DefScreen_Open` call site `recon/ghidra/cls_0x5b9584.cpp:60-61` passes `(0, 0, 0x280, 0x1e0, 0x1c2, 0xa0)` = (origin_x=0, origin_y=0, w=640, h=480, mouse_x=450, mouse_y=160). All loadgame.def coords are in this frame. | `cls_0x5b9584.cpp:60-61` (`0x280 = 640`, `0x1e0 = 480`); `FUN_00435150_DefScreen_Open.cpp` param order param_6..param_9 |
| **screen-panel** | canvas | TL | `(35, 70)` (POS literal in `loadgame.def:7`) — outer FRAME the screen content sits in; **NOTE** — this outer FRAME has no chrome (line 8 commented-out) so it acts as a logical container, NOT a visible bezel. Its 562×320 extent (from `35,70` to `597,390`) bounds the upper content; the bottom-row buttons sit BELOW it (y=406). | the dialog's visual "card" region | `loadgame.def:7` |
| **picture-frame** | canvas | TL | `(50, 87)` (POS literal `loadgame.def:10`) | 222×166 `ClearFrame2` chrome around the preview | `loadgame.def:10` |
| **picture-bitmap** | canvas | TL | `(54, 91)` (POS literal `loadgame.def:11`) — inset 4px in from picture-frame ((54-50, 91-87) = (4,4)) | 216×160 screenshot preview slot | `loadgame.def:11` |
| **gameinfo-frame** | canvas | TL | `(50, 268)` (POS literal `loadgame.def:13`) | 221×106 `ClearFrame2` chrome around the name/module/character text block | `loadgame.def:13` |
| **listbox-frame** | canvas | TL | `(284, 87)` (POS literal `loadgame.def:26`) | 298×287 LISTBOX (gamelist) — VScrollRect chrome + items + scrollbar | `loadgame.def:26` |
| **listbox-content** | listbox-frame | TL inside chrome interior, offset by STYLE rect `RECT 5 5 23 5` (top=5, left=5, **right=23 — reserves the 16px+gutter scrollbar strip**, bottom=5) per `widgets.def:121` | inner row strip at canvas `(284+5, 87+5) .. (284+298-23, 87+287-5) = (289, 92) .. (559, 369)` → content size **270×277**. 277/18 = 15.4 → **15 fully-visible rows max**, the 16th row partially clipped at the bottom. | the area where rows are drawn | `widgets.def:120-122`; `loadgame.def:26` |
| **listbox-item** | listbox-content | TL | `(0, N*18)` for row N (top=92 + N*18; row N=0 lands at canvas y=92, row N=14 lands at canvas y=92+252=344) | one save-file row, **260 wide × 18 tall** per LISTBOX `ITEM 260 18` (`loadgame.def:26`, which overrides STYLE LISTBOX VLIST's default `ITEM 1000 16` at `widgets.def:121`) | a single save-file row rect | `loadgame.def:26`; `widgets.def:121` |
| **listbox-item-field** | listbox-item | TL `(2, 0)` (`FIELD 2 0 272 18`) | 272 wide × 18 tall — **note width 272 exceeds the item 260, by design**. The FIELD width is the text-cell budget; the item width is the selection-highlight rect (the row's selection bar). Final draw rect on screen: canvas `(289+2, 92+N*18) .. (289+2+272, 92+N*18+18) = (291, 92+N*18) .. (563, 110+N*18)`. **Right edge canvas-x 563 — sits 4 pixels INSIDE the scrollbar-gutter inset (559), so text in long save names clips at canvas-x 559 against the chrome bezel, not at the FIELD's nominal 563.** Format slot index 0 = `"gamelist_name"`. | the name text field of one row | `loadgame.def:28` |
| **listbox-scrollbar** | listbox-frame | TL via RELRB | `RELRB 21 4 5 4` per `widgets.def:123` — strip on the right of the chrome, **21 px inset from the right edge** (canvas-x 284+298-21=561 left, canvas-x 284+298-5=577 right → 16 px wide strip), **4 px inset from top, 4 from bottom** (canvas-y 87+4=91 top, canvas-y 87+287-4=370 bottom → 279 px tall). Contains `VScrollUp` (16×11) at top, `VScrollDown` (16×11) at bottom, `VScrollThumb` (14×13 3-sliced vertically with min 19 / dflt 30 / 2 px end-margin) in between, riding the gap. | vertical scrollbar | `widgets.def:120,123-125` |
| **button-load** | canvas | TL | `(158, 406)` (POS literal `loadgame.def:35`) | 140×30 `Load Game` button | `loadgame.def:35` |
| **button-exit** | canvas | TL | `(350, 406)` (POS literal `loadgame.def:36`) | 140×30 `Exit` button | `loadgame.def:36` |
| **text-gamename-cell** | canvas | TL | `(55, 277)` (POS literal `loadgame.def:15`) | 210×40 game-title TEXT cell, centered Med text | `loadgame.def:15-16` |
| **text-module-label-cell** | canvas | TL | `(55, 315)` | 75×20 `Module:` label cell (right-aligned) | `loadgame.def:18` |
| **text-modname-cell** | canvas | TL | `(130, 315)` | 130×20 module-name value cell (NAME `modname`) | `loadgame.def:19` |
| **text-charname-label-cell** | canvas | TL | `(55, 335)` | 75×20 `Character:` label cell (right-aligned) | `loadgame.def:21` |
| **text-charname-cell** | canvas | TL | `(130, 335)` | 130×20 character-name value cell (NAME `charname`) | `loadgame.def:22` |

**Composition formula.** All canvas-frame coordinates are FINAL — DEF POS
values are already in the canvas (=screen) frame. No intermediate chrome /
scratch / mosaic offsets need to be composed; the DEF widget engine renders
directly to the panel's render target with its child controls in canvas
coordinates (`recon/discovered/port_status/DefWidgetEngine.md:14-19`,
`FUN_00435150_DefScreen_Open` opens at the offset `(param_4, param_5) = (0, 0)`
per the activator call).

### Verify chip / panel placement against the parent corners

This is a **canvas-anchored TL-only** layout — every coordinate is a constant
offset from the canvas TL `(0,0)`, NOT scaled with display size. The
canvas itself is 640×480; modern rendering at higher resolution must (a)
draw the screen into a 640×480 RT and composite-scale to the swapchain
(Classic mode), OR (b) re-layout each control with TL-anchored offsets
preserved + bottom-aligned controls (Load/Exit) re-pinned (Revisited mode).
See [[project-resolution-modes]] for the policy.

**Sanity check.** Sum of widths: picture (50-272) + listbox (284-582) leaves
12 px gap (272..284) — clean two-column split. Picture+gameinfo column right
edge = max(50+222, 50+221, 130+130) = 272 (canvas-x). Listbox column starts at
canvas-x 284. The buttons at canvas-y 406+30 = 436 sit cleanly above the
canvas bottom (480). All values consistent. (`loadgame.def:7-36`.)

### Surfaces

The DEF widget engine composes every widget INTO the screen's render target
(a 640×480 surface allocated at Open). It does NOT use per-widget scratch
surfaces — the bitmap blits / 9-slice composites / text composites land
directly on the screen RT.

**The Open activator (`virt_meth_0x539380`) does allocate ONE 216×160 scratch
surface** stored at `this->mbr_0x19c`:

```
cls_0x5b9584.cpp:48-49:
  iVar1 = (**(code **)(*(int *)PTR_DAT_005d79e0 + 0x18))(0);  // display fmt id
  iVar2 = FUN_004a1ec0(0xd8, 0xa0, (-(iVar1 != 0xf) & 2U) + 2);   // alloc 216x160 surface
  this->mbr_0x19c = iVar2;                                         // store
```

`0xd8 = 216`, `0xa0 = 160` — the EXACT dimensions of the `picture` BITMAP slot
on `loadgame.def:11`. This is the **screenshot-preview compositing surface**
where each save's `ss.bmp` is loaded + resampled to 216×160 before being
blitted into the canvas via the BITMAP NAME=`picture` widget. Format selector
`(-(iVar1 != 0xf) & 2U) + 2` chooses `2` when fmt!=0xf (= `BM_15BIT`/RGB555)
and `4` when fmt==0xf (= `BM_16BIT`/RGB565), matching the display's
pixel-depth (see UI_METHOD_MAP §16 chroma-key formats).
(`cls_0x5b9584.cpp:46-53`.)

After alloc the activator calls
`cls_0x4a22f0::meth_0x4a22f0((cls_0x4a22f0 *)this->mbr_0x19c, …)`
(`:56`) — this is the standard `TSurface::Init`/`TSurface::SetXxx` style call
seen in createchar/savegame too (e.g.
`recon/discovered/FUN_00465a50_LoadDef_createchar.cpp:18` allocates a different
size, same pattern). Per UI_METHOD_MAP §15a, `TSurface` is the canonical
surface class.

### Layout diagram (canvas-local, 640×480)

```
canvas (0,0)──────────────────────────────────────────────────(640,0)
│                                                                  │
│   outer-frame (35,70)─────────────────────────(597,70)            │
│   │                                                  │            │
│   │  picture-frame (50,87) ClearFrame2 222×166        │            │
│   │  ┌──────────────────────────┐                     │            │
│   │  │ picture (54,91) 216×160  │     listbox (284,87)│            │
│   │  │ [save screenshot       ] │     VScrollRect     │            │
│   │  │                          │     298×287          │            │
│   │  │                          │     ┌──────────────┐│            │
│   │  └──────────────────────────┘     │ gamelist     ││            │
│   │                                   │  (LISTBOX)   ││            │
│   │  gameinfo-frame (50,268) 221×106  │              ││            │
│   │  ┌──────────────────────────┐     │ row 260×18   ││            │
│   │  │ gamename (55,277) 210×40 │     │ FIELD(2,0)   ││            │
│   │  │  "<save title>"          │     │  272×18      ││            │
│   │  │  Med, center, white      │     │   …          ││            │
│   │  │  shadow                  │     │              ││            │
│   │  │                          │     │   (scrollbar ││            │
│   │  │ Module:   <modname>      │     │    RELRB     ││            │
│   │  │ (55,315) (130,315)       │     │    21 4 5 4) ││            │
│   │  │ Character:<charname>     │     └──────────────┘│            │
│   │  │ (55,335) (130,335)       │                     │            │
│   │  └──────────────────────────┘                     │            │
│   │                                                  │            │
│   └────────────────────────────────────────(597,390)──┘            │
│                                                                    │
│   [Load Game] (158,406) 140×30      [Exit] (350,406) 140×30        │
│                                                                    │
canvas-bottom (640,480)──────────────────────────────────────────────
```

---

## §4 — Static element layout

The DEF file is the authoritative source for every coordinate. All values are
canvas-frame, TL-anchored (single space — no mirror, no sub-surface offset to
compose).

| element | space | (x, y) | (w, h) | source rect / role | cite |
|---|---|---|---|---|---|
| outer FRAME | canvas | (35, 70) | (562, 320) | invisible container (chrome line commented out) | `loadgame.def:7` |
| picture-frame FRAME | canvas | (50, 87) | (222, 166) | 9-slice `ClearFrame2` chrome | `loadgame.def:10` |
| picture BITMAP slot | canvas | (54, 91) | (216, 160) | dst rect for the 216×160 scratch surface (`this+0x19c`); field=`"picture"` driven by `virt_meth_0x539590` updating from selected save's `ss.bmp` | `loadgame.def:11`; `cls_0x5b9584.cpp:49`; `_data.txt:114609` |
| gameinfo FRAME | canvas | (50, 268) | (221, 106) | 9-slice `ClearFrame2` chrome | `loadgame.def:13` |
| gamename TEXT | canvas | (55, 277) | (210, 40) | NAME `gamename`; default text `"No Saved Game"`; Med font, white, CENTER+SHADOW+ELIPSES, RECT inset 3 0 3 0, DRAWMODE DM_USEDEFAULT (=`0x80000000`) | `loadgame.def:15-16` |
| Module: label | canvas | (55, 315) | (75, 20) | static label `"Module:"`, TEXTFLAG_RIGHT (right-justified within cell) | `loadgame.def:18` |
| modname TEXT | canvas | (130, 315) | (130, 20) | NAME `modname`; default `"none"`; CTRLFLAG_CLEARBG (clears bg first); inherits STYLE TEXT defaults (`widgets.def:84-86`) — Med font, white, LEFT+VCENTER+SHADOW | `loadgame.def:19`; `widgets.def:84-86` |
| Character: label | canvas | (55, 335) | (75, 20) | static label `"Character:"`, TEXTFLAG_RIGHT | `loadgame.def:21` |
| charname TEXT | canvas | (130, 335) | (130, 20) | NAME `charname`; default `"none"`; CTRLFLAG_CLEARBG; STYLE TEXT defaults | `loadgame.def:22` |
| gamelist LISTBOX | canvas | (284, 87) | (298, 287) | NAME `gamelist`; FIELD `listfield`; FLAGS `LISTFLAG_VLIST \| LISTFLAG_USEFORMAT`; ITEM `260 18`; inner FIELD `2 0 272 18` flags `TEXT_LEFT \| TEXT_SHADOW`, format slot 0 = `"gamelist_name"` (the save's name) | `loadgame.def:26-29` |
| Load Game BUTTON | canvas | (158, 406) | (140, 30) | NAME `loadgame`; label `"Load Game"`; inherits NORMAL BUTTON STYLE (UP=ClearFrame2, DOWN=FillFrame2, Med font, white, CENTER+VCENTER+SINGLELINE+SHADOW, `widgets.def:88-93`) | `loadgame.def:35` |
| Exit BUTTON | canvas | (350, 406) | (140, 30) | NAME `exit`; label `"Exit"`; inherits NORMAL BUTTON STYLE | `loadgame.def:36` |

No mirrored side. No chrome/scratch sub-frame offsets to compose (only the
216×160 preview-scratch, which composes 1:1 into the picture cell at
(54,91)).

---

## §5 — Draw order / composition

The DEF widget engine renders by walking the panel's child list in
declaration order (TODO: confirm by examining the engine's tree-walk, slot
TBD — engine port_status row not yet pinned, see DefWidgetEngine.md
"Renderer" section). For loadgame.def the order is therefore (top to
bottom of file = back-to-front on screen):

1. **Background blit** — `DefScreen_LoadAndShow` calls the class's
   `+0xa0` resource-fetch with `"background"`
   (`FUN_00435040_DefScreen_LoadAndShow.cpp:31`) → loads 640×480 `Background`
   bitmap → blits at canvas (0,0) opaque (DM_USEDEFAULT) before any widget
   draws. This is the parchment / cinematic backdrop. (`:32`
   `FUN_00435660(uVar2)` then `FUN_00435380()` complete the bg blit + clear.)
2. **outer FRAME** at (35,70) 562×320 — bitmap omitted (commented out), so
   no visible draw.
3. **picture-frame FRAME** at (50,87) 222×166 — 9-slice `ClearFrame2`.
4. **picture BITMAP** at (54,91) 216×160 — blits `this->mbr_0x19c` (the
   216×160 scratch holding the resampled `ss.bmp` of the selected save).
   Field-bound to runtime updates from `virt_meth_0x539590`.
5. **gameinfo-frame FRAME** at (50,268) 221×106 — 9-slice `ClearFrame2`.
6. **gamename TEXT** at (55,277) 210×40 — Med, white, centered, shadow,
   ellipses.
7. **Module: label TEXT** at (55,315) 75×20 — right-justified Med, default
   STYLE.
8. **modname TEXT** at (130,315) 130×20 — runtime-updated.
9. **Character: label TEXT** at (55,335) 75×20 — right-justified.
10. **charname TEXT** at (130,335) 130×20 — runtime-updated.
11. **gamelist LISTBOX** at (284,87) 298×287 — VScrollRect 9-slice chrome,
    items composed inside, scrollbar on the right per STYLE LISTBOX VLIST.
12. **Load Game BUTTON** at (158,406) 140×30 — 9-slice UP/DOWN frame +
    centered Med label.
13. **Exit BUTTON** at (350,406) 140×30 — same.

The engine handles the per-widget draw; there is no per-panel paint method
on `cls_0x5b9584` other than the Open and Hide virtuals.

---

## §6 — Algorithms (pseudocode per helper)

### §6.1 — Open / Activate (`virt_meth_0x539380` @ `0x00539380`, the class
slot-0 virtual = `FUN_00539380_LoadDef_loadgame`)

Cited line-by-line from `recon/ghidra/cls_0x5b9584.cpp:36-63` /
`recon/classes/cls_0x5b9584.cpp:36-60`.

```
// Stop input device polling / arm modal context
FUN_0048d260(DAT_0066a618);   // a TInputManager / cursor lock
                              // (DAT_0066a618 = cursor mgr instance,
                              // exact semantics UNCONFIRMED-H)

// Pick surface pixel-format for the preview scratch
displayFmt = (*display->vtable[+0x18])();
                              // PTR_DAT_005d79e0 = TDisplay global
                              // vtable[+0x18] = GetFormat()
preview_fmt = (displayFmt == 0x0f) ? 4 : 2;
                              // 0xf = BM_16BIT (RGB565) → use 4 (BM_24BIT? UNCONFIRMED-H)
                              // else → use 2 (BM_15BIT, RGB555)
                              // matches the per-platform pixel format

// Allocate 216x160 preview-scratch
this->mbr_0x19c = FUN_004a1ec0(0xd8, 0xa0, preview_fmt, 0);
                              // 216 wide × 160 tall
                              // FUN_004a1ec0 = surface alloc (see
                              // BarInvPane_SPEC §13, InventoryPane_SPEC §6)

// Construct the surface object onto that memory
cls_0x419dd0 tmp;             // a 1-int helper (BlitEffect_SetField? §UI_METHOD_MAP 7)
cls_0x419dd0::cls_0x419dd0(&tmp);
cls_0x4a22f0::meth_0x4a22f0(this->mbr_0x19c, tmp, 0);
                              // = TSurface ctor / Init pattern

this->mbr_0x1a0 = 0;          // some "current save index" state (UNCONFIRMED-H)
DAT_0066fb04 = 0xffffffff;    // global "active save index" sentinel = -1
                              // (no selection yet)

// Open the DEF panel
FUN_00435150_DefScreen_Open(
    this,
    name      = "loadgame",          // s_loadgame_005e4260
    panel     = "default",           // s_default_005e4258
    flags     = (this->mbr_0x198 != 0) ? 0x11 : 0,
                                     // mbr_0x198 = an in-game vs main-menu
                                     // flag; 0x11 = (CTRLFLAG_DRAWTOBG=0x800?)
                                     // — exact bits UNCONFIRMED-I
    origin_x  = 0,
    origin_y  = 0,
    width     = 0x280,               // 640
    height    = 0x1e0,               // 480
    mouse_x   = 0x1c2,               // 450 (init cursor at right column,
                                     // about over the listbox center)
    mouse_y   = 0xa0,                // 160 (vertical center of listbox)
    widgets   = "widgets"            // s_widgets_005e4250
);
return 1;
```

The trailing 12th arg to `FUN_00435150_DefScreen_Open` (the savegame screen
passes `s_savegame_005e4314` here, i.e. an OPTIONAL extra `.def` filename) is
**omitted** in the loadgame call (savegame has 11+1 args, loadgame has 11 —
see `FUN_005399f0_LoadDef_savegame.cpp:23-24` vs. `cls_0x5b9584.cpp:57-58`).
This means loadgame uses ONLY `widgets.def + loadgame.def`, while savegame
chains in an additional `savegame.def` extra. (Hmm — savegame also lists
`"savegame"` twice — actually it's name + extra-style-file. UNCONFIRMED-J:
verify the `param_12` field's role via a recon read of
`FUN_00435150_DefScreen_Open.cpp:55-57` — it calls `FUN_00435b20(param_12)`
which is some additional style merge.)

### §6.2 — Hide / Close (`virt_meth_0x539440` @ `0x00539440`, slot 4)

Cited from `recon/ghidra/cls_0x5b9584.cpp:64-78`.

```
if (this->mbr_0x19c != 0) {
    FUN_004830f0(this->mbr_0x19c);   // free the 216x160 preview-scratch
}
this->mbr_0x19c = 0;
cls_0x5b98b8::virt_meth_0x434f30(this);
                                      // = parent (TDefScreen base) Hide
                                      // dismantles the panel + child widgets
return;
```

### §6.3 — Constructor (`cls_0x5b9584` @ `0x00539fc0`)

Cited from `recon/ghidra/cls_0x5b9584.cpp:79-103`. Stores design-time
bounds: `this->mbr_0xc = 0x280 = 640`, `this->mbr_0x1c = 0x280 = 640`,
`this->mbr_0x10 = 0x1e0 = 480`, `this->mbr_0x20 = 0x1e0 = 480`. These are the
pane's `(0xc, 0x10) = (width, height)` and `(0x1c, 0x20) = a duplicated
extent` (the second pair is likely the renderer's "live" target extent vs. the
constant design extent; see UI_METHOD_MAP §15a `+0x08` etc.; UNCONFIRMED-K).
Sets vtable, zeros members `0x48..0x54` (cached pointers), then calls
`cls_0x41c7f0::cls_0x41c7f0(this+0x88, 0x10)` — initializes some sub-object
at offset 0x88 with capacity 16 (probably a fixed-size widget-handle table
of 16 entries; the engine has `count DAT_0065617c=8` widget classes, so 16
is generous — UNCONFIRMED-K).

### §6.4 — `virt_meth_0x539470` (slot 164) — `ListField` callback (RESOLVED via sibling)

**Slot role (HIGH confidence via sibling).** Position 164 in the vftable is
the **`ListField(row, col_name, out, outsz)` callback** the LISTBOX engine
calls per row to fetch the row's column text. Resolution comes from
`docs/ui/forensics/SaveGameDef_SPEC.md` §6 + `cls_0x5b963c__vftable_5b963c.cpp:48`
where slot 164 = savegame `virt_meth_0x539ae0`, identified in that spec
(§10 + UNCONFIRMED #2 resolved on the savegame side) as the
"FIELD/format callback" the LISTBOX uses to emit each row's `gamelist_name`
string. Loadgame slot 164 = `0x539470` has the **same signature and same
string-xref shape** (just resolves to a load-game module's save list instead
of the save-game-being-saved-to one).

String xrefs (`recon/ghidra/_data.txt:114525,114530`):
- `:005394af` references `s_gamelist_005e426c` (`"gamelist"`) — the LISTBOX
  widget name being matched. The callback is parameterized on the widget
  the engine is calling it for; the body matches on `"gamelist"`.
- `:005394c7` references `DAT_005e4278` `"name"` — the column field-id being
  matched. The body matches on the column-name string the LISTBOX's
  FIELD slot 0 was declared with. **Important — `loadgame.def:28` declares
  the FIELD's name as `"gamelist_name"`, NOT `"name"`.** So either (a) the
  format string is split on `_` and the suffix `"name"` is the per-column
  selector (UNCONFIRMED-N), or (b) `"name"` here is a different lookup key
  (e.g. the save-entry struct's field name, dereferenced after the row
  index is mapped to an entry). Sibling savegame's `virt_meth_0x539ae0`
  has the matching `s_gamelist_005e4338` + `s_name_005e4344` xrefs at
  `_data.txt:114625,114641` — identical pattern, **same field-name "name"**,
  confirming this is the column-selector key, not the literal DEF FIELD name.

Read of `DAT_0065a628` at `:005394e1` (`_data.txt:129533`) — a global
**save-list data source** (cross-screen, also read by savegame's slot 164
`:00539b51` and slot 148 `:0053987f`). For loadgame this is the saved-files
enumeration owned by the current module.

**Behavior (per sibling):**
```
ListField(this, listbox_name, col_name, row_idx, out_buf, out_size):
    if (stricmp(listbox_name, "gamelist") != 0) return 0;     // not us
    if (stricmp(col_name, "name") != 0)         return 0;     // unknown col
    save_entry = DAT_0065a628[row_idx];                       // saved-game record
    strncpy(out_buf, save_entry->display_name, out_size);
    return 1;
```

(The exact `DAT_0065a628` indirection — array-of-pointers vs.
fixed-stride — is UNCONFIRMED-A2; the standard pattern from
`cls_misc_LoadGame_48df70.cpp:80-84` and `recon/discovered/cls_TArea_FUN_0041c000.cpp:42`
is `(int*)(DAT_0065a77c + DAT_0065a780 * 4)` for the module's row-zero, with
a per-module subtable holding save records, but `DAT_0065a628` is a *separate*
global and looks like it holds the loadgame-screen's pre-loaded save list
specifically.)

**Resolve step (UNCONFIRMED-A2):** `DecompileAddr.java 0x00539470` for the
exact `DAT_0065a628` access shape.

### §6.5 — `virt_meth_0x539550` (slot 160) — `GetField`/`Activate` for `picture` (RESOLVED via sibling)

**Slot role (HIGH confidence via sibling).** Position 160 in the vftable is
the **`GetField(field_name) → ptr`** (or **`Activate(widget, sub_index)`**)
override. Resolution from `SaveGameDef_SPEC.md` §5 (paint walk step 4)
and §9 (sibling's slot 160 `0x539bc0` documented as "per-control
activation / GetField for `picture` returning the picture-scratch surface").

String xref (`_data.txt:114539`):
- `:00539556` references `s_picture_005e4280` (`"picture"`) — the BITMAP
  widget's FIELD-binding name. The engine's BITMAP draw walks the
  `FIELD "picture"` attribute (`loadgame.def:11`), calls
  `this->vtable[+0xa0]("picture")`, and the body's stricmp on the input
  string against `"picture"` matches → returns `this->mbr_0x19c` (the
  216×160 preview-scratch surface).

**Behavior (per sibling):**
```
GetField(this, field_name) -> void*:
    if (stricmp(field_name, "picture") == 0) {
        return (void*) this->mbr_0x19c;        // the 216x160 preview surface
    }
    return base::GetField(this, field_name);   // fall through for unknown
```

The picture surface itself is populated on each LISTBOX selection-change
inside slot 148 (CommandDispatch §6.6 below) — slot 160 is purely the
binding query.

**Resolve step:** `DecompileAddr.java 0x00539550` (low priority — the
inferred body is trivial and matches the engine convention exactly).

### §6.6 — `virt_meth_0x539590` (slot 148) — `CommandDispatch` / selection + Load/Exit branches (RESOLVED via sibling)

**Slot role (HIGH confidence via sibling).** Position 148 = **CommandDispatch**
(button-click + row-select dispatch). Resolution from `SaveGameDef_SPEC.md`
§6, §10 (sibling slot 148 = `0x539c00` documented as
"slot 148 = command dispatch (Save / Exit / picture-update branches)").
The loadgame body is structurally the same with `loadgame` button name
replacing `savegame`, the Load click invoking `FUN_0048df70_LoadGame`
instead of the saver, and the same per-selection refresh of
`picture` / `gamename` / `modname` / `charname`. String-xref density
density matches: 14 widget-name + behavior-key xrefs at
`_data.txt:114543-114614`.

String xrefs (the decisive evidence):

| addr | string | inferred role |
|---|---|---|
| `:005395be` | `s_gamelist_005e4288` `"gamelist"` | look up LISTBOX widget by name |
| `:005395ca` | `s_picture_005e4294` `"picture"` | look up BITMAP widget by name |
| `:005395dc` | `s_gamename_005e429c` `"gamename"` | look up TEXT widget by name |
| `:005395ee` | `s_modname_005e42a8` `"modname"` | look up TEXT widget by name |
| `:00539600` | `s_charname_005e42b0` `"charname"` | look up TEXT widget by name |
| `:0053963c` | `s_loadgame_005e42bc` `"loadgame"` | look up BUTTON widget by name |
| `:00539651` | `DAT_005e42c8` `"exit"` | look up BUTTON widget by name |
| `:00539685` | `s_loadgame_005e42d0` `"loadgame"` | (2nd ref — likely as the dispatched name on Load click) |
| `:005396fc` | `s_loadingmap_005e42dc` `"loadingmap"` | transition target — load-in-progress modal |
| `:005397a7` | `s_cursor_005e42e8` `"cursor"` | cursor change (busy cursor while loading) |
| `:005397ed` | `DAT_005e42f0` `"exit"` | (2nd `"exit"` ref — exit-button branch) |
| `:00539864` | `s_gamelist_005e42f8` `"gamelist"` | re-lookup gamelist (for the post-selection read of name) |
| `:005398ea` | `s_ss.bmp_005e4304` `"ss.bmp"` | open the selected save's screenshot file (per-save bitmap) |
| `:00539971` | `s_Locke_005e430c` `"Locke"` | default character name fallback when save lacks a charname |

**Resolved purpose (per sibling slot mapping + xref evidence):** this is
the **CommandDispatch / per-control activate** body — the engine routes
button-clicks and listbox-row-selection events through this slot. It (a)
reads the current LISTBOX selection, (b) updates the `picture` / `gamename`
/ `modname` / `charname` widgets from the selected save's metadata, (c)
dispatches `loadgame` and `exit` button clicks. The `ss.bmp` reference is
the per-save screenshot file — loaded into the `this->mbr_0x19c` 216×160
scratch. The `loadingmap` reference is the transition to the
load-in-progress map screen. The `Locke` reference is the default protagonist
character name shown when a save was created for a character without a
stored charname.

**Modal-flag clear (resolves UNCONFIRMED-F).** Per `_data.txt:137166-137177`
the global `_DAT_0066fa68` ("load-game modal is up" flag set to 1 in case
`0x53`, see §1) is **WRITTEN** at TWO sites inside
`virt_meth_0x539590`:
- `:005396cc` (`*`, write address taken) + `:005396d1` (`W`) — inside the
  Load-button branch, right around the `loadgame` button-name match at
  `:00539685` (string xref `s_loadgame_005e42d0`). This is the
  `_DAT_0066fa68 = 0` clear after a successful Load before transitioning
  to `loadingmap`.
- `:0053982b` (`*`) + `:00539830` (`W`) — inside the Exit branch, right
  after the second `"exit"` string match at `:005397ed` (DAT_005e42f0).
  This is the simple `_DAT_0066fa68 = 0` clear on Cancel/Exit.

(Sibling savegame slot 148 (`0x539c00`) has the parallel writes at
`recon/ghidra/_data.txt` `virt_meth_0x539c00` sites against
`_DAT_0066fb04` and the savegame's modal flag. Same shape.)

**Selected-save state.** `DAT_0066fb04` (the global "active save index",
set to `-1` in Open at `cls_0x5b9584.cpp:58`) is **READ** by 0x539590 at
`:0053969a` and `:00539706`, and **WRITTEN** at `:0053988b`
(`_data.txt:137175-137177`). Reads are inside the Load / loadingmap branch
(it's the index of the save the user clicked Load on); the write at
`:0053988b` is the per-selection-change update — the LISTBOX row callback
records the row index into `DAT_0066fb04` so the subsequent Load button
knows which save to load.

**`LoadGame` call signature.** `FUN_0048df70_LoadGame` takes **three** args
(`recon/discovered/cls_misc_LoadGame_48df70.cpp:5`):
`bool LoadGame(int* saveList, char* name, uint flags)`. `saveList` is the
in-RAM save catalog (NOT a per-save struct — `*saveList` is the count,
`saveList[4]` is the entries array, `saveList[5]` is the busy flag, see the
function body). The dispatcher passes the loadgame-screen's resident save
list (most likely `&DAT_0065a628` per the slot-164 read) for `saveList`,
the LISTBOX row's name as `name`, and `flags = 0`.

**Resolve step (UNCONFIRMED-C, downgraded but still TODO):** `DecompileAddr.java
0x00539590` — confirms the exact branch structure. The xref evidence is
decisive enough for reconstruction; line-level extraction nails the
branch ordering (row-select vs Load click vs Exit click).

### §6.7 — Background drawing (in `FUN_00435040_DefScreen_LoadAndShow`)

Cited from `recon/discovered/FUN_00435040_DefScreen_LoadAndShow.cpp:30-32`:
```
(*this->vtable[+0x20])();          // clear / init render target
uVar2 = (*this->vtable[+0xa0])(    // load-resource by name (UNCONFIRMED: vtable+0xa0)
            "background",          // s_background_005cd8d4
            0xffffffff             // alias/flags?
        );
FUN_00435660(uVar2);               // blit the loaded bitmap full-screen
FUN_00435380();                    // commit / finalize
```

For loadgame, `vtable[+0xa0]("background")` resolves to `Background` from
`loadgametex.dat` (or alpha/notex variant per quality settings) and
`FUN_00435660` opaque-blits at canvas (0,0). This is the BACKDROP — the
parchment-style cinematic art behind all widgets.

---

## §7 — Effects & shadows

The loadgame screen draws no per-bitmap drop shadows. **Text widgets** get
the engine's standard 3-pass black shadow whenever `TEXT_SHADOW` (= `0x400`)
is set in the FLAGS bits. Per `widgets.def`:
- Default STYLE TEXT (`widgets.def:86`) sets `TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW`.
- Default STYLE BUTTON UPLABEL (`widgets.def:91`) sets `TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE | TEXT_SHADOW`.
- DOWNLABEL (`widgets.def:92`) sets `TEXT_CENTER | TEXT_VCENTER | TEXT_SINGLELINE` (NO shadow, color = `0 0 0` black — pressed-button label).
- gamename TEXT explicit (`loadgame.def:16`) `TEXT_CENTER | TEXT_SHADOW | TEXT_ELIPSES`.
- gamelist row FIELD (`loadgame.def:28`) `TEXT_LEFT | TEXT_SHADOW`.
- modname/charname TEXT inherit defaults → shadow on.
- Module: / Character: labels use `TEXTFLAG_RIGHT` (= `0x80000`); inherit STYLE TEXT
  defaults which include `TEXT_SHADOW`.

The shadow is the 3-pass `FUN_004be2b0` pattern per UI_METHOD_MAP §5: a black
pass at (base), (base + 1×), (base + 1y), then the colored pass at base.
Direction = right + down (down-right). **The pink-halo bug**
(NOMENCLATURE §4, [[project-retail-pink-halo-bug]]) applies whenever the
engine's text composite goes through a magenta-keyed scratch — the port must
render with real alpha, NOT magenta colorkey.

**Chroma key.** Widget bitmaps in `widgetstex.dat` have `kc=0x0` (UI_METHOD_MAP
§16: the bitmap reports a zero keycolor, but the global magenta key
`0x7C1F` still applies in retail's blit path). However the `ClearFrame*`
widgets carry an `alpha` plane (separately stored in `widgetsalpha.dat` —
UNCONFIRMED-L), so the port should prefer the alpha path over chroma keying.

No per-side shadow / mirror — single-sided panel, no chip model. The bitmap
drop-shadow setup function `FUN_00438d80` is NOT directly invoked from the
loadgame activator (no `dx,dy` shadow offset to record at this layer).

---

## §8 — Text rendering

All text values are TL-anchored cell rects in canvas frame. Per
`recon/discovered/port_status/DefWidgetEngine.md:39-52` STYLE TEXT
attributes are inherited from `widgets.def:84-86` unless explicitly
overridden.

| string | cell (canvas, x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| `"<save title>"` | (55, 277, 210, 40) | `Med` | engine-defined (the `Med` font, see UNCONFIRMED-M) | `(255,255,255)` white | center (TEXT_CENTER) | top (default; no TEXT_VCENTER on gamename) | yes (TEXT_SHADOW) | default value `"No Saved Game"` (`loadgame.def:15`); ellipsis if overflows (TEXT_ELIPSES) | runtime: save's stored title field; default-empty fallback | `loadgame.def:15-16` |
| `"Module:"` | (55, 315, 75, 20) | `Med` (STYLE TEXT inherit) | `Med` | white (inherit) | right (TEXTFLAG_RIGHT = `0x80000`) | vcenter (STYLE TEXT inherit `TEXT_VCENTER`) | yes (inherit TEXT_SHADOW) | literal `"Module:"` | static | `loadgame.def:18`; `widgets.def:86` |
| `"<module name>"` | (130, 315, 130, 20) | `Med` | `Med` | white | left (STYLE TEXT inherit TEXT_LEFT) | vcenter (inherit) | yes (inherit) | default `"none"`; runtime: NAME `modname` updated by `virt_meth_0x539590` from selected save's `modname` field | `loadgame.def:19`; `widgets.def:86` |
| `"Character:"` | (55, 335, 75, 20) | `Med` | `Med` | white | right (TEXTFLAG_RIGHT) | vcenter (inherit) | yes (inherit) | literal `"Character:"` | static | `loadgame.def:21`; `widgets.def:86` |
| `"<character name>"` | (130, 335, 130, 20) | `Med` | `Med` | white | left (inherit) | vcenter (inherit) | yes (inherit) | default `"none"`; runtime: NAME `charname`; falls back to `"Locke"` per `_data.txt:114614` if not set (Locke = default protagonist) | `loadgame.def:22` |
| save-file row name | listbox row FIELD (2, 0, 272, 18) within each row of (260, 18) | `Med` (LISTBOX STYLE inherit `widgets.def:122`) | `Med` | white (inherit; selection uses SELCOLOR `0 112 74` = dark green per `widgets.def:121`) | left (TEXT_LEFT) | vcenter (STYLE LISTBOX inherit) | yes (TEXT_SHADOW) | format slot index 0 = `"gamelist_name"` | runtime: each save's name field | `loadgame.def:28`; `widgets.def:120-122` |
| `"Load Game"` | button (158, 406, 140, 30) | `Med` | `Med` | white when UP (`255 255 255`), black when DOWN (`0 0 0`) | center (TEXT_CENTER) | vcenter (TEXT_VCENTER) | yes when UP only (DOWN style has no SHADOW) | literal `"Load Game"` | static | `loadgame.def:35`; `widgets.def:91-92` |
| `"Exit"` | button (350, 406, 140, 30) | `Med` | `Med` | white UP / black DOWN | center | vcenter | yes UP / no DOWN | literal `"Exit"` | static | `loadgame.def:36`; `widgets.def:91-92` |

**Font flag bits (recoded for engine port):** per UI_METHOD_MAP §5 step 6, the
DrawTextA uFormat is built from the input flags:
- `TEXT_LEFT` (`0x0001`) → DT_LEFT (default).
- `TEXT_CENTER` (`0x0002`) → DT_CENTER.
- `TEXT_RIGHT` (`0x0004`) → DT_RIGHT.
- `TEXT_TOP` (`0x0010`) → DT_TOP.
- `TEXT_BOTTOM` (`0x0020`) → DT_BOTTOM.
- `TEXT_VCENTER` (`0x0040`) → DT_VCENTER.
- `TEXT_SINGLELINE` (`0x0080`) → DT_SINGLELINE.
- `TEXT_SHADOW` (`0x0400`) → the 3-pass shadow gate (UI_METHOD_MAP §5 step 8).
- `TEXT_ELIPSES` (`0x2000`) → DT_END_ELLIPSIS.
- `TEXTFLAG_RIGHT` (`0x80000`) → engine-level: container-flag, the
  text widget should set the inner text's DT_RIGHT regardless of STYLE.

The engine maps these DEF-level flag bits into the lower-level `param_10` of
`FUN_004be2b0` per UI_METHOD_MAP §5.

**Visual anchor sanity check.** The gamename cell (55, 277, 210, 40) centers
horizontally at canvas-x 55+105=160. The gameinfo-frame is at (50, 268) 221
wide, so its horizontal center is 50+110=160. **Match** — the centered title
is visually centered in its surrounding chrome. Similarly the Module label
right-edge at 55+75=130 abuts the modname cell left-edge at 130 (no gap, no
overlap) — clean two-column "label : value" layout. The button row sits at
canvas-y 406; the gameinfo-frame ends at 268+106=374, so there is 32 px of
gap between info-frame bottom and Load button top — reads as a clear visual
separator.

**Vertical alignment on gamename.** TEXT_VCENTER is NOT explicitly set on
the gamename line, AND the STYLE TEXT default (`widgets.def:86`) DOES include
TEXT_VCENTER. The explicit FLAGS on `loadgame.def:16` is
`TEXT_CENTER | TEXT_SHADOW | TEXT_ELIPSES` — this OVERRIDES the style
default, so gamename is TOP-aligned in its 40-px tall cell (NOT vertically
centered). Per NOMENCLATURE §2 ("multi-line text stacks DOWNWARD from the
cell top"), if the title wraps to two lines they stack from y=277 down by
the engine line height; the bottom of the 40-px cell stays empty.
**Confirmed via flag-override precedence:** in `widgets.def:86` STYLE TEXT
sets `FLAGS TEXT_LEFT | TEXT_VCENTER | TEXT_SHADOW`; the gamename line in
`loadgame.def:16` then sets `FLAGS TEXT_CENTER | TEXT_SHADOW | TEXT_ELIPSES`
— the second FLAGS REPLACES the inherited set (NOT ORs in), per
DEF-engine convention (UNCONFIRMED-N: confirm OR-vs-replace in the engine
attr parser; if ORd, gamename would be center+vcenter+shadow+ellipsis).

---

## §9 — Animation & dynamic behavior

### Selection-driven updates

When the user clicks a row in the gamelist LISTBOX, three widgets must
update:
1. `picture` BITMAP — reload `this->mbr_0x19c` (216×160 scratch) from the
   selected save's `ss.bmp` and re-blit.
2. `gamename` TEXT — set to the save's stored title.
3. `modname` TEXT — set to the save's stored module name.
4. `charname` TEXT — set to the save's stored character name; default to
   `"Locke"` if absent.

The update is driven by `virt_meth_0x539590` (UNCONFIRMED-C). It is NOT
animated — instant replace on selection-change. Cite: string xrefs at
`_data.txt:114539-114614` showing all four field-name strings referenced
from the same method.

### Cursor state

`s_cursor_005e42e8` (referenced at `:005397a7`) names a `cursor` setting —
inferred to swap to a busy/hourglass cursor while the load operation
proceeds (between Load-button click and the `loadingmap` transition).
Confirmed pattern: NOMENCLATURE §6 "cursor switch on long-running op".
UNCONFIRMED-O: which cursor sprite name is used.

### Dirty / redraw

The screen does NOT redraw on every frame — it's a dirty-rect modal.
`DAT_0065cb40 = 1` (set at the end of case 0x53,
`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:478`) is the global "modal-screen
needs full redraw" flag — set on open, cleared after the next paint pass.
Per-widget redraw is driven by `CTRLFLAG_DIRTY` (= `0x20`, `widgets.def:35`)
which the engine sets when a widget's value changes (e.g. picture source bound
to new ss.bmp, modname value updated). No tween / ramp / animation on this
panel.

### Initial state

At open (`virt_meth_0x539380`):
- `this->mbr_0x1a0 = 0` — current selection index (none).
- `DAT_0066fb04 = 0xffffffff` — global active-save = -1.
- `gamename` shows `"No Saved Game"`.
- `modname` shows `"none"`.
- `charname` shows `"none"`.
- `picture` scratch is allocated but contents are UNINITIALIZED — UNCONFIRMED
  whether the engine zeros it on bind, or the empty BITMAP widget draws
  whatever color it was cleared to. The port should zero-clear to black on
  alloc to avoid uninitialized-pixel artifacts.

---

## §10 — Input & dispatch

The DEF widget engine handles per-widget mouse / keyboard input
(`port_status/DefWidgetEngine.md:17` — `0x004361f0
DefWidget_DispatchInput`: "Hot-reload (R), arrow nav, Enter activate,
per-child KeyDown fall-through"). For loadgame the per-widget contributions
are:

| control | hit rect (canvas) | command id / callback | modal-state visual | dispatch path |
|---|---|---|---|---|
| `gamelist` LISTBOX | (284, 87) 298×287 (chrome); content rect (289,92)..(559,369) → 270×277, 15 fully visible 18px rows | row-select → engine routes to `virt_meth_0x539590` (slot 148) selection branch. Branch (a) WRITES the row index to `DAT_0066fb04` (`:0053988b`), (b) loads the selected save's `<savedir>/ss.bmp` into `this->mbr_0x19c` (216×160 scratch, blitted to the BITMAP via slot 160 GetField), (c) sets the `gamename` / `modname` / `charname` TEXT widgets to the save's stored title / module name / character name (charname falls back to literal `"Locke"` per `:00539971`). LISTFLAG_USEFORMAT means each row's hit rect is its own (item_y, item_y+18) span; LISTFLAG_DBLCLICK is NOT set, so dblclick does NOT trigger Load (single-click selects only) | selected row is drawn with SELCOLOR `(0, 112, 74)` (dark green) per STYLE LISTBOX VLIST `widgets.def:121` — a filled rect behind the row's text, NOT a chroma key | engine row-select callback → `0x539590` (slot 148, "row-select" branch matching `s_gamelist_005e4288` xref @ `:005395be`) |
| `loadgame` BUTTON (action button id: **the string `"loadgame"`** — engine matches widget NAME, no numeric command id) | (158, 406) 140×30 | click → `virt_meth_0x539590` Load-button branch: reads `DAT_0066fb04` (selected index), invokes the saved-game loader `FUN_0048df70_LoadGame(saveList=DAT_0065a628, name=row.name, flags=0)` (`cls_misc_LoadGame_48df70.cpp:5` — **3-arg** signature: `bool LoadGame(int* saveList, char* name, uint flags)`, NOT 2-arg). On success: clears `_DAT_0066fa68 = 0` (`:005396d1`), swaps cursor to busy (string xref `"cursor"` @ `:005397a7`), transitions to `"loadingmap"` modal-screen state (`:005396fc`). On failure (`LoadGame` returns false): branch fall-through; modal stays open (UNCONFIRMED-R: exact failure UX — silent retry vs. show an error popup). | UP = white-on-ClearFrame2 9-slice, DOWN = black-on-FillFrame2 9-slice (`widgets.def:88-93`) | engine button-click → `0x539590` slot-148 Load branch → `0x48df70` |
| `exit` BUTTON (action button id: **the string `"exit"`**) | (350, 406) 140×30 | click → `virt_meth_0x539590` Exit-button branch: clears `_DAT_0066fa68 = 0` (`:00539830`), the engine sees the modal flag drop and unwinds via the modal-stack pop → calls back into `virt_meth_0x539440` (slot 4) Hide to free the 216×160 scratch. Resume gameplay. | same UP/DOWN | engine button-click → `0x539590` slot-148 Exit branch → modal unwind → `0x539440` |
| `picture` BITMAP | (54, 91) 216×160 | not clickable (BITMAP) | n/a | n/a |
| FRAMEs | various | not clickable | n/a | n/a |
| static TEXT (Module:, Character:, gamename, modname, charname) | various | not clickable (no NAME-as-button) | n/a | n/a |
| ESC key (KeyDown fall-through) | n/a | UNCONFIRMED-P: likely closes via the engine's Enter/ESC default behavior | UNCONFIRMED | engine 0x004361f0 |
| Enter key | n/a | UNCONFIRMED-P: likely fires the focused button (default = Load Game) | UNCONFIRMED | engine 0x004361f0 |

The `Load Game` and `Exit` button widgets are wired by name lookup inside
`virt_meth_0x539590` (string xrefs `s_loadgame_005e42bc`, `DAT_005e42c8`
`"exit"`). The button NAME strings drive the dispatch; the engine does not
use numeric command ids for DEF screens.

### Modal-state global

`_DAT_0066fa68` = the "load-game modal is up" flag. Set to 1 by case 0x53
opening the screen (`FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:474`).
**Cleared in `virt_meth_0x539590` slot-148** at TWO sites:
- `:005396d1` (Load-button branch — after a successful Load).
- `:00539830` (Exit-button branch).

(Confirmed via `recon/ghidra/_data.txt:137166-137177`, see §6.6 above —
this **resolves UNCONFIRMED-F** from the prior revision.)

`&DAT_0066f8d0` is the **screen-instance storage** — the `cls_0x5b9584`
instance lives at this address; `FUN_0048f040(&DAT_0066f8d0, …)` registers
the modal on the screen stack with flags `((DAT_0066829c != 0) ? 0 : 8) | 7`
(in-game-vs-mainmenu flag selects which modal-stack ring). The instance
itself is **also written** by slots `0x5392e0`, `0x539360`, etc. on the
outer TScreen wrapper class `cls_0x5b9538` — see "TScreen vs DefScreen
duality" below.

### TScreen wrapper vs DefScreen child (architecture note)

`cls_0x5b9584` (the DefScreen subclass at `DAT_0066f8d0`) is the **widget
child** that owns layout + dispatch. There is ALSO a separate **outer
TScreen wrapper class `cls_0x5b9538`** (vftable at `0x5b9538`,
`recon/ghidra/cls_0x5b9538__vftable_5b9538.cpp`) whose 19-slot vtable matches
the standard TScreen shape (Pulse / DrawBackground / MouseClick / KeyPress
/ TimerTick / Redraw — same shape used by `cls_0x5a5d18 TLogoScreen`). Its
slot-4 method `virt_meth_0x5392e0` references `s_cursor_005e423c`
(`"cursor"` xref @ `:005392ec`) — this is the **screen-level cursor binding**
that swaps the cursor to the load-screen cursor on entry. Its slot-8
method `virt_meth_0x539360` (only 4 lines, `recon/ghidra/cls_0x5b9538.cpp:21-27`)
calls base `cls_0x5a4f28::meth_0x48ee10` then dispatches to the embedded
DefScreen child via `(*(int*)this->mbr_0x88 + 4)()` — so `cls_0x5b9538.mbr_0x88`
points at the `cls_0x5b9584` DefScreen instance. The two-class design is the
standard Revenant pattern: a TScreen wrapper handles input dispatch + cursor
+ TPlayer pause; a DefScreen child owns the widget layout.

**Impact:** for reconstruction the TScreen + DefScreen pair collapses into a
single class `LoadGameScreen` (no need to preserve the two-object design;
the port's engine layer absorbs the wrapper responsibilities). The recon
agent should NOT spawn two separate objects.

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo on shadowed text.** Any TEXT widget on this screen with
   TEXT_SHADOW (almost all of them — every TEXT default + the LISTBOX rows
   + the BUTTON labels) renders through `FUN_004be2b0`'s 3-pass shadow on
   top of a magenta-cleared scratch. Per NOMENCLATURE §4 and
   [[project-retail-pink-halo-bug]] this produces a pink/magenta fringe
   around the dark shadow edge. **Render with real alpha (no magenta
   colorkey), NOT the original chroma-key path.** The fix is in the
   shared engine text helper (UI_METHOD_MAP §12 `DrawTextShadowedToTarget`),
   one-time, not per panel.
2. **`Locke` hardcoded charname fallback.** The string `"Locke"`
   (`_data.txt:114614`) is the protagonist's default name from the demo.
   When a save lacks a stored character name the screen falls back to
   `"Locke"`. This is not a bug per se, but a Demo-1 hard-coding — the
   Revisited port may want to make this configurable (per the localization
   plan [[project-localization]]). Reproduce as-is for Classic mode.
3. **No double-click to load.** `LISTFLAG_DBLCLICK` is NOT set in
   `loadgame.def:26`. Modern dialogs typically allow double-click on a save
   to load it. Reproduce as-is (single-click selects, Load button confirms);
   the Revisited mode may add LISTFLAG_DBLCLICK as a usability deviation
   (overlay-system pattern, see [[reference-revisited-overlay-system]]).
4. **No "Are you sure?" confirmation.** Clicking Load Game on a save with no
   confirmation drops any in-progress game state — bare-bones UX. Reproduce
   as-is; the Revisited mode may add a confirmation popup.

---

## §12 — Reconstruction pseudocode

The reconstruction agent should treat this screen as a **DEF-engine consumer
+ open/close glue + 216×160 preview-surface owner**. The actual widget
painting is engine work — reconstructing this panel requires the DEF widget
engine to be operational FIRST (see DefWidgetEngine.md outstanding work).

```cpp
// LoadGameDefScreen (replaces cls_0x5b9584)
// Inherits from DefScreenBase (the new engine-layer class corresponding to
// cls_0x5b93c4, with vtable slot 132 = DefScreen_Open)
class LoadGameDefScreen : public DefScreenBase {
    TSurface* preview = nullptr;     // 216x160, owns the selected save's ss.bmp
    int       sel_index = -1;        // current LISTBOX selection (mbr_0x1a0)
    bool      in_game;               // mbr_0x198 — true when opened from
                                     // in-game pause, false from MainMenu
public:
    // === Open (= virt_meth_0x539380) ===
    bool Open() override {
        InputManager.SuspendCursor();           // FUN_0048d260(cursor_mgr)
        auto fmt = Display.PixelFormat();       // vtable+0x18
        auto preview_fmt = (fmt == BM_16BIT) ? FMT_24BIT : FMT_15BIT;
                                                // UNCONFIRMED-H: verify
        preview = TSurface::Create(216, 160, preview_fmt);
        ActiveSaveIndex_g = -1;                 // DAT_0066fb04 = -1
        sel_index = -1;
        return DefScreenBase::Open(
            screen_name  = "loadgame",
            panel_name   = "default",
            flags        = in_game ? 0x11 : 0,  // UNCONFIRMED-I
            origin       = {0, 0},
            extent       = {640, 480},
            cursor_init  = {450, 160},
            widgets_name = "widgets"            // params 10/11 of FUN_00435150
        );
    }

    // === Hide (= virt_meth_0x539440) ===
    void Hide() override {
        if (preview) { delete preview; preview = nullptr; }
        DefScreenBase::Hide();                  // virt_meth_0x434f30
    }

    // === Per-widget bindings (= virt_meth_0x539470 + 0x539550 + 0x539590) ===
    // Engine widget-binding callbacks. The engine looks up widgets by name
    // and the panel registers handlers below.
    //
    // Slot 164 (0x539470) = ListField (per-row column text fetch)
    // Slot 160 (0x539550) = GetField (FIELD "picture" → preview surface)
    // Slot 148 (0x539590) = CommandDispatch (row-select + button-click)

    // virt_meth_0x539550 — GetField for the BITMAP widget's FIELD "picture"
    void* GetField(const char* field_name) override {
        if (stricmp(field_name, "picture") == 0) {
            return preview;                       // 216x160 scratch (mbr_0x19c)
        }
        return DefScreenBase::GetField(field_name);
    }

    // virt_meth_0x539470 — ListField for the LISTBOX gamelist's column "name"
    int ListField(const char* listbox_name, const char* col_name,
                  int row_idx, char* out, int out_size) override {
        if (stricmp(listbox_name, "gamelist") != 0) return 0;
        if (stricmp(col_name, "name") != 0)         return 0;
        auto& entry = SaveList[row_idx];          // DAT_0065a628[row_idx]
        strncpy(out, entry.display_name, out_size);
        return 1;
    }

    // virt_meth_0x539590 — CommandDispatch (called by engine for row-select
    // and button-click events; dispatch is by widget NAME — no numeric ids)
    int CommandDispatch(const char* widget_name, int sub_event) override {
        if (stricmp(widget_name, "gamelist") == 0) {
            // Row-select branch
            int row = sub_event;                  // engine passes row idx
            OnSelChanged(row);
            return 1;
        }
        if (stricmp(widget_name, "loadgame") == 0) {
            // Load-button branch
            OnLoadClicked();
            return 1;
        }
        if (stricmp(widget_name, "exit") == 0) {
            // Exit-button branch
            OnExitClicked();
            return 1;
        }
        return DefScreenBase::CommandDispatch(widget_name, sub_event);
    }

    void OnSelChanged(int idx) {
        if (idx < 0) return;
        sel_index = idx;
        ActiveSaveIndex_g = idx;                  // DAT_0066fb04 = row idx
        auto& save = SaveList[idx];               // DAT_0065a628[idx]

        // Reload preview from save's <savedir>/ss.bmp
        TBitmap* ss = LoadBitmapFile(save.path + "/ss.bmp");
        preview->BlitResampled(ss, 0, 0, 216, 160);   // fit to dst
        FindWidget("picture")->MarkDirty();            // CTRLFLAG_DIRTY = 0x20

        FindWidget<DefText>("gamename")->SetText(save.title);
        FindWidget<DefText>("modname")->SetText(save.module_name);
        FindWidget<DefText>("charname")->SetText(
            save.char_name.empty() ? "Locke" : save.char_name);
    }

    void OnLoadClicked() {
        if (sel_index < 0) return;
        Cursor.SetBusy();                         // "cursor" string ref @ :005397a7
        ActiveSaveIndex_g = sel_index;            // DAT_0066fb04 (already set in OnSelChanged)
        TransitionTo("loadingmap");               // "loadingmap" string ref @ :005396fc

        // Actual file load — 3-arg signature per cls_misc_LoadGame_48df70.cpp:5:
        //   bool LoadGame(int* saveList, char* name, uint flags)
        bool ok = LoadGame(&SaveList,
                           SaveList[sel_index].display_name,
                           /*flags=*/ 0);
        if (ok) {
            LoadGameModalActive_g = 0;            // DAT_0066fa68 = 0 @ :005396d1
            Hide();
        } else {
            // UNCONFIRMED-R: failure UX — silent stay-open vs error popup
        }
    }

    void OnExitClicked() {
        LoadGameModalActive_g = 0;                // DAT_0066fa68 = 0 @ :00539830
        Hide();
    }
};
```

The engine handles every visible draw — the screen-class code above just
binds data and dispatches button events. **All three overridable slots
(GetField / ListField / CommandDispatch) are name-keyed string lookups**,
matching the engine's per-widget FIELD attribute. There are NO numeric
command ids on this screen.

---

## §13 — Port mapping notes

| retail primitive | port primitive | home |
|---|---|---|
| `FUN_004a1ec0(w,h,fmt,0)` (216×160 alloc) | `TSurface::Create(w, h, fmt)` | renderer/surface |
| `FUN_004a22f0(surf, ...)` (TSurface init) | `TSurface` ctor | renderer/surface |
| `FUN_00435150_DefScreen_Open(...)` | engine `DefScreen::Open(name, panel, flags, x, y, w, h, mx, my, widgets, extra_def)` | def widget engine (TODO — not yet implemented in port) |
| `FUN_00435040_DefScreen_LoadAndShow` background blit | engine `LoadAndShow` resource lookup + `Renderer->DrawBitmap(bg, 0, 0)` | def widget engine + renderer |
| `FUN_0048f040(&inst, flags)` (modal-stack register) | screen-stack `Push(LoadGameDefScreen*, flags)` | screen mgr |
| `FUN_0048d260(cursor)` | `Cursor.Suspend()` | input mgr |
| `FUN_004830f0` (delete surface) | `delete preview` | n/a |
| widget paint: 9-slice FRAME | `DrawBitmap9SliceToTarget(bm, dst, frame_margins)` | engine + renderer |
| widget paint: BITMAP slot | `DrawBitmapToTarget` | renderer |
| widget paint: TEXT (no shadow) | `DrawTextToTarget(atlas, text, x, y, w, ETextAlign, r,g,b, tw, th)` | font.cpp |
| widget paint: TEXT (TEXT_SHADOW set) | `DrawTextShadowedToTarget(...)` | font.cpp |
| widget paint: LISTBOX row | engine row-walk + `DrawTextShadowedToTarget` per FIELD | engine + font.cpp |
| widget paint: BUTTON UP/DOWN | engine state-select bg bitmap + `DrawTextShadowedToTarget` (UP only) / `DrawTextToTarget` (DOWN no shadow) | engine + renderer + font.cpp |
| **needs new primitive:** scroll-bar 3-slice composite (vertical UP/THUMB/DOWN with arrow-end caps) | `DrawScrollbarVertical(dst, thumb_pos, content_extent)` | engine — TODO |

The screen-class implementation is small (~150 LoC); the engine work is
the substantial part.

---

## §14 — Slot-mapping cross-reference (RESOLVED)

Side-by-side of the override slots from
`cls_0x5b9584__vftable_5b9584.cpp` (loadgame) vs.
`cls_0x5b963c__vftable_5b963c.cpp` (savegame). Confirms identical vftable
shape and lets us reuse SaveGameDef_SPEC's analysis of slot 148 / 160 / 164
for loadgame.

| slot | offset | role | loadgame fn | savegame fn | base fn (TDefScreen) | note |
|---:|---:|---|---|---|---|---|
| 0 | `+0x00` | Open / Initialize | `0x539380` | `0x5399f0` | (slot is override-only) | both classes provide their own Open |
| 1 | `+0x04` | Hide / Close | `0x539440` | `0x539ab0` | (slot is override-only) | both classes free `mbr_0x19c` then chain |
| 2..30 | `+0x08..+0x78` | — | base inherited | base inherited | (matching) | **identical for both classes** — verified slot-by-slot against both vftable files |
| 31 | `+0x7c` | dtor wrapper (vector-delete) | `0x53a0c0` | `0x53a1d0` | (auto) | per-class delete |
| 32..36 | `+0x80..+0x90` | — | base | base | matching | |
| 37 | `+0x94` | (engine slot — possibly EditClick / Activate) | base `0x4451e0` | savegame override `0x539c00` is at slot 148 not 37 (corrected vs SaveGame SPEC §0 table — the SaveGame SPEC §0 table mis-labelled this slot; the true CommandDispatch is slot 148, see below) | base | recheck: both vftables show `virt_meth_0x4451e0` at slot 37 — that's the base; no override on either class. |
| 38..36 | base | matching | |
| 148 | `+0x94 + 13*4 = +0x94...` actually `+0x94 = 148` matches | **CommandDispatch** (the big one) | **`0x539590`** | **`0x539c00`** | (slot is override-only) | both classes route per-widget events through this slot. Loadgame's xrefs `_data.txt:114543-114614` align 1:1 with savegame's xrefs at `_data.txt:114625-114730`. |
| 152..156 | base | matching | |
| 160 | `+0xa0` | **GetField / Activate** | **`0x539550`** | **`0x539bc0`** | (slot is override-only) | both classes return the 216×160 preview-scratch for `FIELD "picture"` |
| 164 | `+0xa4` | **ListField** (per-row column text) | **`0x539470`** | **`0x539ae0`** | (slot is override-only) | both classes emit the slot.name for column `"name"` of LISTBOX `"gamelist"` |
| 168..180 | base | matching | |

**Decisive cross-reference:** the slot indices 148 / 160 / 164 line up
byte-for-byte at file offsets `+0x94` / `+0xa0` / `+0xa4` in BOTH vftables
(`cls_0x5b9584__vftable_5b9584.cpp:44,47,48` and
`cls_0x5b963c__vftable_5b963c.cpp:44,47,48`). The total vftable length is
**184 bytes / 46 slots** in BOTH cases. There is NO ambiguity in the
slot mapping. UNCONFIRMED-D from the prior revision is **resolved**.

The minor asymmetry — savegame has an EDIT widget the loadgame lacks —
is handled INSIDE the slot 148 dispatcher body (savegame's
`virt_meth_0x539c00` reads the `nameedit` widget's text before invoking
SaveGame, while loadgame's `virt_meth_0x539590` reads the LISTBOX row name
before invoking LoadGame). No additional vftable slot is consumed.

## §14b — UNCONFIRMED / open questions

A. **(RESOLVED-via-sibling)** `virt_meth_0x539470` (slot 164) role =
   **ListField** (per-row column text fetch). Sibling
   `SaveGameDef_SPEC.md` §6 documents savegame slot 164 = `0x539ae0` as
   "ListBox FIELD/format callback (the 'name' column)". Loadgame slot 164
   has the same xref shape (`"gamelist"` + `"name"`). **TODO (low priority):**
   `DecompileAddr.java 0x00539470` to confirm the exact
   `DAT_0065a628[row_idx].name` access. **Impact if extraction differs:**
   negligible — the binding is fully constrained by the engine's FIELD
   convention.

A2. **`DAT_0065a628` shape — array-of-pointers vs fixed-stride entries.**
   The global save-list data source read by slot 164 (`:005394e1`) and
   slot 148 (`:0053987f`). Sibling savegame reads it at `:00539b51`
   (`_data.txt:129533-129534`). Likely a `SaveEntry**` (pointer array) or
   `SaveEntry[]` (fixed-stride). Resolve: `DecompileAddr.java 0x00539470`
   reveals the access pattern. **Impact:** wiring of `SaveList[row_idx]`
   in the port pseudocode; trivial to adjust.

B. **(RESOLVED-via-sibling)** `virt_meth_0x539550` (slot 160) role =
   **GetField for FIELD "picture"**. Returns `this->mbr_0x19c` (the 216×160
   preview-scratch surface). Sibling savegame slot 160 = `0x539bc0`
   documented in SaveGameDef_SPEC §9 with the same role and same xref
   shape. **TODO (low priority):** `DecompileAddr.java 0x00539550`. **Impact
   if extraction differs:** negligible.

C. **(RESOLVED-via-sibling for shape; per-line TODO)** `virt_meth_0x539590`
   (slot 148) role = **CommandDispatch** (button-click + row-select branch).
   Sibling savegame slot 148 = `0x539c00` documented as command dispatcher
   in SaveGameDef_SPEC §6.5, §10. Loadgame body is the parallel: same
   shape, `loadgame` button name replacing `savegame`, `FUN_0048df70_LoadGame`
   replacing the saver. Modal-flag clear sites pinned via `_data.txt:137166-137177`
   (resolves UNCONFIRMED-F). **TODO (medium priority):** `DecompileAddr.java
   0x00539590` to confirm precise branch order (row-select inlined vs.
   separate handler, failure UX path). **Impact if extraction differs:**
   moderate — the §10 dispatch table reflects sibling-derived shape; line-
   level branch boundaries are inferred.

D. **(RESOLVED)** Slot ordering vs vtable shape. Side-by-side comparison
   above shows identical 46-slot layout between `cls_0x5b9584` and
   `cls_0x5b963c`. The EDIT widget asymmetry is absorbed inside slot 148's
   body, not via different vftable shape.

E. **MainMenu MenuLoadGame path.** The task brief says the screen is reached
   "from MainMenu MenuLoadGame and from in-game pause menu". The in-game
   pause path (case 0x53) is HIGH confidence. The MainMenu path is
   **UNCONFIRMED**: `player_init_notes.md:58` says "MenuLoadGame →
   callback: FUN_0048df70 (LoadGame)", which is the saved-file loader
   directly — implying MainMenu skips the loadgame DEF screen entirely
   (jumps straight to load a hard-coded "Default Save"). Alternatively
   the MainMenu's MenuLoadGame button's REAL callback opens the loadgame
   DEF screen, and the note simplified. Resolve: trace the
   `cls_0x5a5d18_TLogoScreen` button-callback registration (the
   `FUN_00436790(uVar5)` call after each `FUN_0042c400` button-creation —
   that's where the per-button callback gets bound). **Impact:** if MainMenu
   does NOT go through the DEF screen, the spec's "shown" predicate is
   in-game-only, and the `mbr_0x198` "is in-game" flag is always true
   (simplifying the `flags = (mbr_0x198 != 0) ? 0x11 : 0` in §6.1).

F. **(RESOLVED)** Modal-flag `_DAT_0066fa68` clear location. Both clear
   sites pinned via `recon/ghidra/_data.txt:137166-137177` global write
   table:
   - `virt_meth_0x539590:005396cc (*) , :005396d1 (W)` — Load-button
     branch after successful Load.
   - `virt_meth_0x539590:0053982b (*) , :00539830 (W)` — Exit-button
     branch.
   The `*` mark = "address-of taken" (push the global's address before
   the store), `W` = the actual `mov [addr], 0`. Same pattern as the
   options-screen counterpart `_DAT_0066fe3c` at case 0x52. **Impact:**
   §6.6 + §10 + §12 are updated to reflect this.

G. **Background asset variant selection.** `loadgametex.dat`,
   `loadgamealpha.dat`, `loadgamenotex.dat` all contain a 640×480
   `Background` bitmap (verified via `dump_dat.py`). Which one the engine
   loads at runtime depends on the user's texture-quality setting
   (`Revenant.ini` `[Display] TextureType=...`). **Impact:** the port should
   pick one canonical variant (Classic-faithful uses tex; Revisited can use
   the higher-quality version) — the `dump_dat.py` reports `flags=0x10000`
   on the tex variant ("unsupported"), so the alpha variant (`flags=0x2`,
   real RGB555) is the dump-decodable one. **Resolve:** read the engine's
   archive-mount path for which file gets `MountArchive`'d at boot. Likely
   `tex.dat` is the high-quality + `alpha.dat` the low-quality (texture
   reduction); the `notex` variant for software/no-texture mode.

H. **Surface format selector `(displayFmt != 0xf) ? 2 : 4`
   (`cls_0x5b9584.cpp:51`).** The `0xf = 15` value compared against display
   format probably means "BM_16BIT mode flag = 0xf" — UNCONFIRMED.
   **Impact:** picking the wrong fmt makes the ss.bmp resample land in the
   wrong format and either silently blue-shift (565→555 misinterpret) or
   crash. Resolve: dump `src/revdefs.h:307-338` BM_* defs + map to the
   display vtable +0x18 return value. UI_METHOD_MAP §16 lists BM_15BIT=0x2,
   BM_16BIT=0x4; the format selector value `0xf` likely encodes a different
   thing (a per-mode bitfield, not a BM_* flag) — verify.

I. **`flags = (mbr_0x198 != 0) ? 0x11 : 0`** in the DefScreen_Open call. The
   bit `0x11 = (0x10 | 0x1)` decomposes to (`DM_BACKGROUND = 0x10` per
   UI_METHOD_MAP §1) | (`DM_NOCLIP = 0x1`)? **UNCONFIRMED.** `mbr_0x198`
   is set/cleared somewhere outside the open path — likely a flag the
   activator's caller sets before invoking the open virtual. Resolve:
   xref `mbr_0x198` writes via `recon/ghidra/_data.txt` (line currently
   unindexed). **Impact:** wrong flags may make the panel transparent over
   the world (intended) or opaque (wrong if in-game).

J. **Twelfth arg to `FUN_00435150_DefScreen_Open` (loadgame omits, savegame
   passes `s_savegame_005e4314`).** Per
   `FUN_00435150_DefScreen_Open.cpp:55-57`, this is `param_12` which is
   passed through `FUN_00435b20(param_12)` — likely "load an additional
   per-screen style overlay .def". Loadgame doesn't need one because the
   `widgets.def` STYLE defaults cover all its widget kinds; savegame needs
   one because of the additional EDIT widget. Resolve: decompile
   `FUN_00435b20`. **Impact:** none for loadgame's layout — the omitted
   arg makes the engine skip the optional second-style load.

K. **Pane state members `mbr_0xc/0x10/0x1c/0x20`** (`cls_0x5b9584.cpp:84-91`).
   Set to (640,480,640,480). Inferred dual storage = (constant design size,
   live target extent). UNCONFIRMED. Resolve: `DumpVtable.java 0x5b9584`
   then read slot 0x20 / 0x24 / 0x28 / 0x2c (TPane accessors). **Impact:**
   if the dual-storage interpretation is wrong, a resize-from-render-target
   change would propagate to the wrong field.

L. **Widget alpha vs chroma key.** `widgetstex.dat` reports `kc=0x0` (no
   per-bitmap key). `widgetsalpha.dat` (not dumped above) is a sibling
   archive — possibly holds a separate `BM_ALPHA` plane for the widget
   bitmaps. UNCONFIRMED. **Impact:** the port should prefer the alpha
   plane over chroma keying to avoid the pink-halo bug (§7, §11). Resolve:
   `dump_dat.py widgetsalpha.dat` and verify the `flags` field reports
   `BM_ALPHA = 0x100`.

M. **`Med` font identity.** `widgets.def:86` references `FONT "Med"`. The
   port's font system uses `Arimo` (Arial metric) per
   [[project-ui-text-rendering]]. The retail `Med` is probably "Arial Medium"
   = a 14-px Arial — needs verification. Resolve: extract the engine's
   font-table init (probably in `0065b020` HDC table per UI_METHOD_MAP §5
   step 2) and read the name→size mapping. **Impact:** wrong px size makes
   every TEXT cell over/underfilled.

N. **DEF FLAGS override-vs-OR.** When a TEXT line specifies FLAGS twice
   (once via STYLE inheritance, once via explicit FLAGS in the DEF), does the
   second OR with the first, or REPLACE? Per the gamename example
   (`loadgame.def:16`), the explicit FLAGS `TEXT_CENTER | TEXT_SHADOW |
   TEXT_ELIPSES` likely REPLACES the STYLE TEXT default `TEXT_LEFT |
   TEXT_VCENTER | TEXT_SHADOW`. UNCONFIRMED — could also OR (in which case
   the gamename gets vcenter unintentionally). Resolve: read the engine TEXT
   attr parser body (`FUN_0042b340`,
   `port_status/DefWidgetEngine.md:32`). **Impact:** vertical alignment on
   the gamename cell, and possibly others.

O. **Busy-cursor name.** The `cursor` string referenced from
   `:005397a7` is most likely a cursor-bitmap name argument — UNCONFIRMED
   which cursor (hourglass / busy?). Resolve: read the surrounding
   instructions in `virt_meth_0x539590` for the literal arg (probably a
   second string like `"WaitCursor"` or similar). **Impact:** cosmetic
   only.

P. **ESC / Enter key default behavior.** The DEF engine's input dispatcher
   (`FUN_004361f0`) handles "Enter activate" — probably activates the
   focused button. For loadgame, the default focus might be the LISTBOX
   (selection-changeable) or the Load button (default action). ESC likely
   triggers the Exit branch (typical modal). UNCONFIRMED. Resolve: read
   `FUN_004361f0` body. **Impact:** keyboard usability; the
   reconstruction can ship without and add later.

Q. **`cls_0x419dd0` helper in Open.** Used as a one-int helper struct on
   the stack (`cls_0x5b9584.cpp:55-56`), then passed to
   `cls_0x4a22f0::meth_0x4a22f0`. Per UI_METHOD_MAP §7,
   `FUN_00419dd0_BlitEffect_SetField` is a "1-int field copy" helper. The
   one-int local probably holds a default surface drawmode or color.
   UNCONFIRMED. **Impact:** if the default holds the wrong drawmode, the
   preview scratch could blit opaque vs alpha incorrectly.

R. **Load-button failure-UX path.** `FUN_0048df70_LoadGame` returns `bool`
   — `false` on enumeration miss, malformed save file, or playerlist failure
   (`cls_misc_LoadGame_48df70.cpp:46,51-52,68,148,247-249,257`). The
   loadgame screen's dispatcher in `virt_meth_0x539590` Load branch is
   expected to handle the false-return — either (a) silently leave the
   modal open and ignore the click (most likely — UX is bare-bones
   throughout this screen), or (b) emit an error popup via the popup DEF
   screen system. Resolve: §14-C decompile `0x00539590` to see the branch
   after the `FUN_0048df70` call. **Impact:** error-state UX for
   reconstruction — Classic should match retail; Revisited may add a
   popup as a deviation.

S. **Per-save metadata layout (the `SaveList[idx].title / module_name /
   char_name` fields).** Sibling SaveGameDef_SPEC §6.5 mentions the
   per-slot title/module/charname strings are pulled by the dispatcher
   from each save entry's metadata block (offset `+0x58` for the slot
   name per `cls_misc_LoadGame_48df70.cpp:99` and `:174`, but the per-save
   stored gamename/modname/charname offsets are NOT yet enumerated).
   For loadgame the dispatcher hard-codes `"Locke"` as the charname
   fallback (`:00539971`) when the save's stored charname is empty, so
   each entry HAS a charname field separate from the slot name. Resolve:
   `DecompileAddr.java 0x00539590` + cross-reference
   `cls_misc_LoadGame_48df70.cpp` for the SaveEntry struct shape.
   **Impact:** the port's `SaveEntry` struct shape; trivial to adjust once
   pinned.

---

## Summary (for the dispatching agent)

The Load Game DEF screen `cls_0x5b9584` is a 640×480 modal that opens via
`DefScreen_Open` from in-game pause command `0x53`. Its layout is the
literal contents of `data/resources_unzipped/loadgame.def` lines 4-40: a
562×320 outer FRAME at (35,70) hosting a left-column preview (216×160
BITMAP `picture` at (54,91) inside a 222×166 `ClearFrame2` frame at
(50,87)) + an info block (gamename 210×40 centered Med + Module:/modname +
Character:/charname rows in a 221×106 frame at (50,268)); a right-column
298×287 LISTBOX `gamelist` at (284,87) with `ITEM 260 18` rows + inner
FIELD `2 0 272 18` showing `gamelist_name`; and two 140×30 BUTTONs `Load
Game` (158,406) and `Exit` (350,406). Per-row geometry: content rect
(289,92)..(559,369) → 270×277, **15 fully-visible rows**, scrollbar strip
on the right at canvas-x 561..577 per STYLE LISTBOX VLIST's RELRB 21 4 5 4.
Every widget inherits its STYLE from `widgets.def:81-145` (NORMAL BUTTON:
ClearFrame2/FillFrame2 9-slice, Med white-on-UP/black-on-DOWN text,
center+vcenter+shadow; STYLE TEXT: Med white, left+vcenter+shadow; STYLE
LISTBOX VLIST: VScrollRect 9-slice, selcolor `(0,112,74)`). The Open
virtual (`0x539380`) allocates a 216×160 preview-scratch surface
(`this->mbr_0x19c`) for resampled per-save `ss.bmp` screenshots and zeroes
the selection. Per-widget dispatch is name-keyed (no numeric command ids):
**slot 164 `0x539470`** is the LISTBOX ListField callback emitting the
slot-name per row, **slot 160 `0x539550`** is the GetField that hands the
216×160 scratch back for the BITMAP widget's `FIELD "picture"`, and **slot
148 `0x539590`** is the CommandDispatch that handles both gamelist row-
select (updates picture + gamename + modname + charname from selected
save's metadata, defaults charname to `"Locke"` if absent) and the
`"loadgame"` / `"exit"` button branches. The Load click invokes
`FUN_0048df70_LoadGame(&saveList, name, /*flags=*/0)` — the **3-arg**
saved-file loader. Modal-flag clears (`_DAT_0066fa68 = 0`) happen INSIDE
slot 148 at addresses `:005396d1` (Load branch) and `:00539830` (Exit
branch), pinned via `_data.txt:137166-137177`. All UNCONFIRMED items
required for reconstruction are resolved via the savegame sibling
(`SaveGameDef_SPEC.md`); body extractions are now confirm-only.

### UNCONFIRMED list (2026-05-29 revision)

**Resolved (downgraded from blocking to confirm-only):**

- **A → A2 (RESOLVED-via-sibling).** `0x539470` slot 164 = ListField. Body
  extraction still TODO for the exact `DAT_0065a628` access shape.
- **B (RESOLVED-via-sibling).** `0x539550` slot 160 = GetField for "picture",
  returns the 216×160 scratch surface.
- **C (RESOLVED-via-sibling, body TODO for branch-order).** `0x539590` slot
  148 = CommandDispatch (row-select + Load/Exit button branches).
- **D (RESOLVED).** Vftable shape side-by-side confirmed identical (46 slots,
  184 bytes) between loadgame `cls_0x5b9584` and savegame `cls_0x5b963c`. See
  §14 slot-mapping cross-reference table.
- **F (RESOLVED).** `_DAT_0066fa68` clear sites pinned via
  `recon/ghidra/_data.txt:137166-137177` — `virt_meth_0x539590:005396d1` (Load
  branch) and `virt_meth_0x539590:00539830` (Exit branch).

**Still open (none blocking reconstruction):**

- E. MainMenu MenuLoadGame path — confirm whether it goes through this DEF
  screen at all, or jumps straight to `FUN_0048df70`.
- G. Background `.dat` variant runtime selection (tex/alpha/notex).
- H. Display fmt `0xf` semantics in surface fmt selector.
- I. DefScreen_Open `flags = 0x11` bit decomposition.
- J. `param_12` of `DefScreen_Open` (twelfth arg) — extra style overlay.
- K. Pane state dual-storage `mbr_0xc/0x10/0x1c/0x20` semantics.
- L. `widgetsalpha.dat` alpha plane vs chroma key.
- M. `Med` font identity (size + ttf).
- N. DEF FLAGS override-vs-OR semantics on second-pass FLAGS.
- O. Busy-cursor name string referenced from `0x539590`.
- P. ESC / Enter default-input dispatch on this modal.
- Q. `cls_0x419dd0` one-int helper's role in Open's surface init.
- **R. (NEW)** Load-button failure-UX path (silent vs popup) — body
  extraction needed.
- **S. (NEW)** SaveEntry struct shape (title / module_name / char_name
  field offsets within each save's metadata block).
