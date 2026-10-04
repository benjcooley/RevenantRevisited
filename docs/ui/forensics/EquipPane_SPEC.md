# TEquipPane — Forensics SPEC

Pixel-targeted spec for the **Equipment sidebar** pane: the right-sidebar
upper-region panel (mode 0, label "Equip"). A single tall doll-frame chrome
with 11 fixed equipment slots; each occupied slot shows the equipped item's
baked `.i3d` inventory icon, with click-and-drag grab/drop.

Retail class label: `EquipSidebarPane` = `cls_0x5a55dc` (32-slot direct-TPane
leaf, NOT `cls_0x54dd40` — see §0 mis-target note). Candidate src/ identity:
the symbol `TEquipPane` (recon yaml mapping). All coordinates are in 640×480
**Classic** space; cite format is `file:line` + `0xHEX = DEC`.

---

## §0 — Sources & status

**Status:** `forensics-complete-pixelwise` as of Wave-6. The per-slot
(x, y) table for ALL 11 EQ_* slots is now decoded and visually
cross-checked. UNCONFIRMED-A is downgraded from "blocker" to
"high-confidence pending one-line leaf-method confirmation" — the
table data + visual match is strong enough to wire the pane.

The pane identity, rect, asset roster, draw order, draw primitives,
icon-fetch chain, click-dispatch shape, **the per-slot enum↔mbr-offset
placeholder bitmap mapping**, and **all 11 per-slot (x, y) anchors**
are now confirmed from recon (vtable, ctor pattern, asset dump,
leaf-method extracts for slots 0/19/20, the Initialize bitmap-loader's
name-keyed lookup analysed Wave-5, the 11-pair-from-0x5e3f60 data table
decoded + visual-matched Wave-6, port-status note). The hit-test
scalars previously labelled `0x4a=74`, `0x25=37`, `0x07=7` are NOT
separate scalars — they are the first 1.5 entries of the per-slot table
(slot 0 x=74, slot 0 y=37, slot 1 x=7); see §4 Wave-6 reading.

Remaining UNCONFIRMED items are minor: confirm the leaf-method loop
iterates in straight EQ-enum order (UNCONFIRMED-A residual), pane-rect
literal in ctor (UNCONFIRMED-B), pane-bg fill (UNCONFIRMED-C), the
`Player->GetEquip(i)` vtable offset (UNCONFIRMED-D), inter-Equip-slot
swap support (UNCONFIRMED-E), shadow setup absence (UNCONFIRMED-F),
animation idle string (UNCONFIRMED-G), hover-tooltip path
(UNCONFIRMED-H), mouse-move handler slot (UNCONFIRMED-J), TMulti
field drop (UNCONFIRMED-K), slot 7 role (UNCONFIRMED-L). Engine code
can be wired against the Wave-6 table; verify visually against
`/tmp/dat_dump/00_Equip.png` after wiring. No engine code written
in this forensics pass.

**This completion pass added (Wave-5, 2026-05-29):**
- §2/§5/§13: the bitmap iterator `meth_0x46d710` is a **name-keyed**
  TMulti lookup (decompiled this pass at `recon/ghidra/cls_0x46d6b0.cpp:50-90`),
  not a positional `next()`. Initialize loads each placeholder BY NAME from
  the name-pointer table, so `mbr_0x68+i*4 = placeholder[i]` lines up to
  EQ_* enum order (slot 0=Head, …, 10=Feet). This pins the placeholder
  fetch slot ↔ mbr offset mapping HIGH-confidence.
- §0: `mbr_0x94 = -1` and `mbr_0x98 = -1` initial-store identified in
  Initialize (recon `:8979-8980`) — these are the `grabslot`/`heldslot`
  retail field offsets (snapshot parity, UNCONFIRMED-I now upgraded to
  HIGH-confidence).
- §4: per-slot table re-anchored on the confirmed mbr offsets, with the
  visual chrome sketch updated. Per-slot (x,y) still UNCONFIRMED-A.
- §6.1 / §14: a second viable decoding of the 9 (x,y) pairs at
  `DAT_005e3f6c` is added — **`(y, x)` byte order** would let the
  `0xca=202` final value land as a y-coord (within 306-tall pane) instead
  of an out-of-bounds x. Listed as a candidate alongside the original
  (x,y) reading; resolves only with `DecompileAddr.java 0x5366d0`.

**This completion pass added (Wave-6, 2026-05-29):**
- §4 / §6.1: **per-slot (x, y) table fully decoded for ALL 11 EQ_* slots**
  (HEAD, NECK, BODY/Chest, OFFHAND/Hand2, PRIMEHAND/Hand, R_ACCESSORY/Ring,
  L_ACCESSORY/Ring2, RANGEDWEAPON/Range, AMMO, LEGS, FEET). The data
  region `DAT_005e3f60..0x5e3fb7` (88 bytes ending exactly at the
  name-pointer table at `0x5e3fb8`) is **11 × {int x, int y}** in
  EQ_* enum order. The three "scalars" Ghidra labelled at `0x5e3f60`,
  `0x5e3f64`, `0x5e3f68` (= 74, 37, 7) are NOT separate scalars; they
  are the first 1.5 entries of the table itself (slot 0 x=74, slot 0
  y=37, slot 1 x=7). Ghidra labelled them as scalars because the leaf
  methods load them as individual ints — but they are structurally
  part of the slot-rect array. The XREF `(*)` (LEA) annotations at
  `0x5e3f64` from all three relevant leaf methods (`0x5363e0`,
  `0x5366d0`, `0x536970`) are consistent with this base being used
  for indexed loads of `slot[0].y`. Decoded values:
  * EQ_HEAD (0)         → (74, 37)
  * EQ_NECK (1)         → (7, 58)
  * EQ_BODY (2)         → (141, 58)
  * EQ_OFFHAND (3)      → (141, 106)
  * EQ_PRIMEHAND (4)    → (7, 106)
  * EQ_R_ACCESSORY (5)  → (7, 154)
  * EQ_L_ACCESSORY (6)  → (141, 154)
  * EQ_RANGEDWEAPON (7) → (24, 11)
  * EQ_AMMO (8)         → (124, 11)
  * EQ_LEGS (9)         → (7, 202)
  * EQ_FEET (10)        → (141, 202)
