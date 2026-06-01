# TBottomBarPane — Reconstruction SPEC (Bottom command/status bar)

> Pixel-faithful reconstruction spec for the in-game HUD **bottom bar** — the
> full-width dark chrome strip across the bottom of the play screen that backs
> the quick-spell ring, the potion/key (BarInv) slots, and the action arrows.
> Follow the FORENSICS_PROTOCOL §0–§14 template. All coordinates name their
> frame; every literal is cited `file:line` with hex(=dec).

---

## §0 — Sources & status

**Class:** `TBottomBarPane` = `cls_0x5a5808` (vtable @ `0x5a5808`); global instance @ `0x65b638`.
Naming ~85% (could be `TUtilityBarPane`/`TActionBarPane`) — see B.r5; class identity itself is 98%.

**Key method addresses (vtable on `cls_0x5a5808`):**
| role | addr | recon |
|---|---|---|
| Initialize (slot 0) | `0x52c780` | `recon/discovered/FUN_0052c780_BottomBar_init.cpp`; `recon/ghidra/cls_0x5a5808.cpp:106-121` |
| ctor | `0x487da0` | `recon/discovered/cls_0x5a5808_TBottomBarPane_ctor_487da0.cpp`; `cls_0x5a5808.cpp:27-48` |
| **Draw** (slot 20, `0x444fc0`-family) | `0x52c800` | `cls_0x5a5808.cpp:154-170` |
| **DrawChrome / DrawBackground** (slot 84, `0x52c880`) | `0x52c880` | `cls_0x5a5808.cpp:174-190` |
| Relayout/commit (slot 18) | `0x52c910` | `cls_0x5a5808.cpp:194-205` |
| SetRect (slot ?) | `0x52c930` | `cls_0x5a5808.cpp:209-223` |
| OnShow flags (slot 13) | `0x52c7e0` | `cls_0x5a5808.cpp:138-150` |
| Close (slot 1) | `0x52c7c0` | `cls_0x5a5808.cpp:125-134` |

**Recon files read:**
- `recon/ghidra/cls_0x5a5808.cpp` (full class — ctor, dtor, Initialize, Draw `0x52c800`, chrome `0x52c880`, relayout, SetRect)
- `recon/discovered/FUN_0052c780_BottomBar_init.cpp`, `recon/discovered/cls_0x5a5808_TBottomBarPane_ctor_487da0.cpp`
- `recon/discovered/cls_0x5a5468_TBottomPane_Initialize_52d8a0.cpp` (the CONTAINER — trivial)
- `recon/ghidra/cls_0x5a4494.cpp` (TPane base — struct field map: rect at `mbr_0x4/0x8/0xc/0x10` live, `mbr_0x14/0x18/0x1c/0x20` template)
- `recon/ghidra/cls_0x5a68d8.cpp` (surface class — `meth_0x4bd5e0` stretch-blit, `meth_0x4bd680` sprite-stamp; both bodies are Ghidra thunks, arg meaning from call site)
- `recon/ghidra/cls_0x5a5658.cpp` (map/automap class with **Ghidra-merged TBarInvPane methods** — `virt_meth_0x52ca70(surface)` is the BarInv draw misfiled here; per forensics README roster)
- `docs/ui/forensics/README.md:52` (roster note: `cls_0x5a5658` = MapSidebarPane, beware merged TBarInvPane methods)
- `recon/classes/_data.txt:112998-113004` (string anchors `UtilityBar`/`BarEndCap`)

**Prior briefs:** B.r5 (`docs/ui/briefs/B_r5_bottom_panes.md`), B.r3 (`docs/ui/briefs/B_r3_playscreen_panes.md`), `recon/discovered/port_status/TBottomBarPane.md`.

**Method-map references:** UI_METHOD_MAP §4 (`FUN_004bd680` sprite stamp = `meth_0x4bd680`), §8.5 (`FUN_004aacb0_BlitEffect_Iterate`), §15a (surface vtable). The stretch-blit `meth_0x4bd5e0` (7-arg subrect→stretch on the surface class) is NOT yet in the method map → flagged §14 as a method-map gap.

**Status:** `forensics-complete` for the **bar chrome** (UtilityBar fill + BarEndCap). The bar HOSTS but does not own the quick-spell ring, BarInv slots, or arrow buttons — those belong to the sibling panes (relationship documented in §1/§5; their pixel layout is out of scope for THIS spec).

