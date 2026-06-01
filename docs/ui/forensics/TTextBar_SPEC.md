# TTextBar — Reconstruction SPEC

Floating multi-line game-message / status-text overlay (under the map), plus the
optional opponent name + health bar shown over a combat target. Retail leaf class
`cls_0x5a5560` (vtable `0x5a5560`), global singleton `0x65c5d0`.

---

## §0 — Sources & status

**Class:** `cls_0x5a5560` = **TTextBar** (vtable `0x005a5560`). Global instance
`0x0065c5d0`. Wired by global-ctor stub `FUN_00480725`. Confirmed 98% by Wave-2A
(string anchor `"Trouble initializing text bar"` + .CRT$ ctor vtable wire + leaf-
TPane vtable shape + Close↔Initialize structural reciprocity — see brief).

**Vtable / method map** (dumped `0x5a5560`, 35 slots; overrides at 0/1/7/13/19/20/23/28,
the rest inherited from TPane base `cls_0x5a4494`). Override slot positions match
TPlyrStatusBar's leaf-TPane shape (`cls_0x5a54e4_TPlyrStatusBar_vtable.txt`), giving
the slot→role mapping below:

| slot | addr | role (this spec) | recon file |
|---|---|---|---|
| 0 | `0x54bf70` | `Initialize` | `recon/discovered/FUN_0054bf70_TTextBar_init.cpp` |
| 1 | `0x54c3d0` | `Close` | `recon/discovered/cls_0x5a5560_TTextBar_Close_54c3d0.cpp` |
| 7 | `0x54c600` | **Draw (Classic path)** — blit fg surface lines to display w/ per-line alpha fade | `/tmp` extract (see §6) |
| 13 | `0x54c9c0` | `Clear`/Reset (rebuild fg surface empty) | `/tmp` extract |
| 19 | `0x54c460` | **Tick/Update** — age out lines, pull combat-target name into feed | `/tmp` extract |
| 20 | `0x54c440` | `Animate` — recomposite + base draw (TPlyrStatusBar slot-20 twin) | `/tmp` extract |
| 23 | `0x54c780` | **Draw (hi-res path)** — `(*+0x5c)` mosaic blit + effect post-process | `/tmp` extract |
| 28 | `0x54d4a0` | chat/console **keystroke input** handler | `/tmp` extract |
| (helper) | `0x54cd40` | **Composite** — render all line records + health bar into fg/bg surfaces | `recon/discovered/cls_0x5a5560_TTextBar_FUN_54cd40.cpp` |
| (helper) | `0x54cb00` | **DrawHealthBar** — PutHue texthealthbar w/ hue+slide | `recon/discovered/FUN_0054cb00_TTextBar_DrawHealthBlock.cpp` |
| (helper) | `0x54d0c0` | **AddLine** — push a record into scrollback ring | `/tmp` extract |
| (helper) | `0x54d390` | append target-name+health line | `/tmp` extract |
| (helper) | `0x54d700` | clear input-edit buffer (thunk; not extracted — UNCONFIRMED-1) | — |
| (helper) | `0x438ed0` | **DrawLineText** — wraps `FUN_004be2b0` GDI DrawTextA | `/tmp` extract |
| (helper) | `0x521c60` | font-metric getter (ascent/descent from HDC table) | `/tmp` extract |
| (prim) | `0x4bd8c0` | `PutHue(x,y,bm,drawmode,hue)` = `TSurface::PutHue` (`src/surface.h:237`) | method-map / src |

Recon files read: all of the above, `FUN_0054bf70_TTextBar_init.cpp`,
`cls_0x5a5560_TTextBar_FUN_54cd40.cpp`, `FUN_0054cb00_TTextBar_DrawHealthBlock.cpp`,
`cls_0x5a5560_TTextBar_Close_54c3d0.cpp`, `cls_0x5a5560_TTextBar_ctor_globalinit_480725.cpp`,
`cls_0x5a54e4_TPlyrStatusBar_vtable.txt`. Briefs: `docs/ui/briefs/B_r4_textbar_assessment.md`.
Method-map primitives: `FUN_004be2b0` (text), `FUN_00438d80` (shadow/blit struct),
`FUN_004aacb0` (effect post-process), `FUN_00414d70` (blit), `FUN_00419dd0` (field copy).
Color/layout literals decoded from `objdump` (`Revenant.exe`, §7). Asset measured
with `tools/ui/dump_dat.py` + direct TMulti parse (§2).

**Status:** `forensics-complete` for the message-overlay + health-bar render paths
(the on-screen visuals). The chat-input handler (slot 28) and exact bg/fg surface
compositing order are documented but carry the UNCONFIRMED items in §14.

