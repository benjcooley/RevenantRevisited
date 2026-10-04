# CharacterStatsPane (TStatPane) — Reconstruction SPEC

Sidebar **Stats** content panel: the parchment scroll shown in the right
sidebar's upper region when the "Stats" tab is selected. Renders the player's
name / class / level / health-mana / six trainable stats + skill mods as
GDI-drawn text laid out by a data-driven DEF format file (`statpane.def`).

> Forensics agent output. NO engine code here. Cite-first. Every coordinate
> names its frame. Follows `docs/ui/forensics/FORENSICS_PROTOCOL.md` §0–§14.

---

## §0 — Sources & status

**Class identity (golden-path CONFIRMED).** The render method literal
`s_d__revenant_StatPane_cpp_005e5604` = `"d:\revenant\StatPane.cpp"`
(`statpane_render_5491c0.cpp:24,34,48,53`) is the retail source-path
fingerprint — per [[reference-revsync-convention]] this is decisive: the class
is **TStatPane**. Prior waves left it as the role label `StatsSidebarPane`
pending exactly this confirmation (B.r11 §5). It can now be promoted:
`cls_0x5a5ba0 → cls_0x5a5ba0_TStatPane`.

- **Class:** `cls_0x5a5ba0` (TStatPane). **Global instance:** `0x65b140`
  (`DAT_0065b140`). **sizeof:** 436 (`cls_0x5a5ba0.cpp:35`).
- **Base chain:** `TStatPane : TButtonPane (cls_0x5a45c8) : TPane (cls_0x5a4494)`
  with the DEF-widget-engine intermediate (B.r6 §3, B.r8 §4). Vtable = 48 slots
  / 192 bytes (`cls_0x5a5ba0__vftable_5a5ba0.cpp:56`).
- **Ctor:** `FUN_00488910_StatsSidebarPane_ctor` @ 0x488910 (CRT thunk @ 0x480d10).
- **Key vtable slots (leaf overrides):**
  | slot (byte) | addr | role | recon file |
  |---|---|---|---|
  | 0 (0x00) | 0x546b50 | **Initialize** | `cls_unknown_StatPane_Initialize_546b50.cpp` ; `cls_0x5a5ba0.cpp:129` |
  | 1 (0x04) | 0x546d70 | **Close** | `cls_0x5a5ba0.cpp:225` |
  | 80 (0x50) | 0x549010 | **DrawIfDirty** (calls slot84 + slot44) | `cls_0x5a5ba0.cpp:251` |
  | 84 (0x54) | 0x5491c0 | **Draw / DrawBackground** (the paint method) | `/tmp/recon_ui/statpane_render_5491c0.cpp` → save as `recon/discovered/cls_0x5a5ba0_TStatPane_Draw_5491c0.cpp` |
  | 100 (0x64) | 0x5493b0 | **MouseClick** (page1/page2 buttons + dirty) | `cls_0x5a5ba0.cpp:265` |
  | 184 (0xb8) | 0x5496a0 | **SetObject/SetFormat** (binds char, sets format name) | `cls_0x5a5ba0.cpp:292` |
- **Helpers traced:**
  - `FUN_005475e0` — **the DEF format interpreter** (6689 bytes); the heart of
    the layout. Save as `recon/discovered/cls_0x5a5ba0_TStatPane_RenderFormat_5475e0.cpp`.
  - `FUN_00546de0` — **WriteText** (TEXT token; 3-pass black shadow + colored
    `TextOutA`, advances cursor). Save as `…_TStatPane_WriteText_546de0.cpp`.
  - `FUN_00546f40` — **WriteField** (FIELD-with-rect; 3-pass shadow + `DrawTextA`).
    Save as `…_TStatPane_WriteField_546f40.cpp`.
  - `FUN_00547240` — field-value resolver (FIELD `name` → value string). Not
    extracted — see §14 UNCONFIRMED-1.
  - Tokenizer `FUN_00478720/00479450/00479580/004795a0/00479700/0047a410` — the
    shared script tokenizer (also used by the menu DEF engine). `FUN_00479700`
    = "token == keyword?"; `FUN_0047a410` = sscanf-family.
- **Recon files read:** the four briefs (B.r2/r6/r8/r11); ctor + Initialize
  decomps; `cls_0x5a5ba0.cpp` + its vftable; render/format/text/field bodies
  extracted this wave (paths above); `data/Resources/statpane.def`;
  `UI_METHOD_MAP.md` §5/§15; `TPlyrStatusBar_SPEC.md` (asset-citation style).
- **Status:** `forensics-complete` for the **Player/Page1** format (the default
  Stats tab). Page2 + the object-inspector formats (Character/Weapon/Armor/
  Default) are the SAME engine with different DEF blocks — fully reconstructable
  from `statpane.def` + the §6 interpreter, listed in §9 but not individually
  coord-tabulated. FIELD value-getter internals (`FUN_00547240`) UNCONFIRMED.
- **Errors to close (none — new panel).** No existing port shell for the retail
  data-driven design; `src/statpane.{cpp,h}` is the SUPERSEDED snapshot (below).

### Snapshot note (supplementary — superseded by retail)

