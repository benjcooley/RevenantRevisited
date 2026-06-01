# InventorySidebarPane — Reconstruction SPEC

Pixel-accurate spec for the **Inventory** sidebar content panel: the item grid
shown in the lower slot of the right sidebar when the **Inv** tab is selected.

Retail class: `cls_0x5a58c0` (role label `InventorySidebarPane`; candidate src/
class `TInventory`). Global instance `0x65d4f8`. All coordinates are in 640×480
**Classic** space; cite format is `file:line` + `0xHEX = DEC`.

---

## §0 — Sources & status

**Status:** `forensics-complete` for the static layout, asset roster, grid math,
icon/count draw, scroll arrows, and drag/drop dispatch. A few item-object
vtable getters (`+0x108`, `+0x130`, `+0x198`) and two helper bodies are MEDIUM
confidence (see §14). No engine code written.

**Recon files read (authoritative):**
- `recon/ghidra/cls_0x5a58c0.cpp` — class struct + 9 methods (Initialize, Close,
  Show/DrawBackground, MouseClick, MouseMove, dtors).
- `recon/ghidra/cls_0x5a58c0__vftable_5a58c0.cpp` — 46-slot vtable.
- `recon/discovered/cls_0x5a58c0_InventorySidebarPane_ctor_487f50.cpp` — pane
  geometry.
- **Extracted this wave** (Ghidra `DecompileAddr`/`Disasm`, saved to
  `recon/discovered/`):
  - `cls_0x5a58c0_InventorySidebarPane_DrawGrid_537a70.cpp` (vtable slot 84 — the
    grid paint; Ghidra dropped the item loop as "unreachable", so the loop is
    decoded from the raw disassembly `…_DrawGrid_537a70.disasm.txt`).
  - `cls_0x5a58c0_InventorySidebarPane_Animate_537f90.cpp` (slot 88 — hover).
  - `FUN_00438df0_SurfaceBlitWrapper.cpp`, `FUN_004be110_TextDrawWrapper.cpp`,
    `FUN_0046dfb0_InventoryIterStep.cpp`.
- String-literal dumps at `0x5e41a0`, `0x5e4160` (format strings + asset names).

**Prior briefs consulted:**
- `docs/ui/briefs/B_r11_sidebar_tab_cascade.md` — sidebar roster; this is
  **lower-region mode 0** (`DAT_0065d1bc==0`); dispatcher cases 9 & 0xb;
  vtable shape (46-slot TButtonPane-w-DEF).
- `docs/ui/briefs/B_r12_oog_closeout.md` — TButtonPane-w-DEF intermediate
  (`cls_0x5a45c8` / `cls_0x5b98b8`) context for inherited slots.

**Shared references:** `FORENSICS_PROTOCOL.md`, `NOMENCLATURE.md`,
`UI_METHOD_MAP.md`, `AGENT_PROTOCOL.md`.

**Errors to close:** none — no port shell exists for this pane yet.

### PORT NOTE — shared `TInvSlot` class

Per-cell paint + interaction now route through `src/invslot.{h,cpp}` —
the single sub-control all inventory-style panes share:
- TBarInv (bottom-bar 9-slot shelf)
- TInventory (sidebar 4×3 grid)
- TEquipPane (paperdoll 11 EQ_* slots — plug-in next pass)

Constructor `TInvSlot(x,y,w,h, allowed_type, placeholder, style)` — each
pane builds one `SInvSlotStyle` and constructs N slots with per-instance
rects + `allowed_type` filter (EquipPane uses `EQ_HEAD` / `EQ_BODY` /
etc. to restrict drop targets; Inventory/BarInv use 0 = accept any).
Interaction dispatch (MouseDown/MouseUp/UseOrEquip mirroring retail's
`eventType` 1/4/5) is declared but stubbed — the cross-pane drag-state
owner port is the next pass.

The cross-pane drag-state OWNER in retail is `cls_0x5a5320_TPlayScreen`
via `virt_meth_0x44f140`; globals are `DAT_0065b878` (currently-dragged
EQ-slot, 14 read XREFs across InvSidebar / TBarInvPane / others),
`DAT_00668518` (mouse-button-down flag, Win32 WndProc-maintained),
`DAT_00668510`/`14` (mouse x/y), `DAT_0066829c` (swap-allowed flag),
`DAT_00667fcc` (Player global). Per-pane drag state lives in member
fields `mbr_0x180..0x194` (grabslot/startslot/page/dragflag/clickX/Y).
The snapshot has NO shared item-cell class — `TInventory : TPane` and
`TEquipPane : TButtonPane` each duplicate `grabslot`/`heldslot`/
`OnSlot`/`DrawAnim` inline; THAT inline-state duplication is what
justifies one shared `TInvSlot` abstraction in the port. The snapshot's
`TTalismanButton : TButton` (`src/spellpane.h:23`) is the per-cell
sub-control precedent the new class follows.

### LIVE-VERIFIED CORRECTIONS (post-reconstruction)

These supersede the recon readings below. Discovered while building
`--test=ui-inventory` (src/uiinventorytest.cpp) and visually verifying against
the original game. Cite recon evidence + live-verify. Inline corrections are
also folded into §2 / §4 / §8 below.

