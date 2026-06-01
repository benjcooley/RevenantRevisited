# TSideTabsPane — Reconstruction SPEC

The six round sidebar buttons: **two independent 3-way region selectors**.

---

## §0 — Sources & status

- **Class:** `TSideTabsPane`, decomp id `cls_0x5a5750`, vtable @ `0x5a5750`.
- **Global instance:** `DAT_0065be50` (`0x65be50`).
- **Inheritance shape (from B.r11 §1 + vtable dump):** `TPane` (`cls_0x5a4494`, 32
  slots) → `TButtonPane` (`cls_0x5a45c8`, +14 slots) → `TSideTabsPane` leaf
  (overrides only). 46 real slots; the vtable dump's slots 46-49 spill into the
  next class (`cls_0x5a5808` TBottomBarPane).
- **Slot → method (vtable leaf overrides):**
  | slot | addr | method | recon file |
  |---:|---|---|---|
  | 0 | `0x53cc30` | Initialize | `cls_0x5a5750_TSideTabsPane_Initialize_53cc30.cpp` |
  | 1 | `0x53d360` | Close | `cls_0x5a5750_TSideTabsPane_Close_53d360.cpp` |
  | 7 | `0x53d420` | DrawBackground (Classic path) | `cls_0x5a5750_TSideTabsPane_DrawBackground_53d420.cpp` |
  | 19 | `0x53d3a0` | Update / Animate-tick (hover ramp) | `cls_0x5a5750_TSideTabsPane_Update_53d3a0.cpp` |
  | 20 | `0x53d400` | leaf override (NOT extracted — UNCONFIRMED) | — |
  | 23 | `0x53d540` | Draw (hi-res path + effect flush) | `cls_0x5a5750_TSideTabsPane_Draw_53d540.cpp` |
  | 25 | `0x53d6e0` | MouseClick (dirty relay) | `cls_0x5a5750_TSideTabsPane_MouseClick_53d6e0.cpp` |
  | 37 | `0x53d790` | leaf override (NOT extracted — UNCONFIRMED) | — |
- **Dispatch:** `FUN_0047cf40_TPlayScreen_DispatchCommand` (`0x47cf40`), cases
  7-0xc — `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp`.
- **Vtable dump:** `recon/discovered/cls_0x5a5750_vtable.txt`.
- **Prior briefs consulted:** B.r11 (sidebar cascade — the prior full analysis),
  B.r8 (TButtonPane base + two-pass draw), B.r7 / B.r10 (DEF-widget engine — see
  §10 note on which button machinery applies). Shared refs: FORENSICS_PROTOCOL,
  NOMENCLATURE, UI_METHOD_MAP, AGENT_PROTOCOL.
- **Assets measured:** `tools/ui/dump_dat.py` on `sidebartabs.dat` +
  `sidebartabsnotex.dat` (see §2). Both confirmed 3 bitmaps × 52×232.
- **Status: `forensics-complete`** for the visible panel (geometry, assets,
  fade ramp, dispatch). Three items remain UNCONFIRMED (§14): the
  button-index → command-id wiring (`FUN_0042c600`/`FUN_00436790`/`FUN_00436900`
  bodies not extracted), the two un-extracted leaf slots 20/37, and the
  `0x10000`-flag hi-res strip variant's exact composite.