**Errors to close (no existing port):** none — NEW retail class, no `src/` shell.

**Snapshot note (supplementary — superseded by retail):** No pre-release counterpart exists. `src/revtypes.h:1167` forward-declares only `TBottomPane` (the container, `cls_0x5a5468`), not `TBottomBarPane`. `grep` of `src/` for `UtilityBar`/`BarEndCap`/`TBottomBarPane` → no hits. This is a NEW retail class; retail is the sole authority. Nothing to reuse.

---

## §1 — Overview

The **bottom bar** is the full-width (640px @ Classic), 60px-tall dark stone/metal
chrome strip pinned to the bottom edge of the play screen. It is the visual
backdrop for the bottom HUD cluster. `TBottomBarPane` itself draws only the
chrome (a stretched `UtilityBar` texture + a `BarEndCap` trim on the right). Its
`Draw` then drives the two sibling panes that render their controls **on top of**
the same surface: the **quick-spell ring** (`TQuickSpellPane` @ `0x65c6f8`, the
Arrow* graphics) and the **potion/key slot row** (`TBarInvPane` `cls_0x5a56d4` @
`0x65b028`).

> Ghidra-merge caveat: the decomp shows the BarInv draw call cast to
> `cls_0x5a5658::virt_meth_0x52ca70`. `cls_0x5a5658` is actually the map/automap
> class; per the forensics README it has **TBarInvPane methods merged into it**.
> The INSTANCE `&DAT_0065b028` is the BarInv pane (`cls_0x5a56d4`, B.r5) — that
> is the source of truth; the `cls_0x5a5658` label on the method is a merge
> artifact, not a real type for this instance.

**Instances:** exactly one (`0x65b638`); not mirrored.

**When shown:** part of the always-on play-screen HUD; constructed in
`TPlayScreen::Initialize` (B.r5 call site `0x47ac72`). Visibility follows the
TPane base `mbr_0x50` "visible" flag (`cls_0x5a5808.cpp:159,165`) and the
container `TBottomPane`. No independent show/hide predicate beyond the base.

**Plain language:** Picture the bottom 60 pixels of the screen filled by one
horizontal dark textured plate that stretches to whatever the window width is,
capped on its far-right edge by a 10px vertical trim piece. The spell ring and
the potion boxes are stamped over that plate by their own panes — the bottom bar
is the plate, not the buttons.

**Relationship to the game log / console (TConsolePane / TTextBar):** SEPARATE.
The game-log overlay (`TTextBar` cls_0x5a5560 / `TConsolePane` cls_0x5a4358) is a
transparent band that visually overlays the UPPER half of the screen, NOT the
bottom strip (B.r5:90). It is initialized as a sibling in `TPlayScreen::Initialize`
but is not spatially adjacent to or hosted by the bottom bar. Out of scope here.

---

## §2 — Asset roster

Archive: `bottombar.dat` (loaded by `TPlayScreen::Initialize` as `BottomBar.dat`,
B.r3:24). Measured with `tools/ui/dump_dat.py` → `/tmp/bottombar_dump/bottombar.json`.

| asset | archive | entry | WxH (measured) | regx,regy | flags | kc | role | source rects | cite |
|---|---|---|---|---|---|---|---|---|---|
| **UtilityBar** | bottombar.dat | `UtilityBar` (idx 0) | **640×60** | 0,0 | `0x2` BM_15BIT | `0x0` | bar chrome plate (stretched to live width) | full bitmap; sx=0,sy=0,sw=full,sh=60 | dump_dat idx0; `cls_0x5a5808.cpp:181` |
| **BarEndCap** | bottombar.dat | `BarEndCap` (idx 2) | **10×60** | 0,0 | `0x2` BM_15BIT | `0x0` | right-edge vertical trim | full bitmap | dump_dat idx2; `cls_0x5a5808.cpp:182` |

**Not owned by this pane (siblings — listed for context only, NOT drawn by TBottomBarPane chrome):**
| asset | WxH | owner | note |
|---|---|---|---|
| BarInvBox (idx 1) | 42×42 | BarInv (`cls_0x5a5658` @0x65b028) | potion/key slot box |
| ArrowUU/UD/UG/DU/DD/DG (idx 3–8) | 24×24, `0x104` BM_16BIT+BM_ALPHA | TQuickSpellPane (@0x65c6f8) | spell-ring up/down/glow arrow states (alpha-keyed, not magenta) |

