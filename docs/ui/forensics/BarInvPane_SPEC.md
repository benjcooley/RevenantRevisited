# TBarInvPane — Reconstruction SPEC (Pouch / quick-item slot row)

> Pixel-faithful reconstruction spec for the HUD **BarInv pane** — the row of
> 42×42 quick-use Pouch / consumable slots embedded in the bottom bar between
> the spell ring and the right-side end cap. Follow the FORENSICS_PROTOCOL
> §0–§14 template. All coordinates name their frame; every literal is cited
> `file:line` with hex(=dec).

---

## §0 — Sources & status

**Class:** `TBarInvPane` = `cls_0x5a56d4` (vtable @ `0x5a56d4`); global instance @
`0x65b028`. Naming ~85% (could be `TBarInventoryPane`/`TItemBarPane`) — see B.r5;
class identity itself is 98% (B.r5 §"TBarInvPane"). NEW retail class with **no
pre-release predecessor** (no `_CLASSDEF(TBarInvPane)` anywhere in `src/`).

**Key method addresses (vtable on `cls_0x5a56d4`):**

| role | vtable slot | retail addr | recon |
|---|---|---|---|
| Initialize | 0 | `0x52c970` | `recon/discovered/FUN_0052c970_BarInv_init.cpp` (129 B); merged view `recon/ghidra/cls_0x5a5658.cpp:1382-1402` |
| TPane base Close | 1 | `0x491970` | (TPane base, not panel-specific) |
| Commit / re-layout (template→live + recompute slot count) | 18 | `0x52ca00` | objdump `0x52ca00..0x52ca46` |
| Mark dirty (set DAT_0065b688) | 20 | `0x52ca50` | objdump `0x52ca50..0x52ca68` |
| **Draw** (the per-frame paint) | 21 | `0x52ca70` | `recon/ghidra/cls_0x5a5658.cpp:1410-1521` + objdump `0x52ca70..0x52cd5d` (Ghidra-merged into `cls_0x5a5658`; this is the **TBarInvPane** method despite the cls_0x5a5658 label) |
| Hover / per-frame update (cursor highlight) | 22 | `0x52cd80` | objdump `0x52cd80..0x52cfab` |
| Mouse handler (click / drag start) | 25 | `0x52cfb0` | objdump `0x52cfb0..0x52d2ff` |
| Drag-move handler | 26 | `0x52d6e0` | `recon/ghidra/cls_0x5a5658.cpp:1527-1577` |

**Recon files read:**
- `recon/ghidra/cls_0x5a5658.cpp` (Ghidra-merged container; lines 1382-1577 are
  TBarInvPane methods — Init, Draw, drag-move).
- `recon/ghidra/cls_0x5a5658__vftable_5a5658.cpp` (62-slot merged vtable; slot
  124 = TBarInvPane Initialize).
- `recon/discovered/FUN_0052c970_BarInv_init.cpp` (clean standalone 129-byte
  Init body).
- `recon/discovered/port_status/TBarInvPane.md` (vtable + identification trail).
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:129,191` —
  archive load + Initialize call.
- `recon/discovered/renames/agent_ui_wave2_textbar_bottom.txt:79-94,196-205` —
  identification evidence + slot 0 confirmation.
- `recon/ghidra/cls_0x5a5808.cpp:130,143-144,162,186,218-221` — TBottomBarPane
  controls BarInv visibility/SetRect.
- `recon/ghidra/cls_0x5a4494.cpp:308-326` — TPane base field map (`mbr_0x4`=x,
  `0x8`=y, `0xc`=w, `0x10`=h live; `0x14..0x20` template; `0x50`=visible).
- Raw disassembly via `objdump -d --start-address=0xNN /…/data/Revenant.exe`
  for every TBarInvPane method (Ghidra decomp lost the actual constants in two
  spots — see §6 "lea decode").
- `tools/ui/dump_dat.py bottombar.dat` → `/tmp/bottombar_dump/bottombar.json`
  (asset dims).

**Prior briefs:** B.r3 (`docs/ui/briefs/B_r3_playscreen_panes.md:52,86,224`),
B.r5 (`docs/ui/briefs/B_r5_bottom_panes.md:8,39-51,72,85,109,123-141,196-205`).

**Method-map references:** UI_METHOD_MAP §4 (`FUN_004bd680` sprite stamp = the
slot-box draw, the icon stamp), §5 (`FUN_004be2b0_CompositeBuffer` = both text
calls), §6 (`FUN_00429950` PackRGB → white text), §7 (`FUN_00419dd0` 1-int
field copy), §9 (`FUN_0046d710` by-name resource lookup = BarInvBox sprite
load), §10 (`FUN_0058b100`/`FUN_0058d252` = sprintf-family for "%d" count
text), §16 (chroma key / alpha conventions).

**Status:** `forensics-complete` for the box-row chrome, the per-slot item draw
path (icon stamp + quantity text), the per-slot hit / click / drag-start, and
the dirty-redraw gate. UNCONFIRMED for two helper bodies (`FUN_0058d252`
formatter, `FUN_00471900` item-self-draw on the slot, `FUN_004a31a0` icon-tint),
the **`pane_x`** (the pane appears to live at `pane.x = 0` / no horizontal
offset relative to the bottom bar — verified from anchor and from the y=10 /
x=0xdc literals consumed directly to the surface), and an exact name for the
font handles `0x65b010` (count font HDC) and `0x65a9cc` (value font handle).

**Errors to close (no existing port):** none — NEW retail class, no `src/`
shell. The recon agent will produce the first port.

**Snapshot note (supplementary — superseded by retail):** **No 1998/99
counterpart exists** in `src/`. `grep` of `src/` for `BarInv`/`BarInvBox`/
`TBarInvPane`/`Pouch shelf`/`item slot row` → no hits; `src/revtypes.h:1167`
forward-declares only `TBottomPane` (the container). The carried-inventory
class `src/inventory.cpp:TInventory` (used by the sidebar Inventory pane, see
`InventoryPane_SPEC.md`) is a **separate** class with a different surface (4×3
sidebar grid vs the bottom-row Pouch strip). Nothing to reuse; retail is the
sole authority for every literal below.

---

## §1 — Overview

The BarInv pane is the **row of 42×42 quick-use boxes** stamped onto the bottom
bar. Each box can hold a Pouch (a sub-container — the player's potion/key bag).
At runtime the engine walks the player's inventory list (`TPlayer.meth_0x46dfb0`
iterator) and, for any item whose `slot_id` (`field_0x7c` − `0x10b`) falls in
the pane's visible range, draws into the corresponding box: (a) the item's own
self-draw (`item.vtable+0x108(x,y,surf)`), and (b) if the item is named
**`"Pouch"`**, an extra **Pouch contents** overlay (the icon of the first item
inside the pouch) plus the contained item's **quantity** as a small white "%d"
in the box's lower-right. For any item with `value > 1` (`item.vtable+0x198`),
an additional **value** label is drawn near the top-left of the box (40 px
wide, 2 lines tall, font `0x404`).

**Instances:** exactly one (`0x65b028`); not mirrored.

**When shown:** part of the always-on play-screen HUD; constructed in
`TPlayScreen::Initialize` at call site `0x47abcf`
(`recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:191`).
Visibility is gated by the `cls_0x5a5808` (`TBottomBarPane`) parent: BottomBar's
Draw `0x52c800` calls `TBarInvPane::virt_meth_0x52ca70(this, display)`
unconditionally (`cls_0x5a5808.cpp:162`), but the **paint body itself**
early-outs unless `(param_1 != display) || (mbr_0x50 != 0) || (DAT_0065b688 !=
0)` (`cls_0x5a5658.cpp:1434-1435`). I.e. it draws when (a) compositing to a
scratch target, (b) the pane is visible, or (c) the global dirty flag is set.

**Plain language:** the row of small boxes on the bottom bar where you stuff
your Pouches (potion/key bags). Each box shows the item icon, plus — if it is a
Pouch — a tiny number showing how many of the contained item are inside.
Clicking a box at runtime uses or activates the item; click-and-drag picks the
item up to drop it elsewhere.

**Relationship to siblings:**
- Hosted on `TBottomBarPane` (`cls_0x5a5808`) — same surface, drawn directly
  after the UtilityBar/BarEndCap chrome (`cls_0x5a5808.cpp:162`). The bottom
  bar owns the chrome backdrop; the BarInv stamps on top of it.
- Its **template width / height** (`mbr_0x1c`, `mbr_0x20` at struct offsets
  `+0x24`, `+0x28`) are **set by the bottom bar's SetRect** (`0x52c930`,
  `cls_0x5a5808.cpp:218-219`) — the BarInv resizes in lock-step with the
  bottom bar. Its template x/y default to `0` (BL-anchored same as the bottom
  bar).
- The "use item on target" path delegates to `0x4701f0` (player) + `0x5496a0`
  (item-use registry at `0x65b140`) inside the click handler.

---

## §2 — Asset roster

Archive: `bottombar.dat` (loaded into `DAT_0065a570` by `TPlayScreen::Initialize`
at `:129` from `s_BottomBar_dat_005d70ec`). Measured with `tools/ui/dump_dat.py
bottombar.dat` → `/tmp/bottombar_dump/bottombar.json`.

| asset | archive | entry name | WxH (measured) | regx,regy | flags | kc | role | source rects | cite |
|---|---|---|---|---|---|---|---|---|---|
| **BarInvBox** | bottombar.dat | `BarInvBox` (idx 1) | **42×42** | 0,0 | `0x2` BM_15BIT | `0x0` | **slot box chrome**; one stamp per visible slot; stored in `mbr_0x84` | full bitmap | dump idx 1; Init `0x52c981 push s_BarInvBox_005e37b8 / 0x52c993 call meth_0x46d710` (loads handle from bottombar archive via by-name lookup) → `mbr_0x84` (`0x52c9a4 mov eax, 0x84(%esi)`) |

**Not owned by this pane** (drawn by the **items themselves** through the
`item.vtable+0x108` / `+0x130` / `+0x198` getters — out of scope for the BarInv
chrome, but listed for context):
| asset | role | owner |
|---|---|---|
| per-item icon (`item.InventoryImage()` = `+0x130`) | the item's icon (e.g. the Pouch's painted face) | TObjectInstance / inventory item — see [[project-ui-portrait-and-icons]] (`GetInvImage`) |

**Notes:**
- `BarInvBox` is **opaque 15-bit** (`flags=0x2`, `kc=0`) — drawn with no chroma
  key. (kc=0 with no `BM_TRANSPARENT` ⇒ solid copy.)
- The item icons (drawn via the item's own `+0x108` and `+0x130` getters) are
  alpha-keyed in-engine (via the temp 20×20 surface set up in §6); whether the
  icon archive uses a chroma key or real alpha is determined by the item's
  inventory-image bitmap, not by this pane.
- There is **no Pouch icon owned by this pane**; the Pouch's own face is drawn
  by `item.vtable+0x108`, and the contained item's icon is fetched via
  `inner_item.vtable+0x130` (the `InventoryImage()`-ish getter, [[project-
  ui-portrait-and-icons]]).

### Source-rect map (BarInvBox)

`BarInvBox` is consumed whole — no atlas, no sub-rects.

```
BarInvBox 42x42  (single sprite, opaque 15-bit, drawn N times along the row)
 ┌──────────────────────────────────────────┐ 42
 │ slot/box chrome (riveted-iron tile)        │
 └──────────────────────────────────────────┘
   42