- **Errors to close:** none — no prior port exists for this panel ("impl: not
  started" per README).

### Snapshot note (supplementary — superseded by retail)

The retail decomp is the definitive source for this spec; the snapshot is
supplementary context only. **There is no direct snapshot of `TSideTabsPane`** —
the 1998/99 tree has no such class or asset, and the retail six-button
two-region strip is a retail-era redesign. The only reusable snapshot code is
the base button machinery it inherits, `src/button.{h,cpp}` (`TButton` /
`TButtonPane` — binary `down` state, radial hit-test, radio groups; note the
snapshot has no hover-fade and no third "Select" art state, both retail
additions). A single-region 4-button precursor exists at `src/multictrl.{h,cpp}`
(`TMultiCtrlPane`) but is conceptual background only; do not take layout, button
count, the two-region split, the hover ramp, or command routing from it — those
come from the retail sections below (§3-§10).

---

## §1 — Overview

`TSideTabsPane` is the vertical strip of **six round icon buttons** that runs
down the inner edge of the in-game sidebar. They are **not** a flat 1-of-6 mode
picker and **not** independent toggles. They are **two independent 3-way region
selectors** (B.r11 TL;DR):

- **Upper region** (modal state `DAT_0065d1b8` ∈ {0,1,2}): buttons **Book /
  Stats / Equip**.
- **Lower region** (modal state `DAT_0065d1bc` ∈ {0,1,2}): buttons **Spell /
  Inv / Map**.

Exactly one upper-region content pane and one lower-region content pane are
presented at once; clicking a button selects that mode for its region (and, if
that region's content is already open, the dispatcher closes it — a toggle-off).

Plain-language: it is a six-button toolbar baked from three 52×232 vertical
sprite strips (Up = idle, Down = pressed, Select = highlighted-current). Each
button blends between its idle and pressed/select art as the mouse hovers, with
a short linear fade. The *current* selection for each region is shown
permanently pressed; the others are idle and brighten on hover.

**When shown / hidden:** the pane is part of the in-game HUD; it draws whenever
the playscreen HUD is up. Both draw paths early-out on `DAT_006680c8` (the
Classic-vs-hi-res renderer toggle, §3) — DrawBackground runs the Classic path,
Draw runs the hi-res path. There is no per-pane visibility predicate inside the
class itself (the buttons are always present while the HUD is); the *content
panes* they select have their own show/hide. The Draw body also early-outs the
effect flush when `DAT_005d7a18 == 0` (window/display not ready).

---

## §2 — Asset roster

Two parallel archives hold the same three button strips at the **same dimensions**;
which one is used is selected at runtime by the renderer mode flag (§3):

| asset | archive | entry | WxH (measured) | flags | role | source rects (per button) | cite |
|-------|---------|-------|---------------|-------|------|---------------------------|------|
| `Up` | `sidebartabsnotex.dat` | idx 0 | **52×232** | `0x2` = `BM_15BIT` (RGB555) | idle / unpressed strip, all 6 icons | x=3, w=0x24, h=0x23; y∈{2,0x27,0x4c,0x71,0x96,0xbb} | dump §2; Initialize:24-29,145-200 |
| `Down` | `sidebartabsnotex.dat` | idx 1 | **52×232** | `0x2` | pressed strip | (same per-button rects) | dump §2 |
| `Select` | `sidebartabsnotex.dat` | idx 2 | **52×232** | `0x2` | highlighted / current-selection strip | (same per-button rects) | dump §2 |
| `Up` | `sidebartabs.dat` | idx 0 | 52×232 | `0x10000` (alpha/texture bit-depth) | hi-res variant of idle strip | (same) | dump §2 |
| `Down` | `sidebartabs.dat` | idx 1 | 52×232 | `0x10000` | hi-res pressed | (same) | dump §2 |
| `Select` | `sidebartabs.dat` | idx 2 | 52×232 | `0x10000` | hi-res select | (same) | dump §2 |

**Measured (dump tool):** both archives report `topbm=3 datasize=74672`; each
entry `52x232 kc=0x0`. The `notex` archive is RGB555 and decoded cleanly to PNG;
the `sidebartabs.dat` archive uses `flags=0x10000` (the dump tool labels it
"unsupported bit-depth" — it is the alpha/texture variant the hi-res path uses).
`kc=0x0` for all six → these strips key on **alpha, not magenta** (§11).

**Confirmed by visual inspection of the decoded `Select` strip**, the 6 baked
icons top-to-bottom are: scroll/tome (Book), quill-on-red (Stats), sword
(Equip), purple potion (Spell), chest/bag (Inv), map/parchment (Map) — which
matches the Initialize button-add order (§4, §10).

### Source-rect map (each 52×232 strip; identical for Up/Down/Select)

```
sidebartabs strip  52 x 232           (source space)
 col x: 0   3        3+0x24=39    52
        ├───┼──────────┼──────────┤
 y=2    │   │ Book   icon0  35h    │   x=3, w=0x24=36, h=0x23=35
 y=39   │   │ Stats  icon1         │   band stride = 0x25 = 37 px
 y=76   │   │ Equip  icon2         │
 y=113  │   │ Spell  icon3         │
 y=150  │   │ Inv    icon4         │
 y=187  │   │ Map    icon5         │   last band ends y=187+35=222 (< 232)
        └───┴──────────────────────┘
```
The 3px left margin + 13px right margin + the 2px top / 10px bottom gutters are
chrome the strip art carries; the live blit only ever copies the `(3, y, 36, 35)`
sub-rect per button (the button's stored source rect == its dest rect within the
mosaic — see §3/§6).

---

## §3 — Coordinate system & surfaces

### Pane position + size (screen space)

Set at the end of Initialize (`Initialize:217-219`):
```
iVar3            = DAT_00667c60 - DAT_0065be60                 ; pane y (= +0x18 in ints → field[6])
param_1[5] (+0x14) = (display_width - DAT_0066614c) - DAT_0065be5c   ; pane x
param_1[6] (+0x18) = iVar3                                     ; pane y
```
- `param_1` field stride is **ints**: `param_1[4]`=`+0x10`, `param_1[5]`=`+0x14`,
  `param_1[6]`=`+0x18`. (Note: Initialize writes `param_1[5]`/`[6]`; the paint
  bodies read the pane rect from `param_1+4`/`+8`/`+0xc`/`+0x10` — see below.)
- `*(int*)(PTR_DAT_005d79e0 + 4)` = the display surface width. So the pane x is
  **right-anchored**: `pane_x = display_w − DAT_0066614c − DAT_0065be5c`
  (two layout constants — the sidebar chrome width + an inset). **UNCONFIRMED-A:**
  exact decimal of `DAT_0066614c`/`DAT_0065be5c`/`DAT_00667c60`/`DAT_0065be60`
  (BSS layout constants, not literals in the body).
- The paint bodies read the **pane rect** as `screen:(x,y,w,h) =
  (*(param_1+4), *(param_1+8), *(param_1+0xc), *(param_1+0x10))`
  (DrawBackground:14, Draw:35-36/80-81). `+0xc` = live width, `+0x10` = live
  height (the NOMENCLATURE `this+0xc` live-width field).

### The "chip" model for this panel

This panel is **not** the wide player/target chip model — it is a small fixed
strip. The strip's intrinsic size is the source bitmap size: **52 wide × 232
tall** (the mosaic is built at the `Up` bitmap's own dims, Initialize:37-38).
There is **one** chip (no player/target mirror). All six buttons live in
**pane-local** space; pane-local origin maps to screen via the pane rect above.

### The 3 mosaic surfaces

Initialize allocates **three** scratch mosaic surfaces (only when Classic,
`DAT_006680c8 == 0`, Initialize:30-84; the hi-res branch at :85-139 allocates a
different-shaped set), each sized to the `Up` bitmap (`uVar1 = bm[1]` = height,
`uVar2 = bm[0]` = width, via `FUN_004bb5c0(w,h)`):

| field | int idx | role | cite |
|------|--------:|------|------|
| `this+0x188` | `param_1[0x62]` | mosaic surface **A** | Initialize:48, Close:6 |
| `this+0x18c` | `param_1[99]` | mosaic surface **B** | Initialize:66/121, Close:9 |
| `this+0x190` | `param_1[100]` (=`+0x190`) | mosaic surface **C** | Initialize:83/138, Close:12 |

Each is a `&PTR_FUN_005a3e7c`-vtable surface (a `TMosaicSurface`/multi-tile
working surface, UI_METHOD_MAP §2 type-0x10), 52×232, holding a **fully-composited
copy of the strip in one button-state**: the panel pre-renders the resting strip
once, then per-frame composites the hover-fade deltas on top.

> **Naming note:** B.r11 labelled these "the 3 mosaic surfaces @ +0x188/+0x18c/+0x190"
> and Close frees exactly those three. DrawBackground reads `+0x18c` as the base
> strip; Draw reads `+0x18c` as base and `+0x190` (`param_1+400` = `0x190`) as
> the per-button scratch. So **B = base composited strip**, **C = per-button
> blend scratch**, **A = the third state buffer** (the Down/Select pre-compose).
> The three correspond to the three source strips Up/Down/Select but are
> *working* surfaces, not the raw bitmaps (those are at `+0x17c/+0x180/+0x184`
> = `param_1[0x5f]/[0x60]/[0x61]`, Initialize:24-29).

### Raw bitmap handles

| field | int idx | holds | cite |
|------|--------:|-------|------|
| `this+0x17c` | `param_1[0x5f]` | strip bitmap 0 = `Up` | Initialize:24-25 |
| `this+0x180` | `param_1[0x60]` | strip bitmap 1 = `Down` | Initialize:26-27 |
| `this+0x184` | `param_1[0x61]` | strip bitmap 2 = `Select` | Initialize:28-29 |

Resolved by three sequential `FUN_0046d710()` iterator calls on the open
container (UI_METHOD_MAP §9 iterator form → entries 0,1,2). Handle layout
`{int width@[0], int height@[1], …}` (UI_METHOD_MAP §9).

### Per-button state fields

The six buttons' **hover-fade ramp counters** live at `this+0x1ac` onward, one
int per button (`piStack_4 = param_1 + 0x1ac`, indexed 0..count-1):
- `*piVar2` (`this+0x1ac + i*4`) = button *i*'s current ramp value `cur` (0..8).
- `piVar2[6]` (`this+0x1ac + i*4 + 0x18`) = button *i*'s ramp **target**
  (0 or 8) — Update writes it from the hover flag (§9). Initialize zeroes both
  (`piVar5[6]=0; *piVar5=0;` loop at Initialize:208-216, `piVar5 = param_1+0x6b`
  = `+0x1ac`).
- Button **count** = `this+0x88` (`param_1[0x22]`) — the TButtonPane base child
  count (= 6 once all six are added).

The button **handles** (TButton records) returned by `FUN_00436900(i)` are
cached at `param_1[0x65..0x6a]` (`this+0x194..0x1a8`) in Initialize (note the
add-order vs storage-index swap, §10). Each TButton record exposes:
`+0x14` (flags; bit `8` = hovered), `+0x60` dst/src x, `+0x64` (=100) dst/src y,
`+0x68` w, `+0x6c` h (read in DrawBackground:26-31 and Draw:50-53).

### Layout diagram (pane-local)

```
pane-local (origin = screen pane_x, pane_y; pane is ~52 wide, ~232 tall)
x: 0   3        39
   ├───┼──────────┐
   │   │ ◉ Book   │  y=2    button0  upper region, mode 2   cmd 7/0xa
   │   │ ◉ Stats  │  y=39   button1  upper region, mode 1   cmd 8
   │   │ ◉ Equip  │  y=76   button2  upper region, mode 0   cmd 9
   │   │ ◉ Spell  │  y=113  button3  lower region, mode 2   cmd 7/0xa
   │   │ ◉ Inv    │  y=150  button4  lower region, mode 0   cmd 9/0xb
   │   │ ◉ Map    │  y=187  button5  lower region, mode 1   cmd 0xc
   └───┴──────────┘
each ◉ = 36x35 round icon at pane-local (3, y), y stride 37 (0x25)
```

---

## §4 — Static element layout

One element type only: the six round buttons. No text, no portrait, no bars.
The dest rect of each button **equals** its source rect within the strip (the
strip is laid out at 1:1 in pane-local space). Both dst and src share `x=3`,
`w=0x24=36`, `h=0x23=35`; only `y` differs.

| button | add-order | space | dst (x,y) | (w,h) | source rect (sx,sy,sw,sh) | cmd-id (region/mode) | content global | cite |
|---|---:|---|---|---|---|---|---|---|
| Book  | 0 | pane-local | (3, 2)    | (36,35) | (3, 2, 36, 35)    | 7 / 0xa (upper=2) | `DAT_0065a9d8` (cls_0x5a5ae8) | Init:145 |
| Stats | 1 | pane-local | (3, 0x27=39)  | (36,35) | (3, 39, 36, 35)   | 8 (upper=1) | `DAT_0065b140` (cls_0x5a5ba0) | Init:155 |
| Equip | 2 | pane-local | (3, 0x4c=76)  | (36,35) | (3, 76, 36, 35)   | 9 (upper=0) | `DAT_0065b7e0` (cls_0x5a55dc) | Init:166 |
| Spell | 3 | pane-local | (3, 0x71=113) | (36,35) | (3, 113, 36, 35)  | 7 / 0xa (lower=2) | `DAT_006661b0` (cls_0x5a5978) | Init:177 |
| Inv   | 4 | pane-local | (3, 0x96=150) | (36,35) | (3, 150, 36, 35)  | 9 / 0xb (lower=0) | `DAT_0065d4f8` (cls_0x5a58c0) | Init:188 |
| Map   | 5 | pane-local | (3, 0xbb=187) | (36,35) | (3, 187, 36, 35)  | 0xc (lower=1) | `DAT_0065b4f0` (cls_0x5a5658) | Init:199 |

`FUN_0042c600(label, sx=3, sy=Y, sw=0x24, sh=0x23, 0,0,0,0, 0xffffffff,
flagsword, group)` is the TButtonPane add-button call. The literal args confirm
the rects:
- `FUN_0042c600(&DAT_005e4720 /*"Book"*/, 3, 2, 0x24, 0x23, 0,0,0,0, 0xffffffff, <flags>, 1)` (Init:145)
- Stats `(s_Stats_005e4728, 3, 0x27, 0x24, 0x23, …, 1)` (Init:155)
- Equip `(s_Equip_005e4730, 3, 0x4c, 0x24, 0x23, …, 1)` (Init:166)
- Spell `(s_Spell_005e4738, 3, 0x71, 0x24, 0x23, …, 2)` (Init:177)
- Inv   `(&DAT_005e4740, 3, 0x96, 0x24, 0x23, …, 2)` (Init:188)
- Map   `(&DAT_005e4744, 3, 0xbb, 0x24, 0x23, …, 2)` (Init:199)

The **last arg** = the **radio group**: `1` for the upper-region trio (Book /
Stats / Equip), `2` for the lower-region trio (Spell / Inv / Map). Two radio
groups = the "two independent 3-way region selectors" model, at the widget level.

The **flags word** encodes the *initially-pressed* button per region. For Book
(group 1) it is `bVar6 - 1 & 0x10000 | 0x180010` where `bVar6 = (DAT_0065d1b8 != 2)`
(Init:141,145); for Stats `(-(uint)(DAT_0065d1b8 != 1) & 0xffff0000) + 0x10000 |
0x180010` (Init:151,155); Equip uses `(DAT_0065d1b8 != 0)` (Init:162). Lower
trio mirrors with `DAT_0065d1bc` (Init:173/184/195). Decoded: the base flags are
`0x180010` (a CTRLFLAG set: `0x10` = visible/active + `0x180000` = button-radio
bits) and the high word toggles a "pressed" bit **on** only for the button whose
mode index equals the current region modal state — i.e. **the current selection
boots up already pressed** (§10).

---

## §5 — Draw order / composition

The panel splits across **three** vtable slots, gated by the renderer mode
`DAT_006680c8` (Classic = 0, hi-res = nonzero):

1. **Update (slot 19, every sim tick)** — advance each button's hover ramp
   (§9). No drawing. (Update:1-36.)
2. **DrawBackground (slot 7) — CLASSIC path only** (`DAT_006680c8 == 0`,
   DrawBackground:12):
   a. Blit the **base strip** (mosaic B `+0x18c`) opaque to the pane at
      `screen:(param_1+4, param_1+8)`, full size, via `FUN_00414d70` (UI_METHOD_MAP
      §2). This paints all six buttons in their resting state. (DrawBackground:13-16.)
   b. For each button *i* with ramp `cur>0`: compute alpha, then `FUN_00414d70`
      the button's `Select`-state sub-rect from mosaic C (`param_1+400` = `+0x190`)
      onto the pane at the button's dst, **tinted with alpha** (drawmode/flags
      `…,0,4` → blend). This is the hover-fade overlay. (DrawBackground:18-36.)
3. **Draw (slot 23) — HI-RES path** (`DAT_006680c8 != 0`, Draw:40-78) +
   **effect flush** (both paths, Draw:34-38/79-82):
   a. `FUN_00438df0(0,0, mosaicB, 0,0, w,h, 0x100 /*DM_TRANSPARENT*/, 0,0)` —
      stamp the base strip (transparent). (Draw:42.)
   b. For each button *i* with `cur*255/8 > 0x80` (i.e. ramp ≥ 5, the **bright**
      threshold): build an `SDrawParam` from the button rect and blit the
      `Select` strip sub-rect via the display's `(*+0x5c)` (ParamBlit-with-effects,
      UI_METHOD_MAP §8.4) — drawmode `0x100`, intensity `0x1f` (`uStack_8`).
      (Draw:48-74.)
   c. `FUN_004aacb0_BlitEffect_Iterate(x,y,w,h,1)` — flush the deferred
      blit-effect registry over the pane rect (only when `DAT_005d7a18 != 0`).
      (Draw:34-37, 79-82.)

So **DrawBackground is the Classic compositor**, **Draw is the hi-res
compositor + the universal effect-flush tail**. Both reproduce the same visual:
resting strip + per-button hover overlay.

---

## §6 — Algorithms

> Cross-ref (supplementary): the snapshot `TButtonPane`/`TButton` express the
> button state + radio toggle readably (`src/button.cpp:88-336`), but retail
> **supersedes** them with the mosaic composite + hover-fade ramp below — build
> from this section, not the snapshot.

### Hover-fade alpha (both paint paths)

The ramp→alpha conversion is the integer expression
`alpha = (cur*0xff + (cur*0xff >> 31 & 7)) >> 3` = **round(cur·255/8)** with
round-to-nearest for the (here always non-negative) numerator
(DrawBackground:22, Draw:48). Tabulated:

| cur | cur·255 | alpha = round(cur·255/8) | bright pass? (alpha>0x80) |
|---:|---:|---:|:--|
| 0 | 0   | 0   | no (skip entirely) |
| 1 | 255 | 32  | no |
| 2 | 510 | 64  | no |
| 3 | 765 | 96  | no |
| 4 | 1020| 128 | no (== 0x80, **not** > 0x80) |
| 5 | 1275| 159 | **yes** |
| 6 | 1530| 191 | **yes** |
| 7 | 1785| 223 | **yes** |
| 8 | 2040| 255 | **yes** (fully pressed/select) |

- **Classic (DrawBackground):** any `alpha>0` draws the overlay; the alpha is
  packed `uStack_8 = CONCAT13((char)alpha, 0xffffff)` = `0xAArrggbb` with
  rgb=white (`0xffffff`) and the high byte = alpha. Passed as the `color`/tint
  arg to `FUN_00414d70(..., uStack_8, ..., 0, 4)` (the trailing `4` = blend
  flag, UI_METHOD_MAP §2 step 3). (DrawBackground:24-31.)
- **Hi-res (Draw):** only `alpha>0x80` (cur≥5) draws the extra bright Select
  overlay; the base strip already carries the lower-intensity blend implicitly
  via the transparent stamp. (Draw:49.)

### Per-button base/overlay blit (DrawBackground:26-31)

```
for i in 0 .. count-1:
    cur   = ramp[i]                     # this+0x1ac + i*4
    alpha = round(cur*255/8)
    if alpha <= 0: continue
    btn   = FUN_00436900(i)             # TButton record
    dstx  = pane_x + btn[0x60]          # btn[0x60] = pane-local x (=3)
    dsty  = pane_y + btn[0x64]          # btn[0x64] = pane-local y (=2/39/76/…)
    blit( dst=(dstx,dsty), dstSurf=pane,
          src = mosaicC (+0x190),        # the Select/highlight composite
          sw=btn[0x68], sh=btn[0x6c],     # 36, 35
          color = (alpha<<24)|0xffffff,   # white tint, ramp alpha
          sx=btn[0x60], sy=btn[0x64],     # source rect == dest rect in strip
          dw=btn[0x68], dh=btn[0x6c],
          mode=0, flags=4 )               # 4 = alpha-blend
```
`FUN_00436900(0)` is also called bare before the loop (DrawBackground:17,
Draw:43) — a "rewind iterator / select button 0" side-effect of the
TButtonPane button accessor; the loop then calls `FUN_00436900(i)` per index.

### Hi-res SDrawParam (Draw:50-73)

The stack struct `uStack_54…` is an `SDrawParam` (UI_METHOD_MAP §1 layout):
`drawmode=0x100` (`uStack_54`), all clip/origin zero, `dx=dy=dwidth=dheight`
mirrored from the button rect (`uStack_1c..uStack_10` = `btn[0x60]/0x64/0x68/0x6c`),
`sx/sy/sw/sh` = same button rect (`uStack_2c/0x28/0x24/0x20`), `color=0`,
`intensity=0x1f` (`uStack_8`), zpos=0. Blitted via
`(**(*PTR_DAT_005d79e0 + 0x5c))(&dp, param_1+400 /*mosaicC*/, 0, 0)`.

---

## §7 — Effects & shadows

- **No drop shadow.** No `FUN_00438d80` shadow-setup call appears in any
  TSideTabsPane body. The round buttons cast no shadow (the chrome ring is baked
  into the strip art).
- **Blit-effect flush:** Draw calls `FUN_004aacb0_BlitEffect_Iterate(...,1)`
  (UI_METHOD_MAP §8.5) over the pane rect at the end — this is the global
  deferred-effect flush (srcId `1`), not a TSideTabsPane-specific effect. It
  exists so any effects other panels registered that intersect this rect get
  stamped after the strip paints. The port's `DrawBitmap…Shadowed` / GPU compose
  makes this a no-op.
- **Alpha blend, not chroma key:** the strips have `kc=0x0` and carry real alpha
  (`notex` = RGB555 with alpha-or-key handled by the decoder; `sidebartabs.dat`
  = `0x10000` alpha variant). The hover overlay uses blend flag `4` (Classic) /
  `intensity=0x1f` (hi-res). See §11.
- **No text 3-pass shadow** — the panel renders no glyphs.

---

## §8 — Text rendering

**N/A — the panel renders no text.** The button labels ("Book", "Stats",
"Equip", "Spell", "Inv", "Map" at `0x5e4720/28/30/38/40/44`) are passed to
`FUN_0042c600` as the button **name/id** (for hit-test + tooltip identity), not
drawn — the visible glyph for each button is the **baked icon** in the strip
art. (Confirmed: neither DrawBackground nor Draw calls `FUN_004be2b0`.)

---

## §9 — Animation & dynamic behavior

### Hover fade (the textbook ramp)

Per-button counter `cur` at `this+0x1ac + i*4`; per-button target `tgt` at
`this+0x1ac + i*4 + 0x18`. Update (slot 19, per sim tick) sets the target from
the button's hover flag, then steps `cur` one toward `tgt`:

```
ramp button[i].cur:
  range   0..8
  step    +1 per sim tick toward target (Update:21-30)
  target  8 when hovered (TButton[i] flag +0x14 & 8 set), else 0  (Update:14-19)
  maps-to Classic: alpha = round(cur*255/8), drawn whenever alpha>0  (DrawBackground:22)
          Hi-res:  extra bright Select pass only when round(cur*255/8) > 0x80 (cur>=5) (Draw:49)
  drive   per-button hover flag; reverses (steps -1) when hover lost
  on-end  hold at target (8 held while hovered, 0 held while not)
  cite    Update_53d3a0.cpp:14-30; DrawBackground_53d420.cpp:22; Draw_53d540.cpp:48-49
```

Update body (Update:10-34): `for i: btn=FUN_00436900(i); tgt = (btn[0x14]&8)?8:0;
cur = ramp[i]; if cur<tgt: cur++ ; else if tgt<cur: cur-- ; ramp[i]=cur;`

Temporal diagram:
```
overlay alpha (Classic, 0..255)              hi-res bright gate at cur>=5
255 ┤                ●─────────  hover held (cur=8)
223 ┤              ╱
191 ┤            ╱
159 ┤          ╱     ── cur>=5 line: hi-res starts drawing the bright Select pass
128 ┤        ╱
 96 ┤      ╱   linear, alpha = round(cur*255/8)
 64 ┤    ╱
 32 ┤  ╱
  0 ┤●╱                          ●─────────  hover lost, ramp back to 0
    └─────────────────────────────────────── sim ticks (24 Hz)
    cur:  0 1 2 3 4 5 6 7 8 [hold] 8 7 6 5 4 3 2 1 0 [hold]
    ramp: ±1 per tick   →  full fade in/out = 8 ticks ≈ 333 ms @24Hz
```

The **current selection** for each region is held permanently pressed via the
button's radio-group pressed flag (set at Initialize per the modal-state globals,
§4) — that is a static "pressed" state, separate from the hover ramp. A
non-selected button shows idle art, brightening on hover; the selected button
shows the pressed/Select art and (per the flag) does not need the hover ramp to
look engaged.

