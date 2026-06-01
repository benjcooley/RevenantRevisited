# UI Reconstruction Protocol

You are a **UI reconstruction agent**. You rebuild one Revenant UI panel in the
engine from a completed forensics spec, verify it visually, and report any spec
gaps back. Your spec is the single source of truth — if it's complete you should
not need to open the recon at all.

## Hard rules

0. **Write a BRAND-NEW file. Never patch an existing accreted shell.** Clean-room
   means a clean file. Create a new `src/ui<panel>test.{cpp,h}` (and register a
   new `--test=` mode) written entirely from the spec. If a prior hand-iterated
   shell for this panel exists, do NOT edit it — once your new file is verified,
   retire the old one (delete, or move to `attic/`). Editing the old file lets
   its bugs and stale assumptions hide inside your "reconstruction"; a fresh file
   makes the clean-room build verifiably independent. (Proof: the side-tabs panel
   was a brand-new file and was correct first pass; the stat bar was edited
   in-place and reintroduced errors.)
1. **The spec is the contract.** Build from `docs/ui/forensics/<ClassName>_SPEC.md`.
   Every coordinate, asset, color, alignment, shadow, tween, and dispatch
   mapping comes from the spec. If you find yourself opening `recon/discovered/`
   to answer a question, the spec has a GAP — note it (see rule 7) rather than
   silently patching from source.
2. **Read the shared references** so you speak the same language as the spec:
   [NOMENCLATURE.md](NOMENCLATURE.md) (coordinate spaces, tween notation,
   diagrams) and [UI_METHOD_MAP.md](UI_METHOD_MAP.md) (the "Port mapping" line
   for each retail primitive tells you which `src/renderer.h`/`src/surface.h`
   call reproduces it).
3. **No mocks, no stand-ins.** Render via the real retail assets named in the
   spec's §2, through the real port primitives. If a bitmap looks wrong, debug
   the loader/decoder — don't bypass with custom drawing
   ([[feedback-ui-no-mock-use-retail]]). If the spec names an asset the engine
   can't yet load, escalate; don't substitute a procedural fill
   ([[feedback-no-standins]]).
4. **Preserve the coordinate-space contract** (NOMENCLATURE §3 direct-renderer
   contract). Compose the chip into a fixed render target, then blit that surface
   in the HUD swapchain pass. Do NOT mix a bitmap path and a glyph-straight-to-
   swapchain path — window-scale changes will make text and art drift apart.
5. **Reproduce intent, not bugs.** The spec's §11 lists retail rendering bugs
   (pink halo, key bleed). Implement the intended visual, not the artifact.
6. **Gate animation to 24Hz.** Tweens/ramps in the spec's §9 advance per sim
   tick (`kSimTickMs = 1000/24`), not per render frame.
7. **Report spec gaps.** Anything you had to re-derive, guess, or that the spec
   got wrong → report it back so the spec + protocol improve. A reconstruction
   that needed zero source reading is the goal; gaps are the feedback that gets
   us there.

## Port primitives (the toolbox)

**Two hard rules govern this toolbox:**

1. **Use the common methods — never hand-roll.** A drop shadow, a tinted blit,
   a 3-pass text shadow, a glyph walk: these already exist as shared primitives.
   Call them. Do NOT open-code a tinted-black pass + a bitmap pass, or a
   per-glyph composite loop, inside a panel. Panels hold layout + data; the
   *how-to-draw* lives in the shared primitives.
2. **Missing primitive → implement ONCE in the canonical location, register it,
   then use it.** If the retail panel needs a draw operation the toolbox doesn't
   have, do not improvise a panel-local version. Add it to the canonical home so
   every future panel reuses your one implementation:
   - **Pixel/image primitives** (blit, tint, shadow, composite, 9-slice) →
     `TRenderer` in `src/renderer.{h,cpp}`. `TRenderer` is game-agnostic — it
     speaks pixels, not fonts.
   - **Text/font primitives** (glyph walk, alignment, text-with-shadow,
     measurement) → the UI/font layer in `src/font.{h,cpp}`. These emit per-glyph
     blits through the renderer's to-target composite.
   Then add a row to this table AND to `UI_METHOD_MAP.md`'s port-mapping so the
   next agent finds it. Note what you added in your gap report.

### The compose-to-target contract (default for panels)