```

---

## §3 — Coordinate frames & surfaces

### Pane rect (TPane base fields)

The Ghidra dump for `cls_0x5a5658` covers **two merged classes** — the
TBarInvPane fields share offsets with the auto-map fields. For the TBarInvPane
instance @ `0x65b028`, the TPane base field map (verified against
`cls_0x5a4494.cpp:308-326`, the TPane base init that copies template→live):

| field | meaning | typical value | cite |
|---|---|---|---|
| `mbr_0x4` (x) | pane x, live | **`0`** (default; no explicit set — BL of bottom bar) | TPane base init `0x491900` copies from `mbr_0x14` (default 0 — no ctor for TBarInvPane found in extracted recon) |
| `mbr_0x8` (y) | pane y, live | **`0x1a4 = 420`** (BL of screen; tracks bottom bar y) | bottom-bar `cls_0x5a5808.cpp:32`; SetRect propagates y via the bar |
| `mbr_0xc` (w) | pane width, live | **`0x280 = 640`** @ Classic; tracks display width via bottom-bar SetRect | `cls_0x5a5808.cpp:218 DAT_0065b02c._24_4_ = param_1` (where `_24_4_` = template `mbr_0x1c`, copied to live `mbr_0xc` on next layout) |
| `mbr_0x10` (h) | pane height, live | **`0x3c = 60`** (tracks bottom bar h) | `cls_0x5a5808.cpp:219 DAT_0065b02c._28_4_ = in_stack_00000008` |
| `mbr_0x14/0x18/0x1c/0x20` | template x/y/w/h | mirror live; updated by bar SetRect | `cls_0x5a5808.cpp:218-219`; Layout `0x52ca00` copies template→live |
| `mbr_0x40` | "initialized" flag (TPane base) | 0 → 1 after first Init | `cls_0x5a4494.cpp:311` |
| `mbr_0x48/0x4c` | "needs redraw" flags (TPane base) | toggled by bar OnShow | `cls_0x5a5808.cpp:147-148` |
| `mbr_0x50` | visible flag (TPane base) | 0/1 | `cls_0x5a5658.cpp:1434` (Draw early-out gate) |

### TBarInvPane-specific fields (derived from Init `0x52c970` + Draw `0x52ca70`)

| field | meaning | init value | cite |
|---|---|---|---|
| `mbr_0x60` | **active player** pointer (`TPlayer*`, the inventory owner) | unset by Init (left as ctor-zero); set externally when player is bound | `cls_0x5a5658.cpp:1447 if (mbr_0x60 == 0) return` (Draw early-out for no-player); `:1539` (drag-handler also checks) |
| `mbr_0x64` | **selected slot id** (the `field_7c` of the item currently grabbed/clicked, biased by `+0x10b`); `-1 = none` | `-1` (`0xffffffff`) | Init `0x52c989` `mov %eax,0x64(%esi)` after `orl $-1,%eax` |
| `mbr_0x68` | **drag held-item id**; `-1 = none` | `-1` | Init `0x52c986`; cleared by Draw `:1515` `if (int)mbr_0x64 < 0 then mbr_0x68 = -1` |
| `mbr_0x6c` | **drag-active flag** (`0` = no drag in flight; `1` = dragging) | `0` | Init `0x52c98c`; set by drag-move `:1573` `mbr_0x6c = 1`; checked in Draw `:1462` to skip the held item |
| `mbr_0x70` | **last mouse x in pane** (for drag delta) | (ctor zero) | drag handler `:1543` `iVar1 = mbr_0x70 - param_2` |
| `mbr_0x74` | **last mouse y in pane** | (ctor zero) | drag handler `:1548` `iVar1 = mbr_0x74 - in_stack_0000000c` |
| `mbr_0x84` | **BarInvBox sprite handle** (`bottombar["BarInvBox"]`, 42×42) | by-name lookup in Init | Init `0x52c9a4` |
| `mbr_0x88` | **page start slot index** (the slot_index of the leftmost visible box) | `0` | Init `0x52c9ba` |
| `mbr_0x8c` | **visible slot count** = `(DAT_0065b644 − 0xdc) / 0x2d` (i.e. `(pane_w − 220) / 45`) | computed | Init `0x52c99e..0x52c9df`; also Layout `0x52ca39`; also Draw `:1437` |

`DAT_0065b644` and `DAT_0065b648`: external "live width / height" globals read
by Init (`0x52c99e`, `0x52c998`). The Init then calls `vtable+0x20(width,
height)` — almost certainly a SetSize-equivalent. **`DAT_0065b644`** is the
live display/pane width and **`DAT_0065b648`** is the live display height (the
recon naming convention: the same globals are referenced by Layout `0x52ca0f`
and Draw `0x52ca9d` for the slot count recompute, so they are the panel's
authoritative width source). UNCONFIRMED: who writes `DAT_0065b644` (likely
display-side resize or `TPlayScreen` layout) — flagged §14.

`DAT_0065b688`: the **global dirty flag** for BarInv. Written by:
- `cls_0x5a5658::virt_meth_0x47b4d0` (TPlayScreen Pulse, recon `_data.txt:130274`)
- `meth_0x519230` / `meth_0x5199b0` (script item-use paths)
- `virt_meth_0x52ca50` (this class's slot 20 — Mark-dirty)
- `virt_meth_0x5444a0` (QuickSpell-related)

Read by `virt_meth_0x52ca70` (Draw) at `:1435` as the third early-out gate.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | — | `(0, 0)` | final pixels (640×480 Classic; greedy at higher res) | — |
| **bar-pane** | screen | **BL (bottom-anchored)** | `x = 0` (left-anchored); `y = display_h − 0x3c(=60)` (= `0x1a4` @ Classic) | the parent bottom-bar pane | `cls_0x5a5808.cpp:32,34,36,39`; BottomBarPane_SPEC §3 |
| **barinv-pane** | bar-pane | TL of bar-pane (it overlays the bar 1:1) | **`(0, 0)` in bar-pane** — pane rect inherits bar-pane rect via SetRect propagation (`cls_0x5a5808.cpp:218-219` sets BarInv template w/h to bar's w/h; pane x/y default to 0/0 in template, copied to live) | the BarInv pane (same rect as the bar at Classic 640×60) | implicit (no explicit ctor for TBarInvPane found in extracted recon); inferred from SetRect-propagation + the fact that all drawing dx values pass directly to the surface w/o subtracting a pane offset (e.g. `0xdc=220` literal goes straight to `meth_0x4bd680`'s dst_x, `:1442`) |
| **draw-target** | passed in (`param_1`) | — | varies — either the display `PTR_DAT_005d79e0` or a scratch surface; in both cases the dst coords are **bar-pane-local** because the bar draws BarInv with that target (compose-to-target contract, NOMENCLATURE §3) | every dx/dy in §4 below | `cls_0x5a5808.cpp:162,186` `cls_0x5a5658::virt_meth_0x52ca70(&DAT_0065b028, surface)` |
| **slot(N)** | barinv-pane | TL | barinv-local `(0xdc + 0x2d·N, 0xa)` = `(220 + 45·N, 10)` (slot index N = `field_7c − mbr_0x88 − 0x10b`, visible position 0..count-1) | one 42×42 box + the item drawn into it | Init box-stamp loop `0x52cacc-0x52cae0` (`leal 0xdc(...), %edi` start + `0x2d` step); Draw box-stamp loop `:1440-1444` |
| **source-rect** | each bitmap | TL | `(0,0)` | BarInvBox 42×42 consumed whole | dump idx 1 |

**Anchor detection:** the literal `0xdc = 220` is the **starting x of the first
slot**, expressed in `param_1` (the draw target). Because BarInv is drawn
directly onto the bottom-bar surface (which itself has `pane.x = 0`), this `220`
is also screen-x at Classic. The slot row is therefore **left-anchored within
the bar at x = 220** (constant `220` ⇒ TL/left), and the **VISIBLE COUNT
floats** with width: `count = (width − 220) / 45`. The bar's right end is
covered by the BarEndCap `10×60` at `(width−10, 0)` — the slot row terminates
visually before the cap because the count formula leaves room.

**Composition formula to screen** (at Classic):
- slot N box dst (screen): `x = 0 + 220 + 45·N`, `y = (display_h − 60) + 10`
  = `(220 + 45N, 430)`.
- Per-slot item icon (Pouch contents stamp): `x = 220 + 45·N`, `y = 430 + 20`
  = `(220 + 45N, 450)`.
- Per-slot value text TL (top-left corner): `x = 220 + 45·N`, `y = 430 + 0`
  = `(220 + 45N, 430)`.
- Per-slot Pouch quantity text TL: `x = 220 + 45·N + 20`, `y = 430 + 26`
  = `(240 + 45N, 456)`.

### Surfaces (direct-renderer contract)

`virt_meth_0x52ca70(this, param_1)` writes directly into `param_1` — the bottom
bar passes its own target (either the display or a scratch surface) to BarInv,
so BarInv never owns its own compose surface. (Compare InventorySidebarPane
which DOES own a 20×20 temp surface for icon tinting — BarInv allocates the same
20×20 tint scratch *per paint*, see §6, but composes directly to the parent.)

### Layout diagram (barinv-pane-local; origin = screen `(0, display_h−60)`)

Width = `w` (= bar live width, 640 @ Classic). Visible slot count =
`(w − 220) / 45`. Origin of slot N (N = 0, 1, …, count−1): `(220 + 45N, 10)`.

```
barinv-pane (= bar-pane rect 640x60 @ Classic)
0           220 (=0xdc, slot0 x)                      w-10  w
├──────────┬─────┬──────┬──────┬──────┬───            ────┬─────┤   y=0
│          │ box │ box  │ box  │ box  │   …               │ end │
│ (spell   │ 42  │ 42x42│      │      │                   │ cap │
│  ring    │ 42  │      │      │      │                   │ 10  │
│  area)   │     │      │      │      │                   │ x60 │
├──────────┴─────┴──────┴──────┴──────┴───            ────┴─────┤   y=60
            ▲     ▲      ▲      ▲      …  count = (w-220)/45
            220   265    310    355  +45/slot
                        (y=10 top, h=42 → bottom=52)