### Dirty / redraw model

There is no per-button value cache (unlike TPlyrStatusBar). The pane redraws its
strip every frame in the active path. The only "dirty" mechanism is the
MouseClick relay (§10): on a click inside the pane it pulses three external HUD
overlays' wake flags so they re-render after the strip paints over them.
**UNCONFIRMED-B:** whether the base strip composite (mosaic B/C contents) is
rebuilt every frame or cached once at Initialize — Initialize does one
`(**(*param_1 + 0x20))(strip0_width)` call (Init:220, vtable slot 8 — likely the
initial mosaic build); the per-frame paths only *read* the mosaics. Best guess:
built once, read per-frame; resolve by extracting `FUN_004bb5c0` / vtable slot 8.

---

## §10 — Input & dispatch

> Cross-ref (supplementary): the snapshot `TButtonPane::MouseClick` +
> `ClearGroup`/`CheckGroup` (`src/button.cpp:246-336`) show the radio-group
> click flow in readable form. Retail still uses this base flow (its compiled
> form is `FUN_00436530`) but **supersedes** the action wiring: clicks route
> through the `FUN_0047cf40` command dispatcher (cases below), not direct
> function pointers.

### Hit rects (pane-local)

The six button hit rects equal their dst rects (§4): `(3, y, 36, 35)` for
y∈{2,39,76,113,150,187}. Hit-testing is done by the TButtonPane base
(`FUN_00436530`, the per-button callback pump), not by TSideTabsPane.