`src/statpane.{cpp,h}` (1998/99) is a **completely different design**: a
hardcoded **skill-TREE** pane (`SkillTree[]`, expand/collapse gems, scroll
buttons, two `WriteTextShadow` columns at x=12 / x=138, `TEXT_POS_Y=23`,
`NUMLINES=6`, font `"goldfont"`, chrome bitmap `"statpane"` 168×128). The
retail Stats pane discarded all of this for a **DEF-driven object-inspector**:
`statpane.def` with `CLASS/FORMAT/POS/TAB/TEXT/FIELD/COLOR/BUTTON` tokens,
chrome bitmap **`"Stats"`** (188×306), rendered through GDI HDC text. The
snapshot's `TEXT_POS_Y`/`goldfont`/`NUMLINES` do **NOT** correspond — they are
artifacts of the dead skill-tree layout. Use the snapshot only to recognise the
class lineage (`TStatPane : TButtonPane`) and the field name `startline`
(retail's scroll/page index lives in `mbr_0x1a8`). **All coordinates,
layout, and the asset list below come from retail.**

---

## §1 — Overview

When the player picks the **Stats** tab (upper-region mode 1, `DAT_0065d1b8==1`;
B.r11 §3 dispatcher case 8), TSidePane shows TStatPane in the sidebar's upper
content region. The pane is a single parchment scroll (the `"Stats"` bitmap)
onto which the engine prints, as drop-shadowed text, a formatted character
sheet: **Name / Class / Lvl·Exp·Nxt / Hlth·Mana / a Stats·Skills header / six
trainable stats (Strn Cons Agil Rflx Mind Luck) each with a trainer FIELD, the
stat value, and its derived mods / Armor·Dmg**. Two small bitmap buttons
("page1"/"page2") toggle between the stats page and the skills page; six
"trainXxx" buttons (one per stat) raise that stat when the player has skill
points to spend.

The same class + DEF also drives the **object inspector** (Character / Weapon /
Armor / Default formats) used when the sidebar shows an examined object — the
engine picks the DEF block by the bound object's class name (`mbr_0x66`'s
`+0x48` class string). For the **Stats tab** the bound object is the player
(`DAT_00667fcc`) and the format name defaults to `"Page1"`.

Plain-language: it's a parchment "character sheet". A tiny scripting file
(`statpane.def`) says where each label and value goes; the code walks that
script and stamps text onto the scroll with a 1px black shadow. No bars, no
portrait of the player — just text (the Character/Weapon/Armor formats add an
`ICON` of the examined object).

**Visibility predicate:** shown iff upper-region mode == 1 AND the sidebar
upper region is presented (`DAT_0065d190`/`DAT_0065d194` gate, B.r11 §3). Draw
also early-outs if no bound object and no current player (`FUN_005475e0:115-118`).

---

## §2 — Asset roster

Chrome from `statspane.dat`; the target-portrait ring from `SpellIcons.dat`.
Dump cmd (header-only roster + single PNG):
`python3 tools/ui/dump_dat.py <…>/data/resources_unzipped/statspane.dat`.
Measured dims = the bitmap's own width/height words.

| asset | archive | entry | WxH (measured) | flags | role | source rect | cite |
|---|---|---|---|---|---|---|---|
| `Stats` | `statspane.dat` | `Stats` (idx 0) | **188×306** | `0x2` (BM_15BIT), kc=0 | the parchment-scroll chrome backdrop = whole pane | full `0,0,188,306` | dump `[0]`; render `:15-16` (`s_Stats_005e5674`) |
| `Stats` (dup) | `sidepane.dat` | `Stats` (idx 4) | 188×306 | `0x2` | identical copy bundled with the sidepane archive | full | dump |
| `RingT` | `SpellIcons.dat` | `RingT` (idx 55) | **48×48** | `0x2`, kc=0 | examined-object ring frame, drawn only in object-inspector formats (Character/Weapon/Armor) behind the `ICON` | full | Init `:49`; render `:883-887` |
| sidebar font | playscrn font table | (`DAT_0065abc4` index) | bitmap GDI HFONT | — | the text font; selected into both HDCs | n/a | render `:26-27,36-37`; WriteText `:24,33` |

**No bar/portrait/icon atlas.** Unlike TPlyrStatusBar, this pane has no Bars
atlas and draws no player portrait — it is text-on-parchment. The only sprite
blits are the `Stats` chrome (always) and, in object-inspector formats only, the
examined object's live render + the `RingT` frame.

**Chrome blit is opaque.** `Stats` has `flags=0x2` (15-bit, no alpha) and is
blitted with the pane drawmode `param_3` (render `:16`), preceded by a full-rect
`Box(0,0,188,306)` clear (render `:13`). It is NOT keyed — the parchment fills
the whole 188×306 pane.

**Font (UNCONFIRMED-2).** The retail render selects the font via global index
`DAT_0065abc4` (`render :26`, `FUN_00521c60(DAT_0065abc4)+0x38` = the GDI font
object; HDC table `DAT_0065b020[idx*4]` at format-render `:899-903`). This is a
shared sidebar font written once by the HUD font-init `FUN_00485870` and read by
sidepane/spellbook/statpane. The snapshot used `"goldfont"` (playscrn.dat idx 49,
which is a `TFontData`, not a bitmap). The retail global is **almost certainly
the same gold parchment face** but the literal index value was not read — see
§14.

---

## §3 — Coordinate frames & surfaces

### Pane placement

- TStatPane geometry from the ctor: **newwidth = 0xbc = 188**, **newheight =
  0x132 = 306**, newx = newy = 0 at construction (`ctor :6-14`:
  `mbr_0xc/0x1c=0xbc`, `mbr_0x10/0x20=0x132`, `mbr_0x4/0x14=0x1c4=452` is an
  unused secondary width field). The live x,y (`mbr_0x14`/`mbr_0x18` byte
  offsets = `param_1[5]/[6]`) are set by **TSidePane** when it places the pane
  into the upper content region — that on-screen origin is owned by the parent,
  not this class (see §14 UNCONFIRMED-3).
- The pane is **fixed 188×306**; it does NOT mirror (single instance, single
  side). No chip model applies — this is a normal pane, not a left/right chip.

### Surfaces (TWO HDCs — load-bearing)

Two GDI HDCs are set up before the DEF walk; **the Page1 stats text draws
DIRECTLY onto the parent draw surface, NOT into the mosaic**:

- **`mbr_0x5f`** (byte off `0x17c`) = HDC of the **parent draw surface**
  (`param_2`, stored in `mbr_0x61`; locked via its `(*+0xc)` at render `:20-22`).
  **ALL regular `TEXT`/`FIELD` text (WriteText `FUN_00546de0`) goes here**
  (WriteText draws into `param_1+0x17c` = `mbr_0x5f`; cursor MoveTo/SetTextAlign
  all use `param_1[0x5f]`). So the labels, values, and the 1px shadow are stamped
  on the same surface as the chrome.
- **`mbr_0x60`** (byte off `0x180`) = HDC of the **mosaic scratch surface**
  (`mbr_0x18c`=`param_1[99]`, allocated `newwidth×newheight` mode 0x200 in Init
  `:36`/`cls_0x5a5ba0.cpp:164`; locked at render `:30-32`). Used **only** by the
  **multipage / block** FIELD path via `DrawTextExA` (format-render `:569,600`) —
  i.e. the wrapped multi-line `objdesc` description in the Character/Weapon/Armor
  formats. The Player Page1 stats sheet does NOT touch the mosaic.

Both HDCs `SelectObject` the same sidebar font and `SetViewportOrgEx(hdc,
pane_x, pane_y)` (render `:26-28,36-38`).

Frame chain:

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer TL | root | (0,0) | final pixels | — |
| **sidepane content region** | screen | (owned by TSidePane) | TSidePane's upper-region rect; pane x,y = `mbr_0x14`/`mbr_0x18` | the whole 188×306 pane | render `:28,38` (viewport org reads pane x,y) |
| **pane-local** | sidepane region | TL | `(0,0)` | chrome blit + ALL Page1 text | render `:13-16`; WriteText via `mbr_0x5f` |
| **HDC-logical** | pane-local | viewport org = pane (x,y) | `SetViewportOrgEx(hdc, pane_x, pane_y)`; GDI logical coords map back to surface-local | DrawTextA/TextOutA coords | render `:28,38` |
| **mosaic-surface-local** | pane-local | TL | `(0,0)` (pane-sized 188×306) | ONLY multipage `objdesc` text (inspector formats) | render `:30-39` |

**Composition formula (final):** `screen = sidepane_region_origin + pane_local`.
Every Page1 text coordinate below is **pane-local** — directly the value used;
add the TSidePane region origin once to land on screen.

> **Viewport-org note.** `SetViewportOrgEx(hdc, pane_x, pane_y)` (render
> `:28,38`) is GDI plumbing so the cursor coords the DEF emits (POS/TAB) are
> **surface-local** while GDI accounts for the pane's position; do not treat the
> pane_x/pane_y as a per-text offset to add yourself. There is no scratch→pane
> blit for Page1 — the text is composited the moment it is drawn on the parent
> surface, then committed via `(*+0x68)` at render `:50-54`.

### Layout diagram (Player/Page1 — mosaic/pane-local, 188×306)

```
pane-local (mosaic surface), origin (0,0) top-left              188 wide
0                 28        56(stat val)  70(mods)              188
├─────────────────┬──────────┬────────────┬─────────────────────┤  0
│  "Stats" parchment chrome (188×306) fills entire pane           │
│                                                                 │
│  POS(28,62)  Name: <name aqua>                                  │ y=62
│  NEXTLINE    Class: <class aqua>                                │ y=74
│  NEXTLINE    Lvl: <lvl teal> Exp: <exp> Nxt: <nxt>             │ y=86
│  NEXTLINE    Hlth: <hp red>/<maxhp> Mana: <mp blue>/<maxmp>    │ y=98
│  NEXTLINE  TAB40  "Stats"(green)"/Skills"                       │ y=110
│  [BUTTON page1 @63,108 25×14] [BUTTON page2 @89,108 29×14]      │ y=108
│  NEXTLINE rows: per-stat (cursor x reset each NEXTLINE):        │
│    TAB12 <trainXX FIELD> TAB14 "Strn" TAB56 <val> TAB70 <mods> │ y=120
│    ... Cons(132) Agil(144) Rflx(156) Mind(168) Luck(180)       │
│    [BUTTON trainstrn @24,120 62×12] ... trainluck @24,180      │
│  NEXTLINE  Armor: <armor yellow>  Dmg: <damage yellow>         │ y=192
│                                                                 │
└─────────────────────────────────────────────────────────────────┘ 306
```

Y values are cumulative from `POS 28 62` adding `LINEHEIGHT 12` per `NEXTLINE`.
X values are `lineStartX(28) + TAB n` (TAB is absolute-from-line-start, see §6).

---

## §4 — Static element layout

Single instance, no mirror. "space" is **mosaic/pane-local** (== final after
adding the TSidePane region origin). Buttons are the only fixed-bitmap elements;
all text is data-driven (§8). Coords are literals from `statpane.def`
(Player/Page1 block, `data/Resources/statpane.def:6-156`).

| element | space | (x,y) | (w,h) | bitmap (up/down) | cite (def line) |
|---|---|---|---|---|---|
| `Stats` chrome | pane-local | (0,0) | 188×306 | `Stats` (statspane.dat) | render `:13-16` |
| BUTTON `page1` | pane-local | (63,108) | 25×14 | (DEF widget) | def `:11` |
| BUTTON `page2` | pane-local | (89,108) | 29×14 | | def `:12` |
| BUTTON `trainstrn` | pane-local | (24,120) | 62×12 | | def `:13` |
| BUTTON `traincons` | pane-local | (24,132) | 62×12 | | def `:14` |
| BUTTON `trainagil` | pane-local | (24,144) | 62×12 | | def `:15` |
| BUTTON `trainrflx` | pane-local | (24,156) | 62×12 | | def `:16` |
| BUTTON `trainmind` | pane-local | (24,168) | 62×12 | | def `:17` |
| BUTTON `trainluck` | pane-local | (24,180) | 62×12 | | def `:18` |