A panel composes its whole chip into one offscreen `TSurface` render target,
then `DrawSurface`s that to the HUD (NOMENCLATURE §3 — prevents text/bitmap
scale drift). So the per-element primitives a panel uses are the **`…ToTarget`
family** (they emit through the RGBA8 render-target composite into the active
`TSurface` pass). The non-target / `…Swapchain` variants are for drawing
straight to the HUD swapchain (cursor, overlays) — not the usual panel path.

| need | to-target (panel default) | swapchain (direct-HUD) |
|------|---------------------------|------------------------|
| blit a bitmap | `DrawBitmapToTarget(bm,x,y, tw,th)` | `DrawBitmap(bm,x,y)` |
| blit an atlas sub-rect | `DrawBitmapSubrectToTarget(bm, dx,dy,sx,sy,sw,sh, tw,th)` | `DrawBitmapSubrect(...)` |
| tinted blit (color) | `DrawBitmapTintedToTarget` / `DrawBitmapSubrectTintedToTarget(... tr,tg,tb,ta)` | `DrawBitmapTinted` / `DrawBitmapSubrectTinted` |
| **image with drop shadow** | `DrawBitmapShadowedToTarget` / `DrawBitmapSubrectShadowedToTarget(... off_x,off_y, a)` | `DrawBitmapShadowed` / `DrawBitmapSubrectShadowed` |
| **text** | `DrawTextToTarget(atlas, text, cellX,cellY,cellW, align, r,g,b, tw,th)` | (build a swapchain variant if ever needed) |
| **text with shadow** | `DrawTextShadowedToTarget(... same args)` — 3-pass black + colored top | — |
| text width / ascent (for align) | `TextWidth(atlas,text)` / `TextAscent(atlas)` | same |
| compose chip RT → HUD | `DrawSurface(surf, x,y)` / `DrawSurfaceTinted` | — |
| 9-slice stretch panel | `DrawNineSliceToTarget(bm, l,t,r,b, dx,dy,dw,dh, tw,th)` | `DrawNineSlice(bm, l,t,r,b, dx,dy,dw,dh)` |
| solid rect | `DrawSolidRectToTarget(x,y,w,h, tw,th, r,g,b,a)` | `DrawSolidRect(x,y,w,h, r,g,b,a)` |
| build a TTF atlas | `BuildTTFAtlas(path, px)` → `SFontAtlas` | same |
| build a bitmap-font atlas | `BuildFontAtlas(TFont*)` | same |

Text face: the bundled redistributable is **Liberation Sans**
(`thirdparty/fonts/LiberationSans-Regular.ttf` / `-Bold.ttf`), Arial-metric
compatible. Build the px size the spec's §8 gives. `DrawTextShadowedToTarget`
already does the retail 3-pass black shadow (base / +1x / +1y) + colored top —
you never write that loop yourself. `ETextAlign::{Left,Center,Right}` handles
in-cell alignment from `TextWidth`; the baseline is top-aligned at
`cellY + TextAscent` (stable GDI-style ascent, not per-string glyph bbox).

Chroma key is magenta (`0xFF00FF`); the bitmap decoder already keys it to
alpha — verify a sprite that should be transparent actually is.

## Step 1 deliverable — the reconstruction burndown

Before writing code, derive a **burndown** from the spec and write it to
`docs/ui/forensics/<ClassName>_BURNDOWN.md`. It is the checkbox list of every
discrete thing you must build/recreate, pulled straight from the spec sections —
the one place to see "what's left" at a glance. Work it top to bottom, checking
items off (`[ ]` → `[x]`) as you land + visually confirm each, and keep it
updated as the living status of the panel.

Group items by category and cite the spec section each came from:

```markdown
# <ClassName> — Reconstruction Burndown
Spec: <ClassName>_SPEC.md   Status: <not-started|in-progress|done>

## Assets to load            (spec §2)
- [ ] <archive>:<entry> <WxH> → <port handle>            (§2)

## Surfaces / render target  (spec §3)
- [ ] compose chip into fixed RT <WxH>; blit in HUD pass (§3)

## Static elements           (spec §4)
- [ ] <element> at <space:(x,y)> size <w,h> src <rect>   (§4)   [player]
- [ ] <element> mirror                                   (§4)   [target]

## Draw order                (spec §5)
- [ ] sequence step N: <what>                            (§5)

## Algorithms                (spec §6)
- [ ] <helper> (e.g. 4-slice bar fill)                   (§6)

## Effects / shadows         (spec §7)
- [ ] per-side drop shadow <offsets>                     (§7)
- [ ] text 3-pass black shadow                           (§7)

## Text                      (spec §8)
- [ ] <string> cell <rect> font <px> color <c> align <a> (§8)

## Animation                 (spec §9, 24Hz)
- [ ] tween/ramp <property> <from→to/over/easing>        (§9)

## Input / dispatch          (spec §10)
- [ ] hit rect <rect> → command <id>                     (§10)

## New port primitives needed (spec §13)
- [ ] <primitive> (or "none — all exist")               (§13)

## Errors to close            (spec §0)
- [ ] <error> → <fix>                                    (§0)

## Spec gaps found during build (feed back to forensics)
- [ ] <gap> — what was missing / had to re-derive
```