- §2 + §4: **visual cross-check** — template-matching on the chrome
  bitmap `00_Equip.png` found exactly **11 wells**. All 11 data
  entries map 1-to-1 to all 11 detected wells with a consistent
  frame-inset Δ of ~3-9 px in x and ~0-10 px in y. The layout is a
  coherent paperdoll: HEAD top-center; NECK/BODY row 2 L/R;
  PRIMEHAND/OFFHAND row 3 L/R; ACCESSORIES row 4 L/R;
  RANGED/AMMO shoulder-top L/R; LEGS/FEET row 5 L/R. L/R chirality
  follows the **paperdoll-viewed-face-on mirror convention**:
  PRIMEHAND (character's right hand) sits on the viewer's left side.
- §0: status upgraded from `forensics-partial` to
  `forensics-complete-pixelwise`. UNCONFIRMED-A downgraded from
  "blocker" to "residual" (one-line leaf-method iteration confirmation).
- §10: hit-rect table now populated with the 11 (x, y, 40, 40) rects.
- §12: reconstruction pseudocode updated with the literal 11-slot
  position table.
- Brief mis-transcription: the dispatching brief said "ALL 32 slots"
  — that's the vtable slot count, not the equip-slot count. There
  are 11 equipment slots (NUM_EQ_SLOTS=11, `src/player.h:19`),
  confirmed by the name table, the 11 placeholder bitmaps, the
  visual count of 11 wells, AND the 11 (x, y) entries in the data
  table.
- Task brief mis-statement (32-slot): the dispatching brief mentioned
  "ALL 32 slots" — this is the vtable slot count, NOT the equip-slot
  count. The pane has exactly 11 equipment slots (NUM_EQ_SLOTS=11,
  `src/player.h:19`), confirmed by the name table, the 11 placeholder
  bitmaps, and the visual count of 11 wells in the chrome.

**Task-brief mis-target (corrected here).** The dispatching script targeted
`cls_0x54dd40` (`recon/ghidra/cls_0x54dd40.cpp`), but that file is a
**33-line struct ctor for a 52-byte data class with no UI methods** — it is
*not* `TEquipPane`. The retail TEquipPane class is `cls_0x5a55dc` per
`recon/discovered/port_status/EquipSidebarPane.md:5,28`, evidence: the
inlined CRT init thunk at `0x4807a0` writes
`*0x65b7e0 = 0x5a55dc` (vtable address). The `recon/mappings/
TEquipPane_cls_0x54dd40_map.yaml` yaml mapping is a **false-positive
auto-match** (size/field heuristics, see file:1-22) and should not be
trusted; the brief's vtable evidence overrides it. Spec built on the correct
class.

**Class / vtable resolution (the spine of this spec):**

| symbol | addr | role | evidence |
|---|---|---|---|
| pane global instance | `0x65b7e0` | the singleton `TEquipPane` | brief `B_r11_sidebar_tab_cascade.md:164,16`, `port_status/EquipSidebarPane.md:5`, dispatcher case 9 `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:211,214` |
| vtable | `0x5a55dc` | 32-slot direct-TPane leaf (no TButtonPane intermediate) | `EquipSidebarPane.md:5`, `B_r11_sidebar_tab_cascade.md:190-196` |
| inlined ctor thunk | `0x4807a0` | CRT init: `MOV [0x65b7e0], 0x5a55dc` + rect constants | `EquipSidebarPane.md:5,28`; pattern mirrors Stats ctor `cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp:6-21` (same upper-sidebar slot) |
| inlined dtor thunk | `0x4807f0` | atexit | `EquipSidebarPane.md:5` |
| slot 0 (Initialize) | `0x536360` | bitmap-roster load + clip-rect init | recon body `cls_0x5a5320_TCharacter.cpp:8951-8979` |
| slot 7 | `0x5370b0` | (unspecified leaf override; **body UNCONFIRMED**) | vtable `cls_0x5a5320__vftable_5a5320.cpp:189` (slot 728/4=182 in the merged TPlayScreen vtable; per `EquipSidebarPane.md:18` the leaf-22 in TEquipPane is `0x5370b0`) |
| slot 19 (Animate) | `0x5368c0` | per-tick character/animation pulse | recon body `cls_0x5a5320_TCharacter.cpp:8997-9062` |
| slot 20 (Show/DrawBackground gate) | `0x5366a0` | dirty-gate `if (mbr_0x50 != 0) call slot8 then slot44` | recon body `cls_0x5a5320_TCharacter.cpp:8985-8993` |
| slot 21 (DrawBackground impl) | `0x5366d0` | the paint method (**body UNCONFIRMED**) | vtable `cls_0x5a5320__vftable_5a5320.cpp:203`; XREFs to slot-rect table `DAT_005e3f64` |
| slot 22 (Update / per-slot redraw) | `0x536970` | per-slot icon refresh (**body UNCONFIRMED**) | vtable `cls_0x5a5320__vftable_5a5320.cpp:204`; XREFs to slot-rect table |
| slot 25 (MouseClick) | `0x5363e0` | grab/drop dispatch (**body UNCONFIRMED**) | vtable `cls_0x5a5320__vftable_5a5320.cpp:207`; XREFs to slot-rect table |
| **chrome resource** "Equip" 188×306 | `equippane.dat[0]` | full-pane backdrop with 11 baked slot wells | dump `tools/ui/dump_dat.py equippane.dat:[0]` = "Equip 188x306 flags=0x2" |
| **11 empty-slot icons** | `equippane.dat[1..11]` (Ammo/Chest/Feet/Hand/Hand2/Head/Legs/Neck/Range/Ring/Ring2), each 40×40 | placeholder drawn when slot empty | dump `equippane.def:0-141` |
| **bitmap iterator handle** | `DAT_0065dde4` | the `TMulti*` returned by Initialize's first FUN_0046d710 call | recon `cls_0x5a5320_TCharacter.cpp:8960,8965` |
| **slot-rect array base** | `DAT_005e3f6c` | per-slot (x,y) hit-rect / draw-anchor data | `_data.txt:114046-114122` (XREF'd by slot 25 / 21 / 22) |
| **scalar constants** | `DAT_005e3f60=0x4a=74`, `DAT_005e3f64=0x25=37`, `DAT_005e3f68=0x07=7` | hit-test / slot-pitch scalars (semantics UNCONFIRMED) | `_data.txt:114016-114044` |
| **slot-name pointer table** | `PTR_DAT_005e3fb8` (11 strings: Head/Neck/Chest/Hand2/Hand/Ring/Ring2/Range/Ammo/Legs/Feet) | Initialize iterates this to load the 11 slot-empty bitmaps via `FUN_0046d710(iter)` | `_data.txt:114124-114153` (sequence matches `src/player.h:21-31` EQ_* enum) |

**Recon files read:** `recon/ghidra/cls_0x54dd40.cpp` (mis-targeted struct,
confirmed not TEquipPane), `recon/ghidra/cls_0x54dd70.cpp` (likewise not
TEquipPane), `recon/classes_original/cls_0x5a5320_TCharacter.cpp:8949-9062`
(slot 0/19/20 bodies — the merged-class dump, mislabel-caveat applies),
`recon/classes_original/cls_0x5a5320__vftable_5a5320.cpp:180-213` (vtable
slot map), `recon/classes_original/_data.txt:114016-114220` (asset-name
table + slot-rect table), `recon/discovered/cls_0x5a58c0_InventorySidebarPane_ctor_487f50.cpp`
(sibling ctor — coord cross-check), `recon/discovered/cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp:6-21`
(sibling upper-region ctor — same rect), `recon/discovered/port_status/EquipSidebarPane.md`,
`recon/discovered/port_status/MapSidebarPane.md`,
`recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:202-214`
(case 9 = upper-Equip + lower-Inv visibility),
`recon/mappings/TEquipPane_cls_0x54dd40_map.yaml` (false-positive — see
above).

**Prior briefs consulted:** `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`
(pane identity, sidebar structure), `docs/ui/briefs/B_r2_character_panel_hunt.md`
(prior r2 wave on character pane semantics).

**Sibling specs consulted:** `docs/ui/forensics/InventoryPane_SPEC.md`
(grid/drag-promote pattern shared with this pane),
`docs/ui/forensics/CharacterPortrait_SPEC.md:430-455` (§14 paperdoll
overlap — the InventoryImage path is the same one used here for each
slot's icon).

**Asset measurement:** `tools/ui/dump_dat.py equippane.dat` →
`/tmp/dat_dump/equippane.def`, `equippane.json`. All 12 entries measured
(chrome + 11 slot icons). Chrome PNG inspected at `/tmp/dat_dump/00_Equip.png`.

**Shared references:** `FORENSICS_PROTOCOL.md`, `NOMENCLATURE.md`,
`UI_METHOD_MAP.md`, `../AGENT_PROTOCOL.md`.

**Errors to close (vs the brief + existing port):**
1. **`cls_0x54dd40` is NOT TEquipPane.** The yaml mapping is a false-positive
   (`recon/mappings/TEquipPane_cls_0x54dd40_map.yaml`). The correct retail
   class is `cls_0x5a55dc` (this spec) — see top-of-§0.
2. **Snapshot `src/equip.cpp` is a SUPERSEDED earlier design**, not retail
   (see snapshot note below). Treat snapshot as algorithmic intent only, NOT
   coordinates / layout / assets.
3. The CharacterPortrait spec's §14 paperdoll sketch was based on the
   snapshot's 2-section design (`src/equip.cpp:19-22` `EquipLoc[NUM_SECTS]`).
   That sketch is **wrong for retail** in shape (retail is single 188×306
   single-section, 11 slots laid around a single doll figure, not paginated
   top/bottom). Reuse only the InventoryImage()/DrawInvItem path; discard the
   2-section section-flipping.

**Snapshot note (supplementary — superseded by retail).** A 1998/99 version
exists at `src/equip.{h,cpp}` (`TEquipPane : public TButtonPane`,
`equip.h:37`). The snapshot is **structurally different** from retail:
- Snapshot: paginated 2-section paperdoll, two 168×128 chrome bitmaps
  (`equipscr.dat[0/1] = equipscrtop/equipscrbottom`, dump),
  pane rect `(MULTIPANEX=459, MULTIPANEY=346, MULTIPANEWIDTH=168,
  MULTIPANEHEIGHT=128)` = `(459, 346, 168, 128)` (`src/revdefs.h:142-145`,
  `equip.h:45`), per-section slot table `EquipLoc[2][11]`
  (`equip.cpp:19-22`), up/down scroll arrows flipping `section` between
  `SECT_TOP`/`SECT_BOTTOM` (`equip.cpp:24-32, 180-190`).
- Retail: single tall 188×306 chrome (`equippane.dat[0] = "Equip"`), all 11
  slots visible at once with NO section flip, pane rect `(452, 0, 188, 306)`
  (this spec §3), backed by a different vtable layout and a different
  asset archive (`equippane.dat` not `equipscr.dat`).

**Reusable from snapshot (algorithm intent only — NOT layout):**
- Field meanings: `equipdata` (TMulti*), `top`/`bottom` (PTBitmap chrome),
  `grabslot` / `heldslot` (int dragging), `section` (int) — retail likely
  has different fields (no section), but `grabslot`/`heldslot` semantics
  survive (`equip.h:69-77`).
- `OnSlot(x,y)` hit-test loop: walk `EquipLoc[section]`, return slot index
  if `(x,y)` falls in the `INVITEMREALWIDTH × INVITEMREALHEIGHT = 40×40`
  rect at the slot's (x,y) (`equip.cpp:201-210`). Retail likely runs the
  same shape with the (retail) slot-rect table; the 40×40 interior size is
  preserved (matches `equippane.dat` slot-icon dims).
- `DrawBackground` order: chrome → for each occupied slot,
  `Player->GetEquip(i)->DrawInvItem(x,y)` skipping the currently `grabslot`
  (`equip.cpp:134-168`). Reuse the order; retail almost certainly also
  draws the per-slot **empty-slot icon** (e.g. `Head`/`Chest`/…) when the
  slot is unequipped — this is the *retail-specific addition* that
  produced the 11 named placeholder bitmaps in `equippane.dat`. Snapshot
  showed nothing for empty slots; retail shows the placeholder.
- `MouseClick` grab/drop state machine: `MB_LEFTDOWN` → if slot has an
  item, set `heldslot = grabslot = slot` + `SetDragBitmap(item->InventoryImage(),
  x - slot.x, y - slot.y)` + `SetDragObj(item)`; `MB_LEFTUP` → if
  `grabslot >= 0` release the drag; else if `InPane(x,y)` try to drop the
  Inventory pane's `heldslot` here via `Player->CanEquip` /
  `Player->Equip` (`equip.cpp:53-132`). Reuse the algorithm; retail
  likely also handles drops *between* equip slots (the snapshot did not).
- `DrawAnim(inst, bm)` for partial redraws when the equipped item's
  animation frame changes: re-blits the icon at `EquipLoc[section][slot].x,
  .y` (`equip.cpp:192-199`). Retail's slot 22 `0x536970` is the per-slot
  redraw equivalent.

**Definitive: layout, coordinates, assets, archive name are RETAIL
(`equippane.dat`, 188×306, pane at (452,0,…)); the snapshot constants
`MULTIPANEX/Y/WIDTH/HEIGHT` and `equipscr.dat` names are SUPERSEDED.**

---

## §1 — Overview

The Equipment pane is the player's **paperdoll** — a tall right-sidebar
panel showing a body-frame backdrop with 11 fixed equipment slots arranged
around it (one per `EQ_*` enum slot in `src/player.h:21-31`). Each occupied
slot blits the equipped item's `.i3d` inventory icon over its slot well; an
unequipped slot shows a named placeholder bitmap (Head/Chest/Hand/…)
indicating the slot's role. The player drags items between slots, drags
**from** the Inventory pane below to equip something, and drags **to** the
Inventory pane to unequip; while a drag is in flight the dragged item's
icon follows the cursor and the source slot is drawn empty.
The slot begins the drag, but the top-level drag manager owns the transfer:
inventory/bar/equipment moves, playfield pickup/drop, replacement/swap/return,
and the action sound all belong to the same transaction owner.

Plain-language: it's the player's "what am I wearing" screen. A tall body
silhouette with hooks for head, neck, chest, two hands, two rings, ranged,
ammo, legs, feet. Each hook shows what's in it (or a hint icon for what
goes there). Click an item to grab it, drop it on another slot to swap or
on the inventory below to unequip.

**Instances:** single global (`DAT_0065b7e0`, `B_r11:164`).

**Shown/hidden:** managed by `TSideTabsPane` + the TPlayScreen command
dispatcher (`FUN_0047cf40`). Visibility is the upper-region modal state
`DAT_0065d1b8 == 0` (B.r11 §3, dispatcher case 9
`FUN_0047cf40:202-214`). When the dispatcher selects mode 0 it calls
`(*pane+0x28)()` on the pane global (vtable slot 10 = `Show` family) and
sets `DAT_0065d1b8 = 0`; the sibling `Inv` pane (lower mode 0) is selected
simultaneously by case 9.

**Visibility predicate inside paint** (snapshot parity, retail UNCONFIRMED
in body): dirty gate at slot 20 (`0x5366a0`) checks
`this->mbr_0x50 != 0` before calling slot 8 (the heavy chrome rebuild)
then slot 44 (button-children paint) (`cls_0x5a5320_TCharacter.cpp:8985-8993`).
This is the same `IsDirty()` gate the snapshot uses
(`src/equip.cpp:136-137`).

---

## §2 — Asset roster

Archive: `data/resources_unzipped/equippane.dat` (loaded via the
`DAT_0065dde4` TMulti handle; `meth_0x46d710(multi, name)` is a
**name-keyed `stricmp` lookup** — `recon/ghidra/cls_0x46d6b0.cpp:50-90`
shows it walks `mbr_0x4..mbr_0x4+N-1` of name offsets, `stricmp`s each
against the requested name `in_stack_00000004`, and returns the bitmap
offset on match. Initialize at `:8970-8977` iterates `PTR_DAT_005e3fb8`
(the name pointer table) passing each name to the lookup. Ghidra dropped
the second arg in the decomp of the loop body — the name from `*ppuVar4`
is the missing arg — but the XREFs on `PTR_DAT_005e3fbc..PTR_DAT_005e3fe0`
(slots 1..10 of the name table) confirm Initialize reads them. **Net
result: bitmaps are loaded by name in EQ_* enum order, NOT in archive
alphabetical order. The per-slot mbr offset is therefore
`mbr_0x68 + slot * 4` for EQ_* slot 0..10.**)
Measured with `tools/ui/dump_dat.py equippane.dat` →
`/tmp/dat_dump/equippane.{def,json}` (this session).

| asset | archive | entry (archive idx) | WxH (measured) | flags | role | mbr offset (after Init) | cite |
|---|---|---|---|---|---|---|---|
| `Equip` | equippane.dat | [0] | **188×306** | `0x2` (BM_15BIT, opaque, magenta-keyed) | chrome / pane background, doll figure + 11 slot wells | `mbr_0x64` | dump `equippane.def:1-12`; Init `cls_0x5a5320_TCharacter.cpp:8961` |
| `Head` | equippane.dat | [6] | **40×40** | `0x2` | empty-slot placeholder, slot 0 `EQ_HEAD` | `mbr_0x68` (slot 0) | dump; name table [0] `0x5e3fb8 → 0x5e3fe4 = "Head"` |
| `Neck` | equippane.dat | [8] | **40×40** | `0x2` | empty-slot placeholder, slot 1 `EQ_NECK` | `mbr_0x6c` (slot 1) | dump; name table [1] `0x5e3fbc → 0x5e3fec = "Neck"` |
| `Chest` | equippane.dat | [2] | **40×40** | `0x2` | empty-slot placeholder, slot 2 `EQ_BODY` | `mbr_0x70` (slot 2) | dump; name table [2] `0x5e3fc0 → 0x5e3ff4 = "Chest"` |
| `Hand2` | equippane.dat | [5] | **40×40** | `0x2` | empty-slot placeholder, slot 3 `EQ_OFFHAND` (off-hand) | `mbr_0x74` (slot 3) | dump; name table [3] `0x5e3fc4 → 0x5e3ffc = "Hand2"` |
| `Hand` | equippane.dat | [4] | **40×40** | `0x2` | empty-slot placeholder, slot 4 `EQ_PRIMEHAND` (primary hand) | `mbr_0x78` (slot 4) | dump; name table [4] `0x5e3fc8 → 0x5e4004 = "Hand"` |
| `Ring` | equippane.dat | [10] | **40×40** | `0x2` | empty-slot placeholder, slot 5 `EQ_R_ACCESSORY` | `mbr_0x7c` (slot 5) | dump; name table [5] `0x5e3fcc → 0x5e400c = "Ring"` |
| `Ring2` | equippane.dat | [11] | **40×40** | `0x2` | empty-slot placeholder, slot 6 `EQ_L_ACCESSORY` | `mbr_0x80` (slot 6) | dump; name table [6] `0x5e3fd0 → 0x5e4014 = "Ring2"` |
| `Range` | equippane.dat | [9] | **40×40** | `0x2` | empty-slot placeholder, slot 7 `EQ_RANGEDWEAPON` | `mbr_0x84` (slot 7) | dump; name table [7] `0x5e3fd4 → 0x5e401c = "Range"` |
| `Ammo` | equippane.dat | [1] | **40×40** | `0x2` | empty-slot placeholder, slot 8 `EQ_AMMO` | `mbr_0x88` (slot 8) | dump; name table [8] `0x5e3fd8 → 0x5e4024 = "Ammo"` |
| `Legs` | equippane.dat | [7] | **40×40** | `0x2` | empty-slot placeholder, slot 9 `EQ_LEGS` | `mbr_0x8c` (slot 9) | dump; name table [9] `0x5e3fdc → 0x5e402c = "Legs"` |
| `Feet` | equippane.dat | [3] | **40×40** | `0x2` | empty-slot placeholder, slot 10 `EQ_FEET` | `mbr_0x90` (slot 10) | dump; name table [10] `0x5e3fe0 → 0x5e4034 = "Feet"` |

Rows ordered by **EQ_* enum (= load order = mbr offset)** so the placeholder fetch
formula `placeholder = mbr_0x68 + slot * 4` reads top-to-bottom directly.

**Per-item (equipped) icon source:** NOT in `equippane.dat`. Each equipped
item's icon is the **same baked `.i3d` `invitem`** used by the HUD portrait
(see `CharacterPortrait_SPEC.md §6`): `oi->InventoryImage()` returns the
flat `TBitmap*` baked into the item's `.i3d` (`src/object.h:1066` →
`imagery->GetInvImage(state)`, retail call `(*item+0x130)()` =
`FUN_0046f190`). The pane code calls
`oi->DrawInvItem(x,y)` → `imagery->DrawInvItem` →
`Display.Put(x,y, invitem, DM_TRANSPARENT|DM_BACKGROUND)`
(`src/imagery.cpp:858-869`).

Notes:
- The chrome `Equip` 188×306 is **exactly the pane rect** (§3) — no chip
  model overhang. Slot wells are baked into the chrome art.
- All 11 slot placeholder bitmaps share `flags=0x2 = BM_15BIT` opaque
  (magenta-keyed) — the same flag family as the chrome. They are blitted
  with `DM_TRANSPARENT` so their magenta background drops out, leaving the
  placeholder pictogram framed by the chrome's well.

### Source-rect map (chrome `Equip` 188×306)

Slot wells are baked into the chrome art at fixed pixel positions; the
engine blits over each well at its (x,y). All 11 well positions are
now decoded from the data table at `0x5e3f60` and confirmed visually
against `/tmp/dat_dump/00_Equip.png` (Wave-6, §4). The arrangement
(EQ_* slot @ pane-local (x, y)):

```
Equip.bmp 188x306  (= pane rect, blit opaque at pane (0,0))
 ┌──────────────────────────────────────────┐
 │  RANGED(7)@(24,11)   HEAD(0)@(74,37)    AMMO(8)@(124,11)
 │                                            │
 │  NECK(1)@(7,58)                BODY/Chest(2)@(141,58)
 │                                            │
 │  PRIMEHAND(4)@(7,106)          OFFHAND(3)@(141,106)
 │                                            │
 │  R_ACCESSORY(5)@(7,154)        L_ACCESSORY(6)@(141,154)
 │                                            │
 │  LEGS(9)@(7,202)               FEET(10)@(141,202)
 │                                            │
 │   (… doll figure silhouette + gold compass-rose base baked into chrome,
 │    occupies center column between left and right well columns …)
 │   (gold compass-rose base at the bottom, ornamental — y≈250-290)
 └──────────────────────────────────────────┘
```

The (x, y) values above are authoritative — taken from the data table
at `DAT_005e3f60` (§4 Wave-6 table) with the EQ_* slot enum index
matching the table row index (cross-checked against the 11 visually
detected well positions on the chrome bitmap).

---

## §3 — Coordinate frames & surfaces

**Pane rect (screen):** `x=0x1c4 = 452`, `y=0`, `w=0xbc = 188`,
`h=0x132 = 306` — derived from the sibling Stats ctor
`cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp:6-21` (which sits in the
**same upper-region slot**, mode 1 instead of mode 0, and therefore shares
the upper-sidebar rect exactly — verified by chrome dimension equality:
both Stats and Equip chromes are 188×306). The EquipPane inlined ctor at
`0x4807a0` writes the same constants (`EquipSidebarPane.md:28`); the
specific instructions are not extracted in `recon/discovered/` and need
`DecompileAddr.java 0x4807a0` to confirm bit-for-bit (UNCONFIRMED-B). The
chrome bitmap's measured 188×306 equals the asserted pane (w,h)
independently.

Right edge `452+188 = 640`, bottom `0+306 = 306` → the pane fills the
**top-right corner** upper content slot of the right sidebar (lower slot is
the 188×174 Inventory at y=306). Inventory rect `(452, 306, 188, 174)` is
authoritative at `cls_0x5a58c0_InventorySidebarPane_ctor_487f50.cpp:6-14`
and is exactly vertically adjacent.

The live width/height the paint code reads are `mbr_0xc = 188` (width) and
`mbr_0x10 = 306` (height) — used for clip bounds (matches the Inventory
pattern, `InventoryPane_SPEC.md:149`).

**No chip model** here: the chrome bitmap == the pane rect, drawn at
pane-local `(0,0)`. All element coordinates below are **pane-local** unless
tagged screen.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | — | (0,0) | final pixels (640×480) | — |
| **right-sidebar** | screen | TR | screen `(screen_w − 188, 0)` = `(452, 0)` in 640-wide | the two stacked content slots (upper Equip + lower Inv/etc.) | derived (pane x = 452 = 640−188) |
| **equip-pane** | right-sidebar | TR | screen `(0x1c4, 0)` = `(452, 0)` = sidebar `(0, 0)` | the whole Equip pane (chrome + 11 slots + their item icons) | sibling Stats ctor `:6-21` (same rect); chrome dim cross-check; UNCONFIRMED-B for the inlined ctor literal |
| **slot(i)** | equip-pane | TL | pane-local `(slot[i].x, slot[i].y)` from `DAT_005e3f6c` | one equipment slot's 40×40 cell (empty-placeholder + equipped-icon) | slot-rect table `_data.txt:114046-114122`; UNCONFIRMED-A for the per-slot decode |
| **source-rect** | each bitmap | TL | (sx,sy) into the named asset | the sub-rect copied out (full-bitmap for chrome + each placeholder) | — |

**Anchor detection:** the pane's x is the constant `0x1c4 = 452`, but
`452 = 640 − 188` and the parent sidebar is right-aligned, so for resolution
re-layout treat the pane as **TR-anchored** to the screen, offset 188 from
the right edge, 306 from the top. (In Classic 640×480 it is the literal
constant.)

**Composition formula (to screen):**
```
screen_x = 452 + slot_x_in_pane
screen_y = 0   + slot_y_in_pane
```
Each slot's (x_in_pane, y_in_pane) comes from the per-slot table at
`DAT_005e3f60[i]` under the Wave-6 leading-candidate reading (11 ×
(x, y) pairs) — see §4 and the UNCONFIRMED-A list.

### Surfaces / two-stage blit

Like Inventory (`InventoryPane_SPEC.md §3`), retail's paint method (slot 21
`0x5366d0`, body UNCONFIRMED) composes into a **destination surface
`param_2`** — either the live display (`PTR_DAT_005d79e0`) directly, or an
off-screen pane buffer with a clip rect set first. The port follows the
**compose-to-target contract** (NOMENCLATURE §3): compose the pane chrome
+ placeholders + item icons into one offscreen `TSurface` RT, then
`DrawSurface` it to the HUD swapchain.