Both bar-chrome assets are **opaque** (kc=0, 15-bit, no alpha) — they are plain
copy/stretch blits, no chroma key, no shadow.

**Source-rect map** — neither chrome asset is an atlas. UtilityBar is consumed
whole and X-stretched; BarEndCap is consumed whole.

```
UtilityBar 640x60 (whole bitmap, X-stretched to live width)
 ┌──────────────────────────────────────────────────────────┐ 60px tall
 │  dark stone/metal texture, full 640px source              │
 └──────────────────────────────────────────────────────────┘
BarEndCap 10x60 (whole bitmap, opaque copy at right edge)
 ┌┐
 ││ 10px vertical trim
 └┘
```

---

## §3 — Coordinate frames & surfaces

### Pane rect (ctor + TPane base)

The ctor (`cls_0x5a5808.cpp:32-39`) sets the TPane rect fields. TPane field map
(confirmed against `cls_0x5a4494.cpp:308-326`, `virt_meth_0x491900` which copies
template→live): **`mbr_0x4`=x, `mbr_0x8`=y, `mbr_0xc`=width, `mbr_0x10`=height**
(live); **`mbr_0x14/0x18/0x1c/0x20`** = template x/y/w/h.

| field | ctor value | meaning | cite |
|---|---|---|---|
| `mbr_0x4` (x) | `0` | pane x | `cls_0x5a5808.cpp:39` (`mbr_0x4 = 0`) |
| `mbr_0x8` (y) | `0x1a4 = 420` | pane y | `cls_0x5a5808.cpp:32` |
| `mbr_0xc` (w) | `0x280 = 640` | pane width (live; X-stretch target) | `cls_0x5a5808.cpp:34` |
| `mbr_0x10` (h) | `0x3c = 60` | pane height | `cls_0x5a5808.cpp:36` |
| `mbr_0x18/0x1c/0x20` (template y/w/h) | `0x1a4/0x280/0x3c` | template mirror | `cls_0x5a5808.cpp:33,35,37` |

So the pane rect @ Classic 640×480 is **(x=0, y=420, w=640, h=60)** — the bottom
60px strip. **`y = 420 = 480 − 60` ⇒ BOTTOM-anchored.**

### Bottom-anchor formula (mandatory)

`pane_y = display_h − k`, with **`k = 0x3c = 60`** (the pane height). At Classic
`480 − 60 = 420 = 0x1a4` ✓ (matches the ctor literal exactly — two-way confirmed).
Width is **greedy**: `mbr_0xc` (live width) tracks the parent/display width
(set from template `0x280` and re-propagated on resize by SetRect `0x52c930`,
`cls_0x5a5808.cpp:216-221`), so at 1920 the bar is 1920×60 pinned to the bottom.

### Frame table

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer | TL (root) | `(0,0)` | final pixels; `display_w`/`display_h` runtime | — |
| **bar-pane** | screen | **BL (bottom-anchored)** | `x = 0` (left-anchored, constant); `y = display_h − 0x3c(=60)` (= `0x1a4` @ Classic) | the whole bar; chrome + sibling controls drawn relative to it | `cls_0x5a5808.cpp:32,34,36,39`; `cls_0x5a4494.cpp:312-326` |
| **chrome-draw** | bar-pane | TL | bar-pane-local `(0,0)` | UtilityBar fill + BarEndCap, drawn into the target surface at pane-local 0 | `cls_0x5a5808.cpp:181-182` |
| **source-rect** | each bitmap | TL | `(0,0)` | the `(sx,sy,sw,sh)` read from UtilityBar/BarEndCap | `cls_0x5a5808.cpp:181` |

**Composition to screen:**
`screen_x = bar_pane_x(=0) + chrome_local_x` and
`screen_y = (display_h − 60) + chrome_local_y`.
The chrome-draw frame's origin equals the bar-pane origin (chrome draws at
pane-local 0,0), so chrome-local == bar-pane-local here. Chrome is X-stretched to
fill `mbr_0xc` (live width); it does NOT scale Y (always 60).

### Corner verification

- x: bare constant `0` ⇒ **left-anchored**, offset 0 (`cls_0x5a5808.cpp:39`).
- y: `0x1a4` derived as `display_h − 0x3c` ⇒ **bottom-anchored**, offset 60. At
  Classic this is the literal `420`; if it were top-anchored it would be a
  constant 420 regardless of height — but it equals `480−60`, and the SetRect
  path re-pushes height, confirming bottom-anchor. (No sibling/mirror — single
  instance.)