### MouseClick (slot 25) — dirty relay, NOT the dispatch

`FUN_0053d6e0(this, evtType, mx, my)` (MouseClick:5-27):
1. If `evtType ∈ {4,5}` (mouse-down/up) AND `(mx,my)` is inside the pane rect
   (`0 ≤ mx < this+0xc`, `0 ≤ my < this+0x10`) AND any of three external HUD
   overlay flags `DAT_0065d67c / DAT_0065b090 / DAT_0065b878` is "present"
   (≥ −1 / ≠ 0): call `FUN_0043a100(0,0,0); FUN_0043a140(0);
   FUN_0043a170(&DAT_00670218)` (cursor/sound feedback) and set the three
   overlays' wake flags `_DAT_0065d548 / _DAT_0065b078 / _DAT_0065b830 = 1`.
2. **Always** fall through to `FUN_00436530(evtType, mx, my)` — the TButtonPane
   per-button callback pump, which performs the actual hit-test and fires the
   pressed button's callback. The callback emits the button's **command-id** into
   the TPlayScreen command queue, dispatched by `FUN_0047cf40` (below).

The three wake-flag globals are the player status bar / target status bar / text
log overlays (B.r11 §"slot 25 dirty-flag globals" — conservative). They are
**not** content-pane pointers.