> Buttons are created at parse time by the format interpreter: `BUTTON` token →
> `FUN_0042c600(name, x, y, w, h, …)` then `FUN_00436790(btn)` (format-render
> `:910-930`). The `(x,y,w,h)` come straight from the four `%d` after the name.
> Their up/down sprites resolve by name through the button widget's own loader
> (the snapshot's `"statsup"/"statsdown"` etc. are the closest predecessor but
> the retail page1/page2/trainXxx button art names were not separately measured
> — UNCONFIRMED-4). The interpreter only re-adds buttons on the first format
> pass (`local_1a0` guard, format-render `:188-192,919`).

Object-inspector formats (Character/Weapon/Armor/Default) add an `ICON`
(`def :320,389,465,541`) — the examined object's live render at the cursor,
framed by `RingT`; and `Prev`/`Next` buttons at (27,250)/(143,250). Not used by
the player Stats tab.

---

## §5 — Draw order / composition

Top-level paint is **slot 84 `FUN_005491c0`** (called from slot 80
`FUN_00549010` only when `mbr_0x50 != 0`, i.e. open/dirty — `cls_0x5a5ba0.cpp:254`).
Ordered sequence (`statpane_render_5491c0.cpp`):

1. `Box(0,0,188,306, color=0, zpos=0xffff, normal=0, drawmode=param_3)` on the
   **parent draw surface** `param_2` (`:13`) — clear the pane rect.
2. Resolve + blit chrome: `h = FUN_0046d710("Stats")`; `FUN_004bd680(0,0,h,
   param_3,0)` (`:15-16`) — opaque parchment at pane-local (0,0).
3. `this->vtable[0x90/4=36]()` (`:17`) — base hook (TButtonPane redraw-buttons /
   pre-pass; inherited).
4. Bind the parent draw surface as the secondary HDC: store `param_2` →
   `mbr_0x61`; get its HDC via `(*+0xc)` into `mbr_0x5f`; **SelectObject** the
   sidebar font; **SetViewportOrgEx** to pane (x,y) (`:18-29`).
5. Bind the mosaic surface HDC: get `(*+0xc)` of `mbr_0x18c` (=`param_1[99]`)
   into `mbr_0x60`; SelectObject same font; SetViewportOrgEx to pane (x,y)
   (`:30-39`).
6. Allocate the **2 KB working text buffer**: `mbr_0x65 = 0x800`;
   `mbr_0x64 = malloc(0x800)` (`:40-42`).
7. **`FUN_005475e0(this)`** — run the DEF format interpreter (`:43`). This walks
   `statpane.def`, finds the matching `CLASS/FORMAT` block, and emits all
   Page1 text + buttons onto the **parent-surface HDC** (`mbr_0x5f`); only the
   multipage `objdesc` of inspector formats uses the mosaic HDC (`mbr_0x60`).
   (Full algorithm in §6.)
8. `free(mbr_0x64)` (`:44`).
9. Resolve/commit both HDCs back to their surfaces via `(*+0x68)` (`:45-54`).
10. `this->vtable[0x2c/4=11](0)` (`:55`) — base post-pass (button compose).
11. `FUN_00435cb0(param_2)` (`:56`) — intermediate-class finalize on the parent
    surface.
12. If `param_2 == PTR_DAT_005d79e0` (drawing straight to the display):
    `FUN_004aacb0(pane_x, pane_y, pane_w, pane_h, 6)` (`:57-59`) — run the
    deferred blit-effect/dirty-rect pass over the pane rect (UI_METHOD_MAP §8.5).

So: **chrome → text (DEF walk) → button compose → effect flush.** The mosaic
surface holds the text; the chrome is drawn directly on the parent; text is GDI
into the mosaic which is committed and composited as part of the surface.

---

## §6 — Algorithms

### 6a. `FUN_005475e0` — DEF format interpreter (the layout engine)

Signature: `int FUN_005475e0(TStatPane* this)`. Drives a tokenizer over the
loaded `statpane.def` buffer (`mbr_0x1a0`=`param_1[0x68]`, length `mbr_0x1a4`=
`[0x69]`). Maintains a text cursor in **mosaic/pane-local** coords. Pseudocode
(literals cited to `statpane_formatrender_5475e0.cpp`):