Per slot (boxed-out view, slot N at x = 220+45N):
        0  (slot-local x)
        0 ┌─────────────────┐ y=0   ← slot box (barinv y = 10)
          │                 │
          │ [value text]    │ y=10..10+2*font_h, w=40 (font 0x404)
          │                 │
          │   [icon if      │
          │    Pouch:       │ y=20..40, 20x20 (drawmode 0x100 transparent)
          │    a 20x20      │
          │    contents     │
          │    overlay]     │
          │             [%d]│ y=26..26+font_h+2, x=20..40 (font 0x400)
        42└─────────────────┘ y=42 (barinv y = 52)
            42
```

---

## §4 — Static element layout

All coordinates **barinv-pane-local** (compose with screen `(0, display_h − 60)`
per §3). Single instance — no mirror columns. `N` = the visible slot index
(0 ≤ N < `mbr_0x8c`). One row per drawn element:

| element | space | (x, y) | (w, h) | source rect / data | drawmode | cite |
|---|---|---|---|---|---|---|
| slot box (per slot N) | barinv-pane | (`0xdc + 0x2d·N`) = (220 + 45·N, **10**) | (42, 42) | BarInvBox whole | unspecified (likely `0` plain copy — the 5th `meth_0x4bd680` arg is the stack-uninit `in_stack_00000008`) | Draw box loop `:1442` `meth_0x4bd680(param_1, iVar8, 10, mbr_0x84, in_stack_00000008)` with `iVar8 = 0xdc + 0x2d·N` (`:1440-1444`) |
| item self-draw (per item visible at slot N) | barinv-pane (passed to item) | (220 + 45·N, **10**) | item-chosen | item's own `vtable+0x108(x, y, surf)` → e.g. the Pouch's painted face | item-chosen | Draw `:1473`: `(*((this_00->TPlayScreen).TScreen.vftptr_0x0)->FUN_00471900_264)(iVar7, 10, param_1)` — args `(x=iVar7, y=10, surf=param_1)` to the **item's slot-draw** virtual at `+0x108` (player vtable offset 264 = 0x108) |
| Pouch contents icon overlay (only when item name == `"Pouch"` AND inner item has an inventory image) | barinv-pane | (220 + 45·N, **30**) | (20, 20) | 20×20 scratch surface `piVar3` filled by `FUN_004a31a0(inner_inv_image, piVar3, DAT_006668d0)` | **`0x100` DM_TRANSPARENT** | Draw `:1481` `meth_0x4bd680(param_1, iVar7, 0x1e, piVar3, 0x100)` |
| Pouch quantity text (only when Pouch) | barinv-pane | (220 + 45·N + 20, **36**) | (20, font_h + 2) | `"%d"` of `TPlayScreen::meth_0x470040(this_00)` (player's pouch-count getter) | text — see §8 | Draw `:1491-1492`: `meth_0x4be2b0_CompositeBuffer(param_1, dx=char(iVar8+1), dy=0x24, w=0x14, h=iVar6+2, …)` |
| value/durability text (any item with value > 1) | barinv-pane | (220 + 45·N, **10**) | (40, 2 · font_h) | `"%d"` of `item.vtable+0x198()` (the item's value getter, e.g. wand charges) | text — see §8 | Draw `:1506`: `meth_0x4be2b0_CompositeBuffer(param_1, dx=char(iVar7), dy=10, w=0x28, h=iVar8 << 1, …)` |

**Cell-origin verification (cross-check, two ways).**

1. **Forward (Draw)** — `cls_0x5a5658.cpp:1440-1444` / objdump `0x52cac7-0x52caf3`:
   ```
   iVar8 = 0xdc;                      ; box x for slot 0
   loop while iVar7 < mbr_0x8c:
     meth_0x4bd680(param_1, iVar8, 10, mbr_0x84, …);   ; box dst (x, 10)
     iVar7 += 1; iVar8 += 0x2d;       ; +45 per slot
   ```
   So box origin `(0xdc + 0x2d·N, 0xa) = (220 + 45N, 10)`.

2. **Inverse (hover / hit-test in slot 22, `0x52cd80`)** — objdump
   `0x52cdac-0x52ce05`:
   ```
   esi = cursor_x_global(0x668510) − mbr_0x4 − 0xdc        ; local x − 220
   edi = cursor_y_global(0x668514) − mbr_0x8 − 0xa         ; local y − 10
   if (esi < 0 || edi < 0 || edi >= 0x28) → reject         ; y interior 40 (NOT 42!)
   slot_visible_index = esi / 0x2d                          ; floor((x − 220) / 45)
   if (slot_visible_index < mbr_0x88 || slot_visible_index >= mbr_0x88 + mbr_0x8c)
       → reject
   slot_id = slot_visible_index + mbr_0x88 + 0x10b
   ```
   Same constants on the inverse: origin (220, 10), pitch 45 × ?, **y interior
   40** (`0x28`). Note the y interior is **40** even though the box BITMAP is
   42 — the cursor reject `edi >= 0x28` clamps to a 40px-tall hit region (the
   chrome bezel pixels are inert).

3. **Drag-move handler (`0x52d6e0`)** — `cls_0x5a5658.cpp:1559-1560`:
   ```
   iVar4 = (mbr_0x70 − 0xdc) % 0x2d;   ; previous-mouse local-x mod pitch_x
   iVar5 = (mbr_0x74 −  0xa) % 0x28;   ; previous-mouse local-y mod 40
   ```
   Same origin (220, 10), same pitch_x 45, **same y interior 40**. Triple-
   confirmed.

| property | value | cite |
|---|---|---|
| slot row origin (barinv-pane-local, slot 0 TL) | **(220, 10)** = `(0xdc, 0xa)` | Init/Draw/Hover/Drag all agree |
| box bitmap size | **42 × 42** | dump idx 1 |
| slot pitch x | **45** = `0x2d` | Draw `:1444`; Hover `0x52cee0` (the `*5*9` form); Drag `:1559` |
| slot pitch y | **N/A** (single row) | — |
| hit-test interior | **40 × 40** (cursor must satisfy `(x−220) − N·45 ≥ 0 && (x−220) − N·45 < 45` AND `(y−10) < 40`) | Hover `0x52cdf6 cmp $0x28`; Drag `:1560 mbr_0x74 − 0xa mod 0x28` |
| inter-slot gap | x: 45 − 42 = **3** px | derived |
| count formula | `(width − 220) / 45` ⇒ at width 640: `(640−220)/45 = 9` (integer-div), at 1920: `(1920−220)/45 = 37` | Init `0x52c9c4-0x52c9df`; Layout `0x52ca1b-0x52ca37` |
| page-start (`mbr_0x88`) | default **0**; written elsewhere (script-driven page change — not in this method) | Init `0x52c9ba` `movl $0, 0x88(%esi)` |
| slot-id bias | `slot_id = visible_index + 0x10b` (= 267) | Hover `0x52ce64 addl $0x10b, %eax`; Draw `:1465` `field_7c − 0x10b` to invert |

> **Why is the box drawn at y=10 (within a 60px-tall bar)?** The slot row is
> visually centered-ish in the bar: 60 − 10 − 42 = 8 px clear at the bottom.
> The y = 10 is a constant, not derived (`pushl $0xa` literal in the box-
> loop, `0x52cadd`). The slot interior y range is `[10, 50)` (the lower 2 px
> of the 42-px box are the bezel).

---

## §5 — Draw order / composition

The pane has **one** paint vtable slot: `virt_meth_0x52ca70` (slot 21,
`0x52ca70`). It is called by the bottom bar's Draw (`cls_0x5a5808.cpp:162`)
between the bar chrome (UtilityBar + BarEndCap) and the quick-spell ring. The
bar passes the active target (display or scratch); BarInv writes into it.

**`virt_meth_0x52ca70(this, param_1)`** (`cls_0x5a5658.cpp:1410-1521` / objdump
`0x52ca70..0x52cd5d`), in order:

1. **Early-out gate** (`:1434-1435`): if `param_1 == display` AND `mbr_0x50 ==
   0` AND `DAT_0065b688 == 0` → skip to step 9 (`:1519` base post-draw).
   I.e. when drawing to the display, draw only if visible or dirty; when
   drawing to a scratch (off-display) target, always draw.
2. **Recompute slot count** (`:1437-1438`): `mbr_0x8c = (DAT_0065b644 − 0xdc) /
   0x2d`. (Defensive — also done in Layout `0x52ca00` and Init.)
3. **Stamp the slot boxes** (`:1439-1446`): `for N in 0..mbr_0x8c-1: meth_0x4bd680(param_1, 0xdc + 0x2d·N, 0xa, mbr_0x84, in_stack_00000008)`.
   This lays the BarInvBox chrome row.
4. **No-player guard** (`:1447-1448`): if `mbr_0x60 == 0` → skip the per-item
   loop and return (still calls the post-draw at `:1519`).
5. **Allocate 20×20 tint scratch** (`:1450-1452`): `piVar3 = FUN_004a1ec0(0x14,
   0x14, display.format & 0x3001f, 0)` and set its key color
   `piVar3[6] = _DAT_006668d0`. This scratch is reused across the per-item loop.
6. **Init player-inventory iterator** (`:1451-1459`): build a 24-byte iterator
   struct on the stack (`local_6c`, `cls_0x477870`), seeded with `mbr_0x4 =
   mbr_0x60` (the player), and step it once via `meth_0x46dfb0`.
7. **Per-item loop** (`:1461-1513`): while iterator returns a non-null item
   pointer `this_00`:
   - **Skip the held item** (`:1462-1464`): if `mbr_0x6c != 0` (dragging) AND
     this item's slot_id (`field_7c − 0x10b`) equals `mbr_0x64` (the selected
     slot), skip — the held item is rendered separately by the drag cursor.
   - **Compute slot_index** (`:1465`): `slot_index = field_7c − 0x10b`.
   - **Visibility window** (`:1466`): if not (`mbr_0x88 ≤ slot_index <
     mbr_0x88 + mbr_0x8c`) skip.
   - **Compute box x** (`:1470-1472`): `iVar8 = (field_7c − mbr_0x88) × 45 −
     11795`. *(This is the disassembly-direct form;
     see §6 for the lea-decode confirming `iVar8 = 220 + 45·visible_index`.)*
   - **"In-cursor-held" gate** (`:1467-1469`): `iVar7 = (*((this_00->TPlayScreen).TScreen.vftptr_0x0)->virt_meth_0x4ce00_?)
     ...` Actually the recon shows `iVar7 = (**(code **)(*piVar4 + 0xd8))()`
     where `piVar4 = TPlayScreen::meth_0x46e8a0(&this_00->TPlayScreen)` (a
     getter on `this_00`). The result `iVar7` is a "held / in-cursor" flag;
     if non-zero AND the target is the display, skip the draw (the held item
     is drawn elsewhere).
   - **Item self-draw** (`:1473-1474`): `(*((this_00).vtable)->FUN_00471900_264)(iVar7=barx, 10, param_1)`
     — calls `item.vtable+0x108(x, y, surf)`. The item draws its own icon
     into the box; **the BarInv pane does NOT draw a generic item icon
     itself** — it relies on the item's self-paint.
   - **Pouch overlay** (`:1475-1493`): `FUN_0059a530_stricmp(this_00.mbr_0x38,
     "Pouch")` — if the item's name is `"Pouch"`:
     - `piVar4 = cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00)` — get the
       player object holding this pouch? (Actually `this_00` IS a TPlayer-cast
       — odd; this likely gets the held/contained item or the pouch's first
       inner item — see §14 UNCONFIRMED-A).
     - `piVar4 = (*(piVar4.vtable)+0x130)()` — call the inner item's
       `InventoryImage()` getter (`+0x130`, the live inv-image surface; see
       [[project-ui-portrait-and-icons]]).
     - `FUN_004a31a0(piVar4, piVar3, DAT_006668d0)` — tint/copy the inner
       inv-image into the 20×20 scratch with the global key color.
     - `meth_0x4bd680(param_1, iVar7=barx, 0x1e=30, piVar3, 0x100=DM_TRANSPARENT)`
       — stamp the 20×20 scratch at (220 + 45N, 30) with chroma-key
       transparency.
     - Format quantity: `uVar5 = TPlayScreen::meth_0x470040(this_00); FUN_0058d252(uVar5, acStack_50, 10)`
       — itoa-like into `acStack_50`. (`FUN_0058d252` ≈ a 10-base integer-to-
       string with a 10-char buffer cap — UNCONFIRMED.)
     - PackRGB white: `cls_0x429950(&cStack_70, 0xff, 0xff)` →
       PackRGB(R=0xff,G=0xff,B=0xff) per UI_METHOD_MAP §6 (note: the recon
       only shows two args because Ghidra dropped one; the disassembly at
       `0x52cc84-0x52cc8e` pushes three `0xff` bytes — three are needed for
       the (B,G,R) call shape).
     - SetField: `cls_0x419dd0` copies the packed color (UI_METHOD_MAP §7).
     - Get font line height: `iVar6 = cls_0x52d870::meth_0x52d870(&UNK_0065abf8.field_0x418)`
       — this is the font's pixel line height; the text cell h = `iVar6 + 2`.
     - Draw text: `meth_0x4be2b0_CompositeBuffer(param_1, dx=char(iVar8+1)=barx+1,
       dy=0x24=36, w=0x14=20, h=iVar6+2, text=acStack_50, color=cStack_70, font=0,
       line_rect_or_flags=…)`. See §8 for the full call-site decode.
   - **Value overlay** (always, on any item, `:1494-1507`): `uVar2 =
     (*((this_00).vtable)->virt_meth_0x477db0_408)(this_00)` — call `item.vtable+0x198()` (slot 408
     ≈ 0x198 — actually slot 102, addr `+0x198`). Read as "value/durability/charges".
     If `value > 1`:
     - itoa: `FUN_0058d252(value, acStack_50, 10)`.
     - SetField for color (no PackRGB this time — color comes from a
       different source than white).
     - Font height: `iVar8 = cls_0x52d870::meth_0x52d870(…)`; cell h = `iVar8
       << 1` = **two-line tall** (font height × 2).
     - Draw: `meth_0x4be2b0_CompositeBuffer(param_1, dx=char(iVar7)=barx, dy=10,
       w=0x28=40, h=iVar8·2, text=acStack_50, …)`. See §8.
8. **Drag-state reset** (`:1514-1517`): if `mbr_0x64 < 0` (no selection) then
   `mbr_0x68 = -1`; free the 20×20 tint scratch (`FUN_004830f0(piVar3)`).
9. **Base post-draw / mark-drawn** (`:1519`): `(*vtable->slot44 = 0x444fc0)(this)`.

**Net visible order (bottom → top), per slot:**
BarInvBox chrome → item self-draw (e.g. the Pouch face) → if Pouch: 20×20
inner-item icon overlay → if Pouch: %d quantity (white) → if value>1: %d value
(40×2h, color other than white).

> No blit-effect flush from this slot — the bar's own `Draw` calls
> `FUN_004aacb0_BlitEffect_Iterate` over the **bar rect** after all sibling
> draws (`cls_0x5a5808.cpp:166`), so BarInv's text shadows (font `0x400`/`0x404`
> have bit `0x400` set — the 3-pass shadow gate, UI_METHOD_MAP §5) flush there.

---

## §6 — Algorithms (pseudocode per helper)

### Slot-x decode (the suspicious `lea -0x2e13(...)` Ghidra emits)

The Ghidra decomp at `cls_0x5a5658.cpp:1470-1472` reads "`iVar8 = (field_7c −
mbr_0x88) × 0x2d`; `iVar7 = iVar8 + −0x2e13`", which makes no sense as a pixel
x. The raw disassembly is:

```
0x52cbdb: movl 0x88(%ebp), %ecx                ; ecx = mbr_0x88
0x52cbe3: subl %ecx, %eax                       ; eax = field_7c − mbr_0x88
0x52cbea: leal (%eax,%eax,4), %eax              ; eax *= 5
0x52cbed: leal -0x2e13(%eax,%eax,8), %edi       ; edi = eax + eax*8 − 0x2e13 = eax*9 − 0x2e13
                                                ;     = (field_7c − mbr_0x88) × 45 − 11795