### Modal-state globals (the two region selectors)

| global | addr | region | values & effect |
|--------|------|--------|-----------------|
| `DAT_0065d1b8` | `0x65d1b8` | **upper** | 0 = Equip shown, 1 = Stats shown, 2 = Book shown |
| `DAT_0065d1bc` | `0x65d1bc` | **lower** | 0 = Inv shown, 1 = Map shown, 2 = Spell shown |

How a value changes which button is pressed: at Initialize each button's
"pressed" flag is set iff its mode index equals its region's current modal state
(§4 flag-word decode: `bVar6 = (DAT_0065d1b8 != mode)`; the pressed bit is set
when `bVar6` is false). At runtime the dispatcher writes the new mode into the
global and re-pulses button flags via `FUN_00438a50(...)` (three pulses per case
— one per button in the affected region) so the radio group re-points the
pressed state to the newly-selected button. Setters:
`FUN_0053cab0(N)` = upper-region setter, `FUN_0053cb40(N)` = lower-region setter
(bodies not extracted — UNCONFIRMED-C).

### Dispatch cases 7-0xc (`FUN_0047cf40_TPlayScreen_DispatchCommand`)

Per-case body shape (DispatchCommand:149-298). Each case: set region modal state,
then either mark "show this side" (if content not currently displayed,
`DAT_0065d190 == 0`) or close the currently-displayed content global
(`FUN_0048ed90(&content, -1)` removes from open-pane list; reset content+0x48/+0x4c;
`(**(content + 0x28))()` = vtable[10] Close), then pulse the three button flags.

