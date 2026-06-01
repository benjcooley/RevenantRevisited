# SpellbookSidebarPane — Reconstruction SPEC

Panel: the player's **Spell Book** sidebar content pane (UI label "**Book**",
upper-region position, mode value 2 in `DAT_0065d1b8`). Shows the player's
known-spells list as a scrollable parchment scroll: one tall row per spell with
the spell-icon, spell name, talisman-recipe glyph row, a description/skill block,
and Skill / Mana stat lines. Class `cls_0x5a5ae8`.

Status: **forensics-complete** (one UNCONFIRMED group, §14).

---

## §0 — Sources & status

- **Class:** `cls_0x5a5ae8`, vtable `&PTR_FUN_005a5ae8 @ 0x005a5ae8`; single
  global instance `DAT_0065a9d8 @ 0x0065a9d8`. Class identity has **no clean
  src/ predecessor** (retail-era class; B.r9 rejected TScrollPane / TBookPane /
  TSpellPane). Role label `SpellbookSidebarPane` describes role, not a claimed
  retail name.
- **Paint method (the Draw):** **vtable slot 21 = `FUN_005452d0` @ 0x005452d0**
  (extracted this wave → `recon/discovered/cls_0x5a5ae8_SpellbookSidebarPane_Draw_5452d0.cpp`).
  This is the full content paint; the simpler list functions
  (`meth_0x544fb0` / `meth_0x546830`) are the **layout/measure rebuild**, not the
  on-screen draw.