```

Because `field_7c` ranges over `[0x10b, 0x10b + slot_count)` (slot ids are
biased by `+0x10b`) and `mbr_0x88` defaults to 0, evaluating at visible-index
`N` (where `field_7c = 0x10b + N + mbr_0x88` for an item at visible position N):
```
edi = (0x10b + N + mbr_0x88 − mbr_0x88) × 45 − 11795
    = (0x10b + N) × 45 − 11795
    = 0x10b × 45 + 45·N − 11795
    = 12015 + 45·N − 11795
    = 220 + 45·N
    = 0xdc + 0x2d·N                       ← same as the box-stamp x
```

So **`iVar7 = 220 + 45·N`** — the icon stamp lands at the same x as the box's
left edge. Ghidra's decomp is correct mathematically but un-readable; cite this
decode whenever §4 prints `220 + 45N`.

### Box-row pseudocode (Draw step 3)

```
# meth_0x4bd680(surf, x, y, src, drawmode) — opaque sprite stamp (UI_METHOD_MAP §4)
def draw_box_row(target, count, barinvbox):
    x = 0xdc        # 220
    for N in range(count):
        target.Stamp(dst=(x, 0xa), src=barinvbox, mode=in_stack_00000008)   # uninitialized — see §14
        x += 0x2d   # +45
    # cite cls_0x5a5658.cpp:1439-1446; objdump 0x52cac7-0x52caf3