### Surfaces (direct-renderer contract)

`Draw` (`0x52c800`) and chrome (`0x52c880`) blit into a target surface passed in
(`in_stack_00000004`). In the normal path that target is the global display
`PTR_DAT_005d79e0` (`cls_0x5a5808.cpp:162,184`); the chrome method also handles a
non-display scratch target (`cls_0x5a5808.cpp:184` `if (surface != PTR_DAT_005d79e0)`),
re-drawing the BarInv + QuickSpell into the scratch in that branch
(`:185-188`). The port should compose the chip into one offscreen RT then
`DrawSurface` it (NOMENCLATURE §3 direct-renderer contract), preserving the
"stretch UtilityBar to live width, stamp BarEndCap flush-right" contract.

### Layout diagram (bar-pane-local; origin = screen (0, display_h−60))

```
bar-pane-local x:  0                                   w-10    w
                   ├──────────────────────────────────┬───────┤  y(local)=0
                   │  UtilityBar (640 src) X-stretched │ EndCap│
                   │  to width w, height 60            │ 10x60 │  h=60
                   └──────────────────────────────────┴───────┘  y(local)=60
                   ▲                                   ▲
                   stretch fill                        BarEndCap at (w-10, 0)
   (sibling controls — quickspell ring + BarInv slots — stamped on top by
    TQuickSpellPane / cls_0x5a5658; positions owned by THOSE panes)
```

---

## §4 — Static element layout

All coordinates **bar-pane-local** (origin = screen `(0, display_h − 60)`).
`w` = live width `mbr_0xc` (640 @ Classic). Single instance — no mirror columns.

| element | space | (x, y) | (w, h) | source rect | drawmode | cite |
|---|---|---|---|---|---|---|
| UtilityBar fill | bar-pane-local | (0, 0) | (`w`, 60) | UtilityBar sx=0,sy=0,sw=640,sh=60 → **X-stretched** to `w`×60 | stretch copy (opaque) | `cls_0x5a5808.cpp:181` |
| BarEndCap trim | bar-pane-local | (`w − 0xa(=10)`, 0) | (10, 60) | BarEndCap whole 10×60 | `0x80000000` DM_USEDEFAULT (opaque copy) | `cls_0x5a5808.cpp:182` |

Composition to screen: `screen = (0 + x, (display_h−60) + y)`.
E.g. @1920×1080: UtilityBar fills `(0, 1020)`→`(1920, 1080)`; BarEndCap at
`(1910, 1020)`.

> The dh of the stretch blit is the literal `0x3c = 60` (`cls_0x5a5808.cpp:181`),
> NOT `mbr_0x10` — height is fixed at 60 even though the field exists. Only width
> stretches.

---

## §5 — Draw order / composition

Two vtable methods. **Draw** (`0x52c800`, slot 20) is the per-frame entry;
**chrome** (`0x52c880`, slot 84) does the actual UtilityBar/BarEndCap blits and is
invoked from Draw via the vtable.

**Draw `0x52c800`** (`cls_0x5a5808.cpp:154-170`), in order:
1. `if (mbr_0x50 != 0)` (visible) → call `(*vtable->slot84)(this)` = the chrome
   draw `0x52c880` (`:159-160`).
2. `cls_0x5a5658::virt_meth_0x52ca70(&DAT_0065b028, PTR_DAT_005d79e0)` — draw the
   **BarInv** potion/key slots onto the display (`:162`). *(sibling-owned;
   instance = `TBarInvPane` `cls_0x5a56d4` @0x65b028 — the `cls_0x5a5658` method
   label is a Ghidra-merge artifact, see §1 caveat)*
3. `cls_0x5a5a30_TQuickSpellPane::virt_meth_0x5444c0(&DAT_0065c6f8)` — draw the
   **quick-spell ring** (`:163`). *(sibling-owned)*
4. `cls_0x5b98b8::virt_meth_0x435de0(this)` — base post-draw / child traversal
   (`:164`).
5. `if (mbr_0x50 != 0)` → `FUN_004aacb0_BlitEffect_Iterate(mbr_0x4, mbr_0x8,
   mbr_0xc, mbr_0x10)` — flush any registered blit-effects over the bar rect
   (UI_METHOD_MAP §8.5) (`:165-167`).