| case | setter call(s) | state set | content global(s) acted on | behavior | cite |
|---:|---|---|---|---|---|
| 7 | `FUN_0053cab0(2)` + `FUN_0053cb40(2)` | upper=2 **and** lower=2 | `DAT_0065a9d8` (Book), `DAT_006661b0` (Spell) | combined **Book+Spell** preset; closes via vtable-Close if open | :149-183 |
| 8 | `FUN_0053cab0(1)` | upper=1 | `DAT_0065b140` (Stats) | **Stats** only | :184-201 |
| 9 | `FUN_0053cab0(0)` + `FUN_0053cb40(0)` | upper=0 **and** lower=0 | `DAT_0065b7e0` (Equip), `DAT_0065d4f8` (Inv) | combined **Equip+Inv** preset | :202-233 |
| 0xa | `FUN_0053cab0(2)` + `FUN_0053cb40(2)` | upper=2 **and** lower=2 | `DAT_0065a9d8`, `DAT_006661b0` | same as 7 but **opens** via `FUN_0047c500(1)` when empty | :234-265 |
| 0xb | `FUN_0053cb40(0)` | lower=0 | `DAT_0065d4f8` (Inv) | **Inv** only | :266-283 |
| 0xc | `FUN_0053cb40(1)` | lower=1 | `DAT_0065b4f0` (Map) | **Map** only | :284-298 |