```
# --- init cursor & defaults (:106-113) ---
lineStartX = 0x14 = 20      # local_21c  (reset target for x each NEXTLINE)
cursorX    = 0x14 = 20      # local_220
cursorY    = 0x14 = 20      # local_224
lineHeight = 0x0c = 12      # local_1a4  (DEF default; LINEHEIGHT overrides)
lineWrap   = -1             # local_1c0  (none; LINEWRAP <n> sets a wrap width)
color      = 0xffffff       # local_1d8  (white; the DEF's first nameless COLOR
                            #             "255 240 215" overrides via default slot)
firstPass  = 1              # local_1a0  (controls one-time button creation)

# --- if no bound object yet, bind current player (:115-122) ---
if this.mbr_0x66 == 0:                       # no bound object
    if DAT_00667fcc == 0: return 0           # no player -> nothing to draw
    this->vtable[0xb8](DAT_00667fcc)         # SetObject(player) (slot 184, §6c)

if this.defBuffer == 0:                       # mbr_0x68 (:124)
    WriteError(20,20,"Invalid Stat Layout"); return 0

tokenize(defBuffer .. defBuffer+len)          # FUN_00478720 (:135)

# --- find the matching CLASS [+ FORMAT] block (:152-199) ---
loop tokens until EOF(token type 10):
    expect "CLASS" else error
    read class-name token (type 2 = quoted string)
    if class-name == "default"  OR  class-name == this.boundObjClassName:   # (:170-173)
        # optional FORMAT match:
        match = true
        if next token == "FORMAT":                           # s_FORMAT (:179)
            read format-name; match = (format-name == this.formatName)  # mbr_0x67
        if match:
            if blockStart == this.mbr_0x6a: firstPass = 0     # already drawn this block
            this.mbr_0x6a = blockStart
            break       # enter this block's body
    skip to next block

expect "BEGIN" else error (:201)

# --- body token loop (:225-935) — one branch per keyword ---
while token != "END":                                        # s_END 0x5e54e0
    switch token:
      "BUTTON":        # (:247) parse name + 4 ints; first pass only:
          name, bx, by, bw, bh = sscanf("%20s %d %d %d %d")  # (:910)
          if firstPass: btn = FUN_0042c600(name,bx,by,bw,bh,...,0x100000,...);
                        FUN_00436790(btn)                     # (:919-930)
      "POS":           # (:699) "POS %d %d" -> cursorX,cursorY ; lineStartX=cursorX
                       MoveTo(hdc5f, cursorX, cursorY)        # (:699-721)
      "TAB":           # token "TAB" @0x5e5560, fmt "%d" @0x5e5564 (:643)
                       cursorX = lineStartX + n               # (:664)  TAB is
                       # RELATIVE to the current line's start x, not absolute
      "MOVE":          # token "MOVE" @0x5e5550, fmt "%d %d" @0x5e5558 (:670)
                       cursorX += n1 ; cursorY += n2          # (:691-693)
                       lineStartX = cursorX                   # (:693)
      "NEXTLINE":      # token @0x5e5568 (:634) cursorY += lineHeight ; cursorX = lineStartX
                       MoveTo(hdc5f, lineStartX, cursorY)     # (:636-638)
      "LINEHEIGHT":    # (:838) "LINEHEIGHT %d" -> lineHeight
      "LINEWRAP":      # (:804) "none" -> -1 ; else "%d" -> lineWrap
      "COLOR":         # (:754) [name] r g b  -> register a named color; bare
                       #  COLOR sets the default text color (:799 local_1d8)
      "TEXT":          # (:226 via DAT_005e54e0 loop head; the text branch)
                       # optional align word (center/right/grayable/...) then
                       # a quoted/color-prefixed string -> WriteText (§6b)
      "FIELD":         # (:266,280) optional modifiers (right/center/block/
                       # multipage/grayable/prevbutton/nextbutton) + a field
                       # name (and optional inline label) -> resolve value
                       # (FUN_00547240) -> WriteText/WriteField (§6b)
    advance to next token (:934)
```

**Cursor model (load-bearing).**
- `POS x y` sets the absolute cursor AND the line-start-x.
  (`statpane.def:29` `POS 28 62`.)
- `NEXTLINE` drops the cursor by `lineHeight` and snaps x back to line-start.
- `TAB n` (1-arg) sets `cursorX = lineStartX + n` — TAB is **relative to the
  current line's start x**, not absolute. So Page1's `POS 28 62` makes
  `TAB 12 → x=40`, `TAB 56 → x=84`, `TAB 70 → x=98` (`:664`).
- After each `TEXT`/`FIELD`, the cursor advances by the drawn text width
  (WriteText calls `GetCurrentPositionEx`; WriteField adds `DrawTextA` return).
- **LINEWRAP**: if set (>0) and a run would exceed `lineWrap`, the engine drops
  to a new line first (`:482-498` — measures with `GetTextExtentExPointA`).

**Text alignment** is GDI `SetTextAlign` on the cursor HDC: bare/`block`
→ `TA_LEFT(0)` after each field (`:304,307`); `center` → `3` (TA_CENTER|...,
`:312-313`); `right` → `7` (TA_RIGHT|TA_NOUPDATECP, `:316-319`).

### 6b. Text helpers (3-pass black drop shadow, then color)

Both write into the **parent-surface HDC** at `mbr_0x17c` (= `param_1[0x5f]` —
the `param_2` draw surface that also holds the chrome; see §3). Color is 3 bytes
packed `(b0<<16)|(b1<<8)|b2` from the caller's color struct (`WriteText :33`,
`WriteField :45`).

**`FUN_00546de0(this, x, y, text, colorPtr, &outX, &outY)` — WriteText (TEXT/inline FIELD):**
```
n = strlen(text)
SetTextColor(black); SetBkMode(TRANSPARENT)             # (:24-25)
TextOutA(x+1, y+1, text)     # shadow pass 1            # (:27-28)
TextOutA(x+2, y+1, text)     # shadow pass 2 (right)    # (:29-30)
TextOutA(x+1, y+2, text)     # shadow pass 3 (down)     # (:31-32)
SetTextColor(packed RGB)                                # (:33)
TextOutA(x,   y,   text)     # colored base             # (:34-35)
outX,outY = GetCurrentPositionEx()  # advance cursor    # (:36-42)
```

**`FUN_00546f40(this, x, y, w, h, text, colorPtr, _, &widthAccum)` — WriteField (rect):**
```
SetTextAlign(0); SetTextColor(black); SetBkMode(TRANSPARENT)   # (:25-27)
rect=(x+1,y+1,x+1+w,y+1+h); DrawTextA(text, rect, 0x2810)      # shadow (:28-34)
rect=(x+2,y+1,...);          DrawTextA(text, rect, 0x2810)      # shadow (:35-39)
rect=(x+1,y+2,...);          DrawTextA(text, rect, 0x2810)      # shadow (:40-44)
SetTextColor(packed RGB)
rect=(x,y,x+w,y+h); ret=DrawTextA(text, rect, 0x2810)          # colored (:45-50)
*widthAccum += ret                                             # (:51-53)
MoveTo(x+1,y+1)                                               # (:54)
```

**Shadow shape:** colored glyph at `(x,y)`; black copies at `(+1,+1)`,
`(+2,+1)`, `(+1,+2)` — i.e. a 1px black shadow biased **right + down**
(the diagonal `(+2,+1)`/`(+1,+2)` pair gives a slightly heavier SE shadow than
the 1px font shadow in TPlyrStatusBar). This is the **intended** look — see §7.

`0x2810` = `DT_SINGLELINE(0x20)|DT_NOPREFIX(0x800)|0x2000` (base format,
left/top by default; alignment comes from the prior `SetTextAlign`).