**Errors to close (existing port `src/textbar.{cpp,h}`):** the snapshot port is the
*pre-release* single-line `text[80]` design and is **superseded**. It (a) has no
scrollback ring, (b) no multi-surface buffered composition, (c) no per-line color
table, (d) no per-line TTL fade, (e) `Close()` is a no-op (retail frees 3 surfaces +
buffer). The reconstruction must replace its `DrawBackground` entirely; only the
health-bar math (`SetHealthDisplay`/`SetLevels`/health tween) survives verbatim.

**Snapshot note (supplementary — superseded by retail):** `src/textbar.h:42-48`
declares the old layout (`char text[80]; char name[80]; int32 level, targetlevel;
bool animating, pulsecheck;`). `src/textbar.cpp:33-85 DrawBackground` is the readable
intent for the **health block only** — its constants (155,176,16,186, increment 4)
are byte-identical to retail `FUN_0054cb00`, and `PutHue(min(0,-(186-level)),1,...)`
matches retail exactly, so the snapshot is authoritative *only* for the health-bar
algorithm (cited in §6/§9). Its layout, surfaces, and the message-feed are NOT — do
not pull old coordinates into §2–§5/§8.

---

## §1 — Overview

TTextBar is the bottom-of-screen text overlay. It serves two jobs:

1. **Game-message log.** Engine code calls a `Print`-style API to push short
   strings ("The door is now open", "Got the Hammer of Wounding", combat/system
   messages). Retail keeps a **scrollback ring of line records** (not the
   pre-release single line): each record has a 0x50-byte text, a type, a color, and
   a TTL counter. Lines age out and fade. Up to `0x64` records are stored; up to 9
   (`DAT_005e5800`) are visible.

2. **Opponent name + health bar.** When the player is fighting a target with no
   pending message, the bar shows the enemy name and a sliding/hue-shifting health
   strip (`texthealthbar`, 200×11). This feature survives from pre-release verbatim.

It is a **single instance** (the global `0x65c5d0`), not mirrored player/target like
TPlyrStatusBar. The pane is rendered into off-screen TMosaicSurface buffers (1 fg + 2
bg) then blitted to the display each frame; it is shown whenever it has visible
content. There is also a **chat/console input mode** (slot 28) where keystrokes build
a `msgprefix`-prefixed line — a typed-message feature.

Plain-language: it is the scrolling status-line overlay at the bottom of the play
view that prints colored game messages which fade after a few seconds, and doubles as
the enemy-health readout in combat.

---

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| texthealthbar | `legacy/PLAYSCRN.DAT` | `texthealthbar` (TMulti idx 98) | **200×11** (`0xc8 × 0x0b`), flags `0x2` BM_15BIT (RGB555), keycolor `0x0` | the opponent health gradient strip; drawn via PutHue with per-frame hue rotation + horizontal slide | full bitmap (no sub-rects) | measured: TMulti parse of PLAYSCRN.DAT bitmap idx 98 @ file off `642779`; `FUN_0054cb00:42` `s_texthealthbar_005e581c` |
| Font "Small" | (FONT.DEF, registered to HDC table) | `Small` | Arial, **12px**, GDI WINFONT; default COLOR 255 255 255 | all textbar line text (line color overridden per-record, see §8) | n/a (GDI glyph render) | `data/resources_unzipped/font.def` `WINFONT "Small" FONT "Arial" 12 COLOR 255 255 255 ...`; font index `DAT_0065abc4` set @ `0x485dcf` from `"Small"` (push `0x5d8844` @ `0x485db1` → call → eax → store `0x65abc4`) |

No chrome bitmap (BackPanel-style) exists — TTextBar is a **transparent overlay**; its
"background" is the 3 mosaic surfaces composited over the live 3D, not a static plate.

**texthealthbar source-rect note:** PutHue draws the whole 200×11 bitmap; the "fill"
effect is achieved by *sliding the bitmap left* (negative x) as health drops, not by
sub-rect clipping (§6 DrawHealthBar).

---

## §3 — Coordinate frames & surfaces

TTextBar is **NOT** the fixed-chrome chip model used by TPlyrStatusBar. It is a
dynamically-positioned overlay whose pane rect is computed from runtime screen-viewport
globals, and whose content is drawn into a display-width foreground surface then blitted.