> Per B.r11 §"Visibility model": cases 7/9/0xa that set **both** regions are the
> keyboard convenience hotkeys ("switch to Book+Spell view" / "Equip+Inv view").
> The **button strip itself** is one-button-per-mode: each button fires the
> single-region case (8 = Stats, 9-as-Equip, 0xb = Inv, 0xc = Map, and the
> Book/Spell buttons fire the upper/lower-2 setters). **UNCONFIRMED-D:** the exact
> button-index → command-id wiring (which case-id each TButton callback emits)
> is not pinned — `FUN_00436790` (button-callback registration) was not
> extracted. The region/mode mapping in §4 is certain; the precise case-id per
> button (e.g. does the Book button emit 7, 0xa, or a dedicated upper=2 case) is
> the open wire. Resolve by extracting `FUN_00436790`.

---

## §11 — Retail bugs NOT to reproduce

- **Do NOT apply the global magenta chroma key to these strips.** Both archives
  declare `kc=0x0` and carry **alpha**, not a magenta key (NOMENCLATURE §4 /
  UI_METHOD_MAP §16: when a bitmap declares a non-magenta key and relies on
  alpha, applying the global `0x7C1F` key punches holes in legitimately-black
  pixels — the round buttons have black ring shading and dark icon detail that
  would get keyed out). Render with the source alpha.
- **No pink-halo risk here** (no text composited through the magenta-cleared GDI
  scratch). The panel draws only sprites.
- **`0x10000`-flag archive ("unsupported bit-depth"):** the dump tool can't
  decode `sidebartabs.dat` — that is a tool gap, not a retail bug. The hi-res
  path expects the alpha/texture variant; the port should decode the `0x10000`
  format (or, for the Classic faithful mode, just use the RGB555 `notex`
  strips). **UNCONFIRMED-E:** the exact `0x10000` pixel format / how the hi-res
  Draw path's `intensity=0x1f` blend differs from the Classic alpha blend.
- **No `+4,+4` shadow trap** — this panel sets up no shadow (§7), so the
  shadow-offset semantics do not apply.

---

## §12 — Reconstruction pseudocode

```
# ---- init (once) ----
strips = load("sidebartabs(notex).dat")  # 3 entries: Up, Down, Select; each 52x232
buttons = []
for (label, y, group) in [("Book",2,1),("Stats",39,1),("Equip",76,1),
                          ("Spell",113,2),("Inv",150,2),("Map",187,2)]:
    buttons.append(Button(label, rect=(3, y, 36, 35), group=group, cur=0, target=0))
# initial pressed state:
mark_pressed(upper_buttons[upper_modal])   # upper_modal = DAT_0065d1b8
mark_pressed(lower_buttons[lower_modal])   # lower_modal = DAT_0065d1bc
pane.rect = (display_w - sidebar_const - inset, top_const, 52, 232)   # see §3 (right-anchored)

# ---- update (per 24Hz sim tick) ----   §9
for b in buttons:
    b.target = 8 if b.hovered else 0
    b.cur += sign(b.target - b.cur)         # ±1, clamp implied by target

# ---- paint (per frame) ----            §5/§6
draw_bitmap_subrect(pane, strips.Up, dst=pane.origin, sx=0,sy=0,sw=52,sh=232)   # resting strip (all 6)
for b in buttons:
    alpha = round(b.cur * 255 / 8)
    if alpha == 0: continue
    draw_bitmap_subrect_tinted(
        pane, strips.Select,
        dst=(pane.x + b.rect.x, pane.y + b.rect.y),
        src=b.rect, tint=(255,255,255, alpha))     # hover/select overlay
    # current-selection buttons hold cur=8 (alpha=255) → fully Select art
# (the resting strip can be composed from Up; the pressed selection can also be
#  pre-composed from Down/Select — retail keeps 3 working mosaics, §3.)

# ---- click ----                         §10
def on_mouse(evt, mx, my):
    if evt in (DOWN, UP) and pane.contains(mx, my):
        wake_overlays(player_bar, target_bar, text_log)   # dirty relay
    btn = hittest(mx, my)
    if btn and evt == UP:
        emit_command(btn.command_id)        # → TPlayScreen dispatch cases 7..0xc
```