- **Recon files read:**
  - `recon/discovered/cls_0x5a5ae8_SpellbookSidebarPane_Initialize_5449e0.cpp` (slot 0)
  - `recon/discovered/cls_0x5a5ae8_ctor_488620.cpp`
  - `recon/discovered/cls_0x5a5ae8_vtable.txt` (46 slots)
  - `recon/discovered/cls_0x5a5ae8_SpellbookSidebarPane_Draw_5452d0.cpp` (slot 21, **the Draw**, this wave)
  - `recon/discovered/cls_0x5a5ae8_SpellbookSidebarPane_slot25_545f10.cpp` (slot 25, **MouseClick/hit-test**, this wave)
  - `recon/ghidra/cls_0x544fb0.cpp` (`meth_0x544fb0` find-by-name + `meth_0x546830` list-rebuild — operate on this class's fields; OOAnalyzer split them)
  - `recon/ghidra/cls_0x5a4494.cpp:497-701` (slots 1/19/20/26 bodies, mis-attributed to TPane)
  - `recon/ghidra/cls_0x53ed70.cpp` (spell-class registry name lookup)
  - `recon/ghidra/cls_0x45f7c0_likely_TObjectClass.cpp:2970-3080` (spell-name getters `meth_0x49d6d0/49d780/49d800`)
  - `recon/ghidra/cls_0x5a3c68.cpp:80` (`FUN_0042c2d0` — TButton ctor used for arrow buttons)
- **Prior briefs:** `docs/ui/briefs/B_r9_parchment_viewer.md`,
  `docs/ui/briefs/B_r11_sidebar_tab_cascade.md`,
  `recon/discovered/port_status/SpellbookSidebarPane.md`.
- **Assets measured:** `tools/ui/dump_dat.py` on `spellscroll.dat`, `sidepane.dat`,
  `SpellIcons.dat` (§2).
- **Reference image (regression only, no coords derived from it):** `docs/ui/spell_list_ui.jpg`.

**Snapshot note (supplementary — superseded by retail).** Pre-release
`src/spellpane.{cpp,h}` is the **talisman COMPOSER** (`TSpellPane`: buttons
`spell/book/add/back/down/up/min`, iterates `TalismanClass.NumTypes()`), and
`src/spell.{cpp,h}` is the spell-data model. B.r9 rejected both as the source of
this pane (different sprites, different content source — the retail pane iterates
the player's known-spell list `DAT_00667fcc+0x2ec/+0x2fc`). Reusable from the
snapshot: only field-name *intent* (a known-spells list, per-spell mana/skill).
**Layout/coords are superseded by retail (§2–§8).** Do not pull snapshot coords in.

---

## §1 — Overview

The pane renders into an off-screen parchment surface, then blits that surface
plus two scroll-rod chrome caps onto the pane. Content is the player's
known-spell list: for each spell a tall (≈91px) row is laid out vertically; the
visible window scrolls with up/down arrow buttons. Each row shows: a spell-circle
**icon** (left), the **spell name** (top, cream text), the **talisman-recipe
glyph row** (small 16×16 glyphs), a wrapped **description / skill-name block**,
and two stat lines **"Skill: N"** and **"Mana: N"** (the spell's skill
requirement + mana cost).

- **Instances:** ONE (`DAT_0065a9d8`). Not mirrored. No player/target split.
- **Visibility:** shown when upper-region mode `DAT_0065d1b8 == 2` (Book) and the
  side content is presented; toggled via `TPlayScreen::DispatchCommand`
  (`FUN_0047cf40`) cases 7/0xa and the Book tab button (B.r11). The Draw early-outs
  if there is no player (`DAT_00667fcc == 0`) or zero known spells
  (`DAT_00667fcc[0xbb] == 0`, Draw `:0054…` guard).

---

## §2 — Asset roster

All chrome + talisman glyphs are in **`spellscroll.dat`** (the dedicated
spellbook archive; `sidepane.dat` carries lower-res duplicates of the same names
— the spellbook resolves the spellscroll set). Spell circle icons are in
**`SpellIcons.dat`** (resolved by spell name). Dump:
`python3 tools/ui/dump_dat.py <data>/resources_unzipped/spellscroll.dat`.

| asset | archive | entry | WxH (measured) | flags | role | cite |
|---|---|---|---|---|---|---|
| `Scroll` | spellscroll.dat | `Scroll` | **188×306** | 0x2 (15-bit) | full pane chrome frame (parchment + ornate top/bottom scroll-rods); = pane size | dump `[8]`; Init `s_Scroll_005e5280:103` (field `[100]`/+0x190) |
| `ScrollPaper` | spellscroll.dat | `ScrollPaper` | **148×80** | 0x2 | parchment tile blitted into the content surface, vertically tiled at **80px** pitch | dump `[9]`; Init `s_ScrollPaper_005e5274:76` (field `[99]`/+0x18c) |
| `ScrlTop` | spellscroll.dat | `ScrlTop` | **32×12** | 0x2 | top scroll-rod cap composited over content top | dump `[6]`; Init `:29` (field `[0x61]`/+0x184) |
| `ScrlBottom` | spellscroll.dat | `ScrlBottom` | **40×16** | 0x2 | bottom scroll-rod cap | dump `[7]`; Init `:45` (field `[0x62]`/+0x188) |
| `ScrlArwUU/UD/UG` | spellscroll.dat | `ScrlArwUU`,`ScrlArwUD`,`ScrlArwUG` | **24×24** | 0x104 (15-bit+alpha) | scroll-up button (up/down/glow states) | dump `[0],[1],[4]`; Init `:120-122` |
| `ScrlArwDU/DD/DG` | spellscroll.dat | `ScrlArwDU`,`ScrlArwDD`,`ScrlArwDG` | **24×24** | 0x104 | scroll-down button | dump `[2],[3],[5]`; Init `:139-141` |
| talisman glyphs `SSun,SLife,SOcean,SLaw,SSoul,SStars,SDeath,SChaos,SSky,SEarth,SWard,SMoon` | spellscroll.dat | `S<Talisman>` | **20×20** | 0x104 | per-spell recipe glyph row | dump `[10..21]`; Draw `(&PTR_DAT_005e4fb4)[i]` name-table @ 0x5e4fb4 |
| spell circle icon | SpellIcons.dat | `<Spell Name>` | **40×40** | 0x2 | the spell's circle icon, keyed by spell name | dump SpellIcons; Draw `FUN_0046d710`/`FUN_0046d6b0` lookup `:…` |

Notes: the talisman-glyph name table at `PTR_DAT_005e4fb4` (0x5e4fb4) holds the
ordered `S*` names (`SLife@0x5e5064`, `SStars@0x5e5084`, …; `_data.txt:115872+`).
`SpellIcons.dat` entries are 40×40 named exactly by spell name (e.g. `Fire Flash`,
`Heal`); the Draw also has a 32×32 duplicate set in `sidepane.dat[108..128]`
(UNCONFIRMED which set the spellbook samples — §14).

### ASCII — Scroll chrome (188×306)
```
0                                  188
┌────────────────────────────────┐  0
│   ╔══════ornate scroll-rod═════╗│   ← top rod (jewel center)
│   ║ (ScrlTop cap drawn here)   ║│
│   ┌────────────────────────┐    │
│   │  PARCHMENT viewport      │   │  ← content surface 148×229 blitted in
│   │  (ScrollPaper tiled)     │   │
│   │  spell rows scroll here  │   │
│   └────────────────────────┘    │
│   ╔══════ornate scroll-rod═════╗│   ← bottom rod (ScrlBottom cap)
└────────────────────────────────┘ 306
        ▲ScrlArwUU @(169,150)  ▲ScrlArwDU @(169,174)  (24×24 each)
```

---

## §3 — Coordinate frames & surfaces

- **Pane size:** width `0xbc = 188`, height `0x132 = 306` (ctor: `param_1[3]=[7]=0xbc`
  byte 0xc/0x1c = width; `param_1[4]=[8]=0x132` byte 0x10/0x20 = height —
  `cls_0x5a5ae8_ctor_488620.cpp:21-28`). Live width field is `this+0xc`.
- **Pane on-screen position:** owned by the **TSidePane** parent container (the
  upper sidebar region), NOT set in this ctor. The pane occupies the upper-right
  sidebar slot. **UNCONFIRMED-A** (§14): exact screen origin of the side region.
  Everything below is **pane-local**, which is the correct authoring frame (the
  port composes the chip then the side container places it).
- **No chip mirroring.** Single instance, single frame chain.

### Scratch surfaces (allocated in Initialize)
| surface | field | size | role | cite |
|---|---|---|---|---|
| content/parchment | `this+0x180` (Draw `param_1[0x60]`) | **148×229** (`0x94×0xe5`) | rows + paper tile composited here, then blitted to pane | Init `FUN_004a5740(0x94,0xe5,0x400,…):65` |
| text scratch | `(this+0x180)+0x5c` | **132×200** (`0x84×200`) | font composite scratch owned by content surface | Init `FUN_004a5740(0x84,200,…):96` |
| ScrlTop sprite buf | `this+0x184` (`param_1[0x61]`) | 32×12 | top cap | Init `:29-38` |
| ScrlBottom sprite buf | `this+0x188` (`param_1[0x62]`) | 40×16 | bottom cap | Init `:39-54` |
| ScrollPaper sprite | `this+0x18c` (`param_1[0x63]`/`[99]`) | 148×80 | paper tile source | Init `:70-85` |
| Scroll chrome sprite | `this+0x190` (`param_1[0x64]`/`[100]`) | 188×306 | pane frame | Init `:103-105` |

### Frame table (mandatory)
| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | — (root) | — | framebuffer TL `(0,0)` | final pixels | — |
| **side-region** | screen | TR of HUD | UNCONFIRMED-A | the upper sidebar slot | TSidePane (B.r11) |
| **pane** | side-region | TL | side-region TL; size **188×306** | the whole spellbook scroll; arrow buttons | ctor `:21-28` |
| **content-surface** | pane | TL | pane-local **(20, 38)** ⇒ blitted from `(this+0x180)[5],[6]` (the surface's stored dst x,y) | parchment tile + all spell rows + per-row text/icons | Draw `(*+0x44)(piVar14[5],piVar14[6],0x94)` `:…`; slot26 `(*+0x44)(piVar3[5],piVar3[6],0x94,0xe5)` |
| **content-scroll** | content-surface | TL, scrolled | content-surface origin minus scroll `this+0x17c` | the running per-row Y stack (`pcVar9`) | Draw row loop |
| **source-rect** | asset TL | — | `(sx,sy,sw,sh)` out of an asset | sprite sub-rects (all full-frame here) | §2 |

**Composition to final pane-local:**
`pane_x = content_origin_x + x_in_content` and
`pane_y = content_origin_y + (row_top − scrollY) + y_in_row`.
The content-surface stored dst `(this+0x180)[5],[6]` = pane-local **(20, 38)**
(= `0x14, 0x26`) — **UNCONFIRMED-B** on the exact literal (read from the surface
object's dst fields at runtime, not a code immediate; the parchment viewport is
inset inside the 188-wide scroll-rod frame, ~20px from each side, ~38px below the
top rod). All §4/§8 row coords are given **content-surface-local** (the frame the
Draw actually authors in), with this single offset to compose to pane.

**Anchor verification.** Pane is TL-anchored in its side region (constant origin,
no `parent_w − k` expression in ctor). Arrow buttons are TL-anchored in the pane
(bare-constant x,y in Init). Content surface is TL-anchored in the pane. No
right-anchored / mirrored elements exist in this pane.

---

## §4 — Static element layout

All **(x,y)** are **content-surface-local** unless tagged `pane`. Compose to
pane by adding content origin (20, 38) (UNCONFIRMED-B). `iVar10 = row_top −
scrollY` is the current row's top within the content frame; the per-element y
literals below are **relative to that row top** (so absolute content-y =
`row_top − scrollY + literal`).

| element | space | x | y (rel row top) | w×h | source | cite |
|---|---|---|---|---|---|---|
| parchment tile (background) | content | 0 | tiled every 80px, start `−(scrollY mod 80)` | 148×80 per tile, repeated to 0x140=320 | `ScrollPaper` | Draw paper loop `for(iVar11=−(p[0x5f]%0x50); iVar11<0x140; iVar11+=0x50)` |
| spell circle icon | content | `iStack_2d0` (≈ **12 + 0xc** depth-bumped) | `−iVar10` (row top) | 40×40 (`SpellIcons`) | spell-name lookup | Draw `FUN_004bd680(iStack_2d0,uStack_2b4,icon,0x100/0x2000)` |
| spell name text | content | **10** (`0xa`) | **0x2c − iVar10** (row top + 44) | cell 132×… (`0x84`) | `meth_0x49d800` name | Draw `FUN_004be2b0(10,0x2c−iVar10,0x84,…,0x401)` `:0054…` |
| description / skill block | content | **0x36 = 54** | `−iVar10` and `−iVar10+0x10` (two lines) | cell 0x55×0x56 (85×86) | item/skill desc | Draw `FUN_004be2b0(0x36,…,0x55,0x56,…,0x402)` |
| talisman glyph row | content | start **0xc = 12**, step **0x10 = 16** per glyph | row top + **(0x3d − iVar10) = +61** | 20×20 each (`S*`) | recipe glyphs | Draw glyph loop `FUN_004bd680(uStack_2dc,pcVar12+(0x3d−iVar10),glyph,0x2000)` |
| "Skill: N" stat line | content | **10** (`0xa`) | row top + **0x2c − iVar10** (+44) | cell 0x84 (132) | SPANESKILLS + value | Draw `FUN_004be2b0(10,…+0x2c−iVar10,0x84,…,0x401)` |
| "Mana: N" stat line | content | **0x5a = 90** | row top + **0x2c − iVar10** (+44) | cell 0x84 | SPANEMANA + value | Draw `FUN_004be2b0(0x5a,…+0x2c−iVar10,0x84,…,0x401)` |
| scroll-up button | **pane** | **169** (`0xa9`) | **150** (`0x96`) | 24×24 | ScrlArwUU/UD/UG | Init `FUN_0042c2d0(…,0xa9,0x96,0x18,0x18,…)` `:128` |
| scroll-down button | **pane** | **169** (`0xa9`) | **174** (`0xae`) | 24×24 | ScrlArwDU/DD/DG | Init `:147` |
| ScrlTop cap | content | from surface | top of content | 32×12 | ScrlTop | Draw final `(*+0x5c)(…,param_1[0x61],…)` |
| ScrlBottom cap | content | from surface | bottom of content | 40×16 | ScrlBottom | Draw final `(*+0x5c)(…,param_1[0x62],…)` |
| Scroll frame chrome | pane | 0 | 0 | 188×306 | Scroll | Draw composite (pane DrawBackground / `param_1[0x64]`) |

**Row pitch (per-spell height):** stored in the line-height table `this+0x1a8`
(`param_1[0x6a]`), one int per spell, computed by the rebuild as
`lines × font_lineHeight + 0x5b` (`cls_0x544fb0.cpp:255` `iVar6 = iVar6 *
*(int*)(local_64+0x50) + 0x5b`). The `0x5b = 91` is the fixed per-row block
(icon 40 + name + glyph row + stat line); `font_lineHeight` is the current font's
`+0x50` field; `lines` = wrapped name line count. Rows stack downward; `pcVar9`
accumulates the running bottom. The visible content window is 229px tall (≈ 2½
rows).

---

## §5 — Draw order / composition (slot 21 `FUN_005452d0`)

Into the **content surface** (`param_1[0x60]`), top to bottom:
1. **Clear/seed:** stamp ScrollPaper once via `FUN_004bd680` then `Box`-fill the
   148×229 region (`(*+0x64)` with the surface width arg).
2. **Tile parchment:** loop `iVar11 = −(scrollY mod 0x50)` … `< 0x140` step `0x50`,
   each iteration `FUN_00438d80(&dp,0,iVar11,…)` then `(*+0x5c)(&dp, ScrollPaper, 0)`
   — vertically tiled paper, scroll-aligned.
3. **Per-spell row loop** over `DAT_00667fcc[0xbb]` spells (names `[0xbf]`):
   for each row whose Y band intersects the window, draw in order:
   a. spell **name** text (font 0x401, cream),
   b. **description/skill** block (font 0x402),
   c. **Skill / Mana** stat lines (font 0x401, green / cyan),
   d. **spell circle icon** (`FUN_004bd680`, alpha),
   e. **talisman glyph row** (`FUN_004bd680`, alpha, 16px steps).
   Advance `pcVar9` by the row's `[0x6a]` height.
4. **Resolve content surface** (`(*+0x44)` unlock region) and **composite three
   surfaces to the pane** via `(*+0x5c)`: content (`[0x60]`), ScrlTop (`[0x61]`),
   ScrlBottom (`[0x62]`).
5. If drawing straight to the display (`param_2 == PTR_DAT_005d79e0`), run the
   blit-effect flush `FUN_004aacb0(pane.x,y,w,h,6)`; else `FUN_00435cb0` (normal
   pane finalize).

The Scroll-frame chrome (`[0x64]`, 188×306) is drawn by the pane background pass
(it surrounds the inset content viewport); the content composite lands inside it.

---

## §6 — Algorithms

### 6a. Per-row vertical layout (rebuild — `meth_0x546830`, the measure pass)
```
free old line-height table (this+0x1a8); count=0; total=0
font = HDC[DAT_0065abc4]  (current font); lineH = font[+0x50]
for spell in player.spells[0 .. player[+0x2ec]):
    name = spell name (player[+0x2fc][i])
    if spell registered (cls_0x53ed70 lookup ok):
        lines = CompositeBuffer(contentSurf, 0,0, w=0x84, h=1, canonName)  # measures wrapped lines
        rowH  = lines * lineH + 0x5b          # 0x5b = 91 fixed block
        append rowH to table this+0x1a8 (grows by 0x40 when full)
        total += rowH
    else: drop the spell from the list (compact)
this+0x1a0 = total   # full scroll content height
```
Cite: `cls_0x544fb0.cpp:245-341`. `0x84=132`, `0x5b=91`, table grows `0x40=64`
entries at a time (`mbr_0x1a6` capacity, `mbr_0x1a4` count).

### 6b. Scroll step (slot 19 `FUN_00546620`)
```
if scroll-armed flags (this+0x198 && this+0x19c):
    d = this+0x1d0                       # pending scroll delta
    if |d| < 0x28: snap d→0 (this+0x1d0=0)
    elif d<0: this+0x1d0+=0x28; step=−0x28
    else:     this+0x1d0−=0x28; step=+0x28
    this+0x17c = clamp(this+0x17c + step, 0, this+0x1a0 − 0xe5)   # 0xe5=229 window
    if scroll moved: invalidate region, mark dirty (UNK_0065a9dc._76=1), redraw
```
Cite: `cls_0x5a4494.cpp:618-701`. **Scroll step = 0x28 = 40 px/tick**, dead-zone
`< 0x28`, max scroll = `contentHeight − 229`.

### 6c. Per-row paint window test (Draw `FUN_005452d0`)
```
row_top = 0
for i in spells:
    rowH = table[i]
    bottom = row_top + rowH
    if scrollY < bottom and (top-of-window) < scrollY + 0xe5:   # row intersects 229px window
        iVar10 = scrollY − (running)         # row offset within window
        ... draw name/desc/stats/icon/glyphs (§4 y = literal − iVar10) ...
    row_top += rowH
```
Cite: Draw row loop. The `− iVar10` shifts each element by the scroll so rows
slide as a unit.

---

## §7 — Effects & shadows

- **Parchment tile blit** uses `FUN_00438d80(&dp, 0, iVar11, …)` — here the
  `(dx,dy)=(0,iVar11)` is the **tile placement**, not a drop shadow (the tile
  loop reuses the descriptor-init helper to set the dest-y; UI_METHOD_MAP §8.1).
  No visible drop shadow on the tile.
- **Text 3-pass shadow:** every text uses font ids with **bit 0x400 set**
  (`0x401` for name/stats, `0x402` for description). Per UI_METHOD_MAP §5,
  `FUN_004be2b0` draws 3 black passes (base, +1x, +1y) then the colored pass — a
  1px black drop shadow toward right+bottom on all glyphs.
- **Icon / glyph blits** use drawmode `0x2000` (`DM_ALPHA`) — alpha-blended
  (these sprites are flags `0x104` = 15-bit + alpha; do NOT apply the magenta key).
  Some spell-icon blits use `0x100` (`DM_TRANSPARENT`) for the keyed-icon case.
- **Chroma key:** the chrome (`Scroll/ScrollPaper/ScrlTop/ScrlBottom`) is flags
  `0x2` with `kc=0x0` (no magenta) — they are opaque rectangles; draw opaque, no key.

---

## §8 — Text rendering

All cells **content-surface-local** (compose to pane via §3 origin). `Y` shown as
the literal vs row-top (absolute content-y = `row_top − scrollY + literal`).
Font line height = current-font `+0x50`. v-align = **top** (GDI DT_TOP; no
DT_VCENTER) — multi-line wraps stack DOWN from cell top.

| string | cell (x,y rel-row, w,h) | font | h-align | v-align | shadow | format | source value | color (RGB) | cite |
|---|---|---|---|---|---|---|---|---|---|
| spell name | (10, 0x2c, 0x84=132, —) | `0x401` | left (`&1` default) | top | yes (0x400) | (raw) | `meth_0x49d800` canonical spell name | **RGB(0xff,0xe7,0xf2)** cream/pink (PackRGB(0xf2,0xe7,0xff)→R,G,B) | Draw name `FUN_00429950(0xf2,0xe7,0xff)`; `FUN_004be2b0(10,0x2c−iVar10,0x84,…,0x401)` |
| description / skill (line 1) | (0x36=54, 0, 0x55=85, 0x56=86) | `0x402` | left | top | yes | (raw, truncated w/ "..") | item/skill description | `DAT_00667c2c` (theme default) | Draw `FUN_004be2b0(0x36,−iVar10,0x55,0x56,…,0,DAT_00667c2c,…,0x402)` |
| description / skill (line 2) | (0x36=54, 0x10=16, 0x55, 0x56) | `0x402` | left | top | yes | (raw cont.) | wrapped continuation | `DAT_00667c2c` | Draw second `FUN_004be2b0(0x36,−iVar10+0x10,…)` |
| "Skill: N" | (10, 0x2c, 0x84, —) | `0x401` | left | top | yes | `"%s: %d"` (`s_%s:_%d_005e5310`) | label `SPANESKILLS` (via `meth_0x49d800`) + skill value | **RGB(0xf4,0xf4,0x05)** yellow-green (PackRGB(5,0xf4,0xf4)) | Draw `FUN_0058b100(buf,"%s: %d",label); FUN_00429950(5,0xf4,0xf4); FUN_004be2b0(10,…,0x84,…,0x401)` |
| "Mana: N" | (0x5a=90, 0x2c, 0x84, —) | `0x401` | left | top | yes | `"%s: %d"` (`s_%s:_%d_005e5324`) | label `SPANEMANA` + **mana cost** | **RGB(0x25,0xff,0xe9)** cyan (PackRGB(0xe9,0xff,0x25)) | Draw `…"%s: %d"; FUN_00429950(0xe9,0xff,0x25); FUN_004be2b0(0x5a,…,0x84,…,0x401)` |

**PackRGB color decode (UI_METHOD_MAP §6):** `FUN_00429950(dst, B, G, R)` stores
`dst[0]=R,[1]=G,[2]=B`. So call args `(B,G,R)` → final **RGB(R,G,B)**:
- name `(0xf2,0xe7,0xff)` → **RGB(0xff,0xe7,0xf2)** cream;
- skill `(5,0xf4,0xf4)` → **RGB(0xf4,0xf4,5)** yellow-green;
- mana `(0xe9,0xff,0x25)` → **RGB(0x25,0xff,0xe9)** cyan.

**Visual-anchor check.** Name text x=10 sits to the RIGHT of the 40×40 icon
(icon at x≈12 but the name baseline y=+44 is below the icon top — name is on the
parchment beside/under the circle, left-aligned, not centered → no centering claim
to verify). Skill (x=10) and Mana (x=90) share the same baseline (+44) forming one
two-column stat line; both left-aligned. Description block (x=54) is the wide
right column. These are LEFT-aligned cells — no centered-on-element case here.

---

## §9 — Animation & dynamic behavior

```
ramp scroll (this+0x17c, current scroll Y):
  range   0 .. (this+0x1a0 − 0xe5)        # content height − 229px window
  step    ±0x28 = 40 px per sim tick toward target; snaps to 0 when |delta|<0x28
  drive   this+0x1d0 (pending delta) set by arrow-button click / select-into-view
  on-end  hold; clears arm flag this+0x198
  cite    cls_0x5a4494.cpp:641-696 (slot 19)
```
- **Select-into-view:** the find-by-name path (`meth_0x544fb0`) sets
  `this+0x1d0 = rowTop − scrollY` so a programmatically-selected spell scrolls
  into view (`cls_0x544fb0.cpp:108,148`).
- **Dirty model:** redraw marked by `UNK_0065a9dc._76_4_ = 1` (the global
  instance's dirty pad at `0x65a9dc`) then `(*+0x90)()` (request repaint). The
  list is rebuilt (`meth_0x546830`) when the player's spell set changes (called
  from object-class code when a spell is gained — B.r9 §3). Port: use a monotonic
  version counter, not a bool.

---

## §10 — Input & dispatch

**MouseClick / hit-test = slot 25 `FUN_00545f10`** (this wave).
- Gated on `DAT_0065d0d0 != 0` (content presented). Stores last cursor at
  `this+0x1ac/0x1b0`; forwards to `FUN_00436530` (button pump → arrow buttons).
- **Spell-row hit (`param_2 == 1`, left-click):** iterate rows via the
  `[0x1a8]` height table; a row is hit when the click X is in the **icon column
  `0xc < (x − 0x12) < 0x34`** (i.e. pane-x ≈ 30..70) AND click Y is within the
  row's Y band `[rowTop − scrollY, +0x28]`. On hit set selection
  `this+0x1c8 = rowIndex`, invalidate, mark dirty `_DAT_0065aa28=1`, repaint.
  Cite `:24-65`.
- **Selection visual:** slot 26 (`FUN_00546390`) consumes `this+0x1c8`: looks up
  the selected spell name (`player[+0x2fc][sel]`), validates via `cls_0x53ed70`,
  sets a **20×20 (`0x14×0x14`) drag/cast cursor** (`FUN_0043a240(cursorBmp,0x14,0x14)`)
  to the spell, then clears the selection (`this+0x1c8 = 0xffffffff`) and
  repaints. So clicking a spell **picks it up for casting/quick-assign**, it is
  not a persistent highlight. Cite `cls_0x5a4494.cpp:540-614`.

| control | hit rect (pane-local) | command | effect |
|---|---|---|---|
| scroll-up button | (169,150,24,24) | callback `LAB_005449c0` | sets `this+0x1d0` negative → scroll up 40/tick |
| scroll-down button | (169,174,24,24) | callback `LAB_005449d0` | sets `this+0x1d0` positive → scroll down |
| spell icon column | per-row (x≈30..70, row Y band) | — | select spell (slot 25→26): set cast cursor |

---

## §11 — Retail bugs NOT to reproduce

- **Pink-halo on text** (`project_retail_pink_halo_bug`): all text goes through
  `FUN_004be2b0` with magenta-cleared scratch + the 3-pass black shadow; AA shadow
  edges sample the magenta cell and survive the keyed blit → pink fringe. Render
  with real alpha (white coverage in alpha), 3 black passes + colored pass; **no**
  magenta key on text. Reproduce intent (clean cream/green/cyan text + 1px black
  shadow), not the halo.
- No other artifacts observed; the chrome is opaque non-keyed bitmaps.

---

## §12 — Reconstruction pseudocode

```
paint(pane):                              # slot 21
  cs = pane.contentSurface                # 148x229
  drawBitmap(cs, ScrollPaper, 0,0)        # seed
  box(cs, 0,0,148,229)                    # clear region
  for y = -(scrollY mod 80); y < 320; y += 80:    # tile parchment
      drawBitmapToTarget(cs, ScrollPaper, 0, y)
  rowTop = 0
  for i, spell in player.spells:          # player[+0x2ec]/[+0x2fc]
      rowH = lineTable[i]                  # = lines*lineH + 91, see §6a
      if rowIntersects(rowTop, rowH, scrollY, 229):
          k = scrollY - runningTop        # = iVar10
          icon = SpellIcons[spell.name] (40x40) → drawBitmapTinted(cs, icon, ~12, rowTop-k, alpha)
          drawTextShadowed(cs, spell.name,           x=10,  y=rowTop+44-k, w=132, font=0x401, RGB(255,231,242))
          drawTextShadowed(cs, spell.desc,           x=54,  y=rowTop-k,    w=85,h=86, font=0x402, themeColor)
          drawTextShadowed(cs, "Skill: "+spell.skill,x=10,  y=rowTop+44-k, w=132, font=0x401, RGB(244,244,5))
          drawTextShadowed(cs, "Mana: " +spell.mana, x=90,  y=rowTop+44-k, w=132, font=0x401, RGB(37,255,233))
          gx = 12
          for tal in spell.talismans:      # recipe glyphs
              drawBitmapTinted(cs, S<tal> (20x20), gx, rowTop+61-k, alpha); gx += 16
      rowTop += rowH
  composite(pane, cs)                      # content surface → pane (inset in Scroll frame)
  composite(pane, ScrlTop);  composite(pane, ScrlBottom)

update(pane):                              # slot 19 — 24Hz gate
  if scrollArmed:
      step scrollY by ±40 toward target (snap <40); clamp [0, contentH-229]; mark dirty

click(pane, x, y):                         # slot 25
  forward to button pump (arrow buttons)
  if leftDown and x in iconColumn and y in some rowBand:
      selection = thatRow; pick up spell for casting (slot 26); repaint
```
Reference §2/§4/§8 for the literal numbers.

---

## §13 — Port mapping notes

| retail call | port primitive (UI_METHOD_MAP §12) | home |
|---|---|---|
| `FUN_004bd680(...,0x80000000/0x2000/0x100)` sprite stamp | `DrawBitmapToTarget` (opaque) / `DrawBitmapSubrectTintedToTarget` (alpha) | renderer |
| ScrollPaper tile loop (`FUN_00438d80`+`(*+0x5c)`) | repeated `DrawBitmapToTarget` over the content RT (or a tiled-fill helper) | renderer |
| `FUN_004be2b0(... font 0x401/0x402)` text | `DrawTextShadowedToTarget(atlas,text,x,y,w,align=left,r,g,b,...)` (3-pass shadow built-in) | font.cpp |
| `FUN_00429950` color pack | inline RGB on the draw call | — |
| content RT → pane composite (`(*+0x5c)`) | `DrawSurface` of content RT into the pane RT (inside Scroll frame) | renderer |
| `FUN_004a5740(w,h,0x400)` surface alloc | a fixed `TSurface` content RT (148×229) | surface |
| arrow buttons (`FUN_0042c2d0`) | retained-mode TButtonPane buttons w/ up/down/glow bitmaps | ui |
| spell-icon lookup by name (`FUN_0046d710`) | asset-cache lookup `SpellIcons.dat[name]` | assets |

No missing primitives — all covered by the canonical toolbox. Use the
compose-to-target contract (content RT then `DrawSurface`); do not draw glyphs
straight to the swapchain.

---

## §14 — UNCONFIRMED / open questions

- **UNCONFIRMED-A — exact screen origin of the side-region (pane placement).**
  The pane authors pane-local; the TSidePane parent positions it in the upper
  sidebar slot. Impact: only the final on-screen offset of the whole pane, not
  internal layout. Resolve: trace TSidePane (`cls_0x5a53ec`) layout / the
  `FUN_0053cab0` region setter, or visual-verify.
- **UNCONFIRMED-B — content-surface dst origin (≈ pane-local (20,38)).** Read at
  runtime from `(this+0x180)[5],[6]` (the surface's stored dst x,y set when the
  content surface is placed), not a code immediate; the (20,38) is inferred from
  the 188-wide Scroll frame's inner parchment inset. Impact: shifts ALL row
  content by that offset relative to the scroll-rod frame. Resolve: read the
  surface object's `[5]/[6]` at init/draw time (debugger or trace where the
  content surface position is assigned), or visual-verify against the Scroll
  bitmap's inner window.
- **UNCONFIRMED-C — spell-icon archive/size: 40×40 (`SpellIcons.dat`) vs 32×32
  (`sidepane.dat[108..128]`).** Both exist; the Draw resolves by name via
  `FUN_0046d710`/`FUN_0046d6b0` which picks whichever archive is mounted. Impact:
  icon size in the row (the +0x5b=91 row pitch fits 40 comfortably, favoring
  40×40). Resolve: confirm which archive the spellbook mounts (`FUN_0046d710`
  container) or visual-verify icon size against the parchment row.
- **UNCONFIRMED-D — SPANESKILLS / SPANEMANA label text.** The labels resolve via
  `meth_0x49d800` against the object-class string registry (`&UNK_0065d1c0+0x310`),
  not english.def; only the `"%s: %d"` format + the value (skill/mana int) are
  certain. Impact: the literal label words ("Skill"/"Mana") — the cyan/green
  numeric values and layout are confirmed. Resolve: dump the string registry the
  `SPANE*` keys index into.
- **UNCONFIRMED-E — `"%scandy"` template** (`s_%scandy_005e52fc`) used in the
  glyph-row branch. Likely a localized-resource key for the talisman glyph
  variant; the glyph itself comes from the `PTR_DAT_005e4fb4` `S*` table.
  Impact: cosmetic glyph variant only. Resolve: trace `FUN_0046d710("%scandy"
  formatted)` resource resolution.