### Frame table (mandatory)

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|---|---|---|---|---|---|
| **screen** | framebuffer TL (root) | — | (0,0) | final pixels | — |
| **pane** | screen | **bottom-left of play viewport** (computed, not fixed) | `pane.left = DAT_006668dc`; `pane.top = (DAT_006668e8 − maxLines·lineH) + DAT_006668e0`; `pane.right = DAT_006668e4`; where maxLines = `DAT_005e5800 = 9`, lineH = `+0x78` (font ascent+descent) | the whole overlay | `FUN_0054bf70:51-57` (`+0x14=DAT_006668dc`, `+0x18=(DAT_006668e8 − DAT_005e5800·iVar4)+DAT_006668e0`, `+0x1c=DAT_006668e4`) |
| **fg-surface** | pane | TL of pane (the surface is blitted at pane origin) | pane TL; surface size = `display_w × (lineH·12)` | all rendered line text + the health bar | `FUN_0054bf70:33-52,84-95` (`+0x70`=display width `*(PTR_DAT_005d79e0+4)`; `+0x74 = lineH·0xc`; surface alloc `FUN_004a5740`) |
| **bg-surface ×2** | pane | TL of pane | display_w × lineH each (`+0x88`,`+0x8c`) | per-line scratch (one line tall) used as the transparent multi-buffer compositing stage | `FUN_0054bf70:96-184` (`+0x88`,`+0x8c` = two `0x74`-byte TMosaicSurfaces sized `+0x70 × +0x74`) |
| **line-cell** | fg-surface | TL constant | **chrome-local `(4, 9)`** (text draw point inside fg surface, per-line) | one line's glyphs | `FUN_0054cd40:94` `FUN_00438ed0(4, 9, lineText, font,…)` |
| **healthbar-cell** | fg-surface | left edge, slides | **`x = min(0, level−186)`, `y = 1`** | the texthealthbar strip | `FUN_0054cb00:40-43` PutHue(`min(0,level−0xba)`, `1`, …) |
| **source-rect** | texthealthbar bitmap TL | — | full 200×11 | the health gradient | §2 |

**Composition formula (final screen coord of a line):**
`screen = pane_origin + fg_local`, e.g. line text screen pos =
`(pane.left + 4, pane.top + 9 + lineSlot·lineH)` where lineSlot scrolls (see §9).
The fg surface is the single render target; everything composes into it, then it
blits to the display at pane origin.

### Anchor detection / verification

- The pane is **bottom-anchored** to the play viewport: `pane.top = viewport_bottom −
  (9·lineH)` (`FUN_0054bf70:56` reads `DAT_006668e8` = viewport bottom and subtracts
  `9·lineH`). So as the viewport resizes the bar stays glued to the bottom of the 3D
  view — it is NOT a constant screen y. Record `anchor = bottom-left of play viewport,
  offset = 9·lineH up from its bottom`.
- The ctor stub `FUN_00480725` writes a *fallback* static rect **406×198** (`0x196 ×
  0xc6`) with padding 14 (`0xe`) at `0x65c5d8/dc/e0`; **Initialize overrides** width/top
  with the screen-viewport formula above, so 406×198 is only the pre-Initialize default
  and must not be treated as the live size (live width = `+0x70` = display width).

### Surfaces / direct-renderer contract

Retail uses 3 TMosaicSurfaces (1 fg `+0x84`, 2 bg `+0x88`/`+0x8c`) for transparent
double-buffered line scrolling. The port need not mirror the 3 surfaces 1:1 but MUST
keep the contract: **compose all lines + health into one display-width offscreen RT,
then blit that RT to the HUD at the pane origin** (NOMENCLATURE direct-renderer
contract). Do not draw glyphs straight to the swapchain.

### Layout diagram

```
play viewport (screen)
┌───────────────────────────────────────────────────────────┐
│                                                             │
│                    (3D world view)                          │
│                                                             │
│ pane.top = viewport_bottom − 9·lineH ───────────────────┐   │
│ ┌─ fg-surface  (display_w × 12·lineH) ─────────────────┐│   │
│ │ (4,9)  Got the Hammer of Wounding         <- line 0  ││   │
│ │ (4,9+lineH) The door is now open          <- line 1  ││   │
│ │ ...                                                   ││   │
│ │ health: PutHue at (min(0,level-186), 1) [200x11] ─┐  ││   │
│ │ ◄── strip slides left as health drops ────────────┘  ││   │
│ └──────────────────────────────────────────────────────┘│   │
│ pane.left=DAT_006668dc          pane.right=DAT_006668e4 ──┘   │
└───────────────────────────────────────────────────────────┘
```

---

## §4 — Static element layout

TTextBar has no fixed chrome elements; layout is per-line + the health strip. All
coords are **fg-surface-local** (compose `+ pane_origin` for screen — §3). No
player/target mirror (single instance).

| element | space | (x,y) | (w,h) | source rect | cite |
|---|---|---|---|---|---|
| line text (each visible line) | fg-surface | `(4, 9)` then scrolls by `lineH` per slot | clip `10000×10000` (effectively unclipped; single-line) | n/a (GDI render) | `FUN_0054cd40:94`; `FUN_00438ed0:15` (w=h=10000) |
| health strip | fg-surface | `(min(0, level−186), 1)` | `200×11` | full texthealthbar | `FUN_0054cb00:40-43` |