Initialize allocates and stores **12 bitmap handles** in `mbr_0x64..mbr_0xe0`
(consecutive `dword` fields starting at offset 0x64): the chrome at
`mbr_0x64`, then iterating `PTR_DAT_005e3fb8`+ for 11 placeholder names via
`FUN_0046d710(DAT_0065dde4)` and storing each at the next slot
(`cls_0x5a5320_TCharacter.cpp:8961-8969`).

### Layout diagram (pane-local)

```
equip-pane  (pane-local; origin = screen (452, 0))   w=188 h=306
0                                          188
0 ┌──────────────────────────────────────────┐
  │                                            │
  │  chrome `Equip` (full pane backdrop, w/   │  ← stage 1: opaque chrome blit
  │  baked slot wells)                         │     (slot 21 first op)
  │                                            │
  │  empty-slot placeholders (40x40 each)      │  ← stage 2: per slot
  │  drawn at slot[i].(x,y) when no item       │     (DM_TRANSPARENT)
  │  equipped in slot i                        │
  │                                            │
  │  equipped-item icons (40x40 .i3d invitem)  │  ← stage 3: per occupied slot
  │  drawn at slot[i].(x,y) when slot i has    │     oi->DrawInvItem(x,y)
  │  an item                                   │     (DM_TRANSPARENT|DM_BG)
  │                                            │
  │  (gold compass-rose base, baked into       │
  │   chrome, ornamental)                      │
306 └──────────────────────────────────────────┘
```

---

## §4 — Static element layout

All coordinates **pane-local** (compose with screen `(452, 0)` per §3). This
pane is **not mirrored** (single instance, single section). Per-slot
coordinates come from the table at `DAT_005e3f60` — Wave-6 decoded all 11
(x, y) anchors via raw byte read + visual cross-check against the chrome
bitmap. EQ → row mapping is HIGH-confidence under straight-enum-order
assumption (residual UNCONFIRMED-A: confirm the leaf-method loop iterates
this way).

### Static / chrome