### 6c. `FUN_005496a0` — SetObject / SetFormat (slot 184)

`SetObject(this, obj)` (`cls_0x5a5ba0.cpp:292`):
1. If a previous object is bound (`mbr_0x198`), call its `(*+0xa0)()` (release).
2. `mbr_0x198 = obj`; free old format-name string (`mbr_0x19c`).
3. `strdup("page1")` into `mbr_0x19c` — **the default format name is `"page1"`**
   (`s_page1_005e570c/5714`, `:314-346`). (This is `mbr_0x67`/`formatName` the
   interpreter matches against the DEF `FORMAT "Page1"` block, case-insensitive
   via the tokenizer compare.)
4. Mark dirty: `this->vtable[0x44/4=17]()` (`:347`).
5. Call `obj->(*+0x9c)(1)` (notify bound).

Init (`FUN_00546b50`) calls this last with `DAT_00667fcc` (the player) so the
Stats tab opens bound to the player on the Page1 format.

---

## §7 — Effects & shadows

- **Text shadow:** the 3-pass black + colored pattern in §6b. NOT the
  `font-flag 0x400` path of `FUN_004be2b0` — TStatPane uses raw GDI
  `TextOutA`/`DrawTextA` with manual offset passes. Reproduce as: colored glyphs
  + 3 black copies at `(+1,+1)`, `(+2,+1)`, `(+1,+2)`.
- **No sprite drop shadow** for the chrome (opaque blit).
- **Object-inspector ICON** (Character/Weapon/Armor formats only): the examined
  object's live render is stamped (`FUN_004bd680(x,y,render,0x100,0)`,
  format-render `:881`) with the `RingT` frame composited via a `FUN_00438d80`
  blit-effect descriptor + `(*+0x5c)` (`:883-887`). Not used by the player Stats
  tab.
- **Chroma key:** chrome `Stats` is opaque (no key). Text is GDI into the mosaic
  with `SetBkMode(TRANSPARENT)` — no magenta key, **no pink-halo risk** (the
  TPlyrStatusBar pink-halo bug does not apply here; this path never clears a
  cell to magenta — see §11).

---

## §8 — Text rendering (Player/Page1)

Cells are **mosaic/pane-local**; add TSidePane region origin for screen. All
strings come from `statpane.def` (CLASS "Player" FORMAT "Page1", lines 6-156).
Y is cumulative from `POS 28 62` with `LINEHEIGHT 12`. **v-align = top**
(GDI DT_TOP, never DT_VCENTER); **h-align** as noted (the `right` modifier sets
`SetTextAlign(TA_RIGHT)` so the text ENDS at the cursor x). Colors are the DEF
`COLOR` palette (RGB):

| COLOR name | RGB | def line |
|---|---|---|
| (default) | 255,240,215 (cream) | def `:20` |
| aqua | 113,187,255 | def `:21` |
| teal | 95,228,193 | def `:22` |
| red | 255,150,150 | def `:24` |
| blue | 110,160,255 | def `:25` |
| green | 6,221,0 | def `:26` |
| yellow | 255,247,73 | def `:27` |

| line (y) | content (label cream, value colored) | x | h-align | format / source value | cite |
|---|---|---|---|---|---|
| 62 | `Name: ` + FIELD `name` (aqua) | 28 | left | player display name | def `:30-31` |
| 74 | `Class: ` + FIELD `class` (aqua) | 28 | left | player class name | def `:36-37` |
| 86 | `Lvl: `<teal level>` Exp: `<teal exp>` Nxt: `<teal nextexp> | 28 | left | `%d` each | def `:40-44` |
| 98 | `Hlth: `<red health>`/`<red maxhealth>` Mana: `<blue mana>`/`<blue maxmana> | 28 | left | `%d` | def `:46-53` |
| 110 | `Stats`(green)`/Skills`(cream) at TAB40 → x=68 | 68 | left | static | def `:56-58` |
| 120 | FIELD right `trainstrn` @TAB12(x40) · `Strn`@TAB14(x42) · FIELD right `strn`@TAB56(x84) · `Dmg`+FIELD`strnmod1`,` Hit`+`strnmod2`@TAB70(x98) | per-TAB | mixed | `%d` | def `:60-69` |
| 132 | …`Cons` row: `traincons` `Cons` `cons` `Hlth`+`consmod1` ` Ftg`+`consmod2` | per-TAB | mixed | `%d` | def `:71-80` |
| 144 | …`Agil` row: `trainagil` `Agil` `agil` `Atk`+`agilmod1` | per-TAB | mixed | `%d` | def `:82-90` |
| 156 | …`Rflx` row: `trainrflx` `Rflx` `rflx` `Def`+`rflxmod1` | per-TAB | mixed | `%d` | def `:92-100` |
| 168 | …`Mind` row: `trainmind` `Mind` `mind` `Mana `+`mindmod1` ` Spl `+`mindmod2` | per-TAB | mixed | `%d` | def `:102-111` |
| 180 | …`Luck` row: `trainluck` `Luck` `luck` `Rolls `+`luckmod1` | per-TAB | mixed | `%d` | def `:113-121` |
| 192 | `Armor: `<yellow armor>` Dmg: `<yellow damage> | 28 | left | `%d` | def `:123-127` |

**Per-stat row x-stops** (lineStartX=28 because `POS 28 62`; TAB is relative):
`TAB 12 → x=40` (train FIELD, right-aligned), `TAB 14 → x=42` (stat label),
`TAB 56 → x=84` (stat value, right-aligned), `TAB 70 → x=98` (mods, left).

**FIELD with inline label** (e.g. `FIELD "Dmg" strnmod1`, def `:68`): the quoted
string is a label prefix printed immediately before the resolved value, both in
the current color.