```

### Per-item draw (Draw step 7), Pouch branch

```
# Inner-item icon to tint scratch then stamp to slot
def draw_pouch_overlay(target, slot_x, item, tint_scratch, key_color):
    inner = item.GetInner()                       # via meth_0x4701f0 — see §14 UNCONFIRMED-A
    if inner is None: return
    inv_img = inner.GetInventoryImage()           # vtable +0x130 (UI_METHOD_MAP / project-ui-portrait-and-icons)
    if inv_img is None: return
    FUN_004a31a0(inv_img, tint_scratch, key_color)  # tint/copy 20x20 with chroma key — body UNCONFIRMED
    target.Stamp(dst=(slot_x, 0x1e), src=tint_scratch, mode=0x100)   # DM_TRANSPARENT
    # cite cls_0x5a5658.cpp:1478-1481
```

### Per-item value overlay (Draw step 7, value branch)

```
def draw_value_overlay(target, slot_x, item, font_height_helper):
    value = item.GetValue()       # vtable +0x198 (e.g. wand charges, durability — UNCONFIRMED-B)
    if value <= 1: return
    buf = itoa10(value)           # FUN_0058d252 — see §14
    color = (read from struct &UNK_0065abf8.field_0x418 + 0x444e20 helper — see §14)
    h = font_height_helper(&UNK_0065abf8.field_0x418, font_handle=0x65a9cc) * 2  # 2 lines tall
    target.DrawText(dst_xy=(slot_x, 0xa), cell=(0x28, h),
                    text=buf, color=color, font_id=0x404, flags=0x10)
    # cite cls_0x5a5658.cpp:1494-1506; objdump 0x52ccdb-0x52cd2c