The "scroll by lineH" mechanism: `FUN_0054cd40` walks records and decrements the draw
y by `+0x78` (lineH) each record, wrapping by the surface height when negative
(`:131-134` `iVar6 -= lineH; if (iVar6<0) iVar6 += surfaceHeight`). lineH = `+0x78` =
`font_ascent + font_descent` (`FUN_0054bf70:50-52`).

---

## §5 — Draw order / composition

Two render paths selected by `DAT_006680c8` (the Classic-vs-hi-res / hardware flag).

**Per-frame composition (`FUN_0054cd40`, the Composite helper):**
1. Clear the fg surface (`+0x84`): `(*+100)(...)` Box fill (`:45`).
2. For each line record `i` in `[0, +0x60)` whose age/visibility passes
   (`iStack_1c == -1 || iStack_1c == i`, `:51`):
   a. Pick the **line color** from the type→color table by `record.type` switch
      (`:54-79`, §7).
   b. Clear the per-line bg surface (`+0x88`) `(*+100)(0,0,w,h,...)` (`:85`).
   c. If this is the health line slot (`iStack_3c==0 && +0x90!=0`): call
      `FUN_0054cb00` (DrawHealthBar) (`:87-90`).
   d. Draw the line text into the bg/fg via `FUN_00438ed0(4, 9, text, font, color,
      0x400, 0x80000000)` (`:91-94`) — GDI DrawTextA with shadow flag `0x400`.
   e. Build an `SDrawParam` via `FUN_00438d80` and blit the line surface onto the next
      surface with `(*+0x5c)` (`:96-115`); Classic vs hi-res differ only in the
      drawmode (`0x100` transparent vs `0x80000000`) and which scratch buffer
      (`auStack_94` vs `auStack_e8`).
   f. Blit the bg line into the fg accumulation via `(*+0x5c)` on `+0x8c` (`:116-129`).
   g. Advance draw y: `iVar6 -= lineH` (wrap) (`:131-134`).
3. (Done in the Draw slots) Blit the assembled fg surface to the **display**:
   - **Classic (slot 7, `FUN_0054c600`)**: per visible line, `FUN_00414d70(...)` copies
     a band of the fg surface to the display with **per-line alpha** =
     `min(255, ttl·255/24)` packed as `alpha<<24 | 0xffffff` (`:32-41,57-60`).
   - **Hi-res (slot 23, `FUN_0054c780`)**: `(*+0x5c)(SDrawParam, +0x8c, …)` mosaic blit
     per line (`:64-110`), then `FUN_004aacb0(...)` effect post-process (`:116-119`).

**Tick (slot 19, `FUN_0054c460`)** runs before draw: ages records (decrements TTL,
removes expired tail), and pulls the current combat target's name/level into the feed
(`:28-88`, via `DAT_00667fcc` player + `FUN_00570760`).

---

## §6 — Algorithms (pseudocode per helper)

### DrawHealthBar — `FUN_0054cb00`  (matches pre-release `textbar.cpp:43-72`)

```
# health tween toward target, then draw the sliding hue-shifted strip
delta = abs(level /*+0x98*/ - targetlevel /*+0x9c*/)
if delta < 5:                level = targetlevel              # snap when close (HEALTH_INCREMENT=4)
elif level < targetlevel:    level += 4
elif level > targetlevel:    level -= 4

hue = (level * 155 /*0x9b*/) / 176 /*0xb0*/
if hue < 17 /*0x11*/: hue = 0
else:                 hue -= 16 /*0x10*/

x = min(0, level - 186 /*0xba/*)          # (0<u)-1 & u  ==  min(0,u)  with u=level-186
PutHue(x, y=1, texthealthbar, drawmode=0x100 /*DM_TRANSPARENT*/, hue)
```
Cite: `FUN_0054cb00:13-43`. The strip is 200px wide; at level≥186 it sits at x=0
(fully visible), and slides left (x negative) as level drops to 0 (→ x=−186), so only
the right ~14px remain — the drain animation. `hue` rotates the palette so the bar
shifts color (green→red) with health. `level` increments by ±4/tick (24Hz).

### AddLine — `FUN_0054d0c0(this, type, color, text)`