**h-align detail:** `FIELD right name` (def `:62` etc.) sets `SetTextAlign(7)` =
`TA_RIGHT|TA_NOUPDATECP|TA_BASELINE`-ish so the digits END at the cursor x
(right-justified column). After a non-right FIELD/TEXT, align resets to left
(`:304`).

**Visual-anchor check:** the right-aligned stat values (TAB56→x84) and the train
FIELDs (TAB12→x40, right) must END at those x's, not start there — confirm the
reconstructed column right-edges land at x=84/x=40. The cream labels start
(left) at their TAB x. (No centered-over-element text in Page1; the
object-inspector `ICON` formats DO center a name over the icon — verify there if
porting those.)

> **Do NOT specify a font baseline.** GDI DT_TOP/TextOutA top-aligned; the
> shared `font.cpp` draw handles the in-cell vertical baseline once
> (NOMENCLATURE §2). Lines stack DOWN by `lineHeight=12`.

---

## §9 — Animation & dynamic behavior

- **No tweens / no per-tick animation.** TStatPane is a static text sheet; it
  redraws only when dirty.
- **Dirty model:** slot 80 `FUN_00549010` runs the paint (slot 84) + base
  compose (slot 44) **only if `mbr_0x50 != 0`** (`cls_0x5a5ba0.cpp:254`). Dirty
  is set by SetObject (`:347`), MouseClick page-toggle (`:272`), and the train
  buttons. The interpreter's `firstPass`/`mbr_0x6a` guard (block-start cache)
  avoids re-creating buttons on repaint (§6a). Prefer a monotonic version
  counter in the port over the bool (NOMENCLATURE §6, [[feedback-versions-over-flags]]).
- **Multi-format state machine (CLASS/FORMAT selection):** the SAME pane renders
  6 different layouts by binding a different object + format name:
  - **Player / Page1** — the stats sheet (default; SetObject sets `"page1"`).
  - **Player / Page2** — the skills sheet (Invoke/Hands/Sword/Bludgeon/Axes/
    Bows/Lockpick, def `:158-295`); selected when the page2 button toggles
    `formatName` to `"page2"` (MouseClick slot 100 path — UNCONFIRMED-5 the exact
    toggle write).
  - **Character / Weapon / Armor / Default** — object-inspector layouts shown
    when the sidebar binds an examined object (these add `ICON` + Prev/Next).
  The block is chosen by matching the bound object's class name (DEF `CLASS
  "Player"|"Weapon"|...`) and, within Player, the format name; falling back to
  `CLASS "Default"` (format-render `:170`).

---

## §10 — Input & dispatch

Interactive via DEF-created buttons. Hit rects = the BUTTON rects in §4 (the
button widget owns its own hit-test; TButtonPane intermediate pumps callbacks).

| control | pane-local rect | action |
|---|---|---|
| page1 | (63,108,25,14) | select Page1 (stats) — sets formatName "page1", dirty |
| page2 | (89,108,29,14) | select Page2 (skills) — sets formatName "page2", dirty |
| trainstrn..trainluck | (24, 120..180, 62,12) | spend a skill point on that stat (greyed when none / maxed — see grey logic below) |

**MouseClick slot 100 `FUN_005493b0(this, button, x, y)`** (`cls_0x5a5ba0.cpp:265`):
- `button==2` → re-run SetObject (rebind/refresh) + base MouseClick `(2,…)`.
- `button==4` (left-down) within pane bounds → focus pulse
  (`FUN_0043a100/140/170/240`) + base `(*+0x40 → slot16)` + base MouseClick.
- Always falls through to `cls_0x5b93c4::virt_meth_0x436530(this, button, x)` =
  the TButtonPane callback pump (routes to each train/page button's handler).

The train-button **grey/active** state in Page1 is driven in the FIELD branch by
the player's spendable-points fields `mbr_0x6b`/`mbr_0x6c`: a `trainXxx` FIELD
draws GREEN (6,221,0) when points are available and the stat isn't maxed, else
grey `0x3c3c3c` (format-render `:508-543`).

---

## §11 — Retail bugs NOT to reproduce

- **None specific to this pane.** The text path uses opaque GDI with
  `SetBkMode(TRANSPARENT)` into a non-keyed mosaic, so the **pink-halo bug does
  NOT occur here** (unlike TPlyrStatusBar). Reproduce the intended look: cream/
  colored glyphs with a 1px SE black shadow, no magenta fringe.
- The DEF default `red` was hand-bumped from `255,43,43` to `255,150,150`
  ("old red was hard to read", def `:23-24`) — use the **bumped** value; it is
  intentional, not a bug.

---

## §12 — Reconstruction pseudocode

```cpp
void TStatPane::Draw(TSurface* dst, int drawmode) {           // slot 84
  // 1. clear + chrome
  dst->Box(0,0, 188,306, /*color*/0, /*z*/0xffff, /*n*/0, drawmode);
  Bitmap stats = ResolveBitmap("Stats");                       // statspane.dat, 188x306
  dst->DrawBitmap(stats, 0, 0, /*opaque*/);                    // pane-local (0,0)

  // 2. set up GDI text targets, select sidebar font.
  //    dstHDC   = the PARENT surface HDC (mbr_0x5f) — holds chrome + Page1 text
  //    mosaicHDC= the scratch surface HDC (mbr_0x60) — multipage objdesc ONLY
  HDC dstHDC    = dst->HDC();    SelectFont(dstHDC, sidebarFont);
  HDC mosaicHDC = mosaic->HDC(); SelectFont(mosaicHDC, sidebarFont);
  // (viewport orgs are GDI plumbing; emit cursor coords in surface-local)

  // 3. walk the DEF
  textBuf = malloc(0x800);
  RenderFormat();   // FUN_005475e0 (§6a) — Page1 text -> dstHDC (parent surface)
  free(textBuf);

  // 4. commit + compose
  mosaic->Commit(); dst->Commit();
  TButtonPane::ComposeButtons();   // slot 11
  Intermediate::Finalize(dst);     // FUN_00435cb0
  if (dst == display) FlushBlitEffects(paneX,paneY,188,306, 6);
}