```

### Call-site literal tables

`meth_0x4bd680` (sprite stamp, UI_METHOD_MAP §4):

| call | dx (frame) | dy | src handle | drawmode | cite |
|---|---|---|---|---|---|
| box row stamp | `0xdc + 0x2d·N` (barinv-pane) | `0xa = 10` | `mbr_0x84` = BarInvBox | `in_stack_00000008` ⚠ (uninitialized 5th arg in Ghidra; likely **0** opaque copy — UNCONFIRMED-C) | `:1442` / `0x52cae0` |
| Pouch overlay stamp | `iVar7 = 0xdc + 0x2d·N` | `0x1e = 30` | `piVar3` (20×20 tint scratch) | `0x100 = DM_TRANSPARENT` | `:1481` / `0x52cc5d` |

`meth_0x4be2b0_CompositeBuffer` (text, UI_METHOD_MAP §5):

| call | dx (frame) | dy | w | h | text | color source | font_id | flags | cite |
|---|---|---|---|---|---|---|---|---|---|
| Pouch quantity | `0xdc + 0x2d·N + 1` (barinv-pane) | `0x24 = 36` | `0x14 = 20` | `iVar6 + 2` (font line h + 2) | `acStack_50` = `"%d"` of `TPlayScreen::meth_0x470040(item)` | `cStack_70` = PackRGB(0xff,0xff,0xff) = **white** | **`0x400`** (= shadow bit only; HDC index 0 = fallback default font) | `0x20` | `:1491-1492` / `0x52ccc7` |
| Value (any item, val>1) | `0xdc + 0x2d·N` (barinv-pane) | `0xa = 10` | `0x28 = 40` | `iVar8 << 1` (font line h × 2) | `acStack_50` = `"%d"` of `item.vtable+0x198` | (passed-through field copy, source UNCONFIRMED-B; not PackRGB-built) | **`0x404`** (= shadow bit + HDC index 4) | `0x10` | `:1506` / `0x52cd28` |

UI_METHOD_MAP §5 says font index bit `0x400` enables the 3-pass black drop
shadow; both BarInv text calls have it set ⇒ both get the **1-px down-right
shadow**. The HDC index is the **low 8 bits** of the font id (`0x404 & 0xff =
4`; `0x400 & 0xff = 0` falls back to `DAT_0065b024`).

---

## §7 — Effects & shadows

- **No drop shadow** on the slot box chrome — the box stamp does not set up
  `FUN_00438d80`. (Compare TPlyrStatusBar which casts a (4,4) shadow per side.)
- **No drop shadow** on the Pouch inner-item icon stamp either — drawmode
  `0x100 = DM_TRANSPARENT` is a keyed copy, not a shadowed blit.
- **Text 3-pass shadow** on BOTH text calls (Pouch quantity + value overlay):
  font ids `0x400` and `0x404` have bit `0x400` set, which `FUN_004be2b0`
  treats as the shadow-pass gate (UI_METHOD_MAP §5 step 8). Three black
  passes at base / +1x / +1y, then the colored pass — a 1-px BR drop shadow.
- **Chroma key:** the BarInvBox bitmap is **opaque** (`flags=0x2`, `kc=0`) —
  do not apply the global magenta key (you'd punch holes in the rivets).
  The Pouch overlay uses an **alpha-keyed** path via the 20×20 tint scratch
  (`piVar3[6] = _DAT_006668d0` = the global "transparent" sentinel) — port
  to real alpha (UI_METHOD_MAP §16).
- **Blit-effect flush:** TBarInvPane never calls
  `FUN_004aacb0_BlitEffect_Iterate`. The bottom-bar parent flushes over the
  bar rect after BarInv + QuickSpell draw (`cls_0x5a5808.cpp:166`).

---

## §8 — Text rendering

Two text cells per visible Pouch item; one text cell per any-value item. All
coordinates **barinv-pane-local** (compose with `(0, display_h − 60)` for
screen).

| string | cell (space, x, y, w, h) | font | px / id | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| Pouch quantity (per Pouch slot N) | barinv-pane (`0xdc + 0x2d·N + 1`, `0x24=36`, `0x14=20`, `font_lineH + 2`) | **HDC index 0** (low byte of `0x400`); falls back to `DAT_0065b024` (the default-font HDC, UI_METHOD_MAP §5 step 2) | font id `0x400` | **white** (R=0xff, G=0xff, B=0xff via `FUN_00429950` PackRGB at `0x52cc84-0x52cc8e`) | flags `0x20` → UI_METHOD_MAP §5 step 6e maps `&0x20` to `\|= 0x20` (a base DT_TOP single-line) — i.e. **left-aligned** (no `&2 / &4` set ⇒ defaults to DT_LEFT) | top (no DT_VCENTER) — text hangs from cell top | **yes** (`font_id & 0x400` → 3-pass black) | `"%d"` (via `FUN_0058d252(value, buf, 10)`) | `TPlayScreen::meth_0x470040(item)` — the player's "item-count" getter for this Pouch | `:1483-1492` / objdump `0x52cc6b-0x52ccc7` |
| Item value (any item, val>1) | barinv-pane (`0xdc + 0x2d·N`, `0xa=10`, `0x28=40`, `2 × font_lineH`) | **HDC index 4** (low byte of `0x404`) | font id `0x404` | (UNCONFIRMED-B — not white-packed; passed via a field-copy from `0x65abc4` and a `FUN_00444e20` helper at `0x52cd01`) | flags `0x10` → UI_METHOD_MAP §5 step 6e maps `&0x10` to `\|= 0x20` — base DT_TOP single-line; **left-aligned** by default | top | **yes** (`font_id & 0x400`) | `"%d"` (via `FUN_0058d252(value, buf, 10)`) | `item.vtable+0x198()` — value getter (charges/durability) | `:1496-1506` / objdump `0x52cce6-0x52cd2c` |

> **Cell w=20 for the count vs cell w=40 for the value.** The count cell is
> aligned to the icon's bottom-right (offset +20 in x, y=36, w=20) — visually
> sits below the inner-item icon. The value cell is top-aligned in the slot
> (y=10, full 40-wide), styled to look like a small label across the slot top.
> Both cells are LEFT-aligned by default — neither `flags & 0x2` (center) nor
> `flags & 0x4` (right) is set; only `&0x20` (count) or `&0x10` (value) drive
> the vertical/wrap behavior.

### Font handle globals

- `0x65b010` (used at `0x52ccb4` and `0x52cd19`) — passed as `ecx` to
  `meth_0x52d870` (a font-metrics helper that returns the line height in pixels
  + 2 for the count cell, or doubled for the value cell). The fact that the
  same `ecx` is used for both texts but different `eax`-arg handles
  (`0x65abc4` for count, `0x65a9cc` for value) shows there are two distinct
  GDI fonts — **font 0 (default)** for the count, **font 4** for the value.
  UNCONFIRMED: the actual Arimo/Arial point sizes (`0x65a9cc` likely indexes
  into the same Small/Medium/Large font table used by TPlyrStatusBar — flagged
  §14).

### Pink halo

Both text cells run through `meth_0x4be2b0`'s magenta-cleared scratch
(UI_METHOD_MAP §5 step 5). They are therefore SUSCEPTIBLE to the same
`[[project-retail-pink-halo-bug]]` halo as TPlyrStatusBar. **Do not reproduce.**
Port to real alpha via `BuildTTFAtlas` + `DrawTextShadowedToTarget` (§12).

---

## §9 — Animation & dynamic behavior

### Tweens / ramps

- **No tweens** on the BarInv pane itself. There is no hover-glow ramp on the
  slot boxes (the highlight is a one-shot redraw in slot 22 `0x52cd80`, see
  §10), no fade on the box chrome, no slide on slot count.
- **Slot count is reactive, not animated.** When the bar resizes, the slot
  count is recomputed in three places: Init `0x52c970`, Layout `0x52ca00`, and
  Draw `0x52ca70`. The count changes instantly; the box row is redrawn whole
  on the next draw.

### Dirty / redraw model

| field / global | role | semantics | cite |
|---|---|---|---|
| `mbr_0x50` (TPane base "visible") | redraw gate when target == display | Draw early-out at `:1434` | `cls_0x5a5658.cpp:1434` |
| `DAT_0065b688` (global dirty flag) | force-redraw override | written by Pulse / script item-use / slot 20 `0x52ca50`; cleared by reading-then-redrawing in the next Draw | `_data.txt:130274`; Draw `:1435`; slot 20 `0x52ca5e movl $1, 0x65b688` |
| `mbr_0x60` (player) | early-out — no draw without a player | Draw `:1447` | `cls_0x5a5658.cpp:1447` |
| `mbr_0x6c` (drag-active flag) | skips the held item in the per-item loop | Draw `:1462`; drag set `:1573` | `cls_0x5a5658.cpp:1462, 1573` |
| `mbr_0x64` (selected slot id) | held-item slot during a drag; `-1 = none` | Draw `:1462-1464, :1514-1517` | — |

### Per-frame "hover" update (slot 22, `0x52cd80`)

This is **not a paint** — it is a per-frame update that **directly draws** the
hovered-item highlight onto the display when the cursor is over a slot. Called
from the screen Pulse path. Behavior:
- Reject if `mbr_0x60 == 0` (no player) or `DAT_00668518 != 0` (some modal
  block).
- Read mouse globals `cursor_x = DAT_00668510`, `cursor_y = DAT_00668514`.
- Compute local: `local_x = cursor_x − mbr_0x4 − 0xdc`, `local_y = cursor_y −
  mbr_0x8 − 0xa`.
- If in range AND `local_y < 0x28 (=40)`: compute `slot_index = local_x / 0x2d`
  (visible index); reject if outside `[mbr_0x88, mbr_0x88 + mbr_0x8c)`; final
  slot_id = `slot_index + 0x10b`. Else `slot_id = -1`.
- Resolve item via `cls_0x5b4f30_TPlayer::meth_0x4701f0(slot_id)` and call
  `FUN_0043a820(item, 0)` — a hover-feedback dispatch (cursor change /
  tooltip / highlight overlay, body not extracted).
- Always also push the hovered item to a **TTextBar tooltip queue** (`0x65caf0`
  helper at `meth_0x47e920`, then `0x65b140 ← meth_0x5496a0` — the item-use
  registry).

The hover update **runs every frame**; it does not animate (no tween / no
fade). The cursor highlight is a single-frame visual.

---

## §10 — Input & dispatch

### Hit-test (cursor → slot)

Identical math to the inverse cell-origin verification in §4: from cursor
`(cx, cy)`,
```
local_x = cx − mbr_0x4 − 0xdc        # = cx − pane.x − 220
local_y = cy − mbr_0x8 − 0xa         # = cy − pane.y − 10
if local_x < 0 or local_y < 0 or local_y >= 0x28: → no slot
slot_index = local_x / 0x2d
if slot_index < mbr_0x88 or slot_index >= mbr_0x88 + mbr_0x8c: → no slot
slot_id = slot_index + 0x10b
```
Hit rect per visible slot N (pane-local): `(220 + 45·N, 10, 45, 40)` — but
note the **45-px-wide** test is implicit in `(local_x / 0x2d)` floor: every
local_x in `[220+45N, 220+45N+45)` lands on slot N. The 3-px gutter between
boxes is "hot" (clicks on the gap fall to the closer box).

### Click handler (slot 25, `0x52cfb0`)

Dispatch on `msg = param[0x18 + 4]` (the message-type arg, offset 0x18 above
the saved ECX). Only handles **msg == 5** (likely "left-click pressed" — to be
confirmed against the screen-level dispatch table; UNCONFIRMED-D). Behavior at
`msg == 5`:
- Reject if `param[0x1c] < 0` or `param[0x20] < 0` (negative x/y); if x ≥
  `mbr_0xc` (width) or y ≥ `mbr_0x10` (height) — i.e. cursor inside pane rect.
- Branch on `DAT_0065c9e0` (a modal-state global, role UNCONFIRMED-E):
  - **`!= 0`** (some other modal mode): compute `local_x = cursor_x − 0xdc`,
    `local_y = cursor_y − 0xa`; do the slot-index decode; resolve item via
    `TPlayer::meth_0x4701f0(slot_id)`, then call `FUN_0043a820(item, 0)`
    (same hover-feedback as slot 22 — confirms 0x4701f0 = "item-at-slot-id
    lookup").
  - **`== 0`** (normal mode, the inventory branch starting at `0x52d0b8`):
    branches further on `mbr_0x64` (selection state) and `DAT_0065b878`
    (another modal global, role UNCONFIRMED). The complex sub-branches handle
    pick-up / drop / use; not exhaustively decoded — the high-level dispatch
    targets are `meth_0x47eff0` (e.g. clear selection), `meth_0x5496a0`
    (the item-use registry at `0x65b140`), and a path that sets the drag
    state. Treat the click handler as **dispatch-only**: it does not draw, it
    sets `mbr_0x64` / `mbr_0x68` / `mbr_0x6c` and forwards to item/use code.

### Drag-move handler (slot 26, `0x52d6e0`, `cls_0x5a5658.cpp:1527-1577`)

Called on cursor-with-button-held; takes `(param_1 = msg, param_2 = x, in_stack_0000000c = y)`:
- Guard: `mbr_0x60 != 0` AND `param_2 ≥ 0` AND `y ≥ 0` AND x/y inside pane
  rect AND `mbr_0x6c == 0` (not already dragging) AND `param_1 ∈ {1, 2}`
  (button code — UNCONFIRMED-D).
- **2-px move threshold:** `iVar1 = abs(mbr_0x70 − param_2)` (prev_x − cur_x).
  If `iVar1 < 2`: also check y: `iVar1 = abs(mbr_0x74 − y)`; if `iVar1 < 2`
  return (cursor hasn't moved enough — same as the InventoryPane's drag-
  promotion threshold per `InventoryPane_SPEC.md` §1).
- Resolve item: `piVar2 = TPlayer::meth_0x4701f0(mbr_0x60)` (UNCONFIRMED-F:
  the arg is the player, not a slot id — likely "currently-held / cursor
  item"); call `piVar2.vtable+0x130()` (inv-image getter); if non-null AND
  `mbr_0x64 ≥ 0`:
  - Compute click offset within the box: `iVar4 = (mbr_0x70 − 0xdc) % 0x2d`,
    `iVar5 = (mbr_0x74 − 0xa) % 0x28`. (Where the cursor *was* within its slot
    when the drag started.)
  - If `(*piVar2 + 0x130)() == 0` (no live inv-image): default `iVar4 =
    iVar5 = 0x14 = 20` (center the icon under the cursor).
  - `FUN_0043a100(inv_image, iVar4, iVar5)` — set the cursor-drag image with
    grab offset.
  - `FUN_0043a140(piVar2)` — register the dragged item.
  - `FUN_0043a170(&DAT_0066f33c.field_0x30)` — push some drag state.
- Set `mbr_0x6c = 1` (now dragging) and call `vtable->slot40 = 0x444fb0` (a
  TPane base "mark for redraw" hook).

### Modal / global state effecting draw

| global | role | seen in |
|---|---|---|
| `DAT_0065b688` | force-redraw flag | Draw `:1435`, slot 20 `0x52ca50` |
| `DAT_0065c9e0` | modal-mode toggle (?) — picks "highlight only" vs full click | Click `0x52cfb0` `:52cff9` |
| `DAT_0065b878` | another modal global; selects sub-branch in the click path | Click `0x52d0c3` |
| `DAT_0065d1b8` | "use mode active" flag — when `1`, click forwards selection direct to use-registry | Click `0x52d092` |
| `DAT_00668518` | a global "input blocked" — slot 22 (hover) skips if set | Hover `0x52cd9e` |

### Command IDs

No `enum`-style command IDs are emitted; this pane drives the **inventory /
item-use** subsystem directly via the registries at `0x65b140` (item-use,
slot 22 + slot 25) and the drag-feedback functions `FUN_0043a100/0x140/0x170`.
No `TPlayScreen::Command` codes are produced — unlike sidebar buttons (the
tab pane), BarInv is "direct dispatch".

---

## §11 — Retail bugs NOT to reproduce

1. **Pink halo around the two text cells.** Both `meth_0x4be2b0` calls run
   through the magenta-cleared scratch (UI_METHOD_MAP §5 step 5); the
   antialiased glyph edges sample the magenta cell and the resulting pink
   fringe is keyed in during composite. This is the same artifact documented
   in `TPlyrStatusBar_SPEC.md` §10 item 1 — see [[project-retail-pink-halo-
   bug]]. **Correct intent:** white text (count) or color-of-value text
   (value) with a black 3-pass SE drop shadow, no halo. **Fix:** real alpha
   atlas, no magenta key — port via `BuildTTFAtlas + DrawTextShadowedToTarget`
   (§13).
2. **Uninitialized 5th arg to the box-stamp** (`in_stack_00000008` at
   `:1442`). At runtime this is whatever the caller's stack happens to hold;
   in practice it's likely **0** (the BottomBar's draw frame sets it
   implicitly to opaque), but treating an uninitialized stack slot as a draw
   mode is a latent retail bug. **Correct intent:** opaque copy of the 42×42
   chrome with no special mode. **Fix:** pass `0x80000000 = DM_USEDEFAULT`
   (or just plain `DrawBitmapToTarget` opaque) — see UNCONFIRMED-C.

No `+4,+4` mirrored-shadow trap here (no sprite shadow at all) and no
chip-model overhang (the BarInv pane fills its bottom-bar rect).

---

## §12 — Reconstruction pseudocode

```python
class BarInvPane:
    BarInvBox: Bitmap                # bottombar.dat[1], 42x42 opaque 15-bit

    # Pane rect (set by parent bottom-bar SetRect)
    pane_x   = 0
    pane_y   = display_h - 60        # BL-anchored to screen via bottom-bar
    pane_w   = display_w             # greedy, mirrors bottom bar
    pane_h   = 60

    # Runtime state (see §3)
    player          = None
    selected_id     = -1             # mbr_0x64 — held-item slot_id (when dragging)
    drag_held_id    = -1             # mbr_0x68
    dragging        = False          # mbr_0x6c
    last_mouse_xy   = (0, 0)         # mbr_0x70, mbr_0x74
    page_start      = 0              # mbr_0x88 — slot_index of leftmost visible
    visible_count   = 0              # mbr_0x8c = (pane_w - 220) / 45

    dirty_global    = False          # DAT_0065b688
    visible_flag    = False          # mbr_0x50

    # ---- Initialize -------------------------------------------------------
    def init(self):
        TPane.init(self)
        BarInvBox = LoadBitmap("bottombar.dat", "BarInvBox")   # §2
        self.selected_id   = -1
        self.drag_held_id  = -1
        self.dragging      = False
        self.page_start    = 0
        self.set_size(display_w, display_h)                    # vtable+0x20
        self.visible_count = (display_w - 0xdc) // 0x2d
        self.commit()                                          # vtable+0x2c
        # cite cls_0x5a5658.cpp:1382-1402 / 0x52c970

    # ---- Layout / commit (vtable slot 18, 0x52ca00) -----------------------
    def layout(self):
        self.pane_x = self.tmpl_x; self.pane_y = self.tmpl_y
        self.pane_w = self.tmpl_w; self.pane_h = self.tmpl_h
        self.visible_count = (display_w - 0xdc) // 0x2d
        TPane.commit(self)                                     # vtable+0x2c

    # ---- Mark dirty (vtable slot 20, 0x52ca50) ----------------------------
    def mark_dirty(self):
        if self.player and self.visible_flag:
            self.dirty_global = True                           # DAT_0065b688 = 1

    # ---- Draw (vtable slot 21, 0x52ca70) ----------------------------------
    def draw(self, target):
        # Early-out gate (§5 step 1)
        if target is display and not self.visible_flag and not self.dirty_global:
            return TPane.post_draw(self)

        # Recompute count, draw the box row (§5 steps 2-3)
        self.visible_count = (display_w - 0xdc) // 0x2d
        for N in range(self.visible_count):
            target.DrawBitmapToTarget(BarInvBox,
                dst=(0xdc + 0x2d*N, 0xa))                      # opaque copy

        if self.player is None:
            return TPane.post_draw(self)

        # Per-item loop
        tint = target.AllocateScratch20x20(key=GLOBAL_KEY)     # FUN_004a1ec0
        for item in self.player.iter_inventory():
            sid = item.field_7c - 0x10b                        # slot_index
            if self.dragging and item.field_7c == self.selected_id:
                continue
            if not (self.page_start <= sid < self.page_start + self.visible_count):
                continue
            # "held in cursor" gate
            if target is display and item.vtable.held_in_cursor(item):
                continue
            slot_x = 0xdc + 0x2d * (item.field_7c - self.page_start - 0x10b)
            #       = 0xdc + 0x2d * (sid - self.page_start)            (§6 lea-decode)
            #       = 220 + 45 * visible_index_for_this_item

            # Item self-draw (the icon)
            item.vtable.self_draw(slot_x, 0xa, target)         # +0x108

            # Pouch overlay
            if stricmp(item.name, "Pouch") == 0:
                inner = item.GetInner()                        # via meth_0x4701f0 — §14-A
                if inner is not None:
                    inv_img = inner.vtable.GetInventoryImage() # +0x130
                    if inv_img is not None:
                        FUN_004a31a0(inv_img, tint, GLOBAL_KEY)
                        target.DrawBitmapToTarget(tint,
                            dst=(slot_x, 0x1e), mode=0x100)    # DM_TRANSPARENT
                        qty = TPlayScreen.item_count(item)     # meth_0x470040
                        text = itoa(qty)
                        target.DrawTextShadowedToTarget(
                            dst=(slot_x + 1, 0x24),
                            cell=(0x14, font_lineH(0) + 2),
                            text=text, font_id=0,
                            color=(0xff,0xff,0xff), flags="left|top",
                            shadow=True)                       # font_id 0x400 → shadow

            # Value overlay (any item)
            val = item.vtable.value()                          # +0x198
            if val > 1:
                text = itoa(val)
                color = (TBD — UNCONFIRMED-B: read from 0x65abc4 helper)
                target.DrawTextShadowedToTarget(
                    dst=(slot_x, 0xa),
                    cell=(0x28, font_lineH(4) * 2),
                    text=text, font_id=4,
                    color=color, flags="left|top",
                    shadow=True)                               # font_id 0x404 → shadow

        if self.selected_id < 0:
            self.drag_held_id = -1
        target.FreeScratch(tint)
        TPane.post_draw(self)

    # ---- Hover (vtable slot 22, 0x52cd80) ---------------------------------
    def hover_tick(self):
        if self.player is None or DAT_00668518: return
        cx, cy = mouse_cursor_xy()                             # 0x668510 / 0x668514
        lx = cx - self.pane_x - 0xdc
        ly = cy - self.pane_y - 0xa
        slot_id = -1
        if lx >= 0 and ly >= 0 and ly < 0x28:
            visible_index = lx // 0x2d
            if self.page_start <= visible_index < self.page_start + self.visible_count:
                slot_id = visible_index + 0x10b
        if slot_id >= 0:
            item = self.player.find_by_slot(slot_id)           # meth_0x4701f0
            if item:
                FUN_0043a820(item, 0)                          # hover feedback (cursor/tooltip)
        ...
        # tooltip / use-registry push
        ...

    # ---- Drag-move (vtable slot 26, 0x52d6e0) -----------------------------
    def drag_move(self, msg, x, y):
        if (self.player is None or x < 0 or y < 0
            or x >= self.pane_w or y >= self.pane_h
            or self.dragging or msg not in (1, 2)):
            return
        # 2-px move threshold
        if abs(self.last_mouse_xy[0] - x) < 2 and abs(self.last_mouse_xy[1] - y) < 2:
            return
        item = self.player.cursor_item()                       # UNCONFIRMED-F
        if item and item.vtable.GetInventoryImage() and self.selected_id >= 0:
            grab_x = (self.last_mouse_xy[0] - 0xdc) % 0x2d
            grab_y = (self.last_mouse_xy[1] - 0xa)  % 0x28
            if item.vtable.GetInventoryImage() is None:
                grab_x = grab_y = 0x14                         # center
            SetCursorDragImage(item.GetInventoryImage(), grab_x, grab_y)
            BeginDrag(item)
            PushDragState()
        self.dragging = True
        TPane.mark_redraw(self)
