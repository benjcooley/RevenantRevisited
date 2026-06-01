# TDialogPane — Reconstruction SPEC

> Forensic reconstruction contract for the NPC-conversation panel (`TDialogPane`).
> Produced per [FORENSICS_PROTOCOL.md](FORENSICS_PROTOCOL.md). Terms per
> [NOMENCLATURE.md](NOMENCLATURE.md). Primitive behavior cited from
> [UI_METHOD_MAP.md](UI_METHOD_MAP.md).
>
> All paths are relative to repo root
> `/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`.
> Every number states its coordinate space and cites `file:line` + `hex (= dec)`.

---

## §0 — Sources & status

**Status:** `forensics-partial` (extended completion pass — wave 3 narrows the
**choice-text layout** (§6.text), the **per-segment word-wrap** structure
(§6.wrap), the **6-choice overflow** rules (§6.overflow), and the **ESC/Finish
close path** (§10) by reading the per-choice draw body in conjunction with
the second OOAnalyzer view of the same class at `recon/ghidra/cls_0x535a10.cpp`
(slot bodies for `meth_0x535a10` / `meth_0x535d80` / `meth_0x536010` /
`meth_0x5360f0` — the **Hide / ResetResponses / per-character force-done /
ESC-Close** chain). The paint method body at `0x5351d0` is still UNCONFIRMED;
the `choicefont` body at `0x004a4a73` and the AddChoice path remain
UNCONFIRMED — but the **bake destination** (per-choice `mbr_0x4c` text bitmap)
and the **per-segment wrap container** (`mbr_0x80[0..7]` rect array,
`mbr_0x5c` = segment count, `mbr_0x100[]` active flags, `mbr_0x140[]`
per-segment alpha) are now structurally pinned from cls_0x534d40 draw body
(§6.wrap). Items A, B, E remain blockers for full reconstruction; F is now
**partially resolved** for the structural layout (font px / colors still
UNCONFIRMED).

**Class identification — definitive (wave-2 forensics).**

- The retail TDialogPane is **`cls_0x5a5c60`** (vftable @ `0x5a5c60`, instance
  size = 496 bytes, `recon/ghidra/cls_0x5a5c60.cpp:6-31`).
  - Its Init `virt_meth_0x534fd0` (`recon/ghidra/cls_0x5a5c60.cpp:138-179`) is
    the function TPlayScreen_Initialize invokes immediately before printing
    the "Trouble initializing dialog pane" error
    (`recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:205-207`
    → `iVar3 = FUN_00534fd0(); if (iVar3 == 0) FUN_00481c10(s_..dialog_pane..)`).
    UNCONFIRMED-B — needs a Ghidra cross-check (`DecompileAddr 0x00534fd0` to
    confirm it walks the children + loads `Dialog.dat` via `DAT_00666444`).
  - **Inheritance:** `cls_0x5a5c60` → `cls_0x5a45c8` (TButtonPane base, Init
    `0x00434e40`, `recon/discovered/cls_0x5a45c8_TButtonPane_vtable.txt:1-2`)
    → `cls_0x5a4494` (TPane base, Init `0x00491900`,
    `recon/discovered/cls_0x5a4494_TPane_Initialize_491900.cpp`).
  - **TButtonPane sibling:** TDeathPane is `cls_0x5b93c4` (vftable @ `0x5b93c4`,
    Init `0x5339b0`, `recon/ghidra/cls_0x5b93c4__vftable_5b93c4.cpp`). TDeathPane
    is the cleanest sibling that already has `DrawBackground` labeled
    (slot 80, `virt_meth_0x533c10_TDeathPane_DrawBackground`,
    `cls_0x5b93c4__vftable_5b93c4.cpp:27`). **The matching slot 80 on
    cls_0x5a5c60 is `virt_meth_0x535500`** — therefore
    `0x535500` is TDialogPane::DrawBackground.
  - **Animate(draw):** slot 92 in both vtables is the Animate. On TDialogPane
    that's `virt_meth_0x5355b0` (`cls_0x5a5c60__vftable_5a5c60.cpp:30`), which
    iterates the choice-child array and calls `cls_0x534d40::meth_0x534d40` —
    the per-choice fade/draw (`recon/ghidra/cls_0x5a5c60.cpp:333-352`).
- **`cls_0x534d40` is the choice-button child class.** Confirmed by its
  appearance in the child-iteration patterns of `cls_0x5a5c60::meth_0x535500`
  (DrawBackground, `cls_0x5a5c60.cpp:285-306`, calls `FUN_00534470` per child)
  and `meth_0x5355b0` (Animate, `:333-352`, calls `cls_0x534d40::meth_0x534d40`
  per child). The choice has fields at `+0x18..+0x2c` (a rect-like header),
  `+0x48/+0x4c` (two bitmap pointers — left-half/right-half blit), `+0x58`
  (hover-fade counter 0..0xc), `+0x80..+0x140` (up to N sub-glyph rects, see
  §6 fade body). UNCONFIRMED-I — verify via `DumpVtable 0x534d40` (the class
  has no vftable in the dump, suggesting it is a POD widget or a plain
  internal struct used by the parent vtable).
- The earlier mapping `recon/mappings/TDialogPane_cls_0x570900_map.yaml`
  (MEDIUM, 80/100) is **wrong**: `cls_0x570900` is a 56-byte network packet
  helper / TPlayerSlot list node consumed by `cls_0x57d9d0` (a TNetwork
  family class, `recon/classes_original/cls_0x57d9d0_likely_TDirectX_or_TNetwork.cpp`).
  UNCONFIRMED-A — needs the mapping yaml updated.

**Snapshot vs retail summary.** The 1998/99 snapshot at `src/dialog.h:69-116`
/ `src/dialog.cpp:122-407` is a single-pane bottom-anchored chrome
`TPane(0, INVENTORYPANEY - 6, 404, 97, true)` (`src/dialog.h:73`;
`INVENTORYPANEY = 386` in `src/revdefs.h:168`) with MAXCHOICES = 4 and
WriteText-drawn choices. The retail design is fundamentally evolved:
- 640×140 full-screen-wide chrome (`Dialog.dat` `Dialog` measured,
  `tools/ui/dump_dat.py` → `dialog.json`).
- MAXCHOICES = **6** (KeyPress accepts `0x31..0x36` = `'1'..'6'`,
  `recon/ghidra/cls_0x5a5c60.cpp:401-407`). UNCONFIRMED-C — verify array
  bound (`mbr_0x1d8` cap).
- Choices are **child widgets** (`cls_0x534d40`, 324 bytes each) with their
  own hover/fade animation, NOT a flat WriteText.
- TOP **or** BOTTOM anchor (UNCONFIRMED-G — the predicate is not extracted;
  snapshot only has BOTTOM, the task brief notes both cases occur in retail).
- Pulse mode-1 reserves `iStack_4 = 0x8c = 140` px for the dialog strip
  (`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:56-61`),
  matching the chrome height exactly.

The snapshot remains supplementary intent for the `DialogLine([me]/[chr])`
substitution, the `freshresponse`/`grabslot`/`choice` field semantics, the
`Skip()` flow, and the Show/Hide displacement of the lower HUD panes. The
retail layout is definitive; snapshot coordinates do not enter §3–§8.

**Recon files read (this wave + prior):**

- `recon/ghidra/cls_0x535a10.cpp` — **the SECOND OOAnalyzer view of TDialogPane.**
  Same 496-byte instance, same field layout (mbr_0x40, mbr_0x190, mbr_0x1d8,
  mbr_0x1dc, mbr_0x1e4, mbr_0x17c, mbr_0x18c, mbr_0x194), four
  decompiled bodies — `meth_0x535a10` (the inner half of ResetResponses,
  `:27-71`), `meth_0x535d80` (the **Hide / Close common path** with skip-all-
  children branch, `:77-136`), `meth_0x536010` (the **per-character force-done**
  iterator, `:142-180`), `meth_0x5360f0` (the **ESC handler — Skip + Close +
  ScriptJump("Finish") + ForceCommandDone**, `:186-278`). These are the
  same engine code as cls_0x5a5c60 (OOAnalyzer split it into two due to
  vtable-pointer alias). The `meth_0x5360f0` body proves snapshot
  `KeyPress(VK_ESCAPE)` → `Skip + ScriptJump("Finish") + Close()`
  (`src/dialog.cpp:293-300`) survives in retail: `FUN_00471290(this->mbr_0x194,
  "Finish")` then `FUN_004dab80_TActionBlock_Is(..., "say")` test then
  `(*+0x34)(this)` to close. **UNCONFIRMED-R — verify `0x471290` is
  `Character::ScriptJump`, `0x4dab80` is `TActionBlock::Is`.**
- `recon/ghidra/cls_0x5a5c60.cpp` (full body — Init, Close, Hide, KeyPress,
  MouseClick, children iterator) — the retail TDialogPane class.
- `recon/ghidra/cls_0x5a5c60__vftable_5a5c60.cpp` (46-slot vftable, all
  slot addresses).
- `recon/ghidra/cls_0x534d40.cpp` (full per-choice draw body — the only
  method; structure: 324 bytes with rect header at +0x18..+0x2c, two bitmap
  ptrs at +0x48/+0x4c, fade counter at +0x58, up to N sub-glyph rects at
  +0x80..+0x140).
- `recon/ghidra/cls_0x5b93c4__vftable_5b93c4.cpp` (TDeathPane sibling
  vftable — used to identify slot 80 = DrawBackground, slot 92 = Animate).
- `recon/discovered/cls_0x5a4494_TPane_Initialize_491900.cpp` (base TPane Init).
- `recon/discovered/cls_0x5a4494_TPane_Close_491970.cpp` (base TPane Close).
- `recon/discovered/cls_0x5a4494_TPane_vtable.txt` (TPane base vtable, 32 slots).
- `recon/discovered/cls_0x5a45c8_TButtonPane_vtable.txt` (TButtonPane base
  vtable, 32 slots — confirms the inheritance chain).
- `recon/discovered/cls_0x5a45c8_intermediate_init_434e40.cpp` (TButtonPane
  Init — zeros button array, sets `+0x19 = PTR_DAT_005d79e0` display).
- `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` —
  TPlayScreen_Initialize. `:130-134` loads `Dialog.dat` into `DAT_00666444`
  via `FUN_0047f670(name, -1, 0)`; `:205-208` calls dialog-pane Init at
  `0x00534fd0`.
- `recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:56-61` — the
  per-mode-3/mode-1 reserved-height switch (mode-1 = dialog → `0x8c = 140`).
- `recon/discovered/FUN_00435150_DefScreen_Open.cpp` — slot-132 reference
  (inherited TDefWidget loader).
- `recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp` — sibling
  init shape (helps confirm the TButtonPane base chain).
- `recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:769-851` — the
  intermediate base `virt_meth_0x435de0` body, the chain target of
  TDialogPane slot 80 (`recon/ghidra/cls_0x5a5c60.cpp:300, 304`) and slot 92
  (`:350`). This is the parent's compositor that draws all child widgets
  via the panel buffer `+0x84` into the display `+0x64`.
- `recon/ghidra/_data.txt` — string XREFs (`Dialog.dat` at `:86340-86341`;
  `choicefont` at `:92534-92552`); `DAT_006668d{c,0,4,8}` reads from
  `virt_meth_0x5351d0` (the slot-76 top-level draw) at `:132225-132248`.
- `recon/mappings/TDialogPane_cls_0x570900_map.yaml` — the MEDIUM-confidence
  misidentification mapping.
- **Asset measurements:** `tools/ui/dump_dat.py` on
  `data/resources_unzipped/dialog.dat` → `Dialog` (640×140, BM_15BIT,
  keycolor=0) and `DialogEndCap` (10×140, BM_15BIT, keycolor=0). Visual
  inspection of `/tmp/dat_dump/01_Dialog.png` shows a dark-purple stone-and-
  marble chrome with a decorative serrated bottom edge — consistent with a
  panel intended to sit at the BOTTOM of the screen (decorative edge
  faces the game viewport above). `00_DialogEndCap.png` is a thin vertical
  trim column.

**Prior briefs / notes consulted:** none specific to TDialogPane. Sibling
specs read for shape: `docs/ui/forensics/TPlyrStatusBar_SPEC.md`,
`docs/ui/forensics/TTextBar_SPEC.md`, `docs/ui/forensics/InventoryPane_SPEC.md`.

**Existing port shell:** `src/dialog.h:69-116`, `src/dialog.cpp:122-407`
(snapshot, see snapshot note). **No retail port shell or test mode exists**
(no `src/uidialogpanetest.cpp`).

### Snapshot note (supplementary — superseded by retail)

The snapshot pane was `TPane(0, INVENTORYPANEY - 6, 404, 97, true)`
(`src/dialog.h:73`; `INVENTORYPANEY = 386` in `src/revdefs.h:168`) → rect
`(0, 380, 404, 97)`. Retail's chrome is `640×140`, so the snapshot rect is
superseded; only the bottom-anchor intent and the field semantics
(`choice`, `freshresponse`, `grabslot`, `numchoices`, `character`) carry
forward. Per-choice height in snapshot was `CHOICEHEIGHT = 21`
(`src/dialog.cpp:246`); retail per-choice geometry is held in the child's
own rect fields (+0x18..+0x2c on `cls_0x534d40`) and is UNCONFIRMED until
the AddChoice site (the call that fills the child's geometry) is extracted.

### Errors to close

No prior retail port exists. The reconstruction agent is building fresh —
there is no prior wrong port to migrate. The snapshot `src/dialog.cpp`
remains in-tree only as supplementary intent and will be retired by the
reconstruction.

---

## §1 — Overview

`TDialogPane` is the **NPC conversation panel**. When the player interacts
with an NPC ("Say" / "Talk"), the panel pops up showing the NPC's spoken
line at top, then up to MAXCHOICES (= 6 in retail) selectable response
options the player can pick with the mouse or number keys (`1`..`6`).

The panel:

- Is a **TButtonPane** derivative — its choice list is a real child-widget
  array, not a flat text draw. Each choice is a `cls_0x534d40` instance
  with its own hover/fade-in animation (§6, §9).
- Disables player movement while open (snapshot: `PlayScreen.SetControlOn(false)`,
  `src/dialog.cpp:221`). Retail equivalent invoked through the parent
  `cls_0x5b98b8::virt_meth_0x435de0` chain (`recon/ghidra/cls_0x5a5c60.cpp:300`).
- Hides the lower HUD panes (inventory/spellbook) while showing —
  TPlayScreen Pulse mode-1 reserves the bottom 140 px
  (`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:56-61` —
  `if (mode == 1) iStack_4 = 0x8c`).
- Has TOP **or** BOTTOM anchor — chosen by context. The predicate is not yet
  pinned (UNCONFIRMED-G); see §3 for the two anchor cases and how the choice
  is plausibly encoded.
- Tracks **state in the parent `cls_0x5a5c60` instance**:
  - `mbr_0x1d8` = numchoices (cap = 6; KeyPress range check at
    `recon/ghidra/cls_0x5a5c60.cpp:401-407`).
  - `mbr_0x1dc` = currently-selected choice (init = `0xffffffff` = `-1`,
    `recon/ghidra/cls_0x5a5c60.cpp:153, 211, 277`; assigned in KeyPress at
    `:404` as `param_1 - 0x31U`).
  - `mbr_0x1e4` = freshresponse flag (init = `0`, set to `1` on selection in
    KeyPress `:405`).
  - `field_0x190` = "already-selected / locked" gate read in KeyPress `:370`
    and MouseClick `:428`. While zero, input is accepted; non-zero means
    a choice was already committed and further input is ignored. This
    matches the snapshot's `freshresponse`/`choice >= 0` gate.
  - `field_0x18c` / `field_0x17c` / `field_0x184` = the TButtonPane child
    array (pointer / count / capacity-mask) inherited from the
    `cls_0x41c7f0::cls_0x41c7f0(this, 4)` ctor at
    `recon/ghidra/cls_0x5a5c60.cpp:68` (the **TPointerArray** ctor with
    capacity = 4). This is the **dialog choice array** (the TButtonPane base
    holds the 16-button array at `+0x88`; the dialog adds its own 4-capacity
    array at `+0x17c` for choices). UNCONFIRMED — the relationship between
    "+0x17c capacity = 4" (ctor) and "MAXCHOICES = 6" (KeyPress) suggests
    the array auto-grows; verify by reading AddChoice.
- Plays no LightningAnimator effect on response (UNCONFIRMED-H is retired —
  the `cls_0x570900` destructor was network code, not TDialogPane).

The plain-language summary: a 640×140 chrome bar appears across the top or
bottom of the screen; the NPC's spoken line and up to 6 numbered choices
render inside it; each choice widget fades in via a 12-tick alpha ramp; the
player clicks one or presses 1..6; the panel sets `mbr_0x1dc =
choice_index`, `mbr_0x1e4 = 1`, and the script polls the response and
advances or closes the dialog.

---

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| Dialog | `Dialog.dat` | `Dialog` | **640×140** | chrome strip (full screen width @ Classic) | full bitmap `(sx,sy,sw,sh) = (0,0,640,140)` | `tools/ui/dump_dat.py` → `dialog.json` `[1]`; load name `recon/ghidra/_data.txt:86340 "Dialog.dat"`; `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp:133` `DAT_00666444 = FUN_0047f670(s_Dialog_dat_005d710c,-1,0)` |
| DialogEndCap | `Dialog.dat` | `DialogEndCap` | **10×140** | side trim cap (likely right edge, UNCONFIRMED-D) | full bitmap `(0,0,10,140)` | `tools/ui/dump_dat.py` → `dialog.json` `[0]` |

Pixel format: both bitmaps are **BM_15BIT** (`flags=0x2`,
`tools/ui/dump_dat.py` output). `keycolor=0` per bitmap, which per
`src/bitmapdecode.cpp:59-66` (UI_METHOD_MAP §16) defaults to the global
magenta `0x7C1F` chroma key. Visual inspection of `/tmp/dat_dump/01_Dialog.png`
shows the chrome is opaque purple stone — magenta is unlikely to appear in
the chrome's solid body, so the keycolor=0 is harmless. Chroma-key risk is
on the per-choice fade overlay (§7), not the chrome.

**No atlas / source-rect map needed** — both assets are full-bitmap blits.

### Asset visual cues (transcription only — not coord-source)

The `Dialog` chrome has a **decorative serrated bottom edge** (visible in
`/tmp/dat_dump/01_Dialog.png`) and an unornamented top edge. This strongly
suggests the panel sits **above its content boundary** (i.e. it is **bottom-
anchored on screen**, with the decorative edge meeting the game viewport
above). For the TOP anchor case (UNCONFIRMED-G), the chrome would likely be
**vertically flipped** at draw time, or there is a separate top-anchor
chrome asset not yet identified — flag for resolve.

**UNCONFIRMED-D — DialogEndCap placement.** The cap is 10×140 — a thin
vertical strip. Three possibilities, all UNCONFIRMED:
- (i) drawn at the LEFT edge of the chrome (the chrome strip is then 650 wide
  with a 10-px lead cap),
- (ii) drawn at the RIGHT edge (a trailing cap),
- (iii) drawn on BOTH sides (mirrored).

The name "EndCap" most naturally reads as a single trailing cap (option ii).
At wider-than-640 display modes the chrome must either tile / stretch /
be flanked by the cap — the cap exists specifically to terminate the chrome
when the screen is wider than 640. **Resolve:** read the slot-76 paint
method `0x005351d0` (UNCONFIRMED-E).

### Choice font

`choicefont` (registered as a font index, exact index UNCONFIRMED-F).
- Literal: `recon/ghidra/_data.txt:92534-92538 "choicefont"`.
- Caller XREFs: `004a4a73` (the font-table registration) and
  `FUN_004a4d40:004a4dbb` (the lookup), confirming `choicefont` is a named
  font slot — same family as `goldfont` / `silverfont`
  (`_data.txt:92529-92556`).
- Snapshot use: `src/dialog.cpp:260` `GameData->Font("choicefont")`.
- HDC table index, point size, weight, and **whether the font id has bit
  `0x400` set (the 3-pass shadow flag, UI_METHOD_MAP §5)** UNCONFIRMED-F.

---

## §3 — Coordinate frames & surfaces

The pane height in mode-1 is **140 px** (Pulse reserves
`iStack_4 = 0x8c = 140`,
`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:57`), matching
the `Dialog` chrome height — so the pane has the same height as the chrome,
with no extra padding above/below.

The pane width is the screen width
(`pane_w = *(int*)(PTR_DAT_005d79e0 + 4)`,
`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:63`). At 640 px
wide the chrome is an exact match; at wider modes the chrome is either
flanked by the `DialogEndCap` (most likely, see §2) or tiled (UNCONFIRMED-D).

### Frame table (mandatory, NOMENCLATURE §1) — both anchor cases

The table below enumerates every frame in play, for both the BOTTOM and
TOP anchor cases. The cases differ only in the `pane` frame's origin in
its parent (`screen`). All sub-frames are computed from `pane` and so are
identical between cases once `pane_origin_y` is known.

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | (root) | — | `(0, 0)` | screen-space coords; `pane_w = *(int*)(PTR_DAT_005d79e0 + 4)`, `pane_h = *(int*)(PTR_DAT_005d79e0 + 8)` | `_TPlayScreen_Pulse_47b4d0.cpp:63,75` |
| **pane** (BOTTOM case) | screen | **BL** of screen, **TL** of pane = `(0, pane_h − 140)` | screen `(0, pane_h − 0x8c)` | the whole 640×140 strip when anchored to bottom (default) | UNCONFIRMED-G (default per snapshot bottom-anchor `src/dialog.h:73` + Pulse mode-1 strip `:56-61`) |
| **pane** (TOP case) | screen | **TL** of screen = `(0, 0)` | screen `(0, 0)` | the whole 640×140 strip when anchored to top | UNCONFIRMED-G — TOP case is asserted by the task brief; the predicate selecting it is not yet extracted (the cls_0x5a5c60 struct has fields at `+0x40` (a generic flag) and `+0x10/+0x20` (display/dimension carriers from the ctor) but no field is yet confirmed as the anchor flag) |
| **chrome** | pane | **TL** of pane = `(0, 0)` | pane-local `(0, 0)` (assumed full-rect blit, opaque copy `DM_USEDEFAULT`) | the `Dialog` 640×140 bitmap | UNCONFIRMED-E (paint method body @ `0x5351d0` not extracted; chrome blit position inferred from common TPane convention — TPane base has dst at `(this[1], this[3]) = (paneX, paneW)` per Init `:7-9` and the ParamBlit slot writes `dx=0, dy=0` into the pane buffer) |
| **end-cap** | pane | **TR** of pane (most likely option ii in §2) = `(pane_w − 10, 0)` in pane-local | pane-local `(pane_w − 10, 0)` | the 10×140 `DialogEndCap` bitmap | UNCONFIRMED-D |
| **choice-region** | pane | **TL** of pane | pane-local origin `(C_x0, C_y0)` — UNCONFIRMED-F. Snapshot used `(32, 4)` (`src/dialog.cpp:260` with `y = 0 + 4`); retail per-choice `cls_0x534d40::mbr_0x18` (x) + `mbr_0x1c` (y) hold the per-choice top-left in pane-local; the choice-region origin is implicit in those values. | the vertical stack of up to 6 choice cells | UNCONFIRMED-F |
| **choice-cell[i]** | pane | TL (each cell has its own absolute coords in pane-local, NOT relative to a "choice-region" — the per-choice `cls_0x534d40` stores absolute pane-local x/y at `+0x18/+0x1c`, see §6) | pane-local `(choice[i].mbr_0x18, choice[i].mbr_0x1c)`; per-cell offset `+0x20/+0x24` apply on top (see §6 for the (mbr_0x18 + mbr_0x20, mbr_0x24 + mbr_0x1c) draw position) | one choice's hit rect + visible artwork. **Width = `mbr_0x28`, height = `mbr_0x2c`** (read in §6 from the cls_0x534d40 fade-draw body `:91-92`). | UNCONFIRMED-F (the per-instance values are runtime-filled in AddChoice — the AddChoice site is not extracted; presumed-default vertical stride from snapshot is 21 px) |
| **choice-cell-left-half[i]** | choice-cell[i] | TL | choice-cell-local `(0, 0)`, size `(0x32, mbr_0x2c) = (50, h)` | the left-half blit (a slot-number sprite or icon, source `mbr_0x48`). See §6. | `recon/ghidra/cls_0x534d40.cpp:66-71, 88` |
| **choice-cell-right-half[i]** | choice-cell[i] | TL | choice-cell-local `(0x32, 0) = (50, 0)`, size `(mbr_0x28 − 0x32, mbr_0x2c) = (w − 50, h)` | the right-half blit (the choice text region, source `mbr_0x4c`). See §6. | `recon/ghidra/cls_0x534d40.cpp:91-94` |
| **choice-line[i,j]** (renamed from "choice-glyph" — wave-3 shows these are **per-WRAPPED-LINE** rects, not per-letter; see §6.wrap) | choice-cell[i] | TL | cell-local rect `(mbr_0x80[j*4+0], mbr_0x80[j*4+1])` to `(mbr_0x80[j*4+2], mbr_0x80[j*4+3])` (`x0, y0, x1, y1` inclusive corners), count = `mbr_0x5c`, **hard cap 8 lines** (rect array spans `+0x80..+0x100` = 8 × 4 dwords) | one wrapped line of the choice text, drawn into the cell at cell-local `(x0, y0)` to `(x1, y1)`. Width = `(x1 − x0) + 1` px, height = `(y1 − y0) + 1` px. Source bitmap = `mbr_0x4c` (the baked text-region bitmap), sx = x0, sy = y0 (1:1 src). Per-line typewriter alpha at `mbr_0x140[j]` (§6.wrap, §9). | `recon/ghidra/cls_0x534d40.cpp:96-130` (struct: `:17-22`; rect derivation: `:103-105`; per-line alpha: `:101`) |
| **clip-region (paint)** | screen | TL | screen `(DAT_006668e4, DAT_006668e0)`, size `(DAT_006668dc, DAT_006668e8)` — see "scratch/clip surfaces" below | runtime per-frame clip rect read by slot-76 paint | `recon/ghidra/_data.txt:132241 (006668e4 read by 5351d0:5351d3); :132235 (006668e0 read by 5351d0:535206); :132247 (006668e8 read by 5351d0:535201); :132225 (006668dc read by other panels — same group)` |

**Composition formula (BOTTOM case, the snapshot-consistent default):**

```
pane_origin = (0, pane_h − 0x8c)    # = (0, pane_h − 140)

chrome_screen_xy   = pane_origin + (0, 0)          = (0, pane_h − 140)
end_cap_screen_xy  = pane_origin + (pane_w − 10, 0) = (pane_w − 10, pane_h − 140)

# per-choice (uses the cls_0x534d40's own pane-local origin):
choice_cell_screen_xy[i]      = pane_origin + (choice[i].mbr_0x18 + choice[i].mbr_0x20,
                                                choice[i].mbr_0x24 + choice[i].mbr_0x1c)
choice_left_half_screen_xy[i] = pane_origin + (choice[i].mbr_0x18 + choice[i].mbr_0x20,
                                                choice[i].mbr_0x24 + choice[i].mbr_0x1c)
                                # width 0x32 (= 50), height mbr_0x2c
choice_right_half_screen_xy[i]= pane_origin + (choice[i].mbr_0x18 + 0x32 + choice[i].mbr_0x20,
                                                choice[i].mbr_0x24 + choice[i].mbr_0x1c)
                                # width (mbr_0x28 - 0x32), height mbr_0x2c
                                # cite: recon/ghidra/cls_0x534d40.cpp:66-94
```

**Composition formula (TOP case):**

Identical to BOTTOM but with `pane_origin = (0, 0)`.

```
pane_origin = (0, 0)
chrome_screen_xy   = (0, 0)
end_cap_screen_xy  = (pane_w − 10, 0)
choice_cell_screen_xy[i] = (choice[i].mbr_0x18 + choice[i].mbr_0x20,
                            choice[i].mbr_0x24 + choice[i].mbr_0x1c)
```

**TOP-vs-BOTTOM selection signal.** UNCONFIRMED-G. The task brief states
"conditional TOP-or-BOTTOM placement"; the recon does not show the
predicate. The two plausible signals are: (i) a field on the character or
on the script-set context that forces top-anchor when set; (ii) a "preserve
other HUD pane visibility" heuristic that anchors away from the visible-
pane edge (e.g. if the inventory pane is open, the dialog anchors top so
they don't overlap). The chrome's decorative serrated bottom edge (§2)
suggests BOTTOM is the *natural* anchor; TOP probably requires a vertical
flip at blit time. Resolve by extracting Show() / the Show predicate via
`DecompileAddr 0x00435f90` (slot 88 — the TButtonPane Show inherited
override) and the call site that flips the anchor flag.

**Scratch/clip surfaces.** The slot-76 paint `virt_meth_0x5351d0` reads
four sequential globals `DAT_006668dc / DAT_006668e0 / DAT_006668e4 /
DAT_006668e8` (`_data.txt:132225-132248`). The same globals are read by the
TButton-family `meth_0x40ca90` and `FUN_00412680` (a pane drawing helper)
and `virt_meth_0x54bf70` — they are the **screen-wide pending-redraw clip
rect**: by the read order in `0x5351d0` (`6668e4 @ :5351d3 → 6668e0 @ :535206
→ 6668e8 @ :535201` — i.e. reads at offsets `+8, +0xc, +0x4, +0x14` from
struct base, decoded as `x, y, w, h` per the convention `meth_0x40ca90`
follows), they likely encode `{dx, dy, w, h}`. UNCONFIRMED — resolve via
the slot-76 body extract.

The parent `cls_0x5b98b8::virt_meth_0x435de0` (`recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:769-851`)
shows the TButtonPane base compositor — it iterates children and draws each
via vftable slot `+0x40` into `this->mbr_0x64` (the screen/display surface)
or `this->mbr_0x84` (the cached panel buffer when `mbr_0x150 != 0`,
i.e. when a full redraw is requested). The TDialogPane therefore inherits
the **two-surface model**:
- `this->mbr_0x84` (= `cls_0x5b98b8 + 0x84`) = **cached panel buffer**
  (off-screen; the chrome + choice cells composite here when fully dirty).
- `this->mbr_0x64` (= `cls_0x5b98b8 + 0x64`) = the **display target**
  surface (the live game framebuffer chunk).

The cached panel buffer is invalidated by `mbr_0x150 = 1` (= "needs full
re-composite", set by Hide-like in `cls_0x5a5c60::meth_0x535120`
`recon/ghidra/cls_0x5a5c60.cpp:245-246` via `this->mbr_0x48 = 1; this->mbr_0x4c = 1`
— UNCONFIRMED whether `+0x48/+0x4c` are inherited from the cached-buffer
dirty flag pair on the base). UNCONFIRMED-M.

### Layout diagram

```
BOTTOM anchor (default per snapshot intent + Pulse mode-1 reservation):

  screen y=0
  ┌─────────────────────────────────────────────────────────────┐
  │                                                             │
  │                    (game viewport unchanged here)           │
  │                                                             │
  ├─────────────────────────────────────────────────────────────┤  ← pane_origin_y = pane_h − 140
  │ NPC line: "..." (text, source = script.SayCommand line)     │
  │                                                             │
  │   1. choice text                  ← per-choice widget       │
  │   2. choice text                    cls_0x534d40 with       │
  │   3. choice text                    hover-fade ramp (§6,§9) │
  │   4. choice text                                            │
  │   ...                                                       │
  │                                                          ╔══╣ ← DialogEndCap @ (pane_w−10, 0) in pane
  └─────────────────────────────────────────────────────────────┘  ← pane_origin_y + 140 = pane_h (= screen bottom)
  ^ x=0                                                       ^ pane_w

TOP anchor (when whatever predicate triggers it — UNCONFIRMED-G):
  Same composition, but pane_origin_y = 0. Chrome may need vertical flip
  given the decorative bottom edge.

Per-choice (cls_0x534d40) widget interior (pane-local), width = mbr_0x28, height = mbr_0x2c:

  (mbr_0x18+mbr_0x20, mbr_0x24+mbr_0x1c)
    ┌───────────────┬──────────────────────────────┐
    │   left-half   │    right-half                │
    │   (50 wide)   │   (mbr_0x28 − 50 wide)       │
    │   src=mbr_0x48│   src=mbr_0x4c               │
    │   (icon/slot) │   (text bitmap)              │
    └───────────────┴──────────────────────────────┘
    + per-glyph sub-rects (mbr_0x80[j*4..]) re-blitted on top
```

---

## §4 — Static element layout

| element | space | TOP-anchor (x,y) | TOP-anchor (w,h) | BOTTOM-anchor (x,y) | BOTTOM-anchor (w,h) | source rect | cite |
|---|---|---|---|---|---|---|---|
| Dialog chrome | screen | `(0, 0)` | `(640, 140)` | `(0, pane_h − 140)` | `(640, 140)` | `(0,0,640,140)` | `Dialog.dat` `Dialog`; chrome blit position UNCONFIRMED-E |
| DialogEndCap | screen | `(pane_w − 10, 0)` (most likely; UNCONFIRMED-D) | `(10, 140)` | `(pane_w − 10, pane_h − 140)` | `(10, 140)` | `(0,0,10,140)` | `Dialog.dat` `DialogEndCap`; placement UNCONFIRMED-D |
| NPC-line cell | screen | UNCONFIRMED-F | UNCONFIRMED-F | UNCONFIRMED-F | UNCONFIRMED-F | (text — no source rect) | UNCONFIRMED-F |
| choice[i] cell (full) | screen | TOP: `(choice[i].mbr_0x18 + choice[i].mbr_0x20, choice[i].mbr_0x24 + choice[i].mbr_0x1c)` | `(choice[i].mbr_0x28, choice[i].mbr_0x2c)` | BOTTOM: TOP + `(0, pane_h − 140)` | same | (per-half source) | `recon/ghidra/cls_0x534d40.cpp:66-94`; per-instance values runtime-filled (AddChoice site not extracted) |
| choice[i] left-half (icon / slot number) | screen | `(choice[i].mbr_0x18 + choice[i].mbr_0x20, choice[i].mbr_0x24 + choice[i].mbr_0x1c)` | `(0x32 = 50, choice[i].mbr_0x2c)` | + `(0, pane_h − 140)` | same | full src `(0,0,50,mbr_0x2c)` from `mbr_0x48` | `recon/ghidra/cls_0x534d40.cpp:66-71, 88` |
| choice[i] right-half (text) | screen | `(choice[i].mbr_0x18 + 0x32 + choice[i].mbr_0x20, choice[i].mbr_0x24 + choice[i].mbr_0x1c)` | `(choice[i].mbr_0x28 − 0x32, choice[i].mbr_0x2c)` | + `(0, pane_h − 140)` | same | src `(0,0,mbr_0x28−50,mbr_0x2c)` from `mbr_0x4c` | `recon/ghidra/cls_0x534d40.cpp:91-94` |

**Mirrored side.** TDialogPane does not mirror across a vertical axis. The
"two cases" are the **TOP / BOTTOM anchor switch** (entire 640×140 strip
moves), not a player/target mirror.

**Selection-highlight visual.** The selection highlight is encoded as a
peak of the per-choice fade ramp (`mbr_0x58 = 0xc`), NOT as a separate
color-swap on the text. The body at `cls_0x534d40.cpp:60-94` gates the
whole draw on `(mbr_0x58 * 0xff) / 0xc > 0x7f` (= alpha > 128) — so a
fully-faded-in choice draws at full alpha and an un-hovered/un-selected
choice (alpha < 128) draws **nothing**. The "highlighted" appearance is
therefore the choice being fully visible vs partially-faded vs invisible.
This is fundamentally different from the snapshot's `SColor{255, 0, 50}`
red-pink color override at `src/dialog.cpp:256-261`: retail uses alpha
ramping over a pre-rendered chip; snapshot used a runtime color override.
UNCONFIRMED-K — verify there is no additional tint pass (e.g. via a
second blit with a color-multiply mode).

---

## §5 — Draw order / composition

The TDialogPane draw splits across **three vtable slots** that the parent
TPane orchestrator (`cls_0x5b98b8::virt_meth_0x435de0`,
`recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:769-851`) calls in
order during the frame:

1. **Slot 76 — `virt_meth_0x5351d0`** (top-level Draw / clip-update). Reads
   the pending-redraw clip rect from `DAT_006668d{c,0,4,8}`
   (`recon/ghidra/_data.txt:132225-132248`). Body UNCONFIRMED-E — extract
   via `DecompileAddr 0x005351d0`.
2. **Slot 80 — `virt_meth_0x535500`** = **DrawBackground** (by sibling
   identification: TDeathPane's slot 80 is `virt_meth_0x533c10_TDeathPane_DrawBackground`,
   `recon/ghidra/cls_0x5b93c4__vftable_5b93c4.cpp:27`). Iterates
   `field_0x17c` choice children and calls `FUN_00534470(child)` for each
   one (the per-choice background draw — composite the chrome and the
   per-choice non-fade background into the cached panel buffer), then
   chains to the parent's compositor `cls_0x5b98b8::virt_meth_0x435de0`
   (`recon/ghidra/cls_0x5a5c60.cpp:285-306`) and finally to the leaf's slot
   44 `virt_meth_0x444fc0_44`.
3. **Slot 92 — `virt_meth_0x5355b0`** = **Animate(draw)** (sibling slot 92 on
   TDeathPane is the matching slot). Iterates `field_0x17c` choice children
   and calls `cls_0x534d40::meth_0x534d40(child)` for each — the per-choice
   hover-fade draw pass (the alpha-gated chip blit, §6). Then chains to the
   parent's `cls_0x5b93c4::virt_meth_0x436090`
   (`recon/ghidra/cls_0x5a5c60.cpp:333-352`).

**Inferred ordered sequence within the frame:**

1. Compute anchor (TOP / BOTTOM, §3) → final pane origin.
   *(Where the anchor is computed: UNCONFIRMED-G — slot-23 `0x435f90`, the
   inherited TButtonPane Show, is the candidate.)*
2. Slot 76 sets the redraw clip rect from `DAT_006668d{c,0,4,8}` and tells
   the renderer that part of the screen is dirty.
3. Slot 80 (DrawBackground):
   - For each choice child, `FUN_00534470` composites the choice's
     background (the chrome cell behind it) into the cached panel buffer
     `this+0x84`.
   - Parent `cls_0x5b98b8::virt_meth_0x435de0` `:799-820` then blits the
     cached panel buffer to the display: when `mbr_0x150 != 0` (full
     redraw) the chrome + all children re-composite; otherwise only the
     children whose `(child[5] & 0x800)` is set re-composite.
   - The Dialog 640×140 chrome blit + the DialogEndCap blit happen
     somewhere in this chain — most likely as part of TDialogPane's own
     slot-44 `virt_meth_0x444fc0_44`
     (`recon/ghidra/cls_0x5a5c60.cpp:301`) which is the panel's "fill
     background" override. UNCONFIRMED-E.
4. Slot 92 (Animate/draw):
   - For each choice child, advance its `mbr_0x58` fade counter (§9), then
     run `cls_0x534d40::meth_0x534d40` to draw the chip if alpha > 0x7f
     (= 128) — see §6.
   - Each chip-draw is a **two-half blit**: left half (icon/slot, 50 px
     wide) then right half (text, `mbr_0x28 − 50` px wide), both via the
     display's slot `+0x5c` (= `ParamBlit` with effects, UI_METHOD_MAP §15a).
5. TPlayScreen's outer Pulse swap brings the composited dialog strip to
   the swapchain.

The chrome blit position itself is UNCONFIRMED until slot 76 or slot 44 is
extracted (UNCONFIRMED-E). The plausible call shape, by analogy with
TPlyrStatusBar's helper_PlayerSide (UI_METHOD_MAP §4 example call):
`FUN_004bd680(0, 0, dialog_chrome_handle, 0x80000000, 0)` into the cached
panel buffer — i.e. opaque-copy the chrome to pane-local `(0, 0)`.

---

## §6 — Algorithms (pseudocode per helper)

### Per-choice draw / hover-fade — `cls_0x534d40::meth_0x534d40`

**Recon source: `recon/ghidra/cls_0x534d40.cpp:30-140`.**

This is the per-choice paint body called per-frame from slot 92
(`recon/ghidra/cls_0x5a5c60.cpp:345`). The function paints **only when alpha
exceeds 0x7f (= 128)** — so half-faded choices and un-hovered choices are
invisible.

```
// recon/ghidra/cls_0x534d40.cpp:30-140
// Per-choice widget: a left-half (50×h) icon/slot bitmap and a right-half
// (w−50)×h text bitmap, drawn into the display via slot +0x5c (ParamBlit
// with effects, UI_METHOD_MAP §15a), optionally with selective per-glyph
// sub-rect re-blits.
//
// Fields used:
//   mbr_0x18  = pane-local x (the choice's anchor in the pane's frame)
//   mbr_0x1c  = pane-local y
//   mbr_0x20  = x-offset (added to mbr_0x18 — likely a "child-relative" offset
//                set by the parent for slide-in animation; UNCONFIRMED-K)
//   mbr_0x24  = y-offset (added to mbr_0x1c)
//   mbr_0x28  = full cell width
//   mbr_0x2c  = full cell height
//   mbr_0x48  = source bitmap handle for LEFT HALF  (50-wide; icon / slot number)
//   mbr_0x4c  = source bitmap handle for RIGHT HALF (mbr_0x28−50 wide; text)
//   mbr_0x58  = fade counter, range 0..0xc (= 12) (§9)
//   mbr_0x5c  = count of per-glyph sub-rects in mbr_0x80[..]
//   mbr_0x80[j*4+0..3] = j-th per-glyph (x0, y0, x1, y1) cell-local rect
//   mbr_0x100, mbr_0x140 = per-glyph alpha / enable arrays (mbr_0x140[j] is
//                          the j-th glyph's fade counter; mbr_0x80[j*4]-1
//                          is the glyph's "active" flag — `pdVar5[-0x10] != 0`)

draw_choice(self):
  if self.mbr_0x20 == 0xffffd8f0:   # sentinel — "choice is hidden / not yet alive"
    return                          # cite: cls_0x534d40.cpp:62

  alpha = (self.mbr_0x58 * 0xff) / 0xc      # 0..255 over 0..12 ramp
  if alpha <= 0x7f:                          # gate at 128 (50% alpha)
    # still invisible — skip both blits AND skip the per-glyph pass,
    # but DO still run the global blit-effect iterate at the bottom
    pass
  else:
    if g_DAT_006680c8 != 0:        # "high-resolution / chip mode" gate; UNCONFIRMED-Q
      # ============ LEFT HALF ============================================
      # SDrawParam buf (local_54..local_30):
      #   drawmode = 0x100  (DM_TRANSPARENT — chroma-keyed copy)
      #   dx, dy   = (mbr_0x18 + mbr_0x20, mbr_0x24 + mbr_0x1c)
      #   dwidth   = 0x32   (= 50)
      #   dheight  = mbr_0x2c
      #   sx, sy   = (0, 0)         (full src rect)
      #   swidth   = 0x32   (= 50)
      #   sheight  = mbr_0x2c
      #   zpos     = 0
      #   normal   = 0
      #   color    = 0
      #   intensity= 0x1f  (= 31, full)
      # Blit:  display.ParamBlit(buf, src=mbr_0x48, 0, 0)
      # cite: cls_0x534d40.cpp:66-88
      buf = SDrawParam{drawmode=0x100, dx=self.mbr_0x18 + self.mbr_0x20,
                      dy=self.mbr_0x24 + self.mbr_0x1c,
                      dwidth=0x32, dheight=self.mbr_0x2c,
                      sx=0, sy=0, swidth=0x32, sheight=self.mbr_0x2c,
                      intensity=0x1f}
      PTR_DAT_005d79e0.ParamBlit(buf, self.mbr_0x48, 0, 0)

      # ============ RIGHT HALF ===========================================
      # Same struct shape but:
      #   dx       = mbr_0x18 + 0x32 + mbr_0x20     (= left + 50)
      #   dy       = mbr_0x24 + mbr_0x1c            (same row)
      #   dwidth   = 0x32                            ← see UNCONFIRMED-K
      #   dheight  = 0                               ← see UNCONFIRMED-K
      #   sx       = mbr_0x28 - 0x32  (= w − 50; reading the RIGHT slice of mbr_0x4c)
      #   sy       = mbr_0x2c
      #   swidth   = (struct dwidth/swidth fields are passed via
      #               FUN_00438d80_BlitEffect_StructInit args — see Gotchas)
      # cite: cls_0x534d40.cpp:90-94
      FUN_00438d80_BlitEffect_StructInit(&buf,
          self.mbr_0x18 + 0x32 + self.mbr_0x20,   # arg2: dst-x for the shadow-anchor
          self.mbr_0x24 + self.mbr_0x1c,          # arg3: dst-y for the shadow-anchor
          0x32,                                    # arg4: dwidth (= 50; UNCONFIRMED-K)
          0,                                       # arg5: dheight (UNCONFIRMED-K)
          self.mbr_0x28 - 0x32,                    # arg6: sx (mbr_0x28 − 50)
          self.mbr_0x2c,                           # arg7: sy
          0x100)                                   # arg8: drawmode (DM_TRANSPARENT)
      PTR_DAT_005d79e0.ParamBlit(&buf, self.mbr_0x48, 0, 0)  # NOTE src is mbr_0x48 here too
                                                              # NOT mbr_0x4c — UNCONFIRMED-K

      # ============ PER-GLYPH SUB-RECT RE-BLITS ==========================
      # cite: cls_0x534d40.cpp:96-130
      # Iterate j ∈ [0, mbr_0x5c); for each glyph whose "active" flag is set
      # AND whose per-glyph alpha (cross-fade across the parent's alpha) > 0x7f,
      # re-blit a sub-rect of mbr_0x4c at the cell-local rect (mbr_0x80[j*4..]):
      for j in 0 .. self.mbr_0x5c:
        active_flag = self.mbr_0x80[j*4 - 0x40]   # (pdVar5[-0x10] in the decomp;
                                                  # = mbr_0x100 / 0x140 area)
        glyph_alpha = self.mbr_0x140[j]            # per-glyph alpha-driver
        composed_alpha = ((glyph_alpha * 0xff + sign_bias) >> 3) * alpha / 0xff
        if active_flag != 0 and composed_alpha > 0x7f:
          # cell-local rect from mbr_0x80[j*4..]:
          (x0, y0, x1, y1) = (self.mbr_0x80[j*4+0], self.mbr_0x80[j*4+1],
                              self.mbr_0x80[j*4+2], self.mbr_0x80[j*4+3])
          buf2 = SDrawParam{
              drawmode = 0x100,
              dx       = x0 + self.mbr_0x18 + self.mbr_0x20,
              dy       = y1 + self.mbr_0x24 + self.mbr_0x1c,   # dy uses y1 not y0 — UNCONFIRMED
              dwidth   = (x1 - x0) + 1,
              dheight  = (y1 - y0) + 1,
              sx       = x0, sy = y1,
              swidth   = (x1 - x0) + 1,
              sheight  = (y1 - y0) + 1,
              intensity= 0x1f}
          PTR_DAT_005d79e0.ParamBlit(buf2, self.mbr_0x4c, 0, 0)
          # cite: cls_0x534d40.cpp:101-125

  # ============ GLOBAL BLIT-EFFECT POST-PASS =============================
  # The 10-slot effect registry iterate (UI_METHOD_MAP §8.5):
  if g_DAT_005d7a18 == 0:
    FUN_004aacb0_BlitEffect_Iterate(
        self.mbr_0x18 + self.mbr_0x20,
        self.mbr_0x24 + self.mbr_0x1c,
        self.mbr_0x28,
        self.mbr_0x2c)
  # cite: cls_0x534d40.cpp:133-136
```

**The `+0x32 = 50` left/right split.** The choice-cell is composed of a
**left half** at +0 (icon or slot-number badge, 50 px wide, source =
`mbr_0x48`) and a **right half** at +50 (the choice text bitmap, source =
`mbr_0x4c` — sliced at `sx = mbr_0x28 − 50, sy = mbr_0x2c`). UNCONFIRMED-K
flags that the source for the right-half blit at `:94` reads `dVar1 =
mbr_0x48` (line `:90`), NOT `mbr_0x4c`. Two possibilities: (i) `mbr_0x48`
and `mbr_0x4c` are the same surface (split blit from one wide bitmap), or
(ii) it is a Ghidra decompile artifact and the actual call passes
`mbr_0x4c` for the right half. Resolve by reading the raw disassembly at
`0x534e10..0x534e60`.

### String-substitution helper — snapshot reference only

```
// src/dialog.cpp:135-177  (snapshot-confirmed intent; retail equivalent UNCONFIRMED-J)
DialogLine(line, outbuf, buflen):
  scan line for "[<tag>]" tokens  (literal "[[" / "]]" are escapes for "[" / "]"):
    "[me]"  → Player name        (Player->GetName())
    "[chr]" → DlgContext->GetName()  (the NPC currently talking)
  emit copy to outbuf
```

### Parent's compositor — `cls_0x5b98b8::virt_meth_0x435de0` (REFERENCE)

The TButtonPane base compositor that slot-80 chains to. Behavior summary
(`recon/classes/cls_0x5b98b8_likely_MultiplayerSystem.cpp:769-851`):

1. If `mbr_0x150 != 0` (full redraw), clear the parent's def-widget array
   `field_0x88` to zero and call slot-44 to re-fill the cached panel
   buffer (`:780-798`).
2. If `mbr_0x84 != 0` (cached panel buffer exists), clear it via
   `cls_0x438ea0::meth_0x438ea0(buf, 0, 0xffff, 0x7f7f)` and for every
   child whose `(child[5] & 0x800) != 0`, call `child[+0x40](mbr_0x84)`
   to re-composite the child into the buffer (`:805-820`).
3. Blit the panel buffer to the display via
   `cls_0x5a68d8::meth_0x438df0(mbr_0x64, 0, 0, mbr_0x84, 0, 0, mbr_0x84[1],
   mbr_0x84[2], 0x20, 0)` and finalize via `(*+0x20)(0, 0, mbr_0xc, mbr_0x10)`
   (`:821-827`).
4. Animate-pass for **non-cached** children — for each child without the
   `0x800` ("static / cached") flag, if `(child[5] & 0x20) != 0` OR
   `mbr_0x50 != 0`, call `child[+0x40](mbr_0x64)` directly to the
   display (`:828-849`). This is where the **per-frame fade animation**
   on the cls_0x534d40 children would land — they have `0x20` set (the
   "animated" flag) and get drawn straight to the display.

This explains why the per-choice fade (§6.draw_choice) blits to
`PTR_DAT_005d79e0` (= the display) rather than the cached buffer: the
fade-animated chips are NOT cached; they re-paint every frame.

### §6.text — Choice-text layout (FONT, BAKE DESTINATION, METRICS)

**The retail engine does NOT call WriteText per frame for choice text.** It
**bakes the choice text once at AddChoice into the per-choice `mbr_0x4c`
bitmap** and then blits that pre-rendered chip per frame.

**Bake destination — `mbr_0x4c` (the right-half text bitmap):**

The per-choice `cls_0x534d40` has two bitmap handles
(`recon/ghidra/cls_0x534d40.cpp:8-15`):
- `mbr_0x48` (offset 72) — LEFT HALF bitmap. Width = `0x32 = 50` (fixed,
  the slot-number-or-icon column). Height = `mbr_0x2c` = full cell height.
- `mbr_0x4c` (offset 76) — RIGHT HALF bitmap. Width = `mbr_0x28 - 0x32 =
  cell_width - 50`. Height = `mbr_0x2c`.

The right-half bitmap holds the **fully-rendered (font + shadow + color)
choice text composited onto a chroma-key background** at AddChoice time.
Per-frame the chip just transparent-blits `mbr_0x4c` over the chrome.
**Cite:** `recon/ghidra/cls_0x534d40.cpp:90-94, 125`.

**Font slot — `choicefont`:**
- Literal `"choicefont"` at `005dae28` (XREF `004a4a73`, font-table
  registration) and `005dae4c` (XREF `FUN_004a4d40:004a4dbb`, the lookup).
- Same registration pattern as `goldfont` (XREF `004a49fb`) and `silverfont`
  (XREF `004a4ae6`) — all three live in a single font-registration function
  in the 0x4a49..0x4a4b range (the function is **not labeled** in our Ghidra
  dump — UNCONFIRMED-F).
- The registration function is **adjacent to the CreateFontA call**
  (`PTR_CreateFontA_005a3034` XREF at `004a4a13`,
  `recon/ghidra/_data.txt:175-176`) → choicefont is registered via a
  **GDI HDC** with a CreateFontA call, NOT a pre-baked bitmap font.
- This is a **WINFONT** (GDI-rendered) family per the asset convention
  documented in `data/resources_unzipped/font.def:6-10` (BMFONT vs WINFONT
  distinction: BMFONT = pre-rendered glyph sheet; WINFONT = CreateFontA at
  runtime). The `font.def` does NOT declare `"choicefont"` — only
  `"System"`, `"Dialog"`, `"Scroll"`, `"Book"`, `"Numbers"`, `"Small"`,
  `"Med"`, `"Large"`, `"Gold"`, `"GoldMed"`, `"SpellTitle"`, `"Version"`.
  So `"choicefont"`, `"goldfont"`, `"silverfont"` are **hard-coded
  registrations from the engine**, NOT data-driven.
- **Asset note:** `dlgfont.dat` (64,934 bytes) and `dlgfonts.dat`
  (13,872 bytes) are CGSR archives in `data/resources_unzipped/` (magic
  `4347 5352`, topbm = 0x5b = 91 and 0x58 = 88 bitmaps respectively).
  These are **raw bitmap-glyph archives** (NOT TMulti format — `dump_dat.py`
  rejects them with "invalid numoffsets" because they lack the TMulti
  index layer). They are likely the **fallback bitmap font** if the GDI
  CreateFontA registration fails, OR a separate **dialog-only font sheet**
  used pre-Demo. **UNCONFIRMED — verify whether choicefont actually
  references one of these archives.** The registration site at
  `004a4a73` and the surrounding data block `DAT_005dad98..DAT_005dadf4`
  (`recon/ghidra/_data.txt:92390-92500`) contain raw byte sequences that
  look like LOGFONT-A structs (60-byte Win32 structs holding face name +
  size + weight + italics) — UNCONFIRMED-F-1.

**Color / shadow — UNCONFIRMED-F (no body to read).**
- Snapshot used `SColor{255, 0, 50}` (red-pink) for highlight at
  `src/dialog.cpp:256-261` — retail does NOT use a runtime color swap
  (see §4 — selection is the alpha-ramp peak, not a tint).
- The shadow / glow setup is whatever the choicefont registration encodes.
  By analogy with the `font.def`-declared fonts that have `GCOLOR`
  (glow color) and `SCOLOR` (shadow color), the choicefont **probably**
  has both, encoded directly in the registration call. **Resolve:**
  extract `0x004a4a73` body — look for argument pattern matching
  `RegisterFont(name, hFont, glowColor, shadowColor, drawmodeFlags)`
  (where `drawmodeFlags` could include `DM_TRANSPARENT | DM_ALIAS`
  per `font.def:35-38`).

**Per-cell text region — width / height / origin:**

| field | value (source) | cite |
|---|---|---|
| `mbr_0x18` | per-choice pane-local x (set in AddChoice; UNCONFIRMED-F) | `cls_0x534d40.cpp:7, 66, 91` |
| `mbr_0x1c` | per-choice pane-local y (set in AddChoice; UNCONFIRMED-F) | `cls_0x534d40.cpp:8, 68, 91` |
| `mbr_0x20` | per-choice animation x-offset (added to mbr_0x18 in draw; nominal = 0, sentinel `0xffffd8f0` = "hidden") | `cls_0x534d40.cpp:9, 62, 66, 91` |
| `mbr_0x24` | per-choice animation y-offset | `cls_0x534d40.cpp:10, 68, 91` |
| **`mbr_0x28`** | **full cell width** (icon + text) — UNCONFIRMED-F; must satisfy `mbr_0x28 > 50` so right-half is positive | `cls_0x534d40.cpp:11, 92` |
| **`mbr_0x2c`** | **full cell height** = bake-target height = **line-wrapped text-region height** | `cls_0x534d40.cpp:12, 67, 92` |
| `mbr_0x48` | LEFT-half source bitmap (icon / slot number, 50×mbr_0x2c) | `cls_0x534d40.cpp:13, 88, 90` |
| `mbr_0x4c` | RIGHT-half source bitmap (text, (mbr_0x28-50)×mbr_0x2c) | `cls_0x534d40.cpp:14, 125` |

**The bake call (UNCONFIRMED, hypothesis from sibling panels):**
1. AddChoice receives `(label, text)`.
2. Resolves `choicefont` to an HFONT via `FUN_004a4d40:004a4dbb`
   (`recon/ghidra/_data.txt:92549`).
3. Computes word-wrap layout against a target rect width
   `text_w = mbr_0x28 - 50`. Stores resulting line-rects in `mbr_0x80[j*4..]`
   (cell-local x0,y0,x1,y1 per line; max 8 lines per choice).
4. Allocates `mbr_0x4c` bitmap at `(text_w, mbr_0x2c)` and renders the
   wrapped text via `meth_0x4be2b0` (the retail WriteText, GDI-DrawTextA
   under the hood, `recon/ghidra/_data.txt:3006-3008`) onto a chroma-key
   (magenta) background.
5. If `choicefont`'s registration includes shadow bit `0x400`, the bake
   does the 3-pass FUN_004be2b0 shadow walk and bakes the shadow into
   `mbr_0x4c` (so the per-frame chip already has shadow baked in — there
   is **NO runtime shadow pass** in the per-frame draw at `cls_0x534d40.cpp`).
6. Sets `mbr_0x58 = 0` (fade counter starts hidden), `mbr_0x5c =
   num_wrapped_lines`, `mbr_0x100[j]` active flags = 1 for each line,
   `mbr_0x140[j]` per-line alpha drivers = 0..ramp-target.

**Cite:** structure inference from `recon/ghidra/cls_0x534d40.cpp:8-22`
(struct layout) + `:91-94, 125` (per-line re-blit pattern) +
`recon/ghidra/_data.txt:92549, 3006-3008, 175` (font + GDI usage).
The AddChoice site itself is NOT extracted (`FUN_00534470`, `FUN_00534b60`,
`FUN_00533dd0`, `FUN_00533f10` are all unlabeled free functions in the
0x533dd0..0x534b60 range — UNCONFIRMED-S).

### §6.wrap — Per-line wrap structure & per-line typewriter reveal

The choice text is **word-wrapped at bake time** into up to **8 line
segments**, each stored as a cell-local rect in `mbr_0x80[0..7]` and
animated via the per-line alpha drivers `mbr_0x140[0..7]`.

**Storage capacity (computed from struct dump):**

| field | byte offset | count | role | cite |
|---|---|---|---|---|
| `mbr_0x80[j*4 + 0..3]` | `+0x80..+0x100` | **8 entries × 4 dwords = 8 rects max** | per-line cell-local `(x0, y0, x1, y1)` | `cls_0x534d40.cpp:17-20, 96-130` (iterator strides `pdVar4 = pdVar4 + 4` over 4-dword entries) |
| `mbr_0x100[j]` | `+0x100..+0x140` | 16 entries × 1 dword = 16 flags | per-line "active / re-blit-this-frame" flag | `cls_0x534d40.cpp:21, 100` (`pdVar5[-0x10] != 0` reads `mbr_0x100[j]` as `pdVar5` at `+0x140 + j*4`, minus `0x40` bytes = `+0x100 + j*4`) |
| `mbr_0x140[j]` | `+0x140..` (struct ends at 324 = `0x144` per dump) | array runs past the dump end | per-line alpha-ramp tick (0..N typewriter counter) | `cls_0x534d40.cpp:22, 97, 101` |
| `mbr_0x5c` | `+0x5c` | 1 dword | **wrapped-line count** | `cls_0x534d40.cpp:16, 96, 130` (loop bound) |

**Effective cap = 8 wrapped lines per choice** (the rect array dominates).

**Per-line rect format.** The four dwords are `(x0, y0, x1, y1)` —
cell-local inclusive corners, NOT (x, y, w, h). Cite the size derivation
in the draw body at `cls_0x534d40.cpp:103-105, 124`:

```
local_20 = (pdVar4[3] - pdVar4[1]) + 1   # height  = (y1 - y0) + 1
local_24 = (pdVar4[2] - *pdVar4) + 1     # width   = (x1 - x0) + 1
local_28 = pdVar4[1] + this->mbr_0x24 + this->mbr_0x1c    # screen-y = y0 + anim_y + pane_y
local_2c = *pdVar4   + this->mbr_0x18 + this->mbr_0x20    # screen-x = x0 + pane_x + anim_x
local_1c = *pdVar4   = sx          # src reads from same cell-local coords
local_18 = pdVar4[1] = sy
```

So `mbr_0x80[j*4+0] = x0`, `mbr_0x80[j*4+1] = y0`,
`mbr_0x80[j*4+2] = x1`, `mbr_0x80[j*4+3] = y1`. Reading source at the same
cell-local coords means the wrapped-text bitmap `mbr_0x4c` is **exactly
the cell's text-region geometry** (sx = x0, sy = y0).

**Per-line alpha gating (the typewriter):** Each line draws only when its
own alpha gate passes:

```
parent_alpha = (mbr_0x58 * 255) / 12          # 0..255 over 12 ticks
gate_pass = (((mbr_0x140[j] * 255) / 8) * parent_alpha) / 255 > 127
```

So each line has its own 0..N counter at `mbr_0x140[j]` that multiplies
with the parent's hover-fade. If parent is fully faded in (`parent_alpha
= 255`), line j is visible when `mbr_0x140[j] * 255 / 8 > 127`, i.e.
`mbr_0x140[j] > 4`. The line counter is therefore in a small range
(0..maybe 8 or 16) and the per-line reveal IS a **typewriter / word-by-
word reveal**. **UNCONFIRMED — the driver that advances `mbr_0x140[j]`
is in slot 28 `virt_meth_0x535550` (which calls `FUN_00534b60` per child,
`cls_0x5a5c60.cpp:312-329` — UNCONFIRMED-O).**

**Word-wrap behavior at AddChoice (UNCONFIRMED — the bake site is in
unlabeled `FUN_00534470` or earlier).** Inferred shape from the data:

```
wrap(text, font, max_width):
  lines = []
  current_line = ""
  current_w = 0
  for each word w in text.split(' '):
    w_width = font.measure(w + ' ')
    if current_w + w_width > max_width and current_line != "":
      lines.append(current_line)
      current_line = w
      current_w = font.measure(w)
    else:
      current_line += (' ' if current_line else '') + w
      current_w += w_width
  if current_line != "": lines.append(current_line)
  # cap at 8 lines:
  if len(lines) > 8: lines = lines[:8]   # UNCONFIRMED — see §6.overflow
  return lines

# After wrap, each line gets a cell-local rect:
y_cursor = 0
line_h   = font.height + lextra        # lextra from font.def-style spec
for i, line in enumerate(lines):
  line_w_px = font.measure(line)
  rect[i] = (0, y_cursor, line_w_px - 1, y_cursor + line_h - 1)
  active[i] = 1
  alpha_tick[i] = 0
  y_cursor += line_h

mbr_0x2c = max(y_cursor, min_choice_h)   # final cell height grows with wrap
mbr_0x5c = len(lines)
```

The fact that `mbr_0x2c` is per-instance (not a global constant like the
snapshot's `CHOICEHEIGHT = 21`) confirms each choice can be a **different
height** depending on wrap. **UNCONFIRMED — whether the engine grows
`mbr_0x2c` to accommodate wrap, or whether long choices are silently
truncated to a fixed height.** The cap of 8 line-rects gives a **hard
upper bound** on text length per choice; once 8 lines are full, further
text is dropped (or the choice ellipses, see §6.overflow).

### §6.overflow — 6-choice vertical overflow + per-choice text overflow

There are **two overflow regimes**, gated independently:

**(A) Per-choice text overflow (text width / line count exceeds cell):**

- **Width overflow:** word-wrap kicks in at AddChoice time
  (§6.wrap), producing up to 8 line-rects. Width per line is `mbr_0x28 - 50`
  (the right-half region after the 50-px left-half icon column).
- **Line-count overflow:** the per-choice rect array is hard-capped at
  **8 entries** by struct geometry (`(0x100 - 0x80) / 16 = 8`). A choice
  whose word-wrap produces > 8 lines either (i) **truncates** the
  9th-and-later text, (ii) **ellipses**, or (iii) **does not wrap at all**
  and just clips at the cell edge. UNCONFIRMED-T. Snapshot used a single
  WriteText call with no wrap — the entire choice text fits on one line
  or runs off the right edge of the pane (`src/dialog.cpp:260`).
- **Height growth:** the cell's `mbr_0x2c` is per-instance and **larger
  for longer choices**. This means the vertical packing of the 6 choices
  is NOT a fixed-stride table — the cells stack with varying heights and
  the per-choice `mbr_0x1c` is set at AddChoice based on previous cells'
  cumulative heights. UNCONFIRMED-T.

**(B) Six-choice vertical overflow (the panel runs out of vertical space):**

- **Choice cap = 6.** Enforced in `KeyPress` at `cls_0x5a5c60.cpp:401-407`
  via `0x30 < param_1 < 0x37` and `param_1 - 0x31U < this->mbr_0x1d8`.
  The struct's `mbr_0x1d8` (numchoices) is therefore the run-time count and
  the **'7' key is silently ignored** even if a 7th choice were added.
- **AddChoice silently drops at >6.** Snapshot enforces `if (numchoices
  >= MAXCHOICES) return;` (`src/dialog.cpp:362-363`). Retail enforces the
  same cap, but with `MAXCHOICES = 6` (vs snapshot's 4). The retail
  AddChoice site is unlabeled (`FUN_00534470` / `FUN_00533dd0` /
  `FUN_00533f10` are the candidates by data-XREF proximity — UNCONFIRMED-S).
- **The choice-array `field_0x17c..field_0x18c` is a `cls_0x41c7f0`
  (TPointerArray) with initial capacity 4** (constructor at
  `cls_0x5a5c60.cpp:68` → `cls_0x41c7f0::cls_0x41c7f0(this, 4)`). The
  capacity-mask test `*(uint *)&this->field_0x184 & 0x3fffffff`
  (`cls_0x5a5c60.cpp:165, 249`) suggests **TPointerArray auto-grows**
  beyond the initial capacity (the 0x3fffffff mask is a capacity bit-field).
  So a 6-choice load grows the array to ≥ 6.
- **Vertical packing.** With pane height = 140 px and chrome = 140 px,
  the available vertical space inside the chrome is at most 140 px. The
  snapshot's `CHOICEHEIGHT = 21` × 4 = 84 px + NPC line ≈ 4 px top
  padding = 88 px ≤ 97 (snapshot pane h). Retail with **6 choices at
  ~21 px each** = 126 px + NPC line (one or two lines) ≈ 14..28 px
  → total ≈ 140..154 px. This is **right at the limit** of the 140 px
  chrome — and confirms why retail caps at 6 (any more wouldn't fit
  even on one line each). UNCONFIRMED-T-1 — whether retail enforces a
  minimum vertical packing (cells overlap if they each grow tall via
  wrap) or whether the AddChoice silently drops the 6th choice if the
  previous 5 already consumed too much vertical space.

**The conservative reconstruction stance:**

For the port, until UNCONFIRMED-S and UNCONFIRMED-T are resolved by
extracting the AddChoice body, **reconstruct using these rules**:

1. **Choice cap = 6** (hard, gate AddChoice + KeyPress).
2. **Per-choice text width = (cell_width - 50) px** where cell_width is
   set per-choice but **defaults to the full pane width minus a small
   left/right pad** (UNCONFIRMED — likely `pane_w - 2*pad_x` for some
   small `pad_x ≈ 8..16`).
3. **Word-wrap the choice text** at bake time into up to **8 lines**.
4. **Cell height = N_lines × line_height + small top/bottom pad**, where
   `line_height = font.height + lextra` (UNCONFIRMED-F — font.height
   comes from the GDI HFONT created at `0x004a4a13`'s CreateFontA;
   plausible point sizes are 12..16 by visual analogy with the
   "Small"/"Med" fonts in `data/resources_unzipped/font.def:60-61`).
5. **Stack choices vertically with running y-cursor** starting at a
   small top pad (snapshot intent: `y = 4` then `y += CHOICEHEIGHT`).
6. **If running y-cursor > pane_h - bottom_pad** when about to add the
   N-th choice → **silently drop** the choice (matches snapshot's
   `numchoices >= MAXCHOICES` semantic, generalized for variable
   height). UNCONFIRMED — verify by adding a deliberately-tall 6th
   choice and observing whether it draws or is dropped.

### §6.dialogline_retail — Retail `[me]`/`[chr]` substitution (snapshot-equivalent)

Wave-3 finding: the retail body of snapshot's `DialogLine()` lives in
**`FUN_00533dd0`**, which references the literal token strings `"me"`
(`DAT_005e3f3c`, XREF `00533e39`) and `"chr"` (`DAT_005e3f40`, XREF
`00533e62`). `recon/ghidra/_data.txt:113978-113993`. This **resolves
UNCONFIRMED-J**: retail does the same `[me] / [chr]` substitution as the
snapshot, with the same token literals.

The neighboring `FUN_00533f10` references the format string `"\"%s\""`
(`DAT_005e3f44`, XREF `0053400c`) — likely the wrapper that surrounds
substituted character names with quotes in the bake call. This pattern
matches Revenant's convention of quoting NPC names in dialog
(snapshot `src/dialog.cpp:135-177` lacks the quoting; retail adds it).

Per the proximity rule (these helpers live in the 0x533dd0..0x534b60
range, immediately adjacent to TDialogPane methods at 0x53??), they are
**TDialogPane helpers** even though Ghidra has them as bare `FUN_` —
they are the AddChoice / DialogLine / wrap-text helpers, not extracted.

---

## §7 — Effects & shadows

- The per-choice chip's **right-half blit** uses
  `FUN_00438d80_BlitEffect_StructInit` (UI_METHOD_MAP §8.1) to set up the
  blit descriptor (`recon/ghidra/cls_0x534d40.cpp:91-94`). Per
  UI_METHOD_MAP §8.1, the first two coord args after `buf` are the **shadow
  displacement (dx, dy)** — the location where the shadow lands. The actual
  arg values here are the chip's destination corner:
  `dx_arg = mbr_0x18 + 0x32 + mbr_0x20`, `dy_arg = mbr_0x24 + mbr_0x1c`.
  These are NOT a constant shadow offset — they are the runtime per-chip
  destination corner of the right half. **This means the chip does NOT
  carry a fixed drop shadow** (in the TPlyrStatusBar sense); the call's
  shadow direction is whatever the SDrawParam's normal `(dx, dy)` puts it,
  composited per-call through the global blit-effect iterate at
  `cls_0x534d40.cpp:134`. The blit-effect iterate path (UI_METHOD_MAP §8.5)
  applies whatever effects are currently registered in the 10-slot
  registry — which by default for the dialog is **empty**. So in practice,
  the chip composites with no drop shadow unless some other panel
  registered an effect that intersects the chip rect.
- **Text drop shadow.** The choice TEXT is rendered into the `mbr_0x4c`
  bitmap **once at AddChoice time** (the AddChoice path likely calls
  `FUN_004be2b0` with the choicefont; UNCONFIRMED-F). Whether the font id
  for `choicefont` has bit `0x400` set (the 3-pass black drop shadow,
  UI_METHOD_MAP §5) is UNCONFIRMED — visually inspect the resulting
  `mbr_0x4c` cache to see whether the text already has shadow baked in.
- **Chroma key.** Both `Dialog` and `DialogEndCap` declare `keycolor=0`,
  which per `src/bitmapdecode.cpp:59-66` defaults to global magenta `0x7C1F`.
  The chrome is opaque purple stone; visual inspection of the PNG
  confirms no magenta pixels are present, so the keying is harmless for
  the chrome. The per-choice cached chip (`mbr_0x4c`) likely has a magenta
  background that gets keyed out at chip-blit time, so the chip text
  appears against the chrome behind it. Drawmode `0x100 =
  DM_TRANSPARENT` (UI_METHOD_MAP §1) used by both chip half-blits confirms
  the chroma-keyed behavior.
- **Pink-halo risk.** Per [[project-retail-pink-halo-bug]] and
  UI_METHOD_MAP §16: if the choice text was rendered into a magenta-cleared
  scratch with chroma-keyed blit, the AA fringe shows as pink. Avoid by
  using real alpha (no magenta colorkey) in the port — already-standard via
  `BuildTTFAtlas` + `CompositeSwapchainTinted` (UI_METHOD_MAP §12).

---

## §8 — Text rendering

| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|---|---|---|---|---|---|---|---|---|---|---|
| NPC line | UNCONFIRMED-F (likely top region of the chrome, full chrome width minus left/right padding ≈ `(pad_l, 4..16, 640 − pad_l − pad_r, line_h)`) | `choicefont` (or possibly a dialog/NPC-name font — UNCONFIRMED-F) | UNCONFIRMED-F | UNCONFIRMED-F | UNCONFIRMED — likely left or center | top (default unless `&0x20` set, UI_METHOD_MAP §5) | UNCONFIRMED — likely on (font id bit `0x400`) given the chrome's dark-purple background | (no format; line = `DialogList.GetLine(saytag)` then `DialogLine([me]/[chr] expansion)`) | NPC's current SayCommand text (script-driven) | UNCONFIRMED-F |
| choice[i] (entire wrapped paragraph, baked once at AddChoice into `mbr_0x4c`) | screen `(choice[i].mbr_0x18 + 0x32 + choice[i].mbr_0x20, choice[i].mbr_0x24 + choice[i].mbr_0x1c)`, size `(choice[i].mbr_0x28 − 50, choice[i].mbr_0x2c)` (cited from `cls_0x534d40.cpp:91-94` — right-half text-bitmap dest rect; **actual GDI text composite happens once at AddChoice via `meth_0x4be2b0` rendering into `mbr_0x4c`**, see §6.text) | `choicefont` — registered at `0x004a4a73` (XREF of `s_choicefont` literal at `005dae28`), resolved via `FUN_004a4d40:004a4dbb`; HFONT-based (CreateFontA at `0x004a4a13`) | UNCONFIRMED-F (plausibly 12-16 px by visual analogy with `font.def:60-61` "Small" / "Med" sizes); the GDI HFONT carries its own height in the LOGFONTA struct at `DAT_005dad98+` (UNCONFIRMED-F-1) | UNCONFIRMED-F (snapshot used `SColor{255, 0, 50}` red-pink for the highlighted slot at `src/dialog.cpp:256-261`; retail does not have a runtime color swap — selection is the alpha-ramp peak, see §4 + §6.text) | UNCONFIRMED — likely left (snapshot used h-align bit `&1`, `src/dialog.cpp:260` arg `1`); word-wrap may impose left-align by default | top | UNCONFIRMED — likely baked-in (font id bit `0x400` set in registration). The 3-pass `FUN_004be2b0` shadow walk would fire once at bake, NOT per frame; the per-frame chip draw at `cls_0x534d40.cpp:88,94,125` does NOT invoke any shadow path. | `"%d. %s"` or no prefix — UNCONFIRMED-V. KeyPress accepts `'1'..'6'` so a leading "N. " prefix is plausible; snapshot did NOT prefix (`src/dialog.cpp:260`). The 50-px left-half (`mbr_0x48`) may **contain the "N." badge as a baked icon**, in which case the right-half text has no prefix. | `DialogList.GetLine(choices[i])` then `DialogLine(...)` expansion (the retail `FUN_00533dd0` `[me]`/`[chr]` substitution, §6.dialogline_retail); falls back to the raw tag if no translation found (`src/dialog.cpp:257-259` snapshot intent) | UNCONFIRMED-F (retail metrics); snapshot reference `src/dialog.cpp:260`; retail wrap shape `recon/ghidra/cls_0x534d40.cpp:17-22, 96-130` |
| choice[i] line[j] (one wrapped LINE of the choice text, addressable for selective re-blit + per-line typewriter alpha) | screen `(mbr_0x80[j*4+0] + choice[i].mbr_0x18 + choice[i].mbr_0x20, mbr_0x80[j*4+1] + choice[i].mbr_0x24 + choice[i].mbr_0x1c)`, size `((mbr_0x80[j*4+2] − mbr_0x80[j*4+0]) + 1, (mbr_0x80[j*4+3] − mbr_0x80[j*4+1]) + 1)` | (same font as parent — re-blit from the same baked `mbr_0x4c`) | (same) | (same) | (same — wrap layout uses left-align by construction; the rect's `(x0, y0)` is the line's top-left in cell-local) | top | (same — baked) | n/a (per-line is a re-blit, no font call) | one segment of the wrapped paragraph after word-wrap | `recon/ghidra/cls_0x534d40.cpp:96-130` (rect derivation `:103-105`; per-line alpha gate `:101`) |

**Selection visual is alpha-peak, not color-swap.** The snapshot used a
runtime color override `&color` for the touched slot (`src/dialog.cpp:261`).
Retail uses a pre-rendered choice chip in `mbr_0x4c` and the per-frame fade
ramp `mbr_0x58` (§9) controls alpha. The "highlighted" slot is the one
whose ramp has reached 0xc and so blits at full intensity, while non-
highlighted slots either don't draw (alpha ≤ 0x7f) or fade in.

UNCONFIRMED-K — whether there is also a per-glyph re-blit (the `mbr_0x80[..]`
sub-rect pass at `cls_0x534d40.cpp:96-130`) that fires during the highlight
phase to overlay a "selected" variant of the text. The per-glyph fields
`mbr_0x100..mbr_0x140` look like animated alpha drivers for individual
letters — possibly a typewriter-style reveal of the choice text.

---

## §9 — Animation & dynamic behavior

### Per-choice hover/active fade

```
ramp choice[i].fadeCounter:
  storage cls_0x534d40.mbr_0x58 (4-byte, range 0..0x0c)
  range   0..0x0c (= 0..12)
  step    +1 per sim tick toward target  (24Hz, NOMENCLATURE §5)
  target  0x0c when choice is the active/hovered slot, else 0
  maps-to alpha = (mbr_0x58 * 0xff) / 0xc  (0..255 linear)
  gate    chip draws only when alpha > 0x7f (= 128 — i.e. mbr_0x58 > 6)
  drive   slot 92 advances the ramp (UNCONFIRMED — the advance step body is
          not in cls_0x534d40.cpp:30-140, which is draw-only; the advance is
          almost certainly in slot 28 `virt_meth_0x535550` which calls
          FUN_00534b60 per child — UNCONFIRMED-O)
  on-end  hold at target
  cite    recon/ghidra/cls_0x534d40.cpp:63-64
```

### Per-line typewriter fade (REVISED — wave 3)

Wave 3 supersedes the earlier "per-glyph fade" interpretation. The
`mbr_0x80[j*4..]` rects are **per-wrapped-LINE**, not per-letter — see
§6.wrap. The reveal animation is therefore a **per-line typewriter**:
each successive line of the choice text fades in one after another.

```
ramp choice[i].line[j].fadeCounter:
  storage cls_0x534d40.mbr_0x140[j] (parallel to mbr_0x80[j*4..] line-rects)
  range   0..(unknown — likely small, 0..8 or 0..0xc per the divisor 8 in
          the alpha formula `(mbr_0x140[j] * 255) >> 3`)
  maps-to per-line alpha = ((mbr_0x140[j] * 255) >> 3) * parent_alpha / 255
  gate    line draws only when composed_alpha > 0x7f (= 128)
          AND mbr_0x100[j] != 0 (the per-line "active" flag must be set)
  drive   UNCONFIRMED-O — likely a per-line stagger of the parent ramp.
          The advance step is almost certainly in slot 28 (virt_meth_0x535550
          → FUN_00534b60 per child) — both UNCONFIRMED.
  on-end  hold at full alpha; line stays visible
  cite    recon/ghidra/cls_0x534d40.cpp:21, 96-130 (rect + alpha iteration);
          struct dump :17-22
```

UNCONFIRMED-P (revised) — the per-line typewriter may not animate at all
in retail; `mbr_0x140[j]` may just be set to a constant high value at
AddChoice (so all lines appear in lockstep with the parent fade) and the
"per-line" structure exists solely so the engine can selectively re-blit
just one line on damage. Verify by:
1. Reading the AddChoice path (UNCONFIRMED-S) — does it set
   `mbr_0x140[j]` to a per-line stagger value, or to a uniform constant?
2. Reading slot 28 `FUN_00534b60` (UNCONFIRMED-O) — does it advance
   `mbr_0x140[j]` per frame?

If the answers are "uniform constant" and "no advance", the per-line
animation is dead-code from an earlier prototype and the line-rect
structure is just a damage-blit optimization.

### Pane show/hide

The snapshot does Show/Hide instantly (`src/dialog.cpp:208-244`). The
retail TPlayScreen Pulse mode-1 path
(`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:42-100`)
animates the **bottom HUD displacement** between 0 and 140 px (`iStack_4 =
0x8c`) by comparing `DAT_00667c70` against `pane_h - iStack_4` and
issuing one-tick step updates (`:101-129`). This is the lower-HUD slide,
not the dialog pane itself; whether the **dialog pane's chrome** also
slides into view, or appears instantly, is UNCONFIRMED-L.

```
tween bottomHud.y:
  from   pane_h                       # off-screen below
  to     pane_h - 0x8c                # 140 px above bottom edge
  over   UNCONFIRMED (per-tick step rate not extracted)
  easing linear (one-tick step)
  drive  TPlayScreen mode transition (mode-1 enter/exit)
  on-end hold at target
  cite   recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp:42-100
```

The DialogPane itself most likely appears instantly (the snapshot does;
retail Show via `0x435f90` inherits the TButtonPane Show shape, which is
also instant). UNCONFIRMED-L.

### Dirty / redraw model

The retail `cls_0x5a5c60` has fields `mbr_0x48` and `mbr_0x4c` set to `1`
in `meth_0x535120` (the Hide-like, `recon/ghidra/cls_0x5a5c60.cpp:245-246`).
By the parent compositor model (`cls_0x5b98b8::virt_meth_0x435de0`,
`:780-798`), these are plausibly the **needs-full-redraw / cached-buffer-
dirty** flag pair. UNCONFIRMED-M — confirm by reading where else they are
toggled to 1.

The choice-array dirty state is implicit: every AddChoice / SetChoice /
ResetResponses call would set `mbr_0x150 = 1` to invalidate the cached
panel buffer.

---

## §10 — Input & dispatch

| control | hit rect | command id / key | modal-state global | visual effect |
|---|---|---|---|---|
| key `'1'..'6'` (down) | n/a | `0x31..0x36`; gate `key − 0x31 < mbr_0x1d8` (numchoices); gate `field_0x190 != 0` (input enabled) | sets `mbr_0x1dc = key - 0x31`, `mbr_0x1e4 = 1` (freshresponse) | the matching slot's `mbr_0x58` ramps to `0xc` and the chip blits at full alpha | `recon/ghidra/cls_0x5a5c60.cpp:401-407` |
| key `' '` (space, down) | n/a | `0x20` → "skip current say"; iterates children and forces each character's `ppcVar1[0x14] = 1`, `ppcVar1[0x16] = 0`, and frees sub-glyph cache `ppcVar1[0x40 + j]` for `j < ppcVar1[0x17]` | n/a (no choice committed) | dialog text advances; current say command is force-completed | `recon/ghidra/cls_0x5a5c60.cpp:369-399` |
| any other key | n/a | falls through to parent `cls_0x5b93c4::virt_meth_0x4361f0` (`KeyPress` chain on TButtonPane base) | n/a | parent handles (e.g. tab cycling between buttons) | `recon/ghidra/cls_0x5a5c60.cpp:409` |
| MouseClick / button code `0x40a` | n/a (panel-level — exact hit-rect derived from choice cells, UNCONFIRMED-F) | code `0x40a`; gate `field_0x190 != 0`; iterates children identically to space-key Skip — same child-forcing path | n/a | dialog advances on click outside the choice cells | `recon/ghidra/cls_0x5a5c60.cpp:427-455` |
| choice cell click | per-choice rect from §3 (screen `(mbr_0x18+mbr_0x20, mbr_0x24+mbr_0x1c)` size `(mbr_0x28, mbr_0x2c)`) | dispatched by TButtonPane base — children handle their own clicks and commit `mbr_0x1dc = i, mbr_0x1e4 = 1` via the same path as the key handler | (same fields) | the clicked slot ramps to `0xc` and commits | UNCONFIRMED — the per-choice MouseClick handler is on `cls_0x534d40` (which has no extracted methods beyond the draw); the commit is presumed via a parent-callback. Resolve via slot-22 `0x435f90` on `cls_0x5a5c60` (the inherited TButtonPane MouseClick chain, which probably calls into the choice on `(*+0x30)`). |

**Skip semantics.** Both the space key (`'1'..'6'` are NOT a skip — they are
choice commit) and mouse button `0x40a` trigger the same "force say
command done" path: walk each choice child, set `ppcVar1[0x14] = 1`
(probably "selected/dismissed" flag) and `ppcVar1[0x16] = 0`, then free
all sub-glyph cache entries `ppcVar1[0x40 + j]`. This is exactly the
snapshot's `Skip()` behavior (`src/dialog.cpp:400-407` →
`character->ForceCommandDone()`) but issued at the choice-children level
rather than via a Character call. UNCONFIRMED — the
`FUN_00452690_TMapPane_GetInstance(ppcVar1[4], 2)` call at `:377` /
`:434` looks up the **character** the choice is bound to (via the
choice's `+0x4` field) and force-dones its say command via
`cls_0x4d6000::meth_0x4d6000`. So choices ARE bound to characters; per-
choice you can find the bound character via `child->mbr_0x4`.

**Mouse button code `0x40a`.** This is a custom TGameWindow message id —
`0x400` is the standard `WM_USER` base, so `0x40a = WM_USER + 0xa = WM_USER
+ 10`. Almost certainly the "click anywhere to advance dialog" message.
UNCONFIRMED — cross-reference the dispatch table in
`FUN_0047cf40_TPlayScreen_DispatchCommand`.

**Modal-state globals.** The retail TDialogPane's "is open" state lives on
the instance itself — `field_0x40 = mbr_0x40` is the IsOpen-like flag
(zero = not initialized / closed). Every state mutator checks
`this->mbr_0x40 != 0` first (Init `:146`, Close `:193`, Hide `:261`,
KeyPress `:370`, MouseClick `:427`). The global TDialogPane instance is
allocated by TPlayScreen's Initialize (UNCONFIRMED-N — the global address
is the `this` ptr at the call site of `FUN_00534fd0` in
`_TPlayScreen_Initialize_47a660.cpp:205`).

**Dispatch.** TPlayScreen's central command switch is
`FUN_0047cf40_TPlayScreen_DispatchCommand`
(`recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp`); search
this for the dialog open/close commands. UNCONFIRMED whether the dialog's
open is a TPlayScreen command id or a direct script callback (snapshot
uses `COMMAND(CmdChoice)` for script-pushed choices, `src/dialog.cpp:410-426`).

---

## §11 — Retail bugs NOT to reproduce

Per [[project-retail-pink-halo-bug]] and UI_METHOD_MAP §5: if the choice
text bitmap (`mbr_0x4c`) was rendered into a magenta-cleared scratch with
chroma-keyed blit, the AA fringe leaks pink around shadowed glyphs. **Do
not reproduce.** The port already standardizes on TTF-coverage atlases
composited with real alpha (no magenta key), per UI_METHOD_MAP §12.

Beyond that, **no panel-specific bugs are catalogued yet** — the paint
method @ `0x5351d0` has not been read, so additional artifacts may exist.
Re-scan when UNCONFIRMED-E is resolved.

---

## §12 — Reconstruction pseudocode

**Skeleton — the §14 items must still be resolved before this becomes
buildable.** The pseudocode below shows the structure pinned by this
forensics pass; UNCONFIRMED items use placeholder values.

```
class TDialogPane : TButtonPane:

  // Per-choice widget (cls_0x534d40)
  struct Choice {
    int  x, y;             // pane-local anchor (mbr_0x18, mbr_0x1c)
    int  offX, offY;       // animation offset (mbr_0x20, mbr_0x24)
    int  width, height;    // cell size (mbr_0x28, mbr_0x2c)
    TBitmap* leftHalf;     // icon / slot-number bitmap (mbr_0x48)
    TBitmap* rightHalf;    // text bitmap (mbr_0x4c)
    int  fadeCounter;      // 0..0xc ramp (mbr_0x58)
    int  numGlyphs;        // mbr_0x5c
    Rect glyphRects[N];    // mbr_0x80[j*4..]
    int  glyphFade[N];     // mbr_0x140[j]
    TCharacter* boundChar; // mbr_0x4 — character bound to this choice
                            // (for ForceCommandDone on skip)
  };

  Initialize():
    if (open) return true                                  // mbr_0x40 != 0 short-circuit
    TButtonPane::Initialize()                              // cls_0x5a45c8 0x434e40
    choices.array.clear()                                  // field_0x18c cleared via :163-173
    numChoices = 0                                         // mbr_0x1d8
    activeChoice = -1                                      // mbr_0x1dc = 0xffffffff
    freshResponse = false                                  // mbr_0x1e4
    inputLocked = false                                    // field_0x190
    chrome     = LoadBitmap("Dialog.dat", "Dialog")        // 640x140
    endCap     = LoadBitmap("Dialog.dat", "DialogEndCap")  // 10x140
    choiceFont = GetFont("choicefont")
    mbr_0x60  |= 2                                         // sets the "needs paint" flag
    open = true

  Show(anchorMode = BOTTOM):                               // anchorMode UNCONFIRMED-G
    anchor = anchorMode
    PlayScreen.SetControlOn(false)                         // snapshot intent
    PlayScreen.HideLowerPanes()                            // snapshot intent
    PlayScreen.SetFullScreen(false)                        // snapshot intent
    SetDirty()                                             // mbr_0x150 = 1

  Hide():                                                  // = meth_0x535120
    cachedBufferDirty = true                               // mbr_0x48 = 1
    fullRedraw = true                                      // mbr_0x4c = 1
    free per-choice glyph caches                           // :247-256
    choices.array.clear()                                  // :258-260
    numChoices = 0
    activeChoice = -1
    freshResponse = false
    PlayScreen.ShowLowerPanes()
    PlayScreen.SetControlOn(savedControl)
    PlayScreen.SetFullScreen(savedFullScreen)
    character = nullptr

  AddChoice(label, text, character):                       // (not extracted — UNCONFIRMED-S)
    if numChoices >= 6: return                             // §6.overflow B; UNCONFIRMED-C
    c = new Choice
    c.boundChar = character
    c.x         = pane.contentX0                           // UNCONFIRMED-F (pane-local left pad, ~8-32 px)
    c.width     = pane.contentWidth                        // UNCONFIRMED-F (~pane_w - 2*pad_x)

    // --- Word-wrap the choice text (§6.wrap) ---
    text_w = c.width - 50                                  // right-half text region width
    expanded = DialogLine(text)                            // [me]/[chr] substitution per §6.dialogline_retail
    lines = WordWrap(expanded, choiceFont, text_w)         // up to 8 lines max
    if len(lines) > 8: lines = lines[:8]                   // hard cap — UNCONFIRMED-T-2
    line_h = choiceFont.height + lextra                    // lextra UNCONFIRMED-F
    c.height = len(lines) * line_h + topPad + bottomPad    // grows with wrap

    // --- Vertical packing with running y-cursor (§6.overflow B) ---
    c.y = pane.contentY0 + sum(prev.height for prev in choices)
    if c.y + c.height > pane_h - bottomPad:                // overflow — silent drop
      return                                               // UNCONFIRMED-T-1

    // --- Bake the per-line wrap rects into mbr_0x80 + bake text into mbr_0x4c ---
    c.numLines = len(lines)                                // mbr_0x5c
    y_cursor = 0
    for j, line in enumerate(lines):
      line_w = choiceFont.measure(line)
      c.lineRect[j] = (0, y_cursor, line_w - 1, y_cursor + line_h - 1)   // mbr_0x80[j*4..]
      c.lineActive[j] = 1                                  // mbr_0x100[j]
      c.lineAlpha[j]  = 8                                  // mbr_0x140[j] — UNCONFIRMED-P (typewriter?)
      y_cursor += line_h

    c.leftHalf  = renderSlotNumber(numChoices + 1)         // UNCONFIRMED-V — "1." sprite OR zero-filled
    c.rightHalf = bakeWrappedText(lines, choiceFont, text_w, c.height,
                                  shadow=choiceFont.has_shadow_bit_0x400)   // UNCONFIRMED-F
    c.fadeCounter = 0                                      // mbr_0x58 — starts hidden, ramps via slot 28
    c.animOffX = 0                                         // mbr_0x20
    c.animOffY = 0                                         // mbr_0x24
    choices.array.push(c)
    numChoices += 1
    SetDirty()

  Skip():                                                   // = KeyPress(' ')
    for c in choices:
      character = TMapPane.GetInstance(c.boundChar, 2)
      if character: character.ForceCommandDone()
      c.skip_flag = 1                                       // ppcVar1[0x14] = 1
      c.something = 0                                       // ppcVar1[0x16] = 0
      for j in 0 .. c.numGlyphs:                            // ppcVar1[0x17]
        if c.glyphCache[j]:
          freeGlyphResource(c.glyphCache[j])                // cls_0x4367d0::meth_0x4367d0
          c.glyphCache[j] = nullptr

  // === slot 76 — top-level draw / clip update ===
  DrawTopLevel():                                           // virt_meth_0x5351d0 UNCONFIRMED-E
    // reads pending-redraw clip rect from DAT_006668d{c,0,4,8}
    // (likely composes the chrome blit position + sets up the per-frame redraw)
    ...

  // === slot 80 — DrawBackground ===
  DrawBackground():                                         // virt_meth_0x535500
    if not open: TButtonPane::DrawBackground(); return
    for c in choices:
      drawChoiceBackground(c)                               // FUN_00534470
    cls_0x5b98b8::virt_meth_0x435de0(this)                  // parent's compositor
    (*+0x44)(this)                                          // leaf's fill-bg override

  // === slot 92 — Animate ===
  Animate(draw):                                            // virt_meth_0x5355b0
    for c in choices:
      drawChoice(c)                                         // cls_0x534d40::meth_0x534d40
    cls_0x5b93c4::virt_meth_0x436090(this)                  // parent Animate chain

  // === Per-choice chip draw (cls_0x534d40, §6 above) ===
  drawChoice(c):
    if c.offX == 0xffffd8f0: return                         // hidden sentinel
    alpha = (c.fadeCounter * 0xff) / 0xc
    if alpha > 0x7f:
      // LEFT HALF (icon / slot number)
      Display.ParamBlit(buf={drawmode=DM_TRANSPARENT,
                             dx=c.x+c.offX, dy=c.y+c.offY,
                             dwidth=50, dheight=c.height,
                             sx=0,sy=0,swidth=50,sheight=c.height,
                             intensity=0x1f},
                        src=c.leftHalf)
      // RIGHT HALF (baked text bitmap, full cell w-50)
      Display.ParamBlit(buf=BlitEffect_StructInit(...),
                        src=c.rightHalf)                    // UNCONFIRMED-K (src may be c.leftHalf per decomp)
      // per-LINE sub-rect re-blits (§6.wrap)
      for j in 0..c.numLines:                               // mbr_0x5c
        if c.lineActive[j] and lineGate(c.lineAlpha[j], parent=alpha):
          (x0, y0, x1, y1) = c.lineRect[j]                  // mbr_0x80[j*4..]
          w = (x1 - x0) + 1
          h = (y1 - y0) + 1
          Display.ParamBlit(buf={drawmode=DM_TRANSPARENT,
                                 dx=x0+c.x+c.offX, dy=y0+c.y+c.offY,
                                 dwidth=w, dheight=h,
                                 sx=x0, sy=y0, swidth=w, sheight=h,
                                 intensity=0x1f},
                            src=c.rightHalf)
    if not effects_disabled:
      BlitEffect_Iterate(c.x+c.offX, c.y+c.offY, c.width, c.height)

  // === KeyPress ===
  KeyPress(key, down):                                      // virt_meth_0x535610
    if not down: return parent.KeyPress(key, down)
    if key == ' ':                                          // 0x20 — Skip
      if open and not inputLocked:
        for each child: ForceCommandDone via TMapPane.GetInstance
    elif 0x31 <= key < 0x37 and (key - 0x31) < numChoices and not inputLocked:
      activeChoice = key - 0x31                             // mbr_0x1dc
      freshResponse = true                                  // mbr_0x1e4
      return                                                // doesn't chain to parent
    parent.KeyPress(key, down)                              // cls_0x5b93c4::virt_meth_0x4361f0

  // === MouseClick ===
  MouseClick(buttonCode):                                   // virt_meth_0x535760
    if buttonCode == 0x40a and open and not inputLocked:
      // identical to Skip() — force-advance all bound characters
      for each child: ForceCommandDone via TMapPane.GetInstance
    parent.MouseClick(buttonCode)                            // cls_0x5b93c4::virt_meth_0x436340

  HasResponded() -> bool:    return freshResponse           // mbr_0x1e4
  GetResponse() -> string:   return choices[activeChoice].text  if activeChoice >= 0 else null
```

---

## §13 — Port mapping notes

| retail primitive | canonical port method | notes |
|---|---|---|
| Chrome blit (slot 76 / slot 44, presumed `(*+0x5c)`) | `Renderer->DrawBitmapToTarget(chrome, x, y)` | UI_METHOD_MAP §12 |
| End-cap blit | `Renderer->DrawBitmapToTarget(endCap, x, y)` | UI_METHOD_MAP §12 |
| Choice chip left-half blit (DM_TRANSPARENT) | `Renderer->DrawBitmapSubrectToTarget(leftHalf, x, y, 0, 0, 50, h)` with chroma-key→alpha conversion | UI_METHOD_MAP §1 (DM_TRANSPARENT = chroma-keyed) |
| Choice chip right-half blit | `Renderer->DrawBitmapSubrectToTarget(rightHalf, x+50, y, w-50, 0, w-50, h)` | UI_METHOD_MAP §12 |
| Choice text + shadow (bake at AddChoice into rightHalf via word-wrap, see §6.wrap) | `WordWrap(text, choicefont, mbr_0x28-50)` → `DrawTextShadowedToTarget(...)` per line → `RenderToBitmap(mbr_0x4c, w-50, mbr_0x2c)` | UI_METHOD_MAP §12; px UNCONFIRMED-F; line cap = 8; choice cap = 6 |
| Choice fade-alpha tween | `DrawBitmapTintedToTarget(...)` with per-call alpha computed as `(mbr_0x58 * 0xff) / 0xc` | UI_METHOD_MAP §12 |
| Per-LINE sub-rect re-blit (selective damage / per-line typewriter) | `Renderer->DrawBitmapSubrectToTarget(rightHalf, x0+offX, y0+offY, x0, y0, (x1-x0)+1, (y1-y0)+1)` per active line `j` in `[0, mbr_0x5c)` with per-line alpha = `((mbr_0x140[j]*255)>>3) * parent_alpha / 255` | UI_METHOD_MAP §12; per-line typewriter may be unused — see UNCONFIRMED-P |
| Composite cached panel buffer → display | `Renderer->DrawSurface(...)` | UI_METHOD_MAP §12 |
| Font: `choicefont` | `BuildTTFAtlas(<Arimo or fallback>, <px>)` once at Init, cached | UI_METHOD_MAP §12; px UNCONFIRMED-F |
| Show/hide displacement of bottom HUD by 140 px | (orchestrated by TPlayScreen Pulse mode-1, already wired) | `_TPlayScreen_Pulse_47b4d0.cpp:57` |

No new port primitive needed for this panel. The shadow / chroma path is
already supplied by `Renderer->DrawBitmap…Shadowed` (UI_METHOD_MAP §12).

---

## §14 — UNCONFIRMED / open questions

The reconstruction agent **must not start porting until A, B, E, F, and G
are resolved**; the others can land on a second pass.

**UNCONFIRMED-A — `cls_0x570900` identity (yaml fix).** The
`recon/mappings/TDialogPane_cls_0x570900_map.yaml` is wrong. **Impact if
wrong:** future tracers find the wrong class. **Resolve:** retire the
mapping or repoint it to `cls_0x5a5c60`; mark `cls_0x570900` as
TPlayerSlot / network-helper.

**UNCONFIRMED-B — `cls_0x5a5c60` is the retail TDialogPane.** Strongly
implied by Init at `0x534fd0`, the TButtonPane derivation, the TDeathPane
sibling vtable shape, and the "Trouble initializing dialog pane" error
adjacency at `_TPlayScreen_Initialize_47a660.cpp:205-207`. **Impact if
wrong:** would target the wrong class. **Resolve:** `DecompileAddr 0x00534fd0`
and confirm body references `DAT_00666444` (the `Dialog.dat` archive
global). Also run `Window → References to address` in Ghidra on
`DAT_00666444` to find the chrome-fetch consumer.

**UNCONFIRMED-C — MAXCHOICES is 6 (not 4).** `recon/ghidra/cls_0x5a5c60.cpp:401-407`
accepts keys `0x31..0x36`; snapshot caps at 4. **Impact if wrong:** 6 vs 4
changes child-array sizing. **Resolve:** read the AddChoice site (search
for callers of `cls_0x5a5c60::AddChoice` or the script `choice` command
handler) and check the cap against `mbr_0x1d8` and the
`cls_0x41c7f0(this, 4)` capacity ctor at `:68`.

**UNCONFIRMED-D — DialogEndCap placement.** 10×140 cap on a 640-wide chrome.
**Impact if wrong:** a missing or doubled cap, or wrong-edge cap.
**Resolve:** read the slot-76 / slot-44 paint body (UNCONFIRMED-E).
Inspect `/tmp/dat_dump/00_DialogEndCap.png` — already done: it is a
mirror-symmetric trim column (no clear left vs right distinguishing
feature), so visual inspection alone cannot decide. Pure asset-name logic
("EndCap" → trailing) and the wider-than-640 mode argument favor a
**right-edge cap** in pane-local `(pane_w − 10, 0)`.

**UNCONFIRMED-E — Paint method address and body.** Slot 76
`virt_meth_0x5351d0` reads the pending-redraw clip rect from
`DAT_006668d{c,0,4,8}` but body is not extracted. **Impact if wrong:** every
coordinate in §3 / §4 / §8 stays UNCONFIRMED. **Resolve:** `DecompileAddr
0x005351d0`. The companion sites to extract are slot 44 (`virt_meth_0x444fc0_44`,
the leaf-Fill called from slot-80, `cls_0x5a5c60.cpp:301`) and slot 28
(`virt_meth_0x535550`, the Animate-tick that advances the choice ramps —
calls `FUN_00534b60` per child) — `DecompileAddr 0x00444fc0`,
`DecompileAddr 0x00534b60`.

**UNCONFIRMED-F — Text region and choice-cell coordinates / font size /
colors.** NPC line cell, choice cell rect (the per-choice `mbr_0x18..0x2c`
values), font px, default/highlight colors are not extracted.
**STRUCTURAL SHAPE PARTIALLY RESOLVED in wave 3** — see §6.text and
§6.wrap for the per-choice baked-bitmap model + per-line word-wrap
structure + the 8-line hard cap. The **font px**, **colors**, **shadow
on/off**, and **per-choice cell dimensions** (`mbr_0x28`, `mbr_0x2c`)
remain UNCONFIRMED — these are still set in the un-extracted AddChoice
path. **Impact if wrong:** wrong text positions (the #1 reconstruction
bug per FORENSICS_PROTOCOL). **Resolve:** comes out of UNCONFIRMED-E +
reading the AddChoice path (UNCONFIRMED-S). Also extract the
`choicefont` registration at `004a4a73` (`DecompileAddr 0x004a4a73`) to
get the LOGFONTA contents — face name, height, weight, italic flag, and
whether the drawmode flags include `DM_TRANSPARENT | DM_ALIAS | 0x400`
(the shadow bit). The LOGFONTA struct most likely lives in the data
block `DAT_005dad98..DAT_005dadf4` (`recon/ghidra/_data.txt:92390-92500`)
— a Win32 LOGFONTA is 60 bytes, and these data blocks have suspiciously
60-byte-spaced XREFs (`004a41ca, 004a47e6, 004a4945, 004a49c5` at
`DAT_005dad98`; `004a41d4, 004a4a61` at `DAT_005dadc8` — 0x30 bytes apart).
**UNCONFIRMED-F-1**: decode the LOGFONTA structs at `005dad98`,
`005dadc8`, `005dadf4` and identify which one is the choicefont's
HFONT source.

**UNCONFIRMED-G — TOP-vs-BOTTOM anchor signal.** The task brief states
both cases occur; the predicate is not pinned. **Impact if wrong:** the
panel anchors to the wrong edge in some game contexts. **Resolve:** find
the field that drives the anchor — probably a flag on the calling
character or a script-set global. Search the script `say` / `choice`
commands. Specifically: extract slot-23 `virt_meth_0x435f90`
(`DecompileAddr 0x00435f90` — the inherited TButtonPane Show / MouseClick;
see TButtonPane vtable line 22 — that's the Show-side; the `cls_0x5a5c60`
vtable does NOT override slot 23 so the anchor logic, if it lives there,
is the base TButtonPane code). Also check `cls_0x5a5c60` ctor fields
`mbr_0xc / mbr_0x10 / mbr_0x18 / mbr_0x1c / mbr_0x20` — they come from
`DAT_00667c30` and `DAT_0065c5c4` at construction (`:46-60`), which are
likely the screen dimensions; verify if either is the anchor flag.

**UNCONFIRMED-H — Lightning animator on response.** RETIRED. Originally
inferred from `cls_0x570900` destructor, which is the wrong class.
TDialogPane has no LightningAnimator field.

**UNCONFIRMED-I — `cls_0x534d40` is the choice-button entry.** Strongly
implied by the children-iteration patterns in `cls_0x5a5c60`. **Impact if
wrong:** the per-choice draw model in §6 is wrong. **Resolve:** trace the
allocation site (the child array `field_0x18c` is malloc'd somewhere —
search for writes to `field_0x18c` on TDialogPane, OR the AddChoice
prototype).

**UNCONFIRMED-J — Retail string-substitution for `[me]` / `[chr]`.**
**RESOLVED in wave 3** — retail's DialogLine is `FUN_00533dd0` and
references the literal token strings `"me"` (XREF `00533e39` →
`DAT_005e3f3c`) and `"chr"` (XREF `00533e62` → `DAT_005e3f40`)
(`recon/ghidra/_data.txt:113978-113993`). Same semantics as snapshot
`src/dialog.cpp:135-177`. The function body is NOT extracted, but the
token-string XREFs are sufficient evidence to commit to the snapshot
behavior verbatim. The neighbor `FUN_00533f10` (referencing `"\"%s\""`
at `0053400c`) wraps substituted names in quotes — likely the format
helper called by FUN_00533dd0 for character-name expansion.

**UNCONFIRMED-K — Choice right-half blit uses `mbr_0x48` not `mbr_0x4c`.**
The right-half `(*+0x5c)` call at `cls_0x534d40.cpp:94` passes `dVar1 =
this->mbr_0x48` (from `:90`) as the src, NOT `mbr_0x4c`. Also: the
`FUN_00438d80_BlitEffect_StructInit` args show `dwidth=0x32, dheight=0`
for the right half (`:91-92`) — `dheight=0` makes no sense as a draw
height. **Impact if wrong:** the chip would render as only-icon or
double-icon. **Resolve:** disasm `0x534e10..0x534e60` to verify the
actual register at the `(*+0x5c)` call site, and decode the SDrawParam
struct as per FORENSICS_PROTOCOL §"When the decomp is muddy".

**UNCONFIRMED-L — Show/hide tween.** Snapshot is instant. Retail Pulse
animates the bottom HUD displacement but the dialog pane's own
appearance is UNCONFIRMED. **Resolve:** read Show / Hide virtual on
`cls_0x5a5c60` (slot 23 `virt_meth_0x435f90` and the matching hide-
slot — `cls_0x5a5c60`'s slot 52 `virt_meth_0x535120`).

**UNCONFIRMED-M — Dirty-flag model.** `mbr_0x48` and `mbr_0x4c` set to 1
in `meth_0x535120` (the Hide-like) are plausibly the cached-buffer
dirty / full-redraw flag pair. **Resolve:** find all other writers to
`mbr_0x48 / mbr_0x4c` (search instruction range for these stores) and
their relationship to the parent compositor's `mbr_0x150` test.

**UNCONFIRMED-N — Global TDialogPane instance address.** Snapshot is
`TDialogPane DialogPane;` (`src/dialog.cpp:122`). Retail global
UNCONFIRMED. **Resolve:** see UNCONFIRMED-B — the `this` ptr at the
`FUN_00534fd0` call site in TPlayScreen Init is the global address.

**UNCONFIRMED-O — Choice fade ramp advance site.** The `mbr_0x58` ramp
advance is NOT in `cls_0x534d40::meth_0x534d40` (which is draw-only).
The advance is almost certainly in slot 28 `virt_meth_0x535550` which
calls `FUN_00534b60` per child (`cls_0x5a5c60.cpp:312-329`). **Resolve:**
`DecompileAddr 0x00534b60`.

**UNCONFIRMED-P — Per-line typewriter fade (`mbr_0x140[j]`) usage.**
**REVISED in wave 3** — the `mbr_0x140[j]` array is now confirmed
**per-WRAPPED-LINE** (not per-letter; see §6.wrap). It may still be
unused at retail (a leftover from earlier development; the per-line
animation may be dead-code and the rect array exists only for damage-
blit). **Resolve:** check for writers to `mbr_0x140[j]` in the AddChoice
path (UNCONFIRMED-S) and in slot 28 `FUN_00534b60` (UNCONFIRMED-O). If
neither writes non-zero animated values, the per-line typewriter is
dead-code and the port can use a uniform reveal driven only by
`mbr_0x58`.

**UNCONFIRMED-R — Retail `Character::ScriptJump` / `TActionBlock::Is`
addresses.** Wave 3 finding from `cls_0x535a10::meth_0x5360f0:264-271`
(the ESC handler): `FUN_00471290(this->mbr_0x194, "Finish")` then
`FUN_004dab80_TActionBlock_Is(action, "say")`. **Resolve:** confirm
`0x00471290` is `TCharacter::ScriptJump` by comparing its body to the
snapshot `character->ScriptJump("Finish")` at `src/dialog.cpp:297` and
its peer behavior in TCharacter. Likewise confirm `0x004dab80` is
`TActionBlock::Is` (the snapshot signal is `character->IsTalking()` at
`src/dialog.cpp:405` — a sibling predicate). **Impact:** correct
identification of the ESC-close path; the port already needs these
to land the dialog ESC handling.

**UNCONFIRMED-S — AddChoice site, font-bake site, and supporting helpers
in the 0x533dd0..0x534b60 range.** The candidates by data-XREF proximity
to known TDialogPane code are:
- `FUN_00533dd0` — DialogLine `[me]`/`[chr]` substitution (now
  resolved per UNCONFIRMED-J).
- `FUN_00533f10` — `"\"%s\""` quote-wrap helper.
- `FUN_00534470` — slot-80 per-choice DrawBackground helper (called
  per child in `cls_0x5a5c60::virt_meth_0x535500:296`). XREFs
  `DAT_006668d0` at `:005345e2`, `DAT_006680c8` at `:005344f5`, plus
  string `"Ring"` at `:00534701`.
- `FUN_00534b60` — slot-28 per-choice Animate-tick called per child
  in `cls_0x5a5c60::virt_meth_0x535550:322` — almost certainly the
  fade-counter advance (drives `mbr_0x58` and possibly `mbr_0x140[j]`).
- `FUN_005343e0` — appears in Hide / ResetResponses
  (`cls_0x5a5c60::virt_meth_0x535060:218`, `cls_0x535a10::meth_0x535d80:100`)
  — likely the per-choice destructor / glyph-cache free.
- `FUN_004830f0` — paired with `FUN_005343e0` in the same destruct
  context — likely the operator-delete free.
- The **AddChoice site itself** is NOT in the data XREFs we have —
  must be in an unanalyzed function in the same range. **Resolve:**
  run `Window → References to address` on `&this->field_0x18c` writes
  on cls_0x5a5c60 to find the AddChoice site (which inserts into the
  TPointerArray); OR find the script `choice` command handler (the
  snapshot's `COMMAND(CmdChoice)` at `src/dialog.cpp:410-426`) and
  follow its call into TDialogPane.

**UNCONFIRMED-T — Six-choice + per-text overflow rules.** §6.overflow
documents the two regimes but their precise enforcement is UNCONFIRMED:
- **T-1 (vertical):** when 6 cells of varying height stack vertically
  via running y-cursor, does the engine silently drop a choice that
  would exceed `pane_h - bottom_pad`? Or does it clip / overlap?
  **Resolve:** add a deliberately tall 6th choice in the test
  harness (once reconstruction lands) and observe.
- **T-2 (horizontal):** when a single choice's wrapped text produces
  > 8 lines, does the engine truncate at 8 lines, ellipse, or just
  drop the 9th line silently?
- **T-3 (font.height):** the `mbr_0x2c` value per-choice is set by
  AddChoice to `N_lines * line_height + pad`. The `line_height` value
  determines whether 6 short choices fit in 140 px. **Resolve:**
  measure the GDI HFONT height via the LOGFONTA struct
  (UNCONFIRMED-F-1).

**UNCONFIRMED-V — Choice prefix ("1.", "2.", ...).** Snapshot does NOT
prefix `(src/dialog.cpp:260`). Retail's left-half bitmap `mbr_0x48`
(50 px wide) **may** contain the "N." badge as a baked icon — in which
case the right-half text has no prefix. Alternatively the right-half
text starts with `"%d. %s"`. **Resolve:** read the AddChoice body
(UNCONFIRMED-S) to see whether `mbr_0x48` is set from a slot-number
sprite or zero-filled, and whether the text bake includes a numeric
prefix.

**UNCONFIRMED-Q — `g_DAT_006680c8` gate semantic.** The whole chip-draw
body is gated on `DAT_006680c8 != 0` (`cls_0x534d40.cpp:65`). This is
also the **Classic-vs-hi-res** gate used by `FUN_0054a5d0`
(TPlyrStatusBar §6 / UI_METHOD_MAP §11) — Classic = `DAT_006680c8 == 0`.
**Impact if wrong:** at Classic resolution the chip draws NOTHING; at hi-res
it uses the chip path documented here. Resolve by confirming which mode
the retail game ships in (likely both — Classic uses a different draw
helper, NOT extracted). For the modern port we render at hi-res
unconditionally, so only the hi-res path matters.

---

## §15 — Reviewer / agent handoff

This spec is **not yet implementable** (the §14 items A, B, E, F, G must
be resolved first), but the structural model is now pinned, with wave-3
additions to the **text layout** and **6-choice overflow** model:

- TDialogPane is `cls_0x5a5c60`, a TButtonPane derivative with a child
  array of `cls_0x534d40` choice widgets.
- The chrome is `Dialog` (640×140) + `DialogEndCap` (10×140).
- Each choice is a two-half chip (50-px-wide icon/slot + (w−50)-px text)
  with a 0..12 hover-fade ramp gated at alpha = 128.
- **Choice text is BAKED ONCE at AddChoice into the per-choice `mbr_0x4c`
  bitmap** (§6.text). Per-frame draw is just transparent-blit of the
  baked bitmap; the GDI `meth_0x4be2b0` text composite does NOT run
  per frame.
- **Choice text word-wraps to up to 8 lines** (§6.wrap); each line gets
  a cell-local rect at `mbr_0x80[j*4..]` + active flag at `mbr_0x100[j]`
  + per-line alpha at `mbr_0x140[j]`. Hard cap is 8 lines per choice
  (rect-array geometry).
- **Choices stack vertically with per-choice height `mbr_0x2c`** that
  grows with wrap; the 6-choice cap is enforced in KeyPress (key '7'
  ignored). Vertical overflow handling (e.g. silent drop of an
  excess choice) is UNCONFIRMED-T-1.
- **DialogLine `[me]`/`[chr]` substitution** is `FUN_00533dd0`
  (§6.dialogline_retail).
- **ESC handler** is `meth_0x5360f0` (`cls_0x535a10.cpp:184-278`) —
  Skip + Close + `ScriptJump("Finish")` (§10).
- Draw splits across slot 76 (top-level clip), slot 80 (DrawBackground),
  slot 92 (Animate per-frame chips).
- Input: keys '1'..'6' commit choice, space + button `0x40a` force-advance
  the dialog (Skip), all gated on `field_0x190 != 0`.

**Wave 3 closes:**
- UNCONFIRMED-J — RESOLVED (`FUN_00533dd0` is DialogLine).
- UNCONFIRMED-F — **partially resolved** for STRUCTURAL shape (per-choice
  bake destination + per-line wrap rects + 8-line cap). Font px / colors
  / shadow on/off remain UNCONFIRMED-F (need `0x004a4a73` body + LOGFONTA
  decode).
- UNCONFIRMED-P — REVISED to be per-LINE (not per-letter); may still be
  dead-code (typewriter unused).
- New: UNCONFIRMED-R (ScriptJump/IsTalking addrs), UNCONFIRMED-S
  (AddChoice site cluster), UNCONFIRMED-T (overflow rules),
  UNCONFIRMED-V (choice numeric prefix), UNCONFIRMED-F-1 (LOGFONTA
  data block location).

A reconstruction agent invoked on this spec must:

1. **Stop** before any engine code is written.
2. **Resolve UNCONFIRMED-A and UNCONFIRMED-B** (Ghidra extracts of
   `0x00534fd0` body details and the mapping yaml fix).
3. **Resolve UNCONFIRMED-E** (extract `0x005351d0`, `0x00444fc0`,
   `0x00534b60` paint+animate-tick bodies).
4. **Resolve UNCONFIRMED-F + F-1** (extract AddChoice / choicefont
   registration `0x004a4a73` body + decode the LOGFONTA struct at
   `DAT_005dad98`/`DAT_005dadc8`/`DAT_005dadf4` to get face name + px).
5. **Resolve UNCONFIRMED-G** (extract `0x00435f90` Show, find anchor
   predicate).
6. **Resolve UNCONFIRMED-S** (extract the AddChoice + the
   `0x533dd0..0x534b60` helper cluster to confirm per-choice
   coordinates, wrap behavior, and the slot-number prefix question).
7. **Re-read this spec**, fill the §3 / §4 / §8 coordinates from the new
   recon, update §14, and only then begin reconstruction.

The forensics agent that wrote this spec is the right escalation: re-run
it after the Ghidra extracts land in `recon/discovered/`.