// FUN_005475e0 — see §6a. Cursor (cx,cy) in pane-local; lineStartX; lineHeight=12.
// For each TEXT run:  WriteText(cx, cy, str, color);  (cx,cy)=GetCurPos()
// For each FIELD:      val = ResolveField(boundObj, fieldName);
//                      if (right) SetTextAlign(RIGHT);
//                      WriteText/WriteField(cx, cy, label+val, color);
// POS x y     -> cx=lineStartX=x; cy=y
// TAB n       -> cx = lineStartX + n
// NEXTLINE    -> cy += lineHeight; cx = lineStartX
// LINEHEIGHT n-> lineHeight = n
// COLOR ...   -> palette / default color
// BUTTON name x y w h -> (first pass) create button widget

// WriteText(x,y,text,rgb):                                    // FUN_00546de0 §6b
//   black @ (x+1,y+1),(x+2,y+1),(x+1,y+2);  rgb @ (x,y)
```

`SetObject(obj)` (slot 184): bind obj, formatName="page1", mark dirty.
`MouseClick`: route to button pump; page1/page2 set formatName + dirty; train
buttons spend points + dirty.

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `Box(0,0,w,h,…)` (render `:13`) | `dst->Box` / `Renderer->FillRectToTarget` | surface/renderer |
| `FUN_004bd680("Stats",0,0,opaque)` | `Renderer->DrawBitmapToTarget(stats,0,0)` | renderer |
| GDI `TextOutA`/`DrawTextA` + 3-pass black shadow (`FUN_00546de0/f40`) | `DrawTextShadowedToTarget(atlas,text,x,y,w,ETextAlign,r,g,b,...)` — but note the retail shadow is **3 passes at (+1,+1)/(+2,+1)/(+1,+2)**, NOT the font-flag-0x400 single shadow; may need a `DrawTextShadowed3ToTarget` variant or pass the offset list | font.cpp |
| `SetTextAlign(LEFT/CENTER/RIGHT)` | `ETextAlign` arg | font.cpp |
| DEF tokenizer (`FUN_00478720` family) | reuse/port the shared script tokenizer (also needed by the menu DEF engine) | new util |
| `RingT` frame + live ICON render (inspector formats) | `DrawBitmapSubrectToTarget` + object live-render surface | renderer (deferred — Stats tab doesn't need it) |
| mosaic surface compose → HUD | `DrawSurfaceToTarget` | renderer |

**Needs new primitive:** a **DEF "object-sheet" interpreter** for the
`CLASS/FORMAT/POS/TAB/TEXT/FIELD/COLOR/BUTTON` mini-language (distinct from the
menu DEF widget engine — different grammar). It needs a **FIELD value resolver**
that maps field names (`name/class/level/health/strn/strnmod1/…`) to player/
object getters. Possibly also a **3-offset text-shadow** variant in font.cpp.

---

## §14 — UNCONFIRMED / open questions

1. **`FUN_00547240` FIELD value-getter internals.** The mapping from DEF field
   name (`strn`, `strnmod1`, `health`, `armor`, …) to the player/object getter
   is not extracted; the value-format helper `FUN_00547240` (format-render
   `:450`) + the inspector field table were not traced. *Impact:* without it the
   port shows labels but blank/garbage values. *Resolve:* `DecompileAddr.java
   0x00547240` and the field-name dispatch table it indexes; cross-ref
   `src/player.h` `GetObjStat`/`Skill` getters.
2. **Sidebar font identity (`DAT_0065abc4`).** Confirmed it is a shared
   global font index selected into the HDC (render `:26`), written once by
   `FUN_00485870`. NOT verified to be literally playscrn.dat `"goldfont"` (idx
   49). *Impact:* wrong face/metrics → text mis-sized. *Resolve:* read the
   stored value at the `FUN_00485870` write site (`0x485dcf`) and match against
   the playscrn font-table init.
3. **On-screen pane origin.** TStatPane is 188×306 at pane-local (0,0); its
   live screen (x,y) is set by **TSidePane** placing it in the upper content
   region (render reads `mbr_0x14/0x18` as the viewport org). The TSidePane
   region rect was not traced here. *Impact:* whole pane offset. *Resolve:*
   trace TSidePane (`cls_0x5a53ec`) content-region placement +
   `FUN_0053cab0`/`FUN_0053cb40` region setters (B.r11 coord-note 3).
4. **Page1/Page2/trainXxx button sprite names + dims.** The buttons are created
   by name from the DEF, but the retail up/down bitmap names + sizes were not
   measured (the snapshot's `statsup/statsdown/statmin/statmax` 64×19/16×14 are
   predecessors, present in playscrn.dat, but the retail page/train buttons may
   use different art). *Impact:* missing/placeholder button graphics. *Resolve:*
   trace the button widget's loader for the page1/page2/trainXxx names, then
   `dump_dat` the matching archive.
5. **MouseClick page-toggle write.** The exact field write that flips
   `formatName` "page1"↔"page2" on the page2 button click was not isolated
   (slot 100 routes through the TButtonPane callback pump to per-button
   handlers). *Impact:* page switch may not work. *Resolve:* extract the page1/
   page2 button callbacks registered in the BUTTON branch (format-render
   `:919-930`) and the handler that sets `mbr_0x67`.
6. **Default text color slot.** The DEF's first nameless `COLOR 255 240 215`
   (cream) is the default; the interpreter's color default is `0xffffff` until
   that token (format-render `:111`). Confirmed cream is the intended label
   color from the def, but the precise default-vs-named color-slot indexing in
   the COLOR branch (`:754-801`) was read at a high level only. *Impact:* label
   tint slightly off (white vs cream). *Resolve:* fully trace the COLOR branch
   palette store.