6. `(*vtable->slot44=0x444fc0)(this)` — base "mark drawn / clear dirty" (`:168`).

**Chrome `0x52c880`** (`cls_0x5a5808.cpp:174-190`), in order (target = `in_stack_00000004`):
1. `surface.meth_0x4bd5e0(0, 0, mbr_0x180[UtilityBar], 0, 0, mbr_0xc[width], 0x3c[60])`
   — stretch-blit UtilityBar across the full width × 60 at (0,0) (`:181`).
2. `surface.meth_0x4bd680(mbr_0xc − 0xa, 0, mbr_0x184[BarEndCap], 0x80000000)`
   — opaque stamp BarEndCap at (width−10, 0) (`:182`).
3. `(*vtable->slot144=FUN_004361b0)()` — base draw-children/flush (`:183`).
4. **scratch branch** `if (surface != PTR_DAT_005d79e0)` (`:184`): also draw
   children clip (`0x435cb0`), then re-draw BarInv (`:186`) and QuickSpell
   (`:187`) into the scratch surface. This is the off-display compose path.

**Net visible order (bottom→top):** UtilityBar plate → BarEndCap trim → BarInv
slots → quick-spell ring → blit-effects flush.

---

## §6 — Algorithms

The bar chrome is two plain blits — no fill kernel, no gauge math. Pseudocode per
helper (params from the §5 call sites):

```
# meth_0x4bd5e0(surface, dx, dy, srcBitmap, sx, sy, dw, dh)
#   = subrect→rect stretch blit on the surface class (cls_0x5a68d8).
#   Body is a Ghidra thunk; arg meaning is the call-site signature.
DrawUtilityBar(surface, width):
    surface.StretchBlit(
        dst=(0, 0), dstSize=(width, 60),
        src=UtilityBar, srcRect=(0,0, 640,60),   # full source, X-stretched
        mode=opaque)
    # cite cls_0x5a5808.cpp:181 ; literals dx=0 dy=0 sx=0 sy=0 dw=mbr_0xc dh=0x3c

# meth_0x4bd680(surface, x, y, srcBitmap, drawmode)  -- sprite stamp (UI_METHOD_MAP §4)
DrawEndCap(surface, width):
    surface.Stamp(dst=(width - 10, 0), src=BarEndCap, mode=0x80000000)  # opaque
    # cite cls_0x5a5808.cpp:182 ; literals x=mbr_0xc-0xa y=0 mode=0x80000000
```

Call-site literal table:

| call | dx/x | dy/y | src | sx | sy | dw | dh | mode |
|---|---|---|---|---|---|---|---|---|
| `meth_0x4bd5e0` UtilityBar | `0` | `0` | `mbr_0x180` | `0` | `0` | `mbr_0xc` (width) | `0x3c`(60) | opaque |
| `meth_0x4bd680` BarEndCap | `mbr_0xc − 0xa`(w−10) | `0` | `mbr_0x184` | — | — | — | — | `0x80000000` |

`mbr_0x180` = UtilityBar handle, `mbr_0x184` = BarEndCap handle (set in
Initialize, `cls_0x5a5808.cpp:117,119`; `FUN_0052c780_BottomBar_init.cpp:9-12`).

---

## §7 — Effects & shadows

- **No drop shadow** on the bar chrome. Neither blit sets up `FUN_00438d80`; both
  are plain opaque copies (UtilityBar stretch, BarEndCap `0x80000000`).
- **No chroma key:** both assets are 15-bit with kc=0 and no alpha (dump idx0/idx2).
  Do NOT apply the global magenta key — these are solid plates.
- **No text** on the bar → no 3-pass text shadow, no pink-halo risk for this pane.
- **Blit-effect flush** (`FUN_004aacb0_BlitEffect_Iterate`, `cls_0x5a5808.cpp:166`)
  runs over the bar rect after children draw — it applies any shadow/glow
  registered by the SIBLING controls (e.g. quick-spell arrows), not by the bar
  chrome. The bar chrome contributes nothing to it. (UI_METHOD_MAP §8.5.)

---

## §8 — Text rendering

**None.** TBottomBarPane draws no text — only the two chrome bitmaps. The bar is
a passive backdrop; any text/labels in the bottom region belong to the sibling
panes (quick-spell counts, item counts) and are out of scope here. No font, no
align, no cell. (Section explicitly N/A; verified — Draw `0x52c800` and chrome
`0x52c880` contain no `meth_0x4be110`/`FUN_004be2b0` text calls.)

