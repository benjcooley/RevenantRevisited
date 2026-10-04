# UI Nomenclature

Shared vocabulary for Revenant UI forensics + reconstruction. Every panel spec
and both protocols use these terms. Don't invent synonyms — if a term you need
isn't here, add it with a definition.

Where a concept overlaps the VFX domain (animation timing, easing, diagrams),
this file deliberately reuses the VFX wording from
`worktrees/vfx/docs/vfx/forensics/NOMENCLATURE.md` so the two domains read the
same. UI-specific concepts (coordinate spaces, anchoring, the chip model) are
defined fresh here.

---

## 1. Coordinate frames

A **frame** is a coordinate space with an origin. A retail UI element's position
passes through a *chain* of frames (a frame's origin is itself a point in its
parent frame), and the #1 source of reconstruction bugs is conflating them or
dropping a link in the chain. **Every coordinate, every constant, and every draw
in a spec belongs to exactly one frame, and that frame must be named.**

The common frames (each is a parent→child link):

| frame | origin (in its PARENT frame) | meaning |
|-------|------------------------------|---------|
| **screen** | framebuffer top-left (root) | final pixel position. `pane_w` = live display width (`this+0xc`, runtime). |
| **pane-local** | pane top-left, in screen | the pane's own rect; children use pane-local. |
| **chip-local** | chip top-left, in pane/screen (player chip → screen `0`; target chip → screen `pane_w-0xc1`) | a logical panel unit; may be WIDER than its backdrop bitmap (see **chip model**). |
| **chrome/surface-local** | the scratch/chrome/mosaic surface's top-left, in chip-local (e.g. on the target the chrome surface origin = chip-local `0x41`) | coords INSIDE an off-screen surface the panel draws into before blitting to its parent. Text + chrome + icons are drawn here first. |
| **source-rect** | source bitmap/atlas top-left | the `(sx,sy,sw,sh)` sub-rect copied OUT of an asset. |

### RULE: build the frame graph FIRST (principle: spaces matter; communicate them)

Before placing any element, the spec must IDENTIFY every frame in play, state
**how each frame's origin is computed in its parent**, and then tag every draw
and every constant with the frame it is expressed in. The classic bug — a
chrome-surface-local number used as if it were chip-local — is precisely a
*dropped frame link*: the chrome frame's origin (chip-local `0x41` on the target)
was never applied. With the frame graph written down, composing to a final frame
(NOMENCLATURE rule below) is mechanical:
`screen_x = chip_origin_in_screen + chrome_origin_in_chip + element_x_in_chrome`.

See FORENSICS_PROTOCOL §3 for the required **Frame table** + per-element frame
tagging.

**Always write a coordinate as `frame:(x,y)`** when ambiguous, e.g.
`chip-local:(0x44,0x0f)`, `chrome-local:(0x62,0x1f)`, `source:(2,0x31)`.

### The chip model (UI-specific, load-bearing)

A "chip" is a logical panel unit that may extend beyond its chrome bitmap. The
player healthbar's `BackPanel` bitmap is 128×64, but the **chip** is `0xc1 = 193`
px wide because the bars extend left/right past the chrome.

```
chip-local x:  0x00        0x41              0xc1
               ├───────────┼──────────────────┤
               │  bars      │   BackPanel       │   ← bars start LEFT of chrome
               │  start     │   (chrome 128w)   │
```

- **Player chip** origin = screen `0`.
- **Target chip** origin = screen `pane_w - 0xc1`.
- A target-local x converts to screen as `pane_w - 0xc1 + chip_local_x`.

Forgetting the chip is wider than the chrome makes target bars look "mostly
wrong" — their true origin is left of the BackPanel.

### RULE: every spec coordinate is a FINAL, COMPOSED, single-space value

This is the load-bearing rule that prevents the most common reconstruction bug.
Retail draws through nested surfaces (a chrome/scratch surface, drawn into at
*surface-local* coords, is itself blitted to the pane at an *offset*). The recon
gives you values in those native sub-spaces. **A spec must NOT pass those raw
sub-space values through with a single space label.** For every element the spec
gives ONE coordinate in ONE final space (screen, or pane-local with the pane
origin stated), with all intermediate sub-surface offsets already **composed
in**.