```
if text == NULL or text[0] in {'\0',' '}: return        # skip empty/space-only
slot = (this+0xa0 != 0) ? 1 : 0                          # 0xa0 = "input line active" flag
memmove(records[slot+1], records[slot], (records_total - slot)*0x5c)   # shift ring down
if lineCount /*+0x60*/ < maxRecords /*+0x64*/: lineCount++
strncpy(records[slot].text /*+0xc*/, text, 0x4f); text[0x4f]=0
records[slot].type  = type     # +0
records[slot].color = color    # +4
records[slot].ttl   = 0x78     # +8  (= 120 ticks = 5s @24Hz)
Composite(0xffffffff)          # FUN_0054cd40(-1) → recompose all lines
```
Cite: `FUN_0054d0c0:12-29`. **Line record stride = `0x5c` = 92 bytes**; layout:
`+0x00 type`, `+0x04 color`, `+0x08 ttl`, `+0x0c text[0x50=80]`. (Stride confirmed by
`FUN_0054c460:17,25` `i*0x5c` / `piVar2 += -0x17`, and `FUN_0054cd40:136` `iStack_64 +=
0x5c`.)

### DrawLineText — `FUN_00438ed0(x, y, text, font, color, flags, drawmode)`

```
flags2 = flags | 0x80                       # 0x80 = single-line override (UI_METHOD_MAP §5)
SetField(&color)                            # FUN_00419dd0 copies packed color into call struct
FUN_004be2b0(surface, x, y, 10000, 10000, text, color=0, font, flags2, flags2, drawmode)
```
Cite: `FUN_00438ed0:11-15`. Delegates to the GDI DrawTextA compositor
(UI_METHOD_MAP §5). The textbar always passes `font = DAT_0065abc4` (Small/Arial-12),
`flags` carrying the `0x400` shadow bit, drawmode `0x80000000`, and the per-line color
(`FUN_0054cd40:91-94`). Cell is effectively unbounded (10000×10000) → single line,
left-aligned, top.

### Tick/age-out — `FUN_0054c460` (slot 19)

```
for i from (lineCount-1) down to (+0x68 = floor index, =3):
    records[i].ttl -= 1
    if records[i].ttl < 1 and i == lineCount-1:  lineCount--       # pop expired tail
# combat-target feed: if a target exists (DAT_0066829c, DAT_00667fcc),
# walk the active object list (FUN_00570760) and AddLine the target's
# name with a color keyed by its faction/level (FUN_0054d0c0 type 0x40/8)
```
Cite: `FUN_0054c460:14-88`. Confirms TTL is at record+8 and counts down each tick;
`+0x68`(=`DAT_005e5804`=3) is the floor below which lines aren't aged (pinned header
slots). The faction-color path picks color from `&DAT_005e20b8[level*4]` (`:58`).

### Draw (Classic) per-line alpha fade — `FUN_0054c600` (slot 7)

```
for each visible line:
    ttl = records[i].ttl
    alpha = (ttl < 24 /*0x18*/) ? (ttl*255/24) : 255
    if alpha changed since last band: flush band via FUN_00414d70(..., alpha<<24 | 0xffffff, ...)
```
Cite: `FUN_0054c600:31-60`. **Lines fade over their last 24 ticks (~1s @24Hz)**:
alpha = ttl·255/24. The hi-res path (slot 23) does the same fade implicitly via the
mosaic blit + effect pass.

---

## §7 — Effects & shadows

- **Per-line text shadow.** `FUN_00438ed0` passes flag bit **`0x400`** (set in
  `FUN_0054cd40:92` `uVar8 = 0x400`) → `FUN_004be2b0` runs its 3-pass black drop shadow
  (base, +1x, +1y) then the colored pass (UI_METHOD_MAP §5 / NOMENCLATURE §4). So every
  line is white/colored text with a 1px black down-right shadow. Reproduce as
  `DrawTextShadowedToTarget`.
- **Line-type color table** (decoded from the static-init block `0x54be60-0x54bf6b`
  via objdump; each entry is `[+0]=B,[+1]=G,[+2]=R,[+3]=pad`, consumed RGB→BGR-swapped
  by `FUN_004be2b0:378`). The `FUN_0054cd40` switch (`:54-79`) maps `record.type`:

  | record.type | global | bytes (B G R) | **RGB** | meaning |
  |---|---|---|---|---|
  | 1 / default | `DAT_0067064c` | 00 c8 ff | **(255,200,0)** gold/amber | normal game message |
  | 2 | `DAT_00670664` | ff 00 b4 | **(180,0,255)** violet | (system type 2) |
  | 4 | `DAT_00670654` | b4 00 ff | **(255,0,180)** magenta-pink | (system type 4) |
  | 8 | `DAT_00670668` | 28 ff ff | **(255,255,40)** yellow | enemy/combat name (hostile) |
  | 0x10 | `DAT_0067065c` | ff 00 be | **(190,0,255)** purple | (type 0x10) |
  | 0x20 | `DAT_00670658` | ff ff ff | **(255,255,255)** white | chat/input echo |
  | 0x40 | `DAT_00670660` (fallback) or `record.color` (`puVar2[1]`) | 00 d2 00 | **(0,210,0)** green (fallback) | target name (uses per-record color if nonzero, else green) |
  | 0x80 | `DAT_00670650` | ff ff ff | **(255,255,255)** white | (type 0x80) |

  Cite: switch `FUN_0054cd40:54-79`; byte values objdump `0x54be72-0x54bf63`
  (`%al` tracked: xor-zero before 064c/0664/0654/065c/0660, `$0x28` before 0668,
  `$0xff` before 0658/0650/0658-block). Type `0x40` reads `record.color` (`puVar2[1]`)
  and only falls back to green when it is 0 (`:71-76`).