---

## §9 — Animation & dynamic behavior

- **No tweens** on the bar chrome. It is static art; the only dynamic value is the
  live width `mbr_0xc`, driven by window/parent resize (not an animation).
- **Resize / relayout:**
  - `virt_meth_0x52c910` (slot 18, `cls_0x5a5808.cpp:194-205`): commit —
    copies template `mbr_0x14/0x18/0x1c/0x20` → live `mbr_0x4/0x8/0xc/0x10`, then
    base `0x444fc0`. This re-derives the live rect (incl. width) from the template.
  - `virt_meth_0x52c930` (SetRect, `cls_0x5a5808.cpp:209-223`): writes new
    template w/h (`mbr_0x1c`,`mbr_0x20`) AND propagates the same w/h to the BarInv
    global (`DAT_0065b02c._24_/._28_`) and QuickSpell global
    (`UNK_0065c6fc._24_/._28_`) — so the bar resizes its hosted siblings in lock-step.
- **Dirty model:** base TPane `mbr_0x40` "initialized" flag (`cls_0x5a4494.cpp:311`)
  + `mbr_0x50` "visible/needs-draw" gate (`cls_0x5a5808.cpp:159,165`). No
  per-cell value cache (nothing changes per-tick). In the port, redraw the chrome
  only when live width changes (version counter on width, per
  [[feedback-versions-over-flags]]).
- `virt_meth_0x52c7e0` (slot 13, `cls_0x5a5808.cpp:138-150`): on-show — sets
  `mbr_0x48=mbr_0x4c=1` on this pane and the `._68/._72` flags on the BarInv +
  QuickSpell globals (forces a full redraw of the hosted siblings when the bar
  is shown).

---

## §10 — Input & dispatch

The bar **chrome** is non-interactive — UtilityBar/BarEndCap have no hit rects and
emit no command. `TBottomBarPane` derives from TButtonPane (ctor calls the
TButtonPane base ctor `FUN_0041c7f0(0x10,0x10)`, `cls_0x5a5808.cpp:45`), so the
base maintains a button list, but **Initialize registers no buttons**
(`FUN_0052c780_BottomBar_init.cpp` only loads the two chrome sprites). The actual
clickable controls in the bottom region (spell-ring arrows, potion slots) are
owned and dispatched by the SIBLING panes (`TQuickSpellPane`, the BarInv
`cls_0x5a5658`), not by this pane. For THIS spec: **no hit rects, no command ids.**

> If a later pass finds buttons attached via the TButtonPane base ctor's
> `(0x10,0x10)` cell args, they would belong to the bottom-bar pane — flagged §14.

---

## §11 — Retail bugs NOT to reproduce

None identified for the bar chrome. The two blits are clean opaque copies with no
chroma key and no shadow → no pink-halo, no key-bleed, no +offset artifacts. (The
pink-halo / key-bleed family applies only to text/keyed sprites, which this pane
has none of.)

---

## §12 — Reconstruction pseudocode

```
class BottomBarPane:
    UtilityBar : Bitmap   # bottombar.dat "UtilityBar" 640x60, opaque 15-bit
    BarEndCap  : Bitmap   # bottombar.dat "BarEndCap"  10x60,  opaque 15-bit

    init():
        UtilityBar = LoadBitmap("bottombar.dat", "UtilityBar")   # §2
        BarEndCap  = LoadBitmap("bottombar.dat", "BarEndCap")

    # pane rect: x=0, y=display_h-60, w=display_w (greedy), h=60   (§3)
    rect():
        return Rect(x=0, y=display_h - 60, w=display_w, h=60)

    drawChrome(target):                                          # §5 step1 / §6
        w = rect().w
        # UtilityBar X-stretched to (w x 60) at pane-local (0,0); height fixed 60
        target.DrawBitmapSubrectToTarget(UtilityBar,
            dst=(0,0), dstSize=(w,60), src=(0,0, 640,60))        # opaque
        # BarEndCap opaque stamp at right edge
        target.DrawBitmapToTarget(BarEndCap, dst=(w-10, 0))      # opaque

    draw(target):                                                # §5
        if visible:
            drawChrome(target)
        BarInvPane.drawInto(target)        # sibling-owned (cls_0x5a5658)
        QuickSpellPane.draw(target)        # sibling-owned
        base.drawChildren(target)
        if visible:
            flushBlitEffects(rect())       # applies sibling-registered shadows
        base.markDrawn()

    # compose chip -> offscreen RT, then DrawSurface(RT) at screen (0, display_h-60)
```