- Show the composition so it's auditable, e.g.
  `Ring: chrome-local center 0x62; chrome sits at chip-local 0x41 → chip-local
  center 0x41+0x62? NO — chrome is 0x80 wide so chrome-x 0x62 maps to chip 0x41+(0x62)=… → screen pane_w-…`
  — but the value the implementor uses is the final composed one.
- **Never label a value with a space it is not in.** If a number is
  chrome-surface-local, do not put it in a column headed "chip-local." That
  single mislabel (chrome-local printed as chip-local) is what put a whole
  mirrored panel 0x41 px off.
- **Mirrored side: give the target's final composed coordinate explicitly.**
  Do not write "mirror of player" and leave the arithmetic (chrome offset + the
  x-flip) for the reconstruction agent — it will skip a term. Compute it in the
  spec; the agent copies, never derives.

---

## 2. Anchoring & alignment

| term | meaning |
|------|---------|
| **anchor** | which corner/edge of the PARENT a non-filling child is positioned from: `TL` / `TR` / `BL` / `BR` / `center` / an edge. **Mandatory for any element or frame that does not fill its parent** (NOMENCLATURE: most chips/panels don't). State the anchor AND the offset from it. The anchor determines how the coordinate scales with parent size: <br>• TL/top/left-anchored → coord = `offset` (constant). <br>• TR/right-anchored → coord = `parent_w − offset` (moves with width). <br>• BR/bottom-anchored → `parent_h − offset`. <br>The target chip is `pane_w − 0xc1` *because* it is TR-anchored to the pane; the player chip is TL-anchored (= `0`). Getting the anchor wrong puts the element at the wrong corner or makes it drift when the window resizes. Always verify: is the child at the right offset from the right corner of its parent? <br>**How to DETECT the anchor from the recon (so you record it, not miss it):** read the coordinate's expression in the decomp — `pane_w − k` / `parent_w − k` ⇒ **right-anchored**, offset `k`; `parent_h − k` ⇒ bottom-anchored; a bare constant `k` ⇒ left/top-anchored, offset `k`; `(parent_w − w)/2` ⇒ centered. The right/target stat element computes its x as `pane_w − offset` (slot23 `:657/:672/:687`, value cell `pane_w−0x80`, name `pane_w−0x44`) → it is TR-anchored; record `anchor=TR, offset=…` for it and every right-side sub-element. |
| **mirror** | a target/right-side element derived from a player/left-side element by reflecting x across the chip or pane. Y is usually unchanged; x and any directional offset (shadow cast direction) flip. A mirror is usually realized as an anchor flip (TL→TR) plus a chrome/sub-frame offset — state the resulting composed coordinate, not "mirror of player". |
| **h-align (text)** | HORIZONTAL placement of glyphs inside a text cell: `left` / `center` / `right`. In retail this is the DrawTextA flag bits (UI_METHOD_MAP `FUN_004be2b0`): `&1`=left, `&2`=center, `&4`=right. **"center" is HORIZONTAL ONLY** — it never implies vertical centering. |
| **v-align (text)** | VERTICAL placement of text in a cell. Retail uses GDI `DrawTextA` with **DT_TOP** (no DT_VCENTER) — text hangs from the cell TOP. A spec must state v-align separately from h-align; default is **top**. Multi-line text stacks DOWNWARD from the cell top by `lineHeight` — do NOT vertically center the block (a tall cell, e.g. the 64×64 name cell holding 2 lines, leaves the lower part EMPTY). |
| **cell** | a fixed-size box a string is rendered into and clipped/aligned within. Stat values use a `0x32×0x0e` (50×14) cell; the name/level label a `0x40×0x40` (64×64) cell. |
| **baseline** | the y where glyph baselines sit. NOT a spec concern: the spec gives the cell + h-align + v-align (top) and stops there. Reproducing retail's exact in-cell baseline (retail used GDI `DrawTextA` DT_TOP, whose internal-leading lands the glyphs ~2px higher than a naive stb-ascent baseline) is a one-time correction in the **shared draw code** (`font.cpp` `DrawText*ToTarget`), applied for every panel — do not ask the spec/recon to carry a per-font ascent. If text looks ~2px off across all panels, fix the helper once, not each spec. |

---

## 3. Composition & surfaces

| term | meaning |
|------|---------|
| **chrome** | the static backdrop bitmap of a panel (e.g. `BackPanel`). |
| **scratch surface** | an off-screen surface the panel draws into (text, effects) before blitting a rect of it onto the pane. Retail allocates several (`this+0x60/0x64/0x68/0x6c/0x70/0x74`); the port need not mirror them 1:1 but MUST preserve the coordinate-space contract (compose to a fixed render target, then blit). |
| **mosaic surface** | a scratch surface assembled from multiple source bitmaps (e.g. TSideTabsPane's 3 button-state mosaics). |
| **two-stage blit** | retail commonly (a) draws into a scratch/effect surface, then (b) blits a rect from it to the pane. The *final* destination is the stage-(b) blit's `SDrawParam.dx/dy`, NOT the stage-(a) draw position. |
| **direct-renderer contract** | the port composes the whole chip into one fixed render target, then blits that surface during the HUD swapchain pass. Do NOT draw bitmaps via one path and glyphs straight to the swapchain via another — window-scale changes will make text and art move/scale differently. |

---

## 4. Effects (shadow / glow / key)

| term | meaning |
|------|---------|
| **drop shadow** | a darkened copy of a sprite/text offset by `(dx,dy)`. **Retail semantics (load-bearing):** when set up via `FUN_00438d80(buf, dx, dy)`, the `(dx,dy)` is where the SHADOW lands; the sprite/bar blits at `dst = recon_coord − (dx,dy)`. The left/player side casts down-right `(+4,+4)`; the right/target side mirrors to down-left `(−4,+4)`. See UI_METHOD_MAP + [[project-retail-shadow-semantics]]. |
| **text 3-pass shadow** | when a text font flag has bit `0x400` set, `FUN_004be2b0` draws 3 BLACK passes (base, +1x, +1y) then the colored pass at base — a 1px black shadow on the right+bottom. |
| **chroma key** | magenta `0xFF00FF` (`0xF81F` in RGB565, `0x7C1F` in RGB555) marks transparent pixels. Keyed pixels are skipped on blit. |
| **pink-halo bug** | a retail rendering artifact: anti-aliased shadow edge pixels sample a magenta-cleared cell, survive the keyed blit, and show as a pink/magenta halo around text. It is a BUG, not an intended glow — do NOT reproduce it. See [[project-retail-pink-halo-bug]]. |

---

## 5. Animation & timing

Reused from the VFX nomenclature so timing reads the same across domains.

| term | meaning |
|------|---------|
| **sim tick / 24Hz gate** | Retail simulation runs at 24Hz. UI animators (hover fades, bar fills) advance per sim tick, not per render frame. The port MUST gate to 24Hz (`kSimTickMs = 1000/24`) or animation speed scales with framerate. |
| **age / age01** | ticks since an animation started / normalized `age/duration` in `[0,1]`. |
| **ramp** | an integer counter that steps toward a target each tick (e.g. TSideTabsPane hover-fade `cur` steps ±1 toward 0 or 8). State the min, max, step-per-tick, and what advances/reverses it. |
| **tween** | a value interpolated from `a` to `b` over a duration with an easing. Describe with the standard tween block below. |
| **envelope / curve** | a value's trajectory over `age01`. State start, end, and easing. |
| **easing** | `linear` / `ease-in` / `ease-out` / `ease-in-out` / `triangle` / `step`. Name it; if it's a table/LUT, give the table. |
| **triangle-wave** | ramp up then down; specify peak position (e.g. a pulse). |
| **phase** | a named stage of a multi-stage widget (e.g. closing→hidden). Record duration + transition condition. |

### Standard tween/animation notation (use this everywhere)

Describe every animated value as a **tween block** so the implementor reproduces
it without guessing. One block per animated property:

```
tween <property>:
  from   <start value>          # value at age01=0
  to     <end value>            # value at age01=1
  over   <duration>             # ticks @24Hz (state ms too if known)
  easing <linear|ease-in|...>   # or LUT: [v0, v1, ... vN]
  drive  <what advances age>    # e.g. "hover state true", "current/max delta"
  on-end <hold|reverse|loop|kill>
  cite   <file:line>
```

Example (TSideTabsPane button hover fade):

```
tween button.glowAlpha:
  from   0
  to     255
  over   8 ticks               # ramp cur 0->8, ±1/tick
  easing linear (alpha = round(cur * 255 / 8))
  drive  widget hover flag (+0x14 & 8); reverses when hover lost
  on-end hold at target
  cite   cls_0x5a5750_TSideTabsPane_Update_53d3a0.cpp:NN, DrawBackground_53d420.cpp:NN
```

For an integer ramp that isn't a simple from/to (the common retail pattern),
state it as a ramp:

```
ramp button.cur:
  range   0..8
  step    +1 per tick toward target
  target  8 when hovered, else 0
  maps-to alpha = round(cur * 255 / 8); extra bright pass when cur >= 5
  cite    Update_53d3a0.cpp:NN
```

---

## 6. Input

| term | meaning |
|------|---------|
| **hit rect** | the clickable rectangle of a control, in pane-local space. State x,y,w,h. |
| **command id** | the integer a control emits on click, dispatched by the screen's central command switch (e.g. TPlayScreen `FUN_0047cf40`). Tabulate control→command. |
| **modal-state global** | a variable holding the current selection for a group of controls (e.g. the upper/lower region selectors). State the global, its value range, and the visual effect of each value. |
| **dirty flag / version** | what marks a panel for redraw. Retail caches prior values and only redraws changed cells; document the cache fields and conditions. Prefer a monotonic version counter over a bool in the port (see [[feedback-versions-over-flags]]). |

---

## 7. Diagrams (use them)

Include ASCII diagrams wherever spatial layout or temporal behavior is clearer
drawn than described. A diagram orients the implementor fast; it supplements
(never replaces) the coordinate tables + pseudocode.

**Layout (spatial)** — box the chip, mark element rects with their pane/chip-local
coords. Show both player and target where they mirror.

```
player chip (chip-local, origin = screen 0)            target chip (mirror)
0        4        43   68                    187        pane_w-0xc1 ........ pane_w
├────────┬────────┬────┬──────────────────────┤        ┌──────────────────────────┐
│        │ Ring   │icon│  HP bar  [value]      │        │   (x-mirrored layout)    │
│        │ 44x44  │24² │  ───────────────       │        │                          │
│  y=9   │ (4,9)  │y=3 │  y=15 (shadow), bar    │        │  bars extend LEFT of     │
│        │        │    │  pixels at y=11        │        │  the BackPanel chrome    │
└────────┴────────┴────┴──────────────────────┘        └──────────────────────────┘
```

**Animation (temporal)** — value vs age01, or a phase/ramp timeline.

```
glowAlpha (0..255)
255 ┤        ●────────  (hover held)
    │      ╱
    │    ╱  linear, alpha = cur*255/8
  0 ┤●──        cur ramp: 0→8 over 8 ticks (±1/tick)
    └────────────────── ticks
    hover:  off │ on (ramp up) │ held │ off (ramp down)
```

**Source-rect map (atlas)** — when an asset is an atlas, draw the band/cell rows
with their measured pixel ranges.

```
Bars atlas 128x128
 y  0..? health bright   ┐
 y  ?..? mana bright     │ bright bands (full)
 y  ?..? fatigue bright  ┘
 y  ?..? health dim      ┐
 y  ?..? mana dim        │ dim bands (empty/track)
 y  ?..? fatigue dim     ┘
 y 99..125 tiny digit strip (UNUSED by stat values — see spec)
```