```

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `meth_0x4bd680(target, x, y, BarInvBox, mode)` opaque box stamp | `Renderer->DrawBitmapToTarget(bm, x, y)` | renderer (UI_METHOD_MAP §4/§12) |
| `meth_0x4bd680(target, x, y, tint20x20, 0x100)` keyed icon stamp | `Renderer->DrawBitmapToTarget(tint, x, y)` with the source carrying real alpha (post-fix; retail uses chroma key + the global `_DAT_006668d0` sentinel — UI_METHOD_MAP §16) | renderer |
| `meth_0x4be2b0_CompositeBuffer(... font_id=0x400 ... white)` | `Renderer->DrawTextShadowedToTarget(atlas_default, text, cellX, cellY, cellW, ETextAlign::Left, 0xff, 0xff, 0xff, cellW, cellH)` | font.cpp (UI_METHOD_MAP §5/§12) |
| `meth_0x4be2b0_CompositeBuffer(... font_id=0x404 ... value-color)` | `Renderer->DrawTextShadowedToTarget(atlas_4, text, cellX, cellY, cellW, ETextAlign::Left, r,g,b, cellW, 2*lineH)` | font.cpp |
| `cls_0x52d870::meth_0x52d870(font, …)` font-line-height getter | `font.cpp: TextLineHeight(atlas)` (or use the same one TPlyrStatusBar uses for font 0x404) | font.cpp |
| `FUN_004a1ec0(0x14, 0x14, fmt, 0)` temp 20×20 surface alloc | `TSurface::CreateScratch(20, 20, fmt)` or per-paint `TSurface::Begin()` (the port already does this for InventorySidebarPane drag-image; reuse) | renderer / surface |
| `FUN_004a31a0(inv_img, dst20x20, key)` icon-to-scratch tint | implement once: `Renderer->BlitWithKeyToScratch(src, dst, key_rgb)` (NOMENCLATURE §3 mosaic surface analog) | renderer |
| `cls_0x46d6b0::meth_0x46d710(archive, name)` by-name lookup | existing `MountArchive("bottombar.dat").Find("BarInvBox")` | data |
| `FUN_0058d252(v, buf, 10)` itoa-10 | `std::format("{}", v)` / `snprintf(buf, 10, "%d", v)` | std |
| `FUN_004aacb0_BlitEffect_Iterate` | subsumed by per-call `DrawTextShadowedToTarget` (the parent bottom-bar's flush isn't needed when each text call already paints its own shadow) | renderer |

**Missing primitives?** None new for the box row / icon stamps — they reuse
`DrawBitmapToTarget` and the keyed/alpha variants. The text path is the same
`DrawTextShadowedToTarget` the rest of the HUD uses. The Pouch-overlay tint
scratch is the same 20×20 pattern as InventorySidebarPane's drag-image
scratch — reuse that.

---

## §14 — UNCONFIRMED / open questions

1. **UNCONFIRMED-A — Pouch contents getter.** Draw `:1478-1479` reads
   `piVar4 = cls_0x5b4f30_TPlayer::meth_0x4701f0(this_00)` where `this_00` is
   already the item (cast to `TPlayer*` by Ghidra). `meth_0x4701f0` is the
   "item-at-slot-id" lookup in the click handler — when called on an item, it
   may return the item's **first contained item** (a TContainer-style "inner
   item" getter that resolves through the same `+0x4701f0` virtual). Then
   `(*piVar4.vtable + 0x130)()` is the inv-image getter on that inner item.
   **Impact if wrong:** wrong icon stamped over Pouches (might stamp the
   Pouch's own face twice, or the wrong inner item). **Resolve:**
   `DecompileAddr.java 0x004701f0` + check whether `cls_0x5b4f30` is genuinely
   `TPlayer` or also serves as the Pouch class.

2. **UNCONFIRMED-B — Value-text color source.** The Pouch quantity is white
   (via PackRGB at `0x52cc84-0x52cc8e`); the value text instead reads
   `0x65abc4` and pipes it through `FUN_00444e20` + a `cls_0x419dd0::cls_0x419dd0`
   field-copy. The color value at `0x65abc4` is not yet identified. Likely a
   pre-packed `BGR0` color (analogous to other UI text — see TPlyrStatusBar
   stat-value cells which read from FONT.DEF / a font-color table). **Impact
   if wrong:** value text rendered in the wrong color. **Resolve:**
   `DecompileAddr.java 0x00444e20` to confirm the helper, then trace
   `DAT_0065abc4` writers (`grep` xrefs in `_data.txt`).

3. **UNCONFIRMED-C — Box-stamp 5th arg.** `meth_0x4bd680(param_1, iVar8, 10,
   mbr_0x84, in_stack_00000008)` reads `in_stack_00000008` as the drawmode —
   an uninitialized stack slot from the caller's frame. In practice, retail
   probably falls through to `0` (opaque) because the bar's Draw frame writes
   that slot first, but it is technically a latent bug. **Impact if wrong:**
   if the stack slot happens to hold a non-zero drawmode, boxes would draw
   keyed/alpha/etc. **Resolve:** dump the bar's Draw frame, or just port as
   `0x80000000 = DM_USEDEFAULT` (opaque) — the safe interpretation.

4. **UNCONFIRMED-D — Click handler message codes.** Slot 25 (`0x52cfb0`) only
   handles `msg == 5`; the drag-move (slot 26, `0x52d6e0`) only handles `msg ∈
   {1, 2}`. The exact codes (button-down / button-up / move / drag) are not
   yet mapped against `TPlayScreen::Command` / the screen-level dispatcher
   (`FUN_0047cf40`). **Impact:** wrong message code → click does nothing or
   wrong action. **Resolve:** cross-reference `FUN_0047cf40` cases vs.
   InventoryPane's MouseClick (`InventoryPane_SPEC.md` §10) which uses the
   same numbering family.

5. **UNCONFIRMED-E — Modal globals in click branches.** `DAT_0065c9e0` and
   `DAT_0065b878` gate sub-branches of the click handler; their roles are
   unknown. Likely "use-mode active" / "spell-targeting active" / etc., but
   not pinned. **Impact:** wrong click branch taken. **Resolve:** find
   writers of these globals (`grep _data.txt`), especially in
   `FUN_0047cf40` (TPlayScreen command dispatch).

6. **UNCONFIRMED-F — Drag-move's `meth_0x4701f0` arg.** Drag-move calls
   `cls_0x5b4f30_TPlayer::meth_0x4701f0(this->mbr_0x60)` — passing the player
   pointer where elsewhere this method takes a slot id. Likely returns the
   "current cursor-held item" (i.e. the item the player picked up earlier).
   **Impact:** wrong item dragged. **Resolve:** see UNCONFIRMED-A.

7. **UNCONFIRMED-G — `DAT_0065b644` / `0x65b648` writers.** Used by Init,
   Layout, and Draw as the live width/height; they MUST be updated when the
   display/bar resizes, but the writers aren't in any extracted recon yet.
   **Impact:** at non-Classic widths, `mbr_0x8c` (slot count) doesn't grow,
   so resizing the bar wider than 640 still shows 9 slots. **Resolve:** trace
   the resize chain from `TPlayScreen` / display callbacks.

8. **UNCONFIRMED-H — `0x65b010` font-handle struct.** Both text calls read
   `ecx = 0x65b010` before `meth_0x52d870`; the struct shape and where it is
   populated are unknown. Likely a font-cache record used HUD-wide.
   **Impact:** wrong font line height → text shadow misalignment. **Resolve:**
   `grep _data.txt` for `0x65b010` writers + cross-check
   `TPlyrStatusBar_SPEC.md`'s font-loader path.

9. **UNCONFIRMED-I — `0x65a9cc` (value-font handle).** Passed as the
   first arg to the height helper for the value cell (font id `0x404`). May
   index into the same Small/Medium/Large font table as TPlyrStatusBar (font
   `0x404` is one of the StatusBar fonts per `TPlyrStatusBar_SPEC.md:222-229`).
   **Impact:** value text uses the wrong font. **Resolve:** cross-check the
   font initialization at `0x65a9cc` against the StatusBar fonts.

10. **UNCONFIRMED-J — Method-map gap: `FUN_004a31a0`.** Used at `:1480` to
    composite the inner-item inv-image into the 20×20 scratch with a key
    color. Behavior inferred (tint/copy + chroma key) but body not extracted.
    Mirrors the cursor-tint pattern in InventoryPane drag setup. **Impact:**
    Pouch overlay rendered without key → magenta box around the icon.
    **Resolve:** `DecompileAddr.java 0x004a31a0`; extend UI_METHOD_MAP with a
    "tinted/keyed scratch composite" entry.

11. **UNCONFIRMED-K — `FUN_0058d252` body.** The itoa-family used for both
    "%d" formats. Signature inferred `(value, buffer, max_digits=10)` from the
    call shapes at `0x52cc71` and `0x52cced` and `0x52cf3d`. **Impact:** if
    the function actually has `(buffer, value)` ordering or different radix,
    quantity/value text wrong. **Resolve:** `DecompileAddr.java 0x0058d252`.

12. **UNCONFIRMED-L — Pane x/y at non-Classic resolutions.** Confirmed BL-
    anchored to the bottom bar (y = display_h − 60, x = 0) at Classic 640×480
    via SetRect propagation. At higher resolutions the bar resizes greedily;
    BarInv inherits the same w/h via SetRect — but no extracted recon writes
    explicit `pane.x`/`pane.y` for the BarInv instance, so the assumption
    "BarInv pane.x = 0, pane.y = display_h − 60" relies on the bar being the
    only writer of those template fields (which the recon shows is true for
    w/h). **Impact:** if a non-bar writer offsets the pane, all box x-coords
    are off by that offset. **Resolve:** grep writers of `0x65b02c..0x65b048`
    (the TPane fields on the `0x65b028` instance), confirm bar is the only
    writer.

---