| element | space | (x,y) | (w,h) | source | drawmode | cite |
|---|---|---|---|---|---|---|
| pane bg fill (if any — UNCONFIRMED) | pane | (0,0) | (188,306) | color | possibly `Box` `(*+0x64)` (sibling Inv pattern) | sibling `InventoryPane_SPEC.md:227` `DrawGrid:537ac6`; UNCONFIRMED-C for this pane |
| chrome `Equip` | pane | (0,0) | (188,306) | full bmp `mbr_0x64` | `0x80000000 = DM_USEDEFAULT` (opaque) — sibling pattern | Init `:8961`; sibling Inv `DrawGrid:537ada` |

### Per-slot (placeholder + equipped icon)

The 11 slots share a uniform 40×40 cell size (matches every empty-slot
bitmap measured WxH and the `INVITEMREALWIDTH/HEIGHT=40` snapshot constant,
`src/revdefs.h:131-132`). The per-slot anchor (x,y) lives in the data
region `DAT_005e3f60..0x5e3fb7` — **88 bytes = 11 × (x, y) pairs** in
EQ_* enum order (Wave-6 decoded; all 11 entries verified against
visual chrome wells). The first ints that Ghidra labelled as separate
scalars (`DAT_005e3f60=74`, `DAT_005e3f64=37`, `DAT_005e3f68=7`) are
NOT scalars — they are the first 1.5 entries of the per-slot table
itself: slot 0 (HEAD) x=74, slot 0 y=37, slot 1 (NECK) x=7. The
remaining residual UNCONFIRMED-A item is a one-line leaf-method check
that confirms the iteration order (the visual all-11-match makes
permuted-order extremely unlikely).

Below the in-row format is `slot i (NAME) | EQ_* enum | pane (x,y) | placeholder bitmap | mbr offset | equipped-icon source | drawmode (per slot)`:

| slot i | enum constant (value) | pane (x, y) (Wave-6 decoded; ALL 11 match visual chrome wells) | well visual TL (template-match) | placeholder bmp | mbr offset (`this+`) | equipped-icon source | drawmode (per slot) |
|---|---|---|---|---|---|---|---|
| 0 | `EQ_HEAD` (0) | **(74, 37)** | (65, 37) — top-CENTER, slightly dropped | `Head` 40×40 | `0x68` | `Player->GetEquip(0)->InventoryImage()` | placeholder: `0x100 = DM_TRANSPARENT`; icon: `0x110 = DM_TRANSPARENT \| DM_BACKGROUND` (snapshot `imagery.cpp:869`) |
| 1 | `EQ_NECK` (1) | **(7, 58)** | (2, 50) — row 2 LEFT | `Neck` 40×40 | `0x6c` | `Player->GetEquip(1)->InventoryImage()` | same |
| 2 | `EQ_BODY` (2) | **(141, 58)** | (138, 49) — row 2 RIGHT | `Chest` 40×40 | `0x70` | `Player->GetEquip(2)->InventoryImage()` | same |
| 3 | `EQ_OFFHAND` (3) | **(141, 106)** | (138, 96) — row 3 RIGHT (off-hand) | `Hand2` 40×40 | `0x74` | `Player->GetEquip(3)->InventoryImage()` | same |
| 4 | `EQ_PRIMEHAND` (4) | **(7, 106)** | (2, 96) — row 3 LEFT (prime-hand) | `Hand` 40×40 | `0x78` | `Player->GetEquip(4)->InventoryImage()` | same |
| 5 | `EQ_R_ACCESSORY` (5) | **(7, 154)** | (3, 145) — row 4 LEFT (right accessory) | `Ring` 40×40 | `0x7c` | `Player->GetEquip(5)->InventoryImage()` | same |
| 6 | `EQ_L_ACCESSORY` (6) | **(141, 154)** | (138, 144) — row 4 RIGHT (left accessory) | `Ring2` 40×40 | `0x80` | `Player->GetEquip(6)->InventoryImage()` | same |
| 7 | `EQ_RANGEDWEAPON` (7) | **(24, 11)** | (20, 4) — top LEFT | `Range` 40×40 | `0x84` | `Player->GetEquip(7)->InventoryImage()` | same |
| 8 | `EQ_AMMO` (8) | **(124, 11)** | (120, 2) — top RIGHT | `Ammo` 40×40 | `0x88` | `Player->GetEquip(8)->InventoryImage()` | same |
| 9 | `EQ_LEGS` (9) | **(7, 202)** | (2, 192) — row 5 LEFT | `Legs` 40×40 | `0x8c` | `Player->GetEquip(9)->InventoryImage()` | same |
| 10 | `EQ_FEET` (10) | **(141, 202)** | (138, 192) — row 5 RIGHT | `Feet` 40×40 | `0x90` | `Player->GetEquip(10)->InventoryImage()` | same |

**EQ → row mapping is HIGH-confidence** under the assumption that the
slot-rect table at `0x5e3f60` is in EQ_* enum order, identical to the
name-pointer table at `0x5e3fb8` (which Initialize iterates in enum
order to load placeholders into `mbr_0x68 + i*4`). The cross-checks:

1. **Slot 0 (HEAD) at (74, 37)** matches the top-center well that is
   distinctly the "head" position (the only single-well row, with the
   doll's body silhouette starting at the neckline).
2. **All 11 (x, y) entries map 1-to-1 to the 11 visually-detected
   wells** with consistent frame-inset Δ ∈ [3-9, 0-10] (see Wave-6
   table below).
3. **The layout is visually coherent as a paperdoll**: head top-center;
   neck/body row 2; primary/off hands row 3; left/right accessories
   row 4; legs/feet row 5; ranged + ammo on the shoulder-top L/R
   positions. The retail layout looks like a wide-shouldered humanoid
   with hands by the chest, accessories on the wrists, then legs/feet
   at the bottom — coherent.
4. **L/R chirality:** the assignment puts `EQ_PRIMEHAND` (right-handed
   primary) on the LEFT side of the pane (x=7), and `EQ_OFFHAND`
   (left-handed off-hand) on the RIGHT side. This is the **mirror
   convention** of a paperdoll viewed face-on: the viewer's LEFT is the
   character's RIGHT hand. Same for `EQ_R_ACCESSORY` (player's right
   wrist) on LEFT (x=7) and `EQ_L_ACCESSORY` on RIGHT (x=141). Likewise
   `EQ_LEGS` (player's "left" leg? or just "legs slot" placed left)
   and `EQ_FEET` (right). This is **HIGH-confidence the correct
   mirror convention** since `EQ_PRIMEHAND` is canonically the
   right-handed primary weapon and the LEFT-of-viewer position
   corresponds to the character's right side.
5. **Ranged/Ammo on the shoulders** is the classic JRPG/RPG layout
   (bow on left shoulder, quiver on right shoulder, viewed face-on),
   consistent with `EQ_RANGED` (slot 7) at top-LEFT (24, 11) and
   `EQ_AMMO` (slot 8) at top-RIGHT (124, 11).

This is now HIGH-confidence on (x, y) values AND on EQ slot assignment;
the only remaining UNCONFIRMED item is whether the leaf-method body
actually iterates `0x5e3f60` (vs `0x5e3f64` or some other offset) in
straight EQ-enum order — see UNCONFIRMED-A §14 for the one-line
disassembly check.

**Slot↔name↔mbr-offset correspondence is HIGH-confidence** — chain:
1. Initialize loads BY NAME via the name-keyed `meth_0x46d710` lookup
   (`recon/ghidra/cls_0x46d6b0.cpp:50-90`).
2. Names come from `PTR_DAT_005e3fb8[0..10]` in the order Head, Neck,
   Chest, Hand2, Hand, Ring, Ring2, Range, Ammo, Legs, Feet
   (`_data.txt:114124-114153`).
3. That order matches `src/player.h:21-31` EQ_* enum exactly.
4. Initialize stores them sequentially at `mbr_0x68, mbr_0x6c, …, mbr_0x90`
   (`cls_0x5a5320_TCharacter.cpp:8970-8977`).
5. Therefore `placeholder[slot] = *(this + 0x68 + slot * 4)`.

**Per-slot (x, y) is HIGH-confidence** (Wave-6) — all 11 entries decoded
from `DAT_005e3f60` and matched 1-to-1 against visual chrome wells. See
the raw-byte table below for the data dump + visual cross-check.
Residual UNCONFIRMED-A is now scoped to a one-line leaf-method check.

### Wave-6 raw 11-pair reading of `DAT_005e3f60..DAT_005e3fb7`

The full region from `0x5e3f60` to the name-pointer table at `0x5e3fb8`
is **88 bytes = 22 ints = 11 × (x, y) pairs**. Reading as `(x, y)`
pairs starting at `0x5e3f60` gives one entry per EQ_* slot. The raw
byte dump comes from `recon/ghidra/_data.txt:114022-114123`:

| Idx | x addr | y addr | x (hex) | x (dec) | y (hex) | y (dec) | visual chrome well match (template-matched TL of 40×40, `00_Equip.png` analyzed this pass) |
|---|---|---|---|---|---|---|---|
| 0 | 0x5e3f60 | 0x5e3f64 | 0x4a | **74**  | 0x25 | **37**  | top-center, visual TL (65, 37) — Δx=9, Δy=0 ✓ (frame inset) |
| 1 | 0x5e3f68 | 0x5e3f6c | 0x07 | **7**   | 0x3a | **58**  | left-col row 2, visual TL (2, 50) — Δx=5, Δy=8 ✓ |
| 2 | 0x5e3f70 | 0x5e3f74 | 0x8d | **141** | 0x3a | **58**  | right-col row 2, visual TL (138, 49) — Δx=3, Δy=9 ✓ |
| 3 | 0x5e3f78 | 0x5e3f7c | 0x8d | **141** | 0x6a | **106** | right-col row 3, visual TL (138, 96) — Δx=3, Δy=10 ✓ |
| 4 | 0x5e3f80 | 0x5e3f84 | 0x07 | **7**   | 0x6a | **106** | left-col row 3, visual TL (2, 96) — Δx=5, Δy=10 ✓ |
| 5 | 0x5e3f88 | 0x5e3f8c | 0x07 | **7**   | 0x9a | **154** | left-col row 4, visual TL (3, 145) — Δx=4, Δy=9 ✓ |
| 6 | 0x5e3f90 | 0x5e3f94 | 0x8d | **141** | 0x9a | **154** | right-col row 4, visual TL (138, 144) — Δx=3, Δy=10 ✓ |
| 7 | 0x5e3f98 | 0x5e3f9c | 0x18 | **24**  | 0x0b | **11**  | top-LEFT well, visual TL (20, 4) — Δx=4, Δy=7 ✓ |
| 8 | 0x5e3fa0 | 0x5e3fa4 | 0x7c | **124** | 0x0b | **11**  | top-RIGHT well, visual TL (120, 2) — Δx=4, Δy=9 ✓ |
| 9 | 0x5e3fa8 | 0x5e3fac | 0x07 | **7**   | 0xca | **202** | left-col row 5, visual TL (2, 192) — Δx=5, Δy=10 ✓ |
| 10 | 0x5e3fb0 | 0x5e3fb4 | 0x8d | **141** | 0xca | **202** | right-col row 5, visual TL (138, 192) — Δx=3, Δy=10 ✓ |

**ALL 11 ROWS CLEANLY MATCH ALL 11 VISIBLE CHROME WELLS** with a
consistent Δx ∈ [3, 9] and Δy ∈ [0, 10] frame-inset displacement.
This is **HIGH-confidence** for the per-slot (x, y) anchor positions
themselves. The byte-pair packing decoded here is the correct one: the
**data table at `0x5e3f60` is 11 × {int x, int y} pairs** in a single
88-byte block. The three "scalars" Ghidra labelled separately
(`0x5e3f60`, `0x5e3f64`, `0x5e3f68` = 74, 37, 7) are NOT scalars; they
are the first three ints of the table — slot 0 x, slot 0 y, slot 1 x.

(Two prior decoding mistakes are now superseded: the Wave-4A reading
treated `0x5e3f6c` as the table base with 9 8-byte entries — that was
off-by-one in the table base AND included three bogus "scalar" values
as separate fields. The Wave-5 (y, x) byte-order reading also kept the
wrong base and is similarly superseded.)

The chrome image template-matching found exactly **11 wells** at these
TL positions (sorted by row top-to-bottom):

```
Row 1 (top, y≈4-11):  (20, 4)   (65, 37)  (120, 2)    ← 3 wells (LEFT, CENTER-HEAD, RIGHT)
Row 2     (y≈58):     (2, 50)              (138, 49)
Row 3     (y≈106):    (2, 96)              (138, 96)
Row 4     (y≈154):    (3, 145)             (138, 144)
Row 5     (y≈202):    (2, 192)             (138, 192)
```

(The HEAD CENTER well is visually slightly lower than the top-LEFT/RIGHT
wells — y=37 vs y=4 — because the doll figure's head silhouette starts
just below the shoulder level where Ranged and Ammo sit.)

**All 11 DATA rows match all 11 visual wells 1-to-1 under the
straight-EQ-enum reading** (DATA idx 0..10 = EQ_HEAD..EQ_FEET). Mapping
table:

| DATA idx (EQ slot) | DATA (x, y) | visual TL | Δ(x, y) |
|---|---|---|---|
| 0 = EQ_HEAD | (74, 37) | (65, 37) | (9, 0) |
| 1 = EQ_NECK | (7, 58) | (2, 50) | (5, 8) |
| 2 = EQ_BODY | (141, 58) | (138, 49) | (3, 9) |
| 3 = EQ_OFFHAND | (141, 106) | (138, 96) | (3, 10) |
| 4 = EQ_PRIMEHAND | (7, 106) | (2, 96) | (5, 10) |
| 5 = EQ_R_ACCESSORY | (7, 154) | (3, 145) | (4, 9) |
| 6 = EQ_L_ACCESSORY | (141, 154) | (138, 144) | (3, 10) |
| 7 = EQ_RANGED | (24, 11) | (20, 4) | (4, 7) |
| 8 = EQ_AMMO | (124, 11) | (120, 2) | (4, 9) |
| 9 = EQ_LEGS | (7, 202) | (2, 192) | (5, 10) |
| 10 = EQ_FEET | (141, 202) | (138, 192) | (3, 10) |

The Δ(x, y) column shows a tight, consistent frame-inset of ~3-9 px
in x and ~0-10 px in y. This is exactly the kind of displacement you
expect between a template-matcher's well-frame TL and the bitmap-blit
position used by the engine: the visual detector lands on the bright
corner pixel of the well frame, while the engine blits the 40×40
placeholder bitmap at a position that starts a few pixels inside the
frame so the placeholder pictogram sits centred in the well.

**Assessment.** The 11-pair-from-`0x5e3f60` reading is **HIGH-confidence
correct** as of Wave-6: every (x, y) entry matches a unique visual chrome
well, the visual layout is a coherent paperdoll (HEAD center; NECK/BODY
row 2 L/R; HAND/OFFHAND row 3 L/R; ACCESSORIES row 4 L/R; RANGED/AMMO
shoulder-top L/R; LEGS/FEET row 5 L/R), and the L/R mirror convention
matches paperdoll-viewed-face-on (PRIMEHAND on viewer's LEFT = character's
right hand). The 88-byte region is fully accounted for as 11 × 8-byte
(x, y) pairs with no sentinels and no leftover.

**Note on the earlier "scalar" labels.** Ghidra's labels at `0x5e3f60`
(74), `0x5e3f64` (37), `0x5e3f68` (7) as "separate scalars" were a
disassembly artefact: the leaf methods load these individually as
`slot[0].x`, `slot[0].y`, `slot[1].x` rather than via a loop, which
made Ghidra promote them to separate symbols. The full coordinate
table `slot[0..10].{x, y}` is structurally one 88-byte block beginning
at `0x5e3f60`.

**One remaining one-line check** (kept under UNCONFIRMED-A for paranoia):
that the leaf methods actually iterate this table in straight enum
order (i.e. `slots[EQ_HEAD]` is at offset 0, not at some permuted
position). This is supported by the name-pointer table being in enum
order (`PTR_DAT_005e3fb8` → "Head", `PTR_DAT_005e3fbc` → "Neck", …) and
by Initialize storing placeholders sequentially at
`mbr_0x68 + slot*4`. But the iterating loop itself in `0x5366d0` /
`0x536970` / `0x5363e0` is not extracted; `DecompileAddr.java 0x5366d0`
would confirm.

### Superseded readings (Wave-4A and Wave-5)

Two earlier readings of this data region were proposed and superseded
by the Wave-6 reading:

- **Wave-4A**: 9 (x, y) pairs starting at `0x5e3f6c` with three scalars
  at `0x5e3f60..0x5e3f6b`. This had only 9 entries vs 11 EQ slots, and
  entry [8] = (202, 141) had x past pane width. Wave-6 supersedes:
  the apparent "scalars" at `0x5e3f60` are the first 1.5 entries of
  the table, the actual base is `0x5e3f60`, and there are 11 entries
  not 9.
- **Wave-5**: (y, x) byte-order variant of the Wave-4A reading. Same
  problem; superseded by Wave-6.

The Wave-6 reading also resolves the duplicate-row mystery from
Wave-4A: when the table base is `0x5e3f6c` you see duplicate (x, y)
pairs [(58, 141), (58, 141)] and [(106, 7), (106, 7)] because the
8-byte alignment cuts each true 8-byte entry in half (showing only
one int per row, with the next row showing the same int as the next
entry's x). Re-aligning to `0x5e3f60` produces 11 unique (x, y) pairs
with NO duplicates.

### Per-slot composition (when (x,y) is known)

Once slot `i`'s pane-local `(slot[i].x, slot[i].y)` is decoded, the
composed-to-screen draw position is:
```
screen_x = 452 + slot[i].x
screen_y = 0   + slot[i].y
draw_w   = 40
draw_h   = 40
```

---

## §5 — Draw order / composition

Driven by slot 20 (`0x5366a0`, `Show`/DrawBackground gate): if
`this->mbr_0x50 != 0` (dirty) it calls slot 8 (the heavy
`virt_meth_0x4920b0_8` — `TPane::DrawBackground`-ish) then slot 44
(`virt_meth_0x490530_44` — the per-child-button draw)
(`cls_0x5a5320_TCharacter.cpp:8985-8993`). This mirrors the snapshot's
`if (!IsDirty()) TButtonPane::DrawBackground(); else { … chrome … per-slot
icons … TButtonPane::DrawBackground(); }` (`src/equip.cpp:134-168`).

**Slot 21 paint sequence (`0x5366d0`, body UNCONFIRMED, inferred from
snapshot + sibling InventoryPane + the leaf method's XREFs to the slot-rect
table):**
1. (likely) Pane-bg fill via `(*pane_buffer+0x64)(0,0,188,306,…)` —
   `Box`/`Display.Put` clear — UNCONFIRMED-C.
2. Chrome blit: `FUN_004bd680(0, 0, mbr_0x64, 0x80000000, 0)` — opaque blit
   of "Equip" 188×306 at pane (0,0). (Sibling Inv pattern.)
3. **Per-slot loop, i = 0..10:**
   a. Resolve item: `item = Player->GetEquip(i)` — retail call shape
      `(*Player+<eqgetter>)(i)` (UNCONFIRMED-D for the exact vtable
      offset; snapshot `src/player.h:255` `equipment[NUM_EQ_SLOTS]` is
      the canonical field).
   b. If `i == this->grabslot` (`mbr_0x94`) skip the icon (the grabbed slot
      draws empty while a drag is in flight) — snapshot parity
      `src/equip.cpp:144-145`.
   c. **Empty slot:** if `item == null`, blit the placeholder bitmap
      `*(this + 0x68 + i * 4)` at `(slot[i].x, slot[i].y)` with
      `0x100 = DM_TRANSPARENT`. The per-slot `(x, y)` is now decoded
      (§4 Wave-6 table); the per-slot mbr offset is confirmed (§2 + §4
      chain). This is the retail-added behavior (snapshot left empty
      slots blank).
   d. **Occupied slot:** `item->DrawInvItem(slot[i].x, slot[i].y)` →
      `imagery->DrawInvItem(item, x, y)` →
      `Display.Put(x, y, item->InventoryImage(), DM_TRANSPARENT|DM_BACKGROUND)`
      (`src/object.h:1036`, `imagery.cpp:858-869`).
4. (no scroll-arrow buttons in retail — snapshot's up/down arrows are
   superseded; the single-section design has no pagination.)
5. Finalize: dirty-rect register / compose-to-target as in Inventory
   (`InventoryPane_SPEC.md:312-314`).

Then slot 44 paints any button children (none in this pane's retail design,
so this is a no-op tail).

---

## §6 — Algorithms

### 6.1 Slot address ↔ hit (the doll kernel) — UNCONFIRMED-A

The retail kernel is:

```
# paint: slot -> pane (x,y) — Wave-6 decoded table at DAT_005e3f60
(x, y) = slot_rect_table[i]                     # 11 × (int x, int y) at 0x5e3f60
draw_at (x, y) size (40, 40)

# hit-test: (px, py) -> slot
for i in 0..10:
    (sx, sy) = slot_rect_table[i]
    if px >= sx and px < sx+40 and py >= sy and py < sy+40:
        return i
return -1
```

This is the **same shape** as snapshot `TEquipPane::OnSlot`
(`src/equip.cpp:201-210`) — a linear scan over the slot table testing each
40×40 rect. Snapshot used a per-section table `EquipLoc[section][NUM_EQ_SLOTS]`
(11 slots × 2 sections, `equip.cpp:19-22`); retail collapses to a single
section, so the table is 11 entries flat.

The 40-px interior tolerance comes from `INVITEMREALWIDTH=40` (snapshot,
`revdefs.h:131`) and matches every placeholder bitmap WxH in
`equippane.dat`.

The values previously labelled as "scalars" — `DAT_005e3f60=74`,
`DAT_005e3f64=37`, `DAT_005e3f68=7` — are NOT scalars; under the Wave-6
reading they are the first 1.5 (x, y) entries of the table itself
(`slot[0].x=74`, `slot[0].y=37`, `slot[1].x=7`). The Wave-4A guess that
they were hit-test fudge factors or doll-figure anchor coordinates is
superseded.

### 6.2 `oi->InventoryImage()` chain (per-slot icon fetch)

Identical to the HUD portrait chain (`CharacterPortrait_SPEC.md §6`):

```
oi->InventoryImage()                            # src/object.h:1066
  -> imagery->GetInvImage(state)                # src/imagery.h:380
     -> icons[state].invitem                    # src/3dimage.cpp:2005-2014
```

Returns the flat baked `TBitmap*` for the item's current state. The pane's
per-slot draw calls `oi->DrawInvItem(x,y)` which wraps this:

```
TObjectImagery::DrawInvItem(oi, x, y):           # src/imagery.cpp:858-869
    state = oi->GetState()
    if state >= NumStates(): return
    bm = oi->InventoryImage()
    if not bm: return
    Display.Put(x, y, bm, DM_TRANSPARENT | DM_BACKGROUND)
```

In the port the canonical primitive is `Renderer->DrawBitmapToTarget(rt,
bm, x, y)` with the source's chroma key honored — same as the Inventory
spec uses for item-cell icons (`InventoryPane_SPEC.md:583-585`).

### 6.3 Empty-slot placeholder draw (the retail addition)

For slot i with no equipped item, blit `mbr_0x64 + (i+1)*4` at
`(slot[i].x, slot[i].y)` with `DM_TRANSPARENT (0x100)`. The placeholder
bitmaps' name→slot mapping is name-table order (HIGH confidence, §2):

| Name-ptr table idx | Bitmap | EQ_* enum |
|---|---|---|
| 0 (`PTR_DAT_005e3fb8`) | `Head` | EQ_HEAD (0) |
| 1 | `Neck` | EQ_NECK (1) |
| 2 | `Chest` | EQ_BODY (2) |
| 3 | `Hand2` | EQ_OFFHAND (3) |
| 4 | `Hand` | EQ_PRIMEHAND (4) |
| 5 | `Ring` | EQ_R_ACCESSORY (5) |
| 6 | `Ring2` | EQ_L_ACCESSORY (6) |
| 7 | `Range` | EQ_RANGEDWEAPON (7) |
| 8 | `Ammo` | EQ_AMMO (8) |
| 9 | `Legs` | EQ_LEGS (9) |
| 10 | `Feet` | EQ_FEET (10) |

The chrome `mbr_0x64` is the first slot (read by Initialize as the chrome
itself, `:8961`), then each placeholder gets stored in
`mbr_0x68, mbr_0x6c, … mbr_0x90` (12 dwords × 4 bytes = 48 bytes spanning
`mbr_0x64..mbr_0x90`). The actual stored offset for each placeholder needs
the Initialize disassembly's `pdVar3` pointer-walk to confirm
(`:8963-8969`), but the iterator pattern matches Inventory's chrome+icons
load.

### 6.4 Drag promote / state machine (mirrors snapshot, retail body UNCONFIRMED)

The snapshot's `MouseClick` (`src/equip.cpp:53-132`) gives the algorithm
shape, paraphrased here:

```
# MB_LEFTDOWN at (x,y):
slot = OnSlot(x, y)
if slot >= 0 and Player.GetEquip(slot) and
   Player.GetEquip(slot).InventoryImage():
    heldslot = grabslot = slot
    SetDragBitmap(Player.GetEquip(slot).InventoryImage(),
                  x - slot[slot].x, y - slot[slot].y)
    SetDragObj(Player.GetEquip(slot))
    Update()

# MB_LEFTUP at (x,y):
if grabslot >= 0:
    grabslot = -1
    SetDragBitmap(null); SetDragObj(null)
    Update()
elif InPane(x, y):
    # drop from inventory into this pane
    inst = Inventory.GetContainer().GetInventorySlot(Inventory.GetHeldSlot())
    if inst:
        slot = inst.GetStat("eqslot")
        if 0 <= slot < NUM_EQ_SLOTS and Player.CanEquip(inst, slot):
            # ring-accessory left/right swap allowed
            if Player.GetEquip(slot):
                if slot == EQ_R_ACCESSORY and not Player.GetEquip(EQ_L_ACCESSORY):
                    slot = EQ_L_ACCESSORY
                elif slot == EQ_L_ACCESSORY and not Player.GetEquip(EQ_R_ACCESSORY):
                    slot = EQ_R_ACCESSORY
            # swap-or-move
            if inst.GetOwner() == Player:
                if Player.GetEquip(slot):
                    Player.GetEquip(slot).SetInventNum(inst.InventNum())
                inst.SetInventNum(256 + slot)
            else:
                inst.RemoveFromInventory()
                if Player.GetEquip(slot):
                    Player.GetEquip(slot).RemoveFromInventory()
                    Inventory.GetContainer().AddToInventory(
                        Player.GetEquip(slot), Inventory.GetHeldSlot())
                Player.AddToInventory(inst, 256 + slot)
            Player.Equip(inst, slot)
Inventory.SetDirty(true); SetDirty(true)
```

The retail body at `0x5363e0` (slot 25) almost certainly matches this
shape — it XREFs the same slot-rect table (`DAT_005e3f6c`) AND the scalar
hit-test constants (`DAT_005e3f60`, `DAT_005e3f64`, `DAT_005e3f68`), and
the parallel InventoryPane MouseClick (`InventoryPane_SPEC.md:488-507`) uses
the same Inventory↔Equip handoff via `heldslot`. The `256 + slot` invariant
on `InventNum()` (a snapshot device for "this item is equipped, not in the
grid") is what `TInventoryIterator` skips in the grid (snapshot
`src/inventory.cpp:50-83`).

UNCONFIRMED-E: whether retail also dispatches inter-Equip-slot swaps (e.g.
drag chest → head) or only inventory↔equip — snapshot's MouseClick logic
does NOT handle that case (`equip.cpp:75-84` releases on left-up without
slot resolution). Retail might.

---

## §7 — Effects & shadows

- **Chrome** `Equip` drawn opaque (`0x80000000 = DM_USEDEFAULT`) — no drop
  shadow. (Standard pane-chrome pattern.)
- **Empty-slot placeholders** drawn `0x100 = DM_TRANSPARENT` — magenta
  (0x7C1F) chroma key applied, standard 15-bit RGB555 (UI_METHOD_MAP §16).
  No drop shadow on placeholders.
- **Equipped-item icons** drawn `0x110 = DM_TRANSPARENT | DM_BACKGROUND` —
  same as snapshot `src/imagery.cpp:869`. Chroma-keyed; `DM_BACKGROUND`
  selects the background draw path (preserves the chrome behind the icon's
  transparent border).
- **No drop shadow** on slot icons (`(dx,dy)` shadow setup helper
  `FUN_00438d80` is NOT XREF'd from any of the four leaf methods —
  UNCONFIRMED-F is that the body decode confirms this absence). Compare
  TPlyrStatusBar which DOES use shadowed sprite blits.
- **No text** rendering on this pane (no count labels, no slot names
  drawn). The placeholder bitmap *is* the slot label (a pictogram).
  Therefore no `FUN_004be2b0` text composite, no pink-halo concern, no
  shadow-text family.
- **Drag bitmap** for an in-flight drag: rendered by the global cursor
  layer (`SetDragBitmap` in `src/cursor.h:26`), not by the pane itself.
  The pane only sets it on grab and clears it on release.
- Final composite registers a blit-effect dirty rect (sibling pattern
  `FUN_004aacb0(...,6)`); the port's `DrawSurface` of the composed RT
  replaces this (UI_METHOD_MAP §8).

---

## §8 — Text rendering

**N/A.** This pane has no text. The 11 slot wells use baked pictogram
bitmaps (`Head`/`Chest`/…) as their visual labels. No `FUN_004be2b0` /
`FUN_004be110` calls. No font.

(Cross-check: none of the four extracted leaf methods bodies — slot 0/19/20
— calls a text helper, and the asset-name table at `_data.txt:114124-114153`
holds bitmap names only. Slot 21/25/22 bodies UNCONFIRMED but no font-table
XREFs from `DAT_0065b020` are visible in the surrounding region.)

---

## §9 — Animation & dynamic behavior

This pane has **no continuous tween**. Dynamic behavior is event-driven
redraw + a drag state machine + per-tick animated equipped items.

**Dirty/redraw model.** Slot 20 gates paint on `mbr_0x50 != 0`
(`cls_0x5a5320_TCharacter.cpp:8988`). `mbr_0x50` is the dirty flag (sibling
parity = `IsDirty()`). Redraw is requested by `SetDirty(true)` whenever
equipment changes (snapshot `src/equip.cpp:48,189`,
`src/player.cpp:275,326`). Port: use a monotonic version counter
([[feedback-versions-over-flags]]).

**Animate (slot 19, `0x5368c0`, body extracted at
`cls_0x5a5320_TCharacter.cpp:8997-9062`):** runs only when
`DAT_00667fcc != 0` (Player exists, `:9017-9018`). It does a per-tick
animation step: increments `mbr_0xc4` (animation frame counter), gates on
`mbr_0xbc` (current animation id) against
`character->meth_0x46e8a0().vtable[0x90](mbr_0xbc)` (the animation length),
then if exhausted re-fetches the next idle animation via
`character->vtable[0x131]()` (`virt_meth_0x477c10_312`) and resets the
counter (`:9019-9062`). It also copies a fixed string from `DAT_005e4060`
into `mbr_0x9c` (`:9046-9059`) — this is plausibly the **idle animation
name** baked at `0x5e4060` (UNCONFIRMED-G; would need a string-region
re-dump to confirm content). Net effect: when an equipped item has an
animated `.i3d` icon (`invanim` instead of static `invitem`,
`CharacterPortrait_SPEC.md §9`), this slot steps it; otherwise it's a
cheap nop.

**Hover / cursor-over-item:** UNCONFIRMED — snapshot does this via
`Animate(bool draw)` (`src/equip.cpp:171-178`) calling `OnSlot` against
cursor coords and `CursorOverObject(inst)` (tooltip). The retail slot 19 in
recon does NOT do hover resolution — it animates the equipped item only,
suggesting hover/tooltip lives elsewhere (perhaps slot 22 `0x536970` or a
sibling). UNCONFIRMED-H.

**Drag state machine** (fields, mirroring snapshot — retail field offsets
UNCONFIRMED-I; snapshot field roles in `src/equip.h:69-77`):

| field (snapshot) | retail offset (likely) | meaning | cite |
|---|---|---|---|
| `equipdata` | `mbr_0x0` per yaml | `TMulti*` for equippane.dat — or NULL (retail loads via the global iterator instead) | `equip.h:69`; retail Init uses `DAT_0065dde4` |
| `top` (chrome) | `mbr_0x64` per Init | chrome `PTBitmap` | Init `:8961` |
| `bottom` | n/a — retail has no second section | — | — |
| `section` | n/a — retail has no section flip | — | — |
| `grabslot` | `mbr_0x94` (likely) | slot grabbed (-1 = none) | Init stores `0xffffffff` `cls_0x5a5320_TCharacter.cpp:8979`; snapshot `equip.h:75`, `equip.cpp:67,77,80` |
| `heldslot` | `mbr_0x98` (likely) | held-for-transfer slot | Init stores `0xffffffff` `cls_0x5a5320_TCharacter.cpp:8980`; snapshot `equip.h:76`, `equip.cpp:67,69` |
| placeholders[0..10] | `mbr_0x68 + slot*4` | 11 empty-slot `TBitmap*` indexed by EQ_* | Init `:8969-8977` + §2/§4 chain |
| anim cur id | `mbr_0xbc` | current animation id (-1 = none) | Init `:8982`, Animate `:9031,9040,9043` |
| anim prev id | `mbr_0xc0` | prior animation id | Init `:8983`, Animate `:9040` |
| anim frame counter | `mbr_0xc4` | per-tick frame counter (-1 = stopped) | Init `:8984`, Animate `:9027,9029,9055` |
| anim prev frame | `mbr_0xc8` | prior frame counter | Init `:8985`, Animate `:9041` |
| anim name (memcpy dest) | `mbr_0x9c..mbr_0xb8` (string buf) | idle anim name copied from `DAT_005e4060` | Animate `:9046-9061` |

```
ramp/threshold drag-promote (snapshot-parity, retail UNCONFIRMED):
  trigger  LEFTDOWN on a non-empty slot sets grab=heldslot=slot,
           attaches drag bitmap = oi.InventoryImage() with grab-offset
           (x - slot[slot].x, y - slot[slot].y)
           cite snapshot src/equip.cpp:60-73
  in-flight chrome is repainted with this slot drawn empty (grabslot
           is skipped in the slot loop, snapshot equip.cpp:144-145)
  drop     LEFTUP: if grabslot>=0 just release (drop on world via cursor
           system); if InPane and Inventory has a held item, resolve the
           target equip slot and do swap/move/equip per §6.4
```

Port note: do not make equipment slots responsible for global ownership. A
slot may identify itself as `EQ_HEAD`/`EQ_BODY`/etc. and start or accept a
drag, but `UIDragState`/`TPlayScreen` must commit the transfer, including
swap/replacement, returning rejected items, playfield pickup/drop, and the
pickup/drop sound cue. The current harness cue is placeholder audio until the
exact retail sound name is verified.

---

## §10 — Input & dispatch

Interactive. Primary handler is **vtable slot 25 = `0x5363e0`** (MouseClick,
body UNCONFIRMED). Mouse-move (drag-promote) is plausibly slot 24
`virt_meth_0x445060` (per `cls_0x5a5320__vftable_5a5320.cpp:204` ordering —
the slot immediately before MouseClick) — UNCONFIRMED-J.

**Event param decode** (mirror Inventory pattern,
`InventoryPane_SPEC.md:485-491`):
- `param_1 == 4` → button **down** (or `MB_LEFTDOWN = 4` in snapshot
  enum, `src/screen.h` family).
- `param_1 == 5` → button **up**.

**Hit rects** (pane-local — per-slot rects now decoded, Wave-6 §4):

| control | rect (x, y, w, h) | action | cite |
|---|---|---|---|
| slot 0 (EQ_HEAD) | (74, 37, 40, 40) | grab if occupied; drop if equipping | §4 Wave-6 |
| slot 1 (EQ_NECK) | (7, 58, 40, 40) | same | §4 |
| slot 2 (EQ_BODY/Chest) | (141, 58, 40, 40) | same | §4 |
| slot 3 (EQ_OFFHAND/Hand2) | (141, 106, 40, 40) | same | §4 |
| slot 4 (EQ_PRIMEHAND/Hand) | (7, 106, 40, 40) | same | §4 |
| slot 5 (EQ_R_ACCESSORY/Ring) | (7, 154, 40, 40) | same | §4 |
| slot 6 (EQ_L_ACCESSORY/Ring2) | (141, 154, 40, 40) | same | §4 |
| slot 7 (EQ_RANGEDWEAPON/Range) | (24, 11, 40, 40) | same | §4 |
| slot 8 (EQ_AMMO) | (124, 11, 40, 40) | same | §4 |
| slot 9 (EQ_LEGS) | (7, 202, 40, 40) | same | §4 |
| slot 10 (EQ_FEET) | (141, 202, 40, 40) | same | §4 |
| anywhere in pane | (0, 0, 188, 306) | bounds gate / drop area | snapshot `src/screen.h:167` `InPane`, `equip.cpp:85` |

**Drop dispatch (slot 25 `up`)** mirrors snapshot `MouseClick`
(`src/equip.cpp:75-131` — see §6.4 pseudocode). The Inventory↔Equip handoff
shares the **modal global `Inventory.heldslot`** (snapshot
`src/inventory.h::heldslot`, retail field on the Inventory pane).
- Drop-from-Inventory-to-Equip: see §6.4. Retail's slot-availability
  helpers (`Player->CanEquip`, `Player->Equip`,
  `Player->AddToInventory(inst, 256+slot)`) all preserved.
- Drop-from-Equip-to-Inventory: handled by the sibling
  `InventoryPane::MouseClick` (`InventoryPane_SPEC.md §10`), which reads
  `EquipPane.GetHeldSlot()` (snapshot `equip.h:60`) and unequips.

**Modal-state globals:**
- Upper-region selector `DAT_0065d1b8` (==0 selects this pane;
  dispatcher `FUN_0047cf40:204,211-214`).
- `Inventory.heldslot` (sibling pane's drag-source). No other modal
  globals identified for this pane.

---

## §11 — Retail bugs NOT to reproduce

1. **Magenta-key fringe around equipped-item icons.** Each item's `.i3d`
   `invitem` is RGB555 with `0x7C1F` as the implicit transparency key
   (UI_METHOD_MAP §16); blitted with `DM_TRANSPARENT|DM_BACKGROUND`
   (`imagery.cpp:869`). Anti-aliased icon edges sampling the magenta-cleared
   destination can produce a faint pink fringe — same family as the
   pink-halo text bug ([[project-retail-pink-halo-bug]]). Intended visual:
   clean icon on the slot well. Render with real alpha if the source carries
   it; otherwise rely on the icon's own border pixels.

2. **(POSSIBLE) Empty-slot placeholder over-saturating the chrome well.**
   The 11 placeholders (Head/Chest/…) are 40×40 BM_15BIT opaque (`flags=0x2`
   measured). If retail blits them with `DM_TRANSPARENT` (chroma-keyed), the
   placeholder's background outside its pictogram drops out, leaving the
   chrome well visible underneath. If a port instead blits them opaque it
   stamps a 40×40 rectangle, painting over the chrome's well frame. The
   correct behavior is `DM_TRANSPARENT (0x100)` — magenta key. UNCONFIRMED
   on whether retail actually achieves this cleanly (the placeholder bitmaps
   might have a non-magenta background — visual-verify against
   `/tmp/dat_dump/06_Head.png` after wiring).

3. **None observed beyond the chroma-key family.** No drop shadows, no text
   on the pane.

---

## §12 — Reconstruction pseudocode

```cpp
// Composes the equipment pane into the destination render target.
// Called when (re)building the upper sidebar; oi = Player (DAT_00667fcc).

void EquipPane::paint(Target& rt) {                       // vtable slot 21 (~ 0x5366d0)
  if (!dirty) return;                                     // slot-20 gate (mbr_0x50)

  // 1. (Optional, sibling Inv parity) fill pane bg; UNCONFIRMED-C
  // rt.box(0, 0, 188, 306, COLOR_BG);

  // 2. Chrome
  rt.drawBitmap(asset.Equip, 0, 0);                       // 0x80000000 opaque
                                                          // (mbr_0x64; Init :8961)

  // 3. Per-slot icons (placeholder if empty; item icon if equipped)
  //    slotRect[] is the 11-entry table at retail DAT_005e3f60 (Wave-6 §4):
  //      EQ_HEAD=(74,37), EQ_NECK=(7,58), EQ_BODY=(141,58), EQ_OFFHAND=(141,106),
  //      EQ_PRIMEHAND=(7,106), EQ_R_ACCESSORY=(7,154), EQ_L_ACCESSORY=(141,154),
  //      EQ_RANGEDWEAPON=(24,11), EQ_AMMO=(124,11), EQ_LEGS=(7,202), EQ_FEET=(141,202).
  if (Player) {
    for (int i = 0; i < NUM_EQ_SLOTS; ++i) {              // NUM_EQ_SLOTS = 11
      if (i == grabslot) continue;                        // skip currently-dragged
      Point loc = slotRect[i];
      TObjectInstance* item = Player->GetEquip(i);
      if (item) {
        item->DrawInvItem(loc.x, loc.y);                  // DM_TRANSPARENT|DM_BACKGROUND
                                                          // = oi->InventoryImage() + Display.Put
      } else {
        // empty-slot placeholder (retail-specific, snapshot didn't do this)
        // placeholder fetch: per §4 chain, mbr_0x68 + i*4 = bitmap for EQ slot i
        TBitmap* ph = *(TBitmap**)((char*)this + 0x68 + i * 4);
        rt.drawBitmapKeyed(ph, loc.x, loc.y);            // DM_TRANSPARENT
      }
    }
    if (grabslot < 0)
      heldslot = -1;                                      // snapshot src/equip.cpp:149-150
  }

  dirty = false;
  rt.present();                                           // compose to HUD swapchain
}

// Hit-test (same shape as snapshot OnSlot)
int EquipPane::onSlot(int x, int y) {                     // §6.1 (UNCONFIRMED-A coords)
  for (int i = 0; i < NUM_EQ_SLOTS; ++i) {
    Point loc = slotRect[i];
    if (x >= loc.x && x < loc.x + 40 &&
        y >= loc.y && y < loc.y + 40)
      return i;
  }
  return -1;
}

void EquipPane::onMouseDown(int btn, int x, int y) {      // vtable slot 25 (0x5363e0)
  int slot = onSlot(x, y);
  if (slot >= 0) {
    TObjectInstance* item = Player->GetEquip(slot);
    if (item && item->InventoryImage()) {
      heldslot = grabslot = slot;
      SetDragBitmap(item->InventoryImage(),
                    x - slotRect[slot].x, y - slotRect[slot].y);
      SetDragObj(item);
      setDirty();
    }
  }
}

void EquipPane::onMouseUp(int btn, int x, int y) {
  if (grabslot >= 0) {
    grabslot = -1;
    SetDragBitmap(nullptr);
    SetDragObj(nullptr);
    setDirty();
  } else if (inPane(x, y)) {
    // Drop FROM Inventory INTO this pane (full snapshot algorithm, §6.4).
    TObjectInstance* inst =
        Inventory.GetContainer()->GetInventorySlot(Inventory.GetHeldSlot());
    if (inst) {
      int slot = inst->GetStat("eqslot");
      if (uint32_t(slot) < NUM_EQ_SLOTS && Player->CanEquip(inst, slot)) {
        if (Player->GetEquip(slot)) {
          // ring-accessory left/right swap (src/equip.cpp:97-100)
          if (slot == EQ_R_ACCESSORY && !Player->GetEquip(EQ_L_ACCESSORY))
            slot = EQ_L_ACCESSORY;
          else if (slot == EQ_L_ACCESSORY && !Player->GetEquip(EQ_R_ACCESSORY))
            slot = EQ_R_ACCESSORY;
        }
        if (inst->GetOwner() == Player) {
          if (Player->GetEquip(slot))
            Player->GetEquip(slot)->SetInventNum(inst->InventNum());
          inst->SetInventNum(256 + slot);
        } else {
          inst->RemoveFromInventory();
          if (Player->GetEquip(slot)) {
            Player->GetEquip(slot)->RemoveFromInventory();
            Inventory.GetContainer()->AddToInventory(
                Player->GetEquip(slot), Inventory.GetHeldSlot());
          }
          Player->AddToInventory(inst, 256 + slot);
        }
        Player->Equip(inst, slot);
      }
    }
    Inventory.setDirty();
    setDirty();
  }
}
```

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `(*+0x64)` Box fill (UNCONFIRMED) | `Renderer->FillRect` / `DrawSurface` clear | renderer |
| `FUN_004bd680(...,0x80000000)` chrome | `DrawBitmapToTarget` | renderer |
| `FUN_004bd680(...,0x100)` placeholder | `DrawBitmapSubrectToTarget` (magenta key) | renderer |
| `oi->DrawInvItem(x,y)` | inline: `Renderer->DrawBitmapToTarget(rt, oi->InventoryImage(), x, y)` (with chroma key) | renderer + object.h |
| `oi->InventoryImage()` | already implemented (`src/object.h:1066`); `imagery->GetInvImage(state)` | object.h / imagery.h |
| `meth_0x46d710(multi, name)` Initialize bitmap fetch | `TMulti::Bitmap(name)` (name-keyed stricmp lookup in 12-entry table) — 11 placeholders + 1 chrome | resource cache |
| `SetDragBitmap / SetDragObj` | already implemented (`src/cursor.h:24-29`) | cursor |
| `FUN_004aacb0(...,6)` dirty-rect (final composite) | drop — GPU composites RT | — |
| compose to slot-21 dst | `DrawSurface` of the composed pane RT | renderer |

No missing primitives identified; all map to the existing `…ToTarget` family
(UI_METHOD_MAP §12).

---

## §14 — UNCONFIRMED / open questions

**Wave-5 status:** UNCONFIRMED-I is RESOLVED (mbr_0x94/0x98 = grab/heldslot).
The remaining items are unchanged from the Wave-4A spec and all resolve
with one batch of `DecompileAddr.java` runs on slots 21/22/25 (chiefly
`0x5366d0` and `0x5363e0`). The pane SHOULD NOT be wired in the engine
until at least UNCONFIRMED-A resolves.


- **UNCONFIRMED-A (residual) — leaf-method iteration check.** Wave-6
  decoded the per-slot table at `DAT_005e3f60` as **11 × (x, y) pairs
  in EQ_* enum order**. All 11 entries match all 11 visually-detected
  chrome wells 1-to-1 with a consistent ~3-9 px frame-inset Δ (§4
  Wave-6 table). The remaining UNCONFIRMED item is the one-line check:
  - **Does the leaf-method paint loop iterate `0x5e3f60` in straight
    EQ-enum order?** Strongly supported by (a) the name-pointer table
    at `0x5e3fb8` being in enum order with Initialize iterating it
    sequentially, (b) the placeholder mbr offsets being `mbr_0x68 + i*4`,
    and (c) the visual all-11-match. But the loop body in
    `0x5366d0` / `0x536970` / `0x5363e0` has not been disassembled
    in this pass.
  *Impact:* very low — the visual match across all 11 slots makes
  permuted-order extremely unlikely. *Resolve:* `DecompileAddr.java
  0x5366d0` and read the slot loop's index/base computation. If it
  does `for (i=0; i<11; i++) blit(slots[i].x, slots[i].y, mbr_0x68 +
  i*4)`, both straight enum-order AND the table-base-at-0x5e3f60 are
  confirmed in one line. `0x536970` (Update) is a simpler body to
  decode if `0x5366d0` is large.

  *Resolved by this pass:*
  - The data region IS exactly 88 bytes (`0x5e3fb8 - 0x5e3f60 = 0x58`)
    ending precisely at the name-pointer table — fully accounted for as
    11 × (int x, int y) = 22 ints, no leftover bytes.
  - All 11 (x, y) pane-local anchors are decoded:
    * EQ_HEAD (0)         → (74, 37)
    * EQ_NECK (1)         → (7, 58)
    * EQ_BODY (2)         → (141, 58)
    * EQ_OFFHAND (3)      → (141, 106)
    * EQ_PRIMEHAND (4)    → (7, 106)
    * EQ_R_ACCESSORY (5)  → (7, 154)
    * EQ_L_ACCESSORY (6)  → (141, 154)
    * EQ_RANGEDWEAPON (7) → (24, 11)
    * EQ_AMMO (8)         → (124, 11)
    * EQ_LEGS (9)         → (7, 202)
    * EQ_FEET (10)        → (141, 202)
  - Engine code can now be wired against this table. After wiring,
    visual-verify by loading the pane with the player having ANY one
    equipped item and checking the icon lands centred in the well
    that matches the snapshot of `00_Equip.png` (i.e. blit the icon at
    the table position; verify by overlaying the resulting pane on
    the chrome).

- **UNCONFIRMED-B — pane rect literal in the inlined ctor at `0x4807a0`.**
  Inferred (x=452, y=0, w=188, h=306) from the **sibling Stats ctor**
  (which sits in the same upper-region slot and shares the rect by
  symmetry, `cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp:6-21`) and from
  the chrome dimension equality (188×306 dump matches the asserted (w,h)
  exactly). The actual MOV constants emitted at 0x4807a0 are not extracted.
  *Impact:* if y or h differs (e.g. y=1 with a baseline-corrected pane
  height), the pane would render shifted vs. the design.
  *Resolve:* `DecompileAddr.java 0x4807a0` and read the
  `MOV [ebx+offset], imm` sequence (looking for `0x1c4`, `0x132`, `0xbc`,
  `0xae`/`0x132` patterns).

- **UNCONFIRMED-C — pane-bg fill (`Box`) presence.** Sibling Inventory's
  paint starts with `(*pane_buffer+0x64)(0,0,0xbc,0xae,…)` filling with
  color 0xffff before the chrome blit (`InventoryPane_SPEC.md:227`,
  `DrawGrid:537ac6`). Whether retail's EquipPane paint does the same is
  not confirmed (slot 21 body unextracted). *Impact:* small — chrome is
  opaque so the fill is invisible underneath anyway, but the pane-buffer
  initialization may matter for the very-first paint.
  *Resolve:* `DecompileAddr.java 0x5366d0` (first call after entry).

- **UNCONFIRMED-D — `Player->GetEquip(i)` vtable offset.** Snapshot has
  `Player->equipment[i]` (`src/player.h:255`) accessed via inline accessor
  `GetEquip(i)` (`player.h` family). Retail's call shape `(*Player+<eqgetter>)(i)`
  has not been pinned. The Player vtable is `0x5b4f30`
  (`CharacterPortrait_SPEC.md:34`); the equipment getter is likely in the
  range `+0x180..+0x200`. *Impact:* wrong getter → wrong item per slot.
  *Resolve:* `DumpVtable.java 0x5b4f30` and look for a slot whose body
  reads a `equipment[i]` array of 11 dwords (snapshot offset on the Player
  struct is `equipment` per `player.h:255`).

- **UNCONFIRMED-E — inter-Equip-slot drag swaps.** Snapshot's MouseClick
  releases on LEFTUP without resolving an in-pane drop slot
  (`src/equip.cpp:75-84`), meaning a snapshot user cannot drag chest →
  head; the drop just releases. Retail may have extended this to handle
  intra-pane swaps (a natural addition). *Impact:* missing UX feature, not
  a visual bug. *Resolve:* read the LEFTUP branch of `0x5363e0` after the
  `grabslot >= 0` test — if it then calls `onSlot(x,y)` to resolve a target,
  the swap is implemented.

- **UNCONFIRMED-F — shadow setup absence.** I assert no drop shadow on
  any slot icon or chrome (§7), based on the absence of `FUN_00438d80`
  XREFs in the extracted slot-0/19/20 bodies and the absence of shadowed
  blits in the snapshot's `DrawBackground`. The slot 21/22/25 bodies are
  not extracted; they might call shadowed blits. *Impact:* missing shadows
  on equipped icons. *Resolve:* check `FUN_00438d80` XREFs after extracting
  `0x5366d0`.

- **UNCONFIRMED-G — `DAT_005e4060` content.** Animate (slot 19) memcpy's
  a string from `0x5e4060` into `mbr_0x9c`
  (`cls_0x5a5320_TCharacter.cpp:9046-9059`). The data block at 0x5e4060
  is not in the `_data.txt` dump range I read; it's adjacent to the
  equip-pane name table. Plausibly the idle animation name (e.g. "idle1"
  or similar). *Impact:* informational only — affects what animation
  drives the doll figure's idle state. *Resolve:* extract bytes at
  `0x5e4060..0x5e408f` from the binary.

- **UNCONFIRMED-H — hover-tooltip path.** Snapshot's
  `TEquipPane::Animate(true)` (`src/equip.cpp:171-178`) runs `OnSlot` on
  cursor coords and calls `CursorOverObject(inst)` for the tooltip. The
  extracted retail slot 19 (`0x5368c0`) does NOT do this — it only steps
  the item animation frame. The hover/tooltip likely lives in slot 22
  (`0x536970`) or a TPane-base mouse-move slot. *Impact:* missing
  item-tooltip on hover. *Resolve:* `DecompileAddr.java 0x536970`.

- **~~UNCONFIRMED-I~~ → RESOLVED-I — `grabslot`/`heldslot` retail field
  offsets are `mbr_0x94` and `mbr_0x98`.** Confirmed via Initialize's
  explicit initial stores `this->mbr_0x94 = 0xffffffff; this->mbr_0x98 =
  0xffffffff;` at `cls_0x5a5320_TCharacter.cpp:8979-8980`. Matches snapshot
  parity exactly — snapshot `TEquipPane::Initialize()` sets
  `heldslot = grabslot = -1` (`src/equip.cpp:47`). The order is uncertain
  (which of mbr_0x94/mbr_0x98 is grab vs held); snapshot field-decl order
  in `equip.h:75-76` is `grabslot` first then `heldslot`, mirroring the
  Initialize sequence. *Impact:* none — both fields are mid-pane drag
  state, swapping them affects only the field name in the port not
  behavior. *Optional resolve:* read the LEFTDOWN store in `0x5363e0`
  (it does `this->grabslot = slot` AFTER `this->heldslot = slot`; the
  offset order there confirms which is which).

- **UNCONFIRMED-J — mouse-move handler slot.** The drag-promote / hover
  resolution is plausibly slot 24 (`virt_meth_0x445060`,
  `cls_0x5a5320__vftable_5a5320.cpp:206`) — TPane base, **inherited not
  overridden** — but the actual move handling might be in the retail slot
  the parent TPane provides without override. Sibling Inventory has its
  own MouseMove (slot 104 `0x538c10`). EquipPane apparently lets TPane
  base handle it. *Impact:* drag promote may need to live in the parent.
  *Resolve:* check whether `0x5363e0` (MouseClick) handles param_1 ∈
  {3=move?, 6=move-while-down?} branches as the InventoryPane's slot 100
  does (`InventoryPane_SPEC.md:485-491`).

- **UNCONFIRMED-K — `TMulti* equipdata` field (snapshot `mbr_0x0`).**
  Snapshot loads `equipdata = TMulti::LoadMulti("equipscr.dat")`
  (`equip.cpp:41`) and reads top/bottom from it. Retail's Initialize uses
  the global iterator handle `DAT_0065dde4` instead of a per-pane TMulti
  (`cls_0x5a5320_TCharacter.cpp:8960`). So the snapshot's
  `equipdata` field is likely **dropped** in retail (no per-pane TMulti).
  *Impact:* layout/init code might allocate a no-longer-used field.
  *Resolve:* compare retail `0x5363e0`/`0x5366d0` for any `*0x0`
  dereference — absence confirms drop.

- **UNCONFIRMED-L — slot 7 (`0x5370b0`) role.** Per `EquipSidebarPane.md:18`
  the leaf override at slot 7 is `0x5370b0`. Slot 7 in the merged TPlayScreen
  vtable is `virt_meth_0x4920b0_8` family (a TPane base `Show`/`Close`
  variant). Body unextracted. *Impact:* unknown role — possibly a Close
  hook or a `Pulse` override. *Resolve:* `DecompileAddr.java 0x5370b0`.

---

## End notes

This spec separates **HIGH-confidence facts** (pane identity, asset
roster, draw order shape, icon-fetch chain, slot/name correspondence,
algorithm shape, **per-slot (x, y) anchors for all 11 EQ_* slots**)
from **UNCONFIRMED items** (pane-rect literal, several minor
behaviors). Wave-6 (this pass) graduated the per-slot (x, y) table
from UNCONFIRMED-A to HIGH-confidence by decoding the 88-byte data
region at `0x5e3f60` as 11 × (int x, int y) pairs and visually
cross-checking all 11 entries against detected wells in the chrome
bitmap `00_Equip.png`. The remaining UNCONFIRMED items require a
small batch of `DecompileAddr.java` extractions on 4-5 leaf methods
(chiefly `0x5366d0` and `0x5363e0`) to pin the drag dispatch and
confirm the slot-loop iteration order.

The reconstruction agent **MAY now proceed** to a pixel-faithful port:
all 11 (x, y) slot anchors are decoded (§4 Wave-6 table), the chrome
bitmap is identified, the placeholder bitmaps are identified, and the
mbr-offset-to-slot mapping is confirmed. After wiring, visual-verify
against `/tmp/dat_dump/00_Equip.png` as a regression check and confirm
each placeholder bitmap lands centred inside its corresponding well.