Note for the implementor: the bar's own draw is ONLY `drawChrome`. The
`BarInvPane`/`QuickSpellPane` lines are how retail orders them; in the port,
sequence the sibling panes' own draws after the bar chrome (do not re-implement
their contents here). Use the bottom-anchor rect; X-stretch UtilityBar; keep
height at the literal 60.

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `meth_0x4bd5e0(0,0,UtilityBar,0,0,w,60)` stretch | `DrawBitmapSubrectToTarget(bm, dst, dstSize=(w,60), src=(0,0,640,60))` | renderer (UI_METHOD_MAP §12) |
| `meth_0x4bd680(w-10,0,BarEndCap,0x80000000)` opaque stamp | `DrawBitmapToTarget(bm, (w-10,0))` | renderer (UI_METHOD_MAP §4/§12) |
| compose chip RT → HUD | `DrawSurface` / `DrawSurfaceTinted` | renderer |
| `FUN_004aacb0_BlitEffect_Iterate` | subsumed by per-call `DrawBitmap…Shadowed` on the siblings | renderer |

**Missing primitive?** None new for the bar — both calls are existing
`DrawBitmap*ToTarget` variants. The X-stretch path needs the subrect→dstSize
form (dstSize ≠ srcSize); confirm `DrawBitmapSubrectToTarget` accepts an explicit
dst width/height (stretch). If it only does 1:1, that is a needed primitive:
"stretch-blit to target" (flag to reconstruction agent).

---

## §14 — UNCONFIRMED / open questions

1. **Live width source at runtime.** The ctor sets template width `0x280=640`;
   live `mbr_0xc` is copied from template (`0x491900`) and re-propagated by
   SetRect (`0x52c930`). I did NOT trace who CALLS SetRect with the display width
   (the parent `TBottomPane` container). **Impact if wrong:** at non-640 widths
   the bar might not span full width. **Resolve:** trace callers of
   `0x52c930` / how `TBottomPane` sizes its children (`DecompileAddr` on the
   container's resize path; the container's Initialize `0x52d8a0` is trivial so
   sizing happens elsewhere — likely `TPlayScreen` layout). Mitigation: the port
   should size the bar to `display_w` directly (greedy), which matches the
   `0x280` Classic literal.

2. **TButtonPane base button registration.** The ctor calls
   `FUN_0041c7f0(0x10,0x10)` (`cls_0x5a5808.cpp:45`); B.r5 flags `(0x10,0x10)` as
   possibly button cell width/height. Initialize adds no buttons. **Impact:** if
   the bar owns clickable controls beyond the siblings, §10 is incomplete.
   **Resolve:** `DecompileAddr.java 0x0041c7f0` to see if it auto-registers
   buttons, and dump the full `cls_0x5a5808` vtable slots 25–29 (`0x487e80..`).

3. **`meth_0x4bd5e0` exact stretch vs tile semantics.** Body is a Ghidra thunk
   (`cls_0x5a68d8.cpp:1547-1554`); I read the 7-arg signature off the call site
   and inferred subrect→rect STRETCH (dw=width, sw=640 differ ⇒ stretch). It
   could instead TILE the 640 source across a wider dst. **Impact:** at >640
   width the texture either stretches (smooth) or repeats (tiled) — visually
   different. **Resolve:** `DecompileAddr.java 0x004bd5e0` and inspect whether it
   loops the source. Add the result to UI_METHOD_MAP (this primitive is a
   **method-map gap** — `meth_0x4bd5e0`/`FUN_004bd5e0` is not yet catalogued).

4. **Scratch-target branch (`cls_0x5a5808.cpp:184-188`).** When the chrome target
   ≠ display, it re-draws BarInv + QuickSpell into the scratch. I did not fully
   trace WHEN a non-display target is used (likely an off-screen capture / dialog
   compositing path). **Impact:** none for the normal HUD draw (display path);
   matters only if the port reuses the bar in an off-screen compose. **Resolve:**
   find callers of `0x52c880` with a non-display surface arg.

5. **Class name.** "TBottomBarPane" is ~85% (`TUtilityBarPane`/`TActionBarPane`
   plausible) — B.r5. **Impact:** cosmetic (file/class naming). **Resolve:** none
   available beyond the error string; pick a name and note the alternates.