1. **Top-strip icon at pane (40,5) is `GoldPile`, NOT `Backpack`.** §4 row
   "`Backpack` icon" and §2's `mbr_0x19c` symbol guess both misidentify
   this sprite. The chrome at (40,5) carries the **GoldPile** (32×32,
   `inventory.dat[8]`, flags `0x104` alpha) sitting next to the gold readout.
   The Backpack sprite (20×20, `inventory.dat[1]`) is loaded into a
   different member field and is not drawn on this pane at all.

   **Proof — binary disasm of `Initialize` at `0x537650` (objdump
   `Revenant.exe`, .text section, image base 0x400000):**
   ```
   :53767c  push 0x5e411c       ; literal "Inventory"
   :537693  call 0x46d710       ; meth_0x46d710("Inventory") → FindByName
   :5376a3  mov [esi+0x198], eax    ; mbr_0x198 = Inventory chrome bitmap

   :53769e  push 0x5e4128       ; literal "GoldPile"   ← KEY LITERAL
   :5376a9  call 0x46d710       ; meth_0x46d710("GoldPile")
   :5376b0  mov [esi+0x19c], eax    ; mbr_0x19c = GoldPile bitmap (32×32)

   :5376d2  push 0x5e4134       ; literal "Backpack"
   :5376d7  call 0x46d710       ; meth_0x46d710("Backpack") → loaded but
                                ; assigned to a different field, not drawn
                                ; on this pane
   ```
   `meth_0x46d710` is **FindByName** (cls_0x46d6b0.cpp:46-78 — walks the
   multi-resource's name table via `stricmp`), NOT a sequential iterator.
   So the per-call string arg dictates which asset goes into which
   member. Ghidra's OOAnalyzer dropped the string arg from the visible
   decomp signature, causing the §2 misread.

   String VAs verified with python `va_to_file_offset` on the same
   binary:
   - `0x5e411c = "Inventory"`
   - `0x5e4128 = "GoldPile"`
   - `0x5e4134 = "Backpack"`

2. **Gold format is `"%d$"` per the binary.** Recon §8 reads `"%d$"`
   at `0x5e41a8` — that IS the literal in the shipped binary (verified
   via objdump string dump: `0x5e41a8 = '%d$'`). My earlier attempt to
   "live-verify" a `"%dg"` reading was an unsubstantiated guess; the
   correct retail suffix is the dollar sign. Reverted in the port.

3. **Gold font: retail uses `GoldMed`, port uses `Gold` per user override.**
   §8 row "Gold total" cites the font as `DAT_006663dc`.

   **Proof — binary disasm of `FUN_00485870` (the font-loader):**
   ```
   :485e3d  push 0x5d8878       ; literal "Gold"
   :485e47  mov [0x65b7d8], eax     ; small-gold global
   :485e51  push 0x5d8880       ; literal "GoldMed"
   :485e5b  mov [0x6663dc], eax     ; medium-gold global
   ```
   The inventory pane's `DrawGrid` reads `[0x6663dc]` (disasm
   `…DrawGrid_537a70.disasm.txt:73`), so retail literally uses the
   **medium** face. But the medium face's glyphs are ornate runes that
   don't read as digits at small sizes — the port uses the small `Gold`
   face per user instruction, which has conventional gold-colored
   digit glyphs.

   Asset family in `data/resources_unzipped`:
   - `smallgold*.dat` (`smallgold` + `smallgoldshad` / `glow` / `sel` /
     `s` / `silv` / `g`) → BMFONT `Gold` (FONT.DEF line 63)
   - `medgold*.dat` companions → BMFONT `GoldMed` (FONT.DEF line 64)

   Glyphs are baked gold-colored with shadows already painted in —
   render with a pass-through white tint and no extra shadow pass.

4. **Top-strip sprite at (40, 5) stamps as native-size TL — no slot,
   no centering, no bottom-align.** Retail call (disasm
   `…DrawGrid_537a70.disasm.txt:90-96`):
   ```
   00537b71  PUSH 0x2000           ; arg5 = DM_ALPHA
   00537b77  PUSH 0x5              ; arg3 = y = 5
   00537b79  PUSH 0x28             ; arg2 = x = 40 (0x28)
   00537b7d  CALL 0x004bd680       ; FUN_004bd680(x, y, bm, mode, 0)
   ```
   `FUN_004bd680(x, y, bm, mode, 0)` stamps the bitmap at TL `(x, y)` at
   its native pixel size — no slot rect, no width/height args. The
   §4 row's literal `(0x28, 5)` IS the TL of the bitmap (not the TL of
   some anchor rect that the bitmap then centers within). The GoldPile
   bitmap is 32×32, so the on-screen extent is pane-local (40, 5)..(72,
   37) — that "extends downward" effect was where the earlier
   "bottom-aligned" reading was trying to land.

5. **Per-cell item icon is the item's own `DrawAt(cellX, cellY)` — TL
   stamp at the cell origin, no centering, no offset.** Retail call
   (disasm `…DrawGrid_537a70.disasm.txt:205-207`):
   ```
   00537d17  PUSH EBX              ; arg2 = y = cellY (= row*44+42)
   00537d18  PUSH EDI              ; arg1 = x = cellX (= col*45+8)
   00537d19  CALL [EDX+0x108]      ; item->DrawAt(x, y)
   ```
   The item object's vtbl slot `0x108` self-draws at TL `(cellX, cellY)`
   at the icon's natural size. Items with varied icon sizes (small potion,
   32×32 gold pile) stamp at the cell origin and extend down-right;
   icons larger than the 40×40 cell interior are clipped by the chrome's
   baked cell border.

   **Pouch contents are a DIFFERENT branch.** The blit at
   `:537d83 FUN_004bd680(cellX, cellY+20, …)` (composited 20×20 temp
   surface) is the **pouch-contents** path, NOT the regular item icon.
   The +20 y-offset draws the bag-contents preview in the cell's bottom
   half, beneath whatever non-pouch icon shares the slot. Confused with
   the regular icon in earlier reads of the disasm because Ghidra
   dropped the loop as "unreachable" and the two paths sit a few
   instructions apart.