- **Health bar hue rotation** (not a shadow): `PutHue` rotates the texthealthbar palette
  by `hue` (§6) so the strip color tracks health. drawmode `0x100` = DM_TRANSPARENT.
- **Chroma key:** texthealthbar keycolor field = `0x0`; the global magenta key
  (`0x7c1f` RGB555) applies on blit (UI_METHOD_MAP §16). Text uses real GDI coverage,
  not magenta — see §11 pink-halo note.

---

## §8 — Text rendering

All textbar text uses font **`DAT_0065abc4` = "Small" = Arial 12px** (GDI WINFONT),
via `FUN_00438ed0` → `FUN_004be2b0`. Coords are fg-surface-local (compose `+ pane_origin`).

| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format / source | cite |
|---|---|---|---|---|---|---|---|---|---|
| each message/feed line | fg-surface, `(4, 9, 10000, 10000)` (single-line, scrolls by lineH per slot) | Small (Arial) | 12 | per-record (§7 table; default gold (255,200,0)) | **left** (`0x80` single-line override; no `&2`/`&4` set → DrawTextA default left) | **top** (DT_TOP; no DT_VCENTER) | yes (`0x400` 3-pass black) | record.text (`+0xc`), filled by AddLine `strncpy(..,text,0x4f)` | `FUN_0054cd40:91-94`; `FUN_00438ed0:15`; `FUN_0054d0c0:21` |

- **h-align = left, v-align = top.** `FUN_00438ed0` ORs `0x80` (single-line) into the
  flags; no center/right bit (`&2`/`&4`) is set, so DrawTextA defaults to left/top
  (UI_METHOD_MAP §5). Lines stack **downward** from the cell top by lineH; do not
  vertically center.
- **Visual-anchor check:** the only "centered-looking" element is the health bar, which
  is a bitmap (not text) positioned by formula (§6) — no text-centering claim to verify.
  Line text is left-anchored at x=4 inside the pane; its visual left edge = `pane.left+4`.