`emit_command` routes to the §10 case table: the case sets the region modal
global, opens/closes that region's content pane, and re-points the radio-group
pressed state.

---

## §13 — Port mapping notes

| retail call | port primitive | source |
|---|---|---|
| `FUN_00414d70(dst…, color, …, 4)` (Classic base + overlay blit) | `Renderer->DrawBitmapSubrect(bm,dx,dy,sx,sy,sw,sh)` (base) / `…SubrectTinted(...,r,g,b,a)` (overlay) | UI_METHOD_MAP §2 |
| `FUN_00438df0(0,0,strip,…,0x100,…)` (hi-res transparent stamp) | `Renderer->DrawBitmap(bm,x,y)` with transparent/alpha mode | UI_METHOD_MAP §1 (DM_TRANSPARENT) |
| `(*display+0x5c)(&dp, mosaic, 0,0)` (hi-res ParamBlit-with-effects) | `Renderer->DrawSurface` / `DrawBitmapSubrect` (GPU blend) | UI_METHOD_MAP §8.4, §15a `+0x5c` |
| `FUN_004aacb0_BlitEffect_Iterate(...)` (effect flush) | no-op (GPU composites with blending) | UI_METHOD_MAP §8.5 |
| `FUN_0046d710()` iterator (3 strip entries) | asset-cache: load 3 named bitmaps once | UI_METHOD_MAP §9 |
| `FUN_00436900(i)` / `FUN_00436530` / `FUN_0042c600` (TButtonPane button mgmt) | the ported `TButtonPane` retained-mode button list (per feedback-ui-retained-mode) | B.r8 |
| mosaic surfaces `+0x188/0x18c/0x190` | one fixed render target per strip-state, or compose-on-read; direct-renderer contract (NOMENCLATURE §3) | — |

**Needs new primitive:** none strictly — `DrawBitmapSubrectTinted` covers the
ramp-alpha overlay. If the port wants the hi-res `0x10000` strips, the bitmap
decoder must handle that format (see §11 UNCONFIRMED-E) — otherwise use the
RGB555 `notex` strips for Classic-faithful rendering.

---

## §14 — UNCONFIRMED / open questions

- **UNCONFIRMED-A — pane screen position constants.** `pane_x = display_w −
  DAT_0066614c − DAT_0065be5c`; `pane_y = DAT_00667c60 − DAT_0065be60`
  (Initialize:217-219). These are BSS layout constants, not body literals.
  *Impact:* the strip lands at the wrong screen offset if guessed. *Resolve:*
  `FindImmRefs.java` / read the init that populates `0x6614c / 0x65be5c /
  0x667c60 / 0x65be60`, or visual-verify against `sample_screen_1.jpg`.
- **UNCONFIRMED-B — mosaic build cadence.** Whether mosaics B/C are composited
  once (Init slot-8 call, Init:220) or rebuilt per frame. *Impact:* perf only —
  visual result identical. *Resolve:* extract vtable slot 8 (`FUN_00444f90`
  region) + `FUN_004bb5c0`.
- **UNCONFIRMED-C — region setters.** `FUN_0053cab0` (upper) / `FUN_0053cb40`
  (lower) bodies not extracted; role inferred from call context (B.r11 §3).
  *Impact:* low — they set the modal global + re-flag buttons, both observable
  from the dispatcher. *Resolve:* `DecompileAddr.java 0x53cab0` / `0x53cb40`.
- **UNCONFIRMED-D — button-index → command-id wiring.** Which command-id each of
  the 6 TButton callbacks emits into the dispatcher is not pinned;
  `FUN_00436790` (callback registration) + `FUN_0042c600` (add-button) bodies
  not extracted. The region/mode/content-pane mapping (§4) IS certain; only the
  precise case-id per button is open. *Impact:* a click could route to the
  wrong dispatcher case (e.g. Book→7 vs →0xa). *Resolve:* `DecompileAddr.java
  0x436790` and `0x42c600`; cross-check the closure arg against cases 7-0xc.
- **UNCONFIRMED-E — hi-res `0x10000` strip format + blend.** `sidebartabs.dat`
  doesn't decode in the dump tool; the hi-res Draw path uses `intensity=0x1f`
  not an alpha tint. *Impact:* hi-res mode renders nothing / wrong blend if the
  format is unhandled. *Resolve:* extend `src/bitmapdecode.cpp` for the `0x10000`
  flag; for Classic-faithful mode, use the `notex` RGB555 strips and the
  DrawBackground path (fully specified here).
- **Un-extracted leaf slots 20 (`0x53d400`) & 37 (`0x53d790`).** Present as leaf
  overrides; not read. *Impact:* possibly a second animate/cleanup hook. B.r11
  flags slot 20 as "another animate-related override," slot 37 unknown.
  *Resolve:* `DecompileAddr.java 0x53d400` / `0x53d790`.