6. **Stack-count digits draw in WHITE (`1,1,1`) with the canonical 3-pass
   black drop shadow.** §8 footnote at line 426 ("UNCONFIRMED-B count vs
   gold color source") + §14 UNCONFIRMED-B both wonder which color path
   reaches the count pass. Live-verified: white digits, black drop shadow
   via the canonical `DrawTextShadowedToTarget` (3 offset passes). Renders
   cleanly over the cell's baked chrome bezel and the item icon beneath.

7. **Animated inventory icons (`invanim`) WORK via the retail
   InventoryImage fallback.** §6 step 6.1 reads the icon source as
   `(*item+0x130)()` = `InventoryImage`, whose retail body (FUN_0046f190)
   tries `GetInvImage(state)` first then falls back to frame 0 of
   `GetInvAnimation(state)`. Items shipped with only `invanim` (potions
   etc.) were invisible in the port until the fallback landed in
   `TObjectInstance::InventoryImage` (`src/object.cpp`).

**Snapshot note (supplementary — superseded by retail).** A 1998/99 version
exists at `src/inventory.{h,cpp}` (`TInventory : public TPane`). Its field names
(`container`, `grabslot`, `heldslot`, `isdragging`, `startposx/y`) and the
`OnSlot()` / `DrawBackground()` / `MouseMove()` algorithm **intent** map 1:1 to
retail and are cited where they aid understanding. **Its coordinates/layout are
superseded:** `src/revdefs.h:131-176` describes a *horizontal 7×2 bottom-bar*
inventory (`INVITEMSX=7, INVITEMSY=2, INVENTORYPANEX=16, INVENTORYPANEY=386,
…WIDTH=382, …HEIGHT=85, INVENTORYSTARTX=72, INVENTORYSTARTY=0`). The retail
**sidebar** Inventory is a **4×3 vertical grid** in a 188×174 pane. Item-cell
dimensions partly survive (`INVITEMREALWIDTH/HEIGHT=40`, x-pitch
`INVITEMWIDTH=45`), but the grid count, pane rect, origin, and slot index
order all changed. **Retail is authoritative for every coordinate in §2–§8.**

---

## §1 — Overview

The Inventory pane is the player's carried-items grid. It renders a `188×174`
chrome bitmap (`Inventory`) with a **4-column × 3-row** array of item cells, a
left-margin **container icon** (the open backpack/pouch you are viewing), a
**"%d$" gold total** in the top-left, and **left/right scroll arrows** for
paging through containers wider than one screen. Each occupied cell blits the
item's inventory icon; a stacked item (`Amount() > 1`) gets its count drawn over
the icon.

It is **interactive**: click-drag moves items between cells (swap), uses one item
on another, transfers to/from the Equip pane, opens a sub-container, or drops to
the world. A 2-px move threshold promotes a click into a drag (a drag cursor
sprite is attached).

**Instances:** one global (`DAT_0065d4f8`). The pane can display the player's own
inventory **or** a target's / a sub-container's — `mbr_0x17c` holds the
currently-shown container object, and `mbr_0x5f` (`+0x17c`'s sibling at byte
0x17c is the live container; `param_1[0x5f]` in DrawGrid is the same field) is
compared against `DAT_00667fcc` (the player) to pick which of two pre-sized
work-surfaces to composite into.

**Shown/hidden:** managed by `TSideTabsPane` + the TPlayScreen command dispatcher
(`FUN_0047cf40`, cases 9 and 0xb). Visibility is the lower-region modal state
`DAT_0065d1bc == 0` (B.r11 §3). DrawGrid early-outs when the pane is not dirty
(`mbr_0x17c != 0 && mbr_0x50 != 0`, slot-80 gate `…_537a30.cpp:270`).

---

## §2 — Asset roster

Archive: `data/.../inventory.dat` (loaded via the `DAT_0065c130` resource-
iterator; `meth_0x46d710` returns entries 0,1 in Initialize order). Measured
with `tools/ui/dump_dat.py inventory.dat`.

| asset | archive | entry | WxH (measured) | flags | role | cite |
|---|---|---|---|---|---|---|
| `Inventory` | inventory.dat | [0] | **188×174** | 0x2 (15-bit) | chrome / pane background; stored `mbr_0x198` | dump; Init `…ctor`+`DrawGrid:38` `[EDI+0x198]` |
| `Backpack` | inventory.dat | [1] | **20×20** | 0x2 | container icon (the bag you're viewing); `mbr_0x19c` ⚠ live-verify says `mbr_0x19c` actually holds the **GoldPile** sprite (see §0 LIVE-VERIFIED #1); the literal Backpack bitmap may instead be carried by `mbr_0x1a0`/`1a8` as the container art (10,10) | dump; `DrawGrid:89` `[EDI+0x19c]` |
| `InvArwLU/LD/LG` | inventory.dat | [5/6/7] | 24×24 | 0x104 (15-bit+alpha) | **left** scroll-arrow button states (up/down/glow) | dump; Init `s_scrollleft_005e4164` |
| `InvArwRU/RD/RG` | inventory.dat | [2/3/4] | 24×24 | 0x104 | **right** scroll-arrow button states | dump; Init `s_scrollright_005e4194` |
| `GoldPile` | inventory.dat | [8] | 32×32 | 0x104 | gold-stack item icon (drawn as a grid item, not chrome) | dump |

Notes:
- The pane's chrome (`Inventory` 188×174) is exactly the pane rect (§3) — no
  chip-model overhang. Slot cells are baked **into** the chrome art; the engine
  draws icons on top, it does not draw cell borders (the per-cell `Box` calls in
  the snapshot are gone — retail relies on the chrome bitmap).
- `flags=0x104` = `BM_15BIT(0x2)` is **not** set but `0x100`+`0x4`; these
  arrow/gold sprites carry **alpha** (drawmode `0x2000 = DM_ALPHA` at the blit),
  not a magenta key (UI_METHOD_MAP §16). The 15-bit chrome (`Inventory`,
  `Backpack`) is opaque.
- A second 188×174 `Inventory` bitmap exists in `sidepane.dat[1]`; it is **not**
  the one this pane loads — the pane loads from `inventory.dat`. (Don't confuse
  the two during reconstruction.)

### Source-rect map (chrome `Inventory` 188×174)

```
Inventory.bmp 188x174  (= pane rect, blit opaque at pane (0,0))
 ┌──────────────────────────────────────────┐
 │ [Gold "%d$" text @ (0x50,0xd)]  [Backpack │  ← top strip
 │  drawn over chrome @ (0x28,5) 20x20]      │
 ├────────┬──────┬──────┬──────┬──────┬──────┤  4 cols (x = col*45 + 8)
 │ margin │ cell │ cell │ cell │ cell │      │  3 rows (y = row*44 + 42)
 │  col   │ 0,0  │ 1,0  │ 2,0  │ 3,0  │      │  cell interior 40x40
 │ (icon, │ cell │ cell │ cell │ cell │      │
 │  arrows│ 0,1  │ 1,1  │ 2,1  │ 3,1  │      │
 │  here) │ cell │ cell │ cell │ cell │      │
 │        │ 0,2  │ 1,2  │ 2,2  │ 3,2  │      │
 └────────┴──────┴──────┴──────┴──────┴──────┘
```
(Cell origins are decoded in §3/§4 from the recon, not measured off the art.)

---

## §3 — Coordinate frames & surfaces

**Pane rect (screen):** `x=0x1c4 = 452`, `y=0x132 = 306`, `w=0x bc = 188`,
`h=0xae = 174` — ctor `…_487f50.cpp:6-14` (`param_1[1..4]` = x/y/w/h, dup'd in
`[5..8]` = newx/newy/neww/newh). TPane field meaning per `src/screen.h:86,157-163`.
Right edge `452+188 = 640`, bottom `306+174 = 480` → the pane fills the
**bottom-right corner** lower content slot of the right sidebar (upper slot is the
188×306 panes above it at y=0).

The live width/height the paint code reads are `mbr_0xc = 188` (width) and
`mbr_0x10 = 174` (height) — used as click bounds (`MouseClick:315-318`,
`DrawGrid:37` fills `0,0,0xbc,0xae`).

**No chip model** here: the chrome bitmap == the pane rect, drawn at pane-local
`(0,0)`. All element coordinates below are **pane-local** unless tagged screen.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | — | (0,0) | final pixels (640×480) | — |
| **right-sidebar** | screen | TR | screen `(screen_w − 188, 0)` = `(452,0)` in 640-wide | the two stacked content slots | derived (pane x = 452 = 640−188) |
| **inv-pane** | right-sidebar | BR | screen `(0x1c4, 0x132)` = `(452, 306)`; = sidebar `(0, 306)` | the whole Inventory pane (chrome + grid + chrome icons + text) | ctor `:6-14`; bottom-anchored: pane bottom = 480 |
| **grid** | inv-pane | TL | pane-local origin `(8, 0x2a)` = `(8, 42)` of cell (0,0) | the 4×3 item-cell array | DrawGrid loop `:533d0e` & MouseClick `:406-413` |
| **cell(col,row)** | grid | TL | pane-local `(col*0x2d + 8, row*0x2c + 0x2a)` | one item icon + count | DrawGrid `LEA EDI…+8`, `LEA EBX…*4+0x2a` (`:537cfc-537d0e`) |
| **source-rect** | each bitmap | TL | (sx,sy) into the named asset | the sub-rect copied out (full-bitmap here) | — |

**Anchor detection:** the pane's x is the constant `0x1c4 = 452`, but
`452 = 640 − 188` and the parent sidebar is right-aligned, so for resolution
re-layout treat the pane as **TR/BR-anchored** to the screen, offset 188 from the
right edge, 174 from the bottom. (In Classic 640×480 it is the literal constant.)

**Composition formula (to screen):**
```
screen_x = 452 + cell_x_in_pane          # = 452 + (col*45 + 8)
screen_y = 306 + cell_y_in_pane          # = 306 + (row*44 + 42)
```

### Surfaces / two-stage blit

DrawGrid composes into a **destination surface `param_2`** (the slot-84 argument).
Two cases (`DrawGrid:34-37, 95-101`):
- **`param_2 == PTR_DAT_005d79e0`** (the live display): draw straight, then
  register the dirty rect for blit-effects via `FUN_004aacb0(x,y,w,h, 6)`
  (`:537f72`).
- **`param_2 != display`** (an off-screen pane buffer): set clip
  `(*+0x40)(0,0x132)`, draw, then `FUN_00435cb0(param_2)` + `(*+0x40)(0,0)` to
  finalize (`:537f44-537f50`). This is the **compose-to-target contract**
  (NOMENCLATURE §3): the port composes the chip into one RT then blits it.

The icon scratch path: a small `0x14×0x14 = 20×20` temp surface is allocated per
paint via `FUN_004a1ec0(0x14,0x14, fmt, 0)` (`:537b96`) and used by the
drag-image / tint helper; the `mbr_0x1a0`/`mbr_0x1a8` (`param_1[0x68]/[0x6a]`)
are two 20×20 `cls_0x5a68d8` surfaces created in Initialize (`:129-157`) holding
the live container art; one is chosen by player-vs-target at `DrawGrid:39-46` and
blitted at pane `(10,10)` (see §4).

### Layout diagram (pane-local)

```
inv-pane  (pane-local; origin = screen (452,306))   w=188 h=174
0   8                                          188
0 ┌──┬───────────────────────────────────────────┐
  │  │  [Backpack icon 20x20 @ (0x28,5)=(40,5)]   │
  │  │  [Gold "%d$"  cell @ (0x50,0xd)=(80,13)]   │
10│  │  [container art surface @ (10,10) 20x20]   │
  │  ├────────┬────────┬────────┬────────┐        │
42│LM│ (0,0)  │ (1,0)  │ (2,0)  │ (3,0)  │        │  y=42  (row0)
  │  │ 40x40  │        │        │        │        │
86│  ├────────┼────────┼────────┼────────┤        │  y=86  (row1, +44)
  │  │ (0,1)  │ (1,1)  │ (2,1)  │ (3,1)  │        │
130 │ ├────────┼────────┼────────┼────────┤        │  y=130 (row2, +88)
  │  │ (0,2)  │ (1,2)  │ (2,2)  │ (3,2)  │        │
  │  └────────┴────────┴────────┴────────┘        │
  │  x=8     x=53     x=98    x=143  (col*45+8)    │
  └──┴───────────────────────────────────────────┘
  [scrollleft btn 24x24 @ (0x8c,0xb)] [scrollright @ (0xa1,0xc)]
   (the L/R arrow buttons sit in the top-right header area)
```

---

## §4 — Static element layout

All coordinates **pane-local** (compose with screen `(452,306)` per §3). This
pane is **not mirrored** (single instance), so no target column.

| element | space | (x,y) | (w,h) | source | drawmode | cite |
|---|---|---|---|---|---|---|
| pane bg fill | pane | (0,0) | (188,174) | — (color 0xffff) | `0x80000000` (`Box`) | `DrawGrid:537ac6` `(*+0x64)(0,0,0xbc,0xae,…)` |
| chrome `Inventory` | pane | (0,0) | (188,174) | full bmp `mbr_0x198` | `0x80000000` opaque | `:537ada` `FUN_004bd680(0,0,[0x198],0x80000000,0)` |
| container art surf | pane | (10,10) | (20,20) | `mbr_0x1a0`/`1a8` (20×20) | `0x100` DM_TRANSPARENT | `:537b14` `FUN_00438df0(0xa,0xa,surf,…,0x100,…)` |
| Gold count text | pane | cell (0x50,0xd)=(80,13) | w=auto, h=auto | string **`"%d$"`** (binary-verified at 0x5e41a8 via objdump string dump — see §0 LIVE-VERIFIED #2) | text | `:537b65` (see §8) |
| **GoldPile icon** (live-verified — recon misnames this row "Backpack"; the sprite at (40,5) is the small GoldPile, BOTTOM-aligned in the chrome's header anchor rect — see §0 LIVE-VERIFIED #1 & #4) | pane | (0x28,5)=(40,5) TL of anchor rect | 32×32 sprite, bottom-aligned in ~40-tall anchor | full bmp `mbr_0x19c` (carries GoldPile, not Backpack) | `0x2000` DM_ALPHA | `:537b7d` `FUN_004bd680(0x28,5,[0x19c],0x2000,0)` |
| **item icon** (per occupied cell) | cell | (col*45+8, row*44+42) — literal TL via `item->DrawAt(EDI, EBX)` at `:537d19` (`CALL [EDX+0x108]`), see §0 LIVE-VERIFIED #5 | bitmap's native size, no scale | item's vtbl slot `0x108` self-draws (uses `InventoryImage()` internally) | (item-defined) | `:537d19` (regular item path); `:537d83` is the SEPARATE pouch-contents path at (cellX, cellY+20) — not the regular icon |
| **item count** (Amount>1) | cell | (col*45+8, row*44+42) — literal `PUSH EDI / PUSH EBX` at `:537e4c/:537e4b` | w-arg `0x28=40` | "%d" | text font `0x404` (port: Arimo TTF @ 12px, WHITE with 3-pass black drop shadow, RIGHT-aligned in the 40px cell — see §0 LIVE-VERIFIED #6) | `:537e4f` (see §8) |
| scrollleft button | pane | (0x8c,0xb)=(140,11) | (0x18,0x18)=(24,24) | InvArwL{U,D,G} | button | Init `:165` ctor `(…,0x8c,0xb,0x18,0x18,…)` |
| scrollright button | pane | (0xa1,0xc)=(161,12) | (24,24) | InvArwR{U,D,G} | button | Init `:185` ctor `(…,0xa1,0xc,0x18,0x18,…)` |

**Cell origin verification (cross-check, two ways).**
1. *Forward (paint)*, `DrawGrid:537cfc-537d0e`:
   `EDI = EDX*45 + 8` (col→x), `EBX = ECX*44 + 0x2a` (row→y) where
   `EDX = (invslot−page)/3` and `ECX = (invslot−page)%3`.
2. *Inverse (hit-test)*, `MouseClick:406-413`:
   `iVar3 = x − 8; iVar6 = y − 0x2a; col = iVar3/0x2d; row = iVar6/0x2c;
   reject if col>3 or row>2 or (x−8)%0x2d>0x27 or (y−0x2a)%0x2c>0x27;
   slot = col*3 + page + row`.
   Same in `MouseMove:407-413` and `Animate:537fa…:25-32`.

Both agree: **origin (8,42), x-pitch 45 (0x2d), y-pitch 44 (0x2c), interior 40
(0x28; valid 0..0x27=39), 4 cols (0..3), 3 rows (0..2), column-major slot index
`col*3 + row` plus a horizontal page offset `mbr_0x188`.** Confirmed three
independent ways.

### Item-slot grid table

| property | value | cite |
|---|---|---|
| grid origin (pane-local, cell 0,0 TL) | **(8, 42)** = `(0x8, 0x2a)` | DrawGrid/MouseClick |
| columns | **4** (index 0..3) | `3 < col` rejects (`:407`) |
| rows | **3** (index 0..2) | `2 < row` rejects (`:407`) |
| cell pitch x | **45** = `0x2d` | `:537cfc`/`:406` |
| cell pitch y | **44** = `0x2c` | `:537d0e`/`:407` |
| cell interior (icon clip) | **40×40** = `0x28×0x28` | modulo `>0x27` rejects |
| inter-cell gap | x: 45−40 = **5**; y: 44−40 = **4** | derived |
| slot index | `slot = col*3 + row + page` (column-major) | `:537d… `, `:413` |
| page offset field | `mbr_0x188` (`+0x188`), default 0 | Init `:200` `mbr_0x188=0` |

> The snapshot used **row-major** `slot = row*INVITEMSX + col` with 7 cols
> (`src/inventory.cpp:242`). Retail transposed to **column-major, 3 rows**, with a
> horizontal scroll page. Use the retail formula.

---

## §5 — Draw order / composition (vtable slot 84 = `0x537a70`, DrawGrid)

Driven by slot 80 (`0x537a30`, `Show`/DrawBackground): if `mbr_0x17c != 0 &&
mbr_0x50 != 0` it calls slot 84 then slot 44 (`virt_meth_0x444fc0`, the
button-children draw) (`…_537a30.cpp:270-272`).

Slot 84 sequence (`DrawGrid_537a70.disasm.txt`):
1. `(*this+0x90)()` — pre-draw setup on `this` (`:537a82`).
2. If dst `param_2 != display`: `(*param_2+0x40)(0,0x132)` — set clip to pane
   (`:537aa4`).
3. `(*param_2+0x64)(0,0,0xbc,0xae,0,0xffff,0,0x80000000)` — **fill pane bg**
   (`Box`, color 0xffff) (`:537ac6`).
4. `FUN_004bd680(0,0, mbr_0x198, 0x80000000, 0)` — **chrome** `Inventory` opaque
   at (0,0) (`:537ada`).
5. Pick container surface: `mbr_0x1a0` if `mbr_0x17c==DAT_00667fcc` (player) else
   `mbr_0x1a8` (`:537adf-537af6`). `FUN_00438df0(10,10, surf, …, w,h, 0x100,…)`
   — blit container art at (10,10), DM_TRANSPARENT (`:537b14`).
6. If `DAT_00667fcc != 0`: format **Gold** — `(*player+0x84)("Gold")` →
   `FUN_0058b100(buf,"%d$",gold)` → `FUN_004be110(buf,0x50,0xd,1,…)` text
   (`:537b1f-537b65`); then `FUN_004bd680(0x28,5, mbr_0x19c, 0x2000,0)` —
   **Backpack icon** alpha (`:537b7d`).
7. Allocate 20×20 temp surface `FUN_004a1ec0(0x14,0x14,fmt,0)` (`:537b96`);
   set its key `[+0x18]=DAT_006668d0`.
8. **Item loop** — init iterator `FUN_0046dfb0(&iter)` (`:537bd2`); for each item
   (`ESI`, while `iter[0x30]!=0`):
   a. compute cell (x,y) (§4); call item `(*+0x108)(x,y)` (store draw pos)
      (`:537d19`).
   b. if item is a "Pouch" (`FUN_0059a530(name,"Pouch")`) skip the icon; else get
      owner `FUN_004701f0`, then `InventoryImage = (*owner+0x130)()`; tint via
      `FUN_004a31a0(image, tempSurf, key)`; **blit icon**
      `FUN_004bd680(x, y, image, 0x100, 0)` (`:537d83`). (See §6 for the bad-slot
      fixup branch at `:537c2c`.)
   c. **count:** if `(*item+0x198)() > 1` (Amount): `itos(amount,buf,10)` →
      `FUN_004be2b0(buf, x, y, …, 0x28, font=0x404,…)` (`:537df4-537e4f`, §8).
   d. advance: `FUN_0046dfb0(&iter)` again (`:537e5c`); loop while item != 0.
9. `FUN_004830f0(tempSurf)` — free temp surface (`:537e74`).
10. Selected-slot fixups: if `mbr_0x180 < 0` set `mbr_0x184=-1` (`:537e86`);
    toggle the two scroll-arrow buttons' enabled flag (`& ~4` / `| 4`) based on
    `mbr_0x188` page (left arrow `:537e90-537ee5`) and the `0xf3` right limit
    (right arrow `:537ee5-537f34`).
11. Finalize: display path → `FUN_004aacb0(x,y,w,h,6)` dirty-rect
    (`:537f72`); pane-buffer path → `FUN_00435cb0(param_2)` + clip reset
    (`:537f44`).

Then slot 44 paints the two arrow buttons (`virt_meth_0x444fc0`).

---

## §6 — Algorithms

### 6.1 Cell address ↔ slot (the grid kernel)

```
# paint: slot -> cell pixel (pane-local)
s        = item.InventNum() - page          # page = mbr_0x188
col      = s / 3                            # 3 rows per column (column-major)
row      = s % 3
icon_x   = col*45 + 8                       # 0x2d, 0x8
icon_y   = row*44 + 42                      # 0x2c, 0x2a
# guard: only draw if page <= InventNum < page+12  (:537bff-537c1c)

# hit-test: cell pixel -> slot (MouseClick/MouseMove/Animate)
cx = x - 8 ;  cy = y - 42
if cx<0 or cy<0 or cx/45>3 or cy/44>2 or cx%45>39 or cy%44>39: slot = -1
else: slot = (cx/45)*3 + page + (cy/44)
```
Tabulated cell origins (pane-local, page 0):

| col\row | 0 (y=42) | 1 (y=86) | 2 (y=130) |
|---|---|---|---|
| 0 (x=8)   | slot 0  | slot 1  | slot 2  |
| 1 (x=53)  | slot 3  | slot 4  | slot 5  |
| 2 (x=98)  | slot 6  | slot 7  | slot 8  |
| 3 (x=143) | slot 9  | slot 10 | slot 11 |

### 6.2 `FUN_00438df0` — SDrawParam blit wrapper (container art)
Builds an `SDrawParam` on the stack and calls `(*surface+0x5c)` (ParamBlit-with-
effects, UI_METHOD_MAP §8.4/§15a). Args observed: `(dstX, dstY, surf, srcX,
srcY, srcW, srcH, drawmode, p9, …)`. Call site `(10,10, surf, 0,0, w,h, 0x100,
0,0)` → blit the whole 20×20 container surface at (10,10), DM_TRANSPARENT.
Body: `FUN_00438df0_SurfaceBlitWrapper.cpp:32-54`.

### 6.3 `FUN_004be110` — text wrapper over `FUN_004be2b0` (gold/money line)
Signature `(surf, text, x, y, scaleY?, fontIdx, colorPtr, flags, lineW, p10,
alignFlags, color, drawmode)`. Resolves the HDC from `DAT_0065b020[fontIdx*4]`,
applies h-align offset (`&2` center → `x-=w/2`, `&4` right → `x-=w`), then calls
`FUN_004be2b0` (UI_METHOD_MAP §5). Body: `FUN_004be110_TextDrawWrapper.cpp`.

### 6.4 `FUN_0046dfb0` — inventory iterator step
Advances an inventory-list cursor held in the 0x18-byte stack struct
(`[ESP+0x18..0x30]`); `[+0x30]` is the current item pointer (0 = end). Mirrors
the snapshot `TInventoryIterator` (`src/inventory.cpp:50`). Body:
`FUN_0046dfb0_InventoryIterStep.cpp` (MEDIUM — outer role clear, internal list
walk inferred).

### 6.5 Bad-slot fixup (`DrawGrid:537c2c-537cb0`)
If an item's `InventNum < 0` (uninitialised), retail asks the container for a
free slot (`(*container+0x94)()`), writes it back, and emits a TextBar warning
using strings `EQUIPBADSLOT`/`FULLEQUIPBADSLOT` (`0x5e41ac/…c0/…d4/…e4`). This
matches snapshot `src/inventory.cpp:56-60` ("Bad inventory slot for %s"). Port:
reproduce the free-slot assignment; the warning text is optional dev diagnostics.

---

## §7 — Effects & shadows

- **Chrome / container** (`Inventory`, container surface) drawn opaque
  (`0x80000000`) or DM_TRANSPARENT (`0x100`) — no drop shadow.
- **Backpack / arrows / gold-pile** carry **alpha** (`flags 0x104`), blit with
  `0x2000 = DM_ALPHA` (UI_METHOD_MAP §4 / §16) — **not** magenta-keyed; do not
  apply the global magenta key to these.
- **Item icons** drawn DM_TRANSPARENT (`0x100`) — the item bitmaps use the
  standard `0x7C1F` 15-bit magenta key (UI_METHOD_MAP §16).
- **Count text** uses font id `0x404`: low byte `4` = HDC table index 4, **bit
  `0x400` set ⇒ 3-pass black drop shadow** (UI_METHOD_MAP §5 step 8 + §7). The
  gold "%d$" line uses the font from `DAT_006663dc` (also a shadowed UI font).
  Render white text + 1px black shadow (right+bottom); **do not** add a glow or
  reproduce the pink-halo chroma artifact ([[project-retail-pink-halo-bug]]).
- Final composite to the live display registers a blit-effect dirty rect
  (`FUN_004aacb0(...,6)`); the port's `DrawSurface` of the composed RT replaces
  this (UI_METHOD_MAP §8).

---

## §8 — Text rendering

Both strings render via `FUN_004be2b0` (GDI `DrawTextA`, UI_METHOD_MAP §5). All
coords pane-local (compose +screen(452,306)).

| string | cell (space, x,y,w,h) | font id | h-align | v-align | shadow | format | source value | cite |
|---|---|---|---|---|---|---|---|---|
| Gold total | pane (0x50,0xd, w=auto, h≈font) = (80,13) | `DAT_006663dc` → retail = BMFONT **`GoldMed`** (binary-verified at FUN_00485870:485e51); port = BMFONT `Gold` per user override (see §0 LIVE-VERIFIED #3) | **left** (flags `&1`, arg `1`) | **top** | baked into the bitmap atom (shadows already painted; no extra shadow pass) | **`"%d$"`** (binary-verified at 0x5e41a8) | `(*player+0x84)("Gold")` (`0x5e41a0`) | `DrawGrid:537b3b/:537b65` |
| item count | cell (col*45+8, row*44+42, **w=0x28=40**, h=auto) | **`0x404`** (port: Arimo TTF @ 12px to match the rest of the HUD's typography — see §0 LIVE-VERIFIED #6) | live-verified RIGHT-aligned (top-right of the 40-px cell over the icon) | **top** | canonical 3-pass black drop shadow via `DrawTextShadowedToTarget` | `"%d"` via `itos(amount,buf,10)` | `(*item+0x198)()` = `Amount()` | `:537e0e/:537e4f` |

**h-align of the count.** `FUN_004be2b0` is called directly (`:537e4f`) with the
format-flag arg derived from `FUN_00444e20(-1, …, 0x404, 0x20)` — the flag word
is built at `:537e1d-537e29`. The width arg is `0x28 = 40` (the full cell
interior). The x passed is the **cell left** (`EDI = icon_x`), and the colored
text width is doubled (`SHL EAX,1` at `:537e46`) before the call, suggesting a
**2× scaled count** rendered from the cell left. Treat as **left-aligned within
the 40-px cell, top**, drawn at the icon's top-left corner — i.e. the count sits
in the **top-left** of the item cell. (UNCONFIRMED-A: the `SHL EAX,1` doubling
and exact align bit; see §14.)

**Visual-anchor check (count over icon).** The count cell shares the icon's
(x,y) and uses w=40 = the icon clip width, so the count box exactly overlays the
icon cell. The snapshot drew the count at `(x, y-1)` left-aligned
(`src/inventory.cpp:73`), consistent with a top-left count over the icon. No
centering is implied — **h-align left, v-align top.**

**Gold visual-anchor.** Gold "%d$" at pane (80,13), left-aligned, sits in the top
strip to the right of the Backpack icon (40,5). Consistent with the chrome art's
header band.

Font color source: white (the count packs `0xff,0xff,0xff` via `FUN_00429950`
at `:537db4` for the *drag* path; the count pass uses the UI font's own color).
Cite `:537db4` (RGB pack) — UNCONFIRMED whether the count uses that white or the
font default (§14).

---

## §9 — Animation & dynamic behavior

This pane has **no continuous tween**. Dynamic behavior is event-driven redraw +
a drag state machine.

**Dirty/redraw model.** Slot 80 gates paint on `mbr_0x17c != 0 && mbr_0x50 != 0`
(`…_537a30.cpp:270`); `mbr_0x50` is the dirty flag (snapshot `IsDirty()`).
Redraw is requested by `(*this+0x445210)` (slot 40, the pane's `Update`/SetDirty,
`MouseClick:375` etc.). `SetContainer` (snapshot `src/inventory.cpp:39`) marks
dirty on container change. Port: use a monotonic version counter
([[feedback-versions-over-flags]]).

**Hover (slot 88 `Animate`, `0x537f90`).** When the mouse is over a cell and no
button held (`DAT_00668518==0`), it resolves the slot under the cursor (same grid
math), fetches the item (`FUN_004701f0(slot)`), and updates the cursor-over-item
tooltip (`FUN_0043a820`) — mirrors snapshot `TInventory::Animate`
(`src/inventory.cpp:84-91`).

**Drag state machine** (fields & snapshot names):

| field | byte off | meaning (snapshot) | cite |
|---|---|---|---|
| `mbr_0x17c` | +0x17c | `container` (shown object) | DrawGrid, MouseClick |
| `mbr_0x180` | +0x180 | `grabslot` (slot grabbed; -1 = none) | MouseClick `:416` |
| `mbr_0x184` | +0x184 | `heldslot` (for cross-pane transfer) | MouseClick `:417` |
| `mbr_0x188` | +0x188 | scroll **page** offset (cells skipped) | Init `:200`, grid math |
| `mbr_0x18c` | +0x18c | `isdragging` flag | MouseClick `:419` |
| `mbr_0x190` | +0x190 | `startposx` | MouseClick `:415` |
| `mbr_0x194` | +0x194 | `startposy` | MouseClick `:418` |
| `mbr_0x1a4` | +0x1a4 | "cursor in grid" highlight flag | MouseMove `:632-639` |

```
ramp/threshold drag-promote:
  trigger  LEFTDOWN/RIGHTDOWN in a cell sets grab=heldslot=slot, start=(x,y),
           isdragging=false                          (MouseClick param_1==4 hi-branch / :393-420)
  promote  on MouseMove, if |x-startx|>=2 or |y-starty|>=2 -> isdragging=1,
           attach drag bitmap = item.InventoryImage() with grab-offset
           (snapshot grabx/y = (start-origin)%pitch, fallback (20,20))
           cite MouseMove FUN_00538c10:641-673; snapshot src/inventory.cpp:184-207
  drop     LEFTUP resolves target slot; swap / use / transfer / sub-container
           (MouseClick :308-391, :422-558)
```

---

## §10 — Input & dispatch

Interactive. Handlers: vtable **slot 100 `0x538210`** (button down/up = the
real MouseClick) and **slot 104 `0x538c10`** (move/drag-promote). Both call the
TButtonPane intermediate first (`meth_0x436530` / `meth_0x436660`) so the scroll
arrows get their click, then run the grid logic.

**Event param decode** (`virt_meth_0x538210(this, param_1=button, param_2=x,
in_stack_0000000c=y)`):
- `param_1 == 4` → button **down** (`:422` block — but note the high-branch at
  `:560` handles the left-margin container icon region 10≤x≤0x1e,10≤y≤0x1d).
- `param_1 == 5` → button **up** (`:308` block — swap/use/transfer/drop).
- `param_1 == 1` → also a press path that records grab (`:393`).

**Hit rects** (pane-local):

| control | rect (x,y,w,h) | action | cite |
|---|---|---|---|
| item cell (i) | (col*45+8, row*44+42, 40, 40) | grab / drop / use | grid math |
| container icon (left margin) | (10,10, ~20,~20) i.e. `10..0x1e × 10..0x1d` | open owner container / move-to-owner | `:425` `param_2 10..0x1e, y 10..0x1d` |
| scrollleft button | (140,11,24,24) | page− (`mbr_0x188`) | Init `:165` |
| scrollright button | (161,12,24,24) | page+ | Init `:185` |
| anywhere in pane | (0,0,188,174) | bounds gate | `:315-318` |

**Drop dispatch (slot 100 `up`)** mirrors snapshot `MouseClick`
(`src/inventory.cpp:105-176`): same slot → use; different occupied → use-on or
swap; empty → move; left margin → climb to owner container or move item out;
Equip-pane held-slot → transfer. The scroll arrows are wired in Initialize to
`LAB_00537500` (left) / `LAB_00537510` (right) callbacks via the button ctor
(`cls_0x5a3c68`, `:164/:184`).

**Modal-state globals:** lower-region selector `DAT_0065d1bc` (==0 selects this
pane); `DAT_0065c9e0` (a "stats-mode" interaction modifier, `:321`);
`DAT_0066829c` (a use-vs-swap gate, `:454`). Visual effect: when `DAT_0065d1bc !=
0` the pane is not shown (B.r11).

---

## §11 — Retail bugs NOT to reproduce

1. **Pink halo on count/gold text.** The shadowed UI font path (font `0x404`,
   bit 0x400) over a magenta-cleared scratch produces a pink fringe on antialiased
   shadow edges (UI_METHOD_MAP §16, [[project-retail-pink-halo-bug]]). Intended
   visual: white digits + 1px black shadow, no halo. Render with real alpha.
2. **Doubled count width (`SHL EAX,1`, `:537e46`).** If this was an attempt to
   2×-scale the count and it overflows the 40-px cell, clamp to the cell; intended
   visual is a small count tucked in the item cell corner (UNCONFIRMED-A — verify
   against a reference capture before treating as a bug).

No other artifacts observed.

---

## §12 — Reconstruction pseudocode

```cpp
void InventoryPane::paint(Target& rt) {              // vtable slot 84
  if (!container || !dirty) return;                  // slot-80 gate
  rt.setClip(0,0,188,174);
  rt.box(0,0,188,174, COLOR_BG);                     // pane bg
  rt.drawBitmap(asset.Inventory, 0,0);               // chrome, opaque
  Surface& cont = (container==player) ? contSurfA : contSurfB; // 20x20
  rt.drawSurfaceTransparent(cont, 10,10);
  if (player) {
    char g[16]; snprintf(g,16,"%d$", player.stat("Gold"));
    drawTextShadowed(uiFont, g, 80,13, /*left,top*/);          // §8
    rt.drawBitmapAlpha(asset.Backpack, 40,5);                  // 20x20
  }
  for (Item* it : InventoryIterator(container)) {    // §6.4
    int slot = it->invNum();
    if (slot < page || slot >= page+12) continue;    // visible window
    if (slot < 0) { slot = container->findFreeSlot(); it->setInvNum(slot); }
    int col = (slot-page)/3, row = (slot-page)%3;
    int x = col*45 + 8, y = row*44 + 42;             // §6.1
    if (it->isPouch()) continue;                     // pouches skip icon
    PTBitmap img = it->inventoryImage();             // owner +0x130
    if (img) rt.drawBitmapClipped(img, x, y, /*clip*/40,40);   // 0x100
    if (it->amount() > 1) {
      char c[16]; itoa(it->amount(), c, 10);
      drawTextShadowed(font0x404, c, x, y, /*cellW=40, left, top*/);
    }
  }
  // scroll-arrow enable: left enabled iff page>0; right iff more items right
  leftArrow.setEnabled(page > 0);
  rightArrow.setEnabled(page < 0xf3 && /*more cols*/);
  drawChildButtons(rt);                              // slot 44: L/R arrows
  rt.present();
}

int InventoryPane::onSlot(int x, int y) {            // §6.1 inverse
  int cx = x-8, cy = y-42;
  if (cx<0||cy<0||cx/45>3||cy/44>2||cx%45>39||cy%44>39) return -1;
  return (cx/45)*3 + page + (cy/44);
}
// click()/move() drag SM per §9/§10; logic = src/inventory.cpp:93-208 (retail-confirmed).
```

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `(*+0x64)` Box fill | `Renderer->FillRect` / `DrawSurface` clear | renderer |
| `FUN_004bd680(...,0x80000000)` opaque | `DrawBitmapToTarget` | renderer |
| `FUN_004bd680(...,0x2000)` alpha | `DrawBitmapTintedToTarget` (src alpha) | renderer |
| `FUN_004bd680(...,0x100)` item icon | `DrawBitmapSubrectToTarget` (magenta key) | renderer |
| `FUN_00438df0` surface blit | `DrawSurfaceToTarget` / `DrawBitmapSubrectToTarget` | renderer |
| `FUN_004be110` / `FUN_004be2b0` text | `DrawTextShadowedToTarget(atlas,text,x,y,cellW,ALIGN_LEFT,…)` | font.cpp |
| count font `0x404` shadow | `DrawTextShadowedToTarget` (3-pass) | font.cpp |
| `FUN_004aacb0(...,6)` dirty-rect | (drop — GPU composites RT) | — |
| `FUN_0046dfb0` iterator | `for (oi : container->inventory())` | game data |
| compose to slot-84 dst | `DrawSurface` of the composed pane RT | renderer |

No missing primitives identified; all map to the existing `…ToTarget` family
(UI_METHOD_MAP §12).

---

## §14 — UNCONFIRMED / open questions

- **UNCONFIRMED-A — count text align + `SHL EAX,1`.** `DrawGrid:537e46` doubles
  the measured text width before the colored `FUN_004be2b0` count pass; the align
  flag word is built via `FUN_00444e20(-1,…,0x404,0x20)`. I read it as
  left-aligned, top, in a 40-px cell drawn at the icon corner, possibly 2×
  scaled. *Impact:* count could be centered or scaled differently. *Resolve:*
  decode `FUN_00444e20`'s flag output + visual-verify a stacked item (e.g. arrows
  / gold) against a 640×480 retail capture.
- **UNCONFIRMED-B — count vs gold color source.** The white pack
  (`FUN_00429950(0xff,0xff,0xff)` `:537db4`) is on the **drag-image** path;
  whether the in-grid count uses white or the font-default color is not pinned.
  *Impact:* wrong digit color. *Resolve:* trace which color int reaches the count
  `FUN_004be2b0` at `:537e4f` (vs the drag blit at `:537def`).
- **UNCONFIRMED-C — item-object getters `+0x108`/`+0x130`/`+0x198`.** Identified
  by role (set-draw-pos / InventoryImage / Amount) from call shape + snapshot
  parity, not by dumping the item (`TObjectInstance`/`TPlayer`) vtable. *Impact:*
  wrong getter → wrong icon/count. *Resolve:* `DumpVtable.java` on the item
  object's vtable and confirm the three slots.
- **UNCONFIRMED-D — `mbr_0x188` page step + right-arrow limit `0xf3`.** The page
  field drives horizontal scrolling; the `0xf3 = 243` compare bounds the right
  arrow. The exact per-click page delta (set in the `LAB_00537500/510` arrow
  callbacks) was not extracted. *Impact:* scroll paging speed/limits. *Resolve:*
  `DecompileAddr.java 0x00537500` and `0x00537510`.
- **UNCONFIRMED-E — `FUN_0046dfb0` iterator internals (§6.4).** Outer role
  (advance to next inventory item, `[+0x30]` = cur) is clear; the list-walk
  fields are inferred. *Impact:* item enumeration order. *Resolve:* full decode
  of `FUN_0046dfb0` + the container's inventory-list field.
- **UNCONFIRMED-F — container art surfaces `mbr_0x1a0`/`1a8` (20×20) content.**
  Confirmed they are two `cls_0x5a68d8` surfaces blitted at (10,10), one per
  player/target; what is rendered INTO them (the live container portrait?) was
  not traced. *Impact:* the (10,10) art. *Resolve:* trace writers of
  `mbr_0x1a0`/`1a8` (the `cls_0x5a68d8::meth_0x4a3a40`/`4a39a0` calls in
  Initialize).