- **Color source:** packed at the `FUN_0054cd40` call site from the §7 type→color table
  (NOT from FONT.DEF "Small"'s default white). The `0x400` shadow bit lives in the
  flags arg, not the font id. Format strings: AddLine takes a pre-formatted C string;
  the combat-feed line is the object name `*(target+0x38)` copied via strncpy
  (`FUN_0054d390:35`). The chat-input line is `msgprefix` + typed chars
  (`FUN_0054d4a0:38-128`).

---

## §9 — Animation & dynamic behavior

```
ramp record.ttl (per line):
  range   0..0x78 (120)
  step    -1 per tick (24Hz) once aged (slot 19 FUN_0054c460:18-22)
  start   0x78 (120) on AddLine (FUN_0054d0c0:27)  ≈ 5.0 s lifetime
  maps-to alpha = (ttl<24) ? ttl*255/24 : 255   (slot 7 FUN_0054c600:32-36)
  on-end  line popped from tail when ttl<1 (FUN_0054c460:21-22)
  cite    FUN_0054d0c0:27, FUN_0054c460:18-22, FUN_0054c600:32-36
```

```
tween health level (+0x98) -> targetlevel (+0x9c):
  from   level
  to     targetlevel
  over   |Δ|/4 ticks (±4 per tick, snap when |Δ|<5)
  easing linear (step ±4)
  drive  SetHealthDisplay/SetLevels updates targetlevel; pulsecheck keeps it alive
  on-end hold; bar cleared if pulsecheck not refreshed (ClearHealthDisplay)
  maps-to hue=(level*155/176) then -16 floor 0; x=min(0,level-186)
  cite   FUN_0054cb00:13-43 (== textbar.cpp:43-72)
```

Temporal (line fade):
```
alpha (0..255)
255 ┤────────────────●╮
    │                  ╲  linear, alpha=ttl*255/24
  0 ┤                   ●
    └──────────────────────── ttl
    ttl: 120 ............ 24 ...... 0   (held opaque, then 24-tick fade-out, then popped)
```

**Dirty / redraw model:** retail recomposes the whole fg surface on AddLine
(`FUN_0054cd40(-1)`) and on Clear; the Draw slots blit it every frame. There is a
pulse-keepalive for the health line (pre-release `pulsecheck`; retail `+0x90`/`+0xa0`
flags) — if `SetHealthDisplay` isn't called each tick the health line clears
(`ClearHealthDisplay`). Port should use a version counter (NOMENCLATURE §6) marking the
fg RT dirty on AddLine/age/health-change.

**Phases:** normal-message mode ↔ input/chat mode (`+0xa0` flag; slot 28 builds a
`msgprefix`-led line as the user types, committed on Enter `0xd`).

---

## §10 — Input & dispatch

TTextBar is mostly non-interactive (no hit rects / click commands). It has one input
surface: the **chat/console keystroke handler, slot 28 `FUN_0054d4a0(this, key, down)`**:

- `+0xa0 == 0` and key `0xd` (Enter): enter input mode — seed line with `msgprefix`,
  set `+0xa4 = 0x50 − prefixLen` (remaining capacity), `+0xa0 = 1` (`:29-53`).
- In input mode: Enter `0xd` → commit (`FUN_0054d390`); Backspace `0x8` → delete last
  char (`:56-74`); printable (`0x20..0xff`) → append into the edit buffer at `+0xd0`
  (`:77-110`); then recompose (`FUN_0054cd40(0)`).
- The edit buffer is at `this+0xd0` (`+0xd0`); committed lines copy it into a record.

This is the typed-message / debug-console path; gameplay messages arrive via the
`Print`/AddLine API (`FUN_0054d0c0`), not input. Skip click/hit-rect handling (none).

---

## §11 — Retail bugs NOT to reproduce

1. **Pink-halo on shadowed text.** The `0x400` 3-pass shadow over a magenta-cleared
   scratch produces a pink fringe in retail (NOMENCLATURE §4, UI_METHOD_MAP §16,
   `TPlyrStatusBar_SPEC.md:295`). **Do not reproduce.** Render glyphs with real alpha
   coverage (white RGB, coverage in alpha), 3 black-tinted passes (base/+1x/+1y) then
   the colored pass — no magenta key on text.
2. **406×198 fallback rect.** The ctor stub's static rect (`FUN_00480725`) is a
   pre-Initialize default that does NOT reflect the live overlay; using it as the
   render size would mis-size the bar. Always use the Initialize-computed pane rect
   (display-width × viewport-anchored top).

---

## §12 — Reconstruction pseudocode

```
struct Line { int type; uint32 color; int ttl; char text[0x50]; };   // 0x5c bytes
state: Line lines[0x64]; int lineCount; int maxVisible=9; int lineH; Surface fgRT;
       int healthLevel, healthTarget; bool healthActive; bool inputActive; char editBuf[...];

Initialize():
    lineH = font.ascent + font.descent           # Small / Arial-12 metrics
    fgRT  = new Surface(display_w, lineH*12)      # one display-width RT (replaces 3 retail surfaces)
    pane.left  = viewport.left
    pane.right = viewport.right
    pane.top   = viewport.bottom - maxVisible*lineH   # bottom-anchored

AddLine(type, color, text):                       # FUN_0054d0c0
    if !text || text[0]=='\0' || text[0]==' ': return
    slot = inputActive ? 1 : 0
    memmove(lines+slot+1, lines+slot, (lineCount-slot)*sizeof(Line))
    if lineCount < 0x64: lineCount++
    lines[slot] = { type, color, ttl=0x78, strncpy(text,0x4f) }
    markDirty()

Tick():                                            # slot 19 FUN_0054c460
    for i in [lineCount-1 .. 3]:
        if --lines[i].ttl < 1 and i==lineCount-1: lineCount--
    feedCombatTargetName()                          # AddLine target name if fighting
    healthTween()                                   # ±4 toward healthTarget
    markDirty()

Composite():                                       # FUN_0054cd40 — into fgRT
    clear(fgRT)
    y = 9
    for i in [0 .. lineCount):
        col = colorForType(lines[i].type, lines[i].color)   # §7 table
        if i==healthSlot and healthActive: DrawHealthBar(fgRT)
        DrawTextShadowedToTarget(fgRT, atlasSmall, lines[i].text, x=4, y, w=∞,
                                 ALIGN_LEFT, col.r, col.g, col.b)   # 0x400 shadow
        y += lineH                                   # stack downward
    markClean()

DrawHealthBar(rt):                                 # FUN_0054cb00 / textbar.cpp:43-72
    healthTween()
    hue = (healthLevel*155/176); hue = (hue<17)?0:hue-16
    PutHue(rt, x=min(0, healthLevel-186), y=1, texthealthbar, DM_TRANSPARENT, hue)

Draw():                                            # slot 7 (classic) / 23 (hi-res)
    for each visible line band:
        alpha = (lines[i].ttl < 24) ? lines[i].ttl*255/24 : 255
        blit fgRT band -> display at pane_origin, with band alpha   # fade-out tail
    # (port: composite fgRT to HUD with per-line alpha, or bake alpha into RT)

OnKey(key,down):                                   # slot 28 FUN_0054d4a0 (chat input)
    ... msgprefix-led edit buffer; Enter commits via AddLine ...
```

---

## §13 — Port mapping notes

| retail call | port primitive | home |
|---|---|---|
| `FUN_00438ed0`→`FUN_004be2b0` text (flags `0x80` single-line, `0x400` shadow) | `DrawTextShadowedToTarget(atlas, text, x, y, w, ETextAlign::Left, r,g,b)` | font.cpp |
| build "Small" font atlas | `BuildTTFAtlas("<Arial-substitute>", 12)` | font.cpp |
| `PutHue` (`FUN_0054cb00`) | **needs new primitive:** `DrawBitmapHueRotatedToTarget(bm, x, y, hue)` — a palette/hue-rotation blit. No existing renderer primitive does hue rotation on a 555 bitmap. (Alternative: pre-bake N hue variants of texthealthbar.) FLAG for renderer. | renderer (new) |
| `FUN_00414d70` band blit w/ alpha (slot 7) | `DrawBitmapSubrectTintedToTarget` / compose fgRT with per-band alpha | renderer |
| `(*+0x5c)` mosaic blit + `FUN_004aacb0` (slot 23) | `DrawSurface` / `DrawSurfaceTinted` (compose fgRT → HUD) | renderer |
| `(*+100)` Box clear of surfaces | `Surface::Box` / clear RT | surface |
| 3 TMosaicSurfaces | one offscreen `TSurface` RT (direct-renderer contract) | renderer/surface |

Missing primitive: **hue-rotated bitmap draw** for the health strip (texthealthbar +
`PutHue`). Either add a hue-rotation composite or pre-generate hue LUT variants.

---

## §14 — UNCONFIRMED / open questions

1. **`FUN_0054d700` (edit-buffer clear) not extracted.** Appears as a thunk/jmp; the
   `Disasm.java` extract returned empty. Impact: low (only affects chat-input reset).
   Resolve: `DecompileAddr.java 0x0054d700` (retry; may need disassembly mode).
2. **Exact health-line compositing y inside `FUN_0054cd40`.** `:88` calls
   `FUN_0054cb00(iVar6)` where `iVar6` is the running scroll-y; the health bar's own
   draw uses y=1 (its cell), but which line *slot* the health line occupies and how it
   interleaves with text lines is inferred (`iStack_3c==0 && +0x90!=0` gate, `:87`).
   Impact: medium (vertical placement of the health strip relative to text lines).
   Resolve: visual-verify against a combat capture; or trace `+0x90`/`iStack_3c` set
   sites.
3. **Line-record sub-fields beyond +0/+4/+8/+0xc.** Stride 0x5c (92) is confirmed but
   only `type(+0)`, `color(+4)`, `ttl(+8)`, `text(+0xc..+0x5b, 80 bytes)` are pinned.
   Bytes `+0x5c..` of the 92-byte record beyond the 80-char text + 12-byte head are
   padding/unknown. Impact: low. Resolve: inspect AddLine writers for other offsets.
4. **`DAT_005e5804`(=3) at `+0x68` semantics.** Used as the age-floor index in slot 19
   (`FUN_0054c460:14`) — lines `[0..2]` are not aged (pinned). Read as "3 header/pinned
   slots." Impact: low (top lines may persist). Resolve: confirm against which lines are
   pinned in-game (system header vs scrollback).
5. **Type→color table type meanings (types 2, 0x10) labels.** RGB values are decoded
   exactly (§7); the *gameplay meaning* of types 2 (violet) and 0x10 (purple) is
   inferred. Impact: cosmetic (right color regardless). Resolve: grep AddLine call
   sites for which engine events pass each type.
6. **Hue-rotation exact palette math in `PutHue`/`FUN_004bd8c0`.** The 200×11 strip is
   RGB555 (no palette in the bitmap header) — "hue" likely indexes a hue-shift LUT or
   rotates 555 channels. The pre-release `PutHue` does a hue change on the bitmap;
   exact transform not extracted. Impact: medium (health-bar color accuracy). Resolve:
   `DecompileAddr.java 0x004bd8c0` (PutHue body) — currently only signature confirmed.
7. **Classic vs hi-res path divergence (slot 7 vs 23).** Both render the same lines;
   they differ in blit primitive + the `0x100`/`0x80000000` drawmode and the effect
   post-process (`FUN_004aacb0`). The port should use the single compose-to-RT path;
   the two retail paths are an artifact of the software/hardware split. Confirmed both
   produce the same visual (per-line fade); no UNCONFIRMED on the visual, only on which
   path the target build uses (`DAT_006680c8`).