Keep it concise — one line per item. The burndown is the contract's task list;
the spec is the contract's truth. When every box is checked and the visual
matches, the panel is done.

## Test harness

Wire a `--test=<mode>` example screen for the panel (see existing
`src/ui*test.cpp`). Default to driving it in isolation before testing through a
full game boot ([[feedback-test-modes-for-ui]]). Feed synthetic state that
exercises every cell/value/animation the spec describes.

## Visual verification (be thorough; double-check your own work)

A prior reconstruction was reported "matches reference" when it did not. Treat
your own "it looks right" with suspicion until you've verified element-by-element.

1. Build clean (`cmake --build build -jN`).
2. Run the test mode; capture the window:
   ```python
   import sys, subprocess; sys.path.insert(0,'tools/ui'); import snap_compare
   r = snap_compare.find_revenant_window_info(pid)   # (pid of the launched binary)
   subprocess.run(['screencapture','-x','-o','-l',str(r[0]),'/tmp/out.png'])
   ```
   or use `tools/ui/snap_compare.py <mode> <reference.png>` for a same-scale A/B
   board (native integer zoom; the reference PNG is for regression check only,
   per [[reference-visual-verification]]).
3. Crop to the panel, `Read` the PNG, and compare **element by element** against
   the spec's layout diagram AND the reference image. Go down a checklist, not a
   gestalt glance: each element position; text position/size/color/h-align AND
   v-align; shadow direction+offset; animation cadence; **the mirrored side
   independently** (it's the most error-prone). Note every deviation, even ~2px.
4. **Double-check before declaring done:** confirm you ran the freshly-built
   binary (kill stale instances first — an old process keeps the old binary in
   memory and shows old output); re-capture; re-compare. Stating "done" without a
   capture you actually examined is forbidden.
5. Iterate until it matches. If something can't match because the spec is
   ambiguous/wrong, that's a gap — fix forward by reporting, not by guessing new
   coordinates.

## Workflow checklist

1. Read the spec end to end. If status is `forensics-partial` or §14 is large,
   flag before sinking time in.
2. **Derive the burndown** (`<ClassName>_BURNDOWN.md`) from the spec — your task
   list for everything below.
3. Build per §5 draw order + §6 algorithms, pulling coords (§4), assets (§2),
   text (§8), effects (§7), animation (§9) directly from the spec. Check off
   burndown items as you land + visually confirm each.
4. Compose to a fixed render target; blit in the HUD pass (§3 contract).
5. Wire the `--test=` harness with synthetic state.
6. Build clean; verify visually against the spec diagram + reference.
7. Report spec gaps (rule 7) — list every fact you had to re-derive; record them
   in the burndown's "Spec gaps found" section.
8. Commit on the UI feature branch.

## Anti-patterns (forbidden)

- **Hand-rolling a shadow or a glyph walk in a panel.** Use
  `DrawBitmap{,Subrect}ShadowedToTarget` and `DrawText{,Shadowed}ToTarget`. If a
  shared method is missing, add it to the canonical location (renderer for
  pixels, font.cpp for text) — never a panel-local copy.
- **Inventing a panel-local draw primitive.** New primitives go in the shared
  toolbox + get registered in this protocol's table and `UI_METHOD_MAP.md`, so
  the next panel reuses them.
- Opening the recon to fill a spec gap silently (report it instead).
- Procedural stand-in for a named retail asset.
- Mixing a to-target path and a swapchain path in the same panel (scale drift) —
  pick the compose-to-target contract and stay in the `…ToTarget` family.
- Reproducing a spec §11 bug.
- Ungated animation (framerate-dependent speed).
- Declaring done without a visual capture comparison.
