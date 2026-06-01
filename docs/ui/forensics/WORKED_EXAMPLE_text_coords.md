# TPlyrStatusBar Text Coordinate Forensics

This note documents the workflow used to recover the player/target stat-value
text placement from the retail Ghidra decomp. It is intentionally procedural so
another agent can reuse it on other buffered UI panes.

## Files To Start From

- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp`
- `recon/discovered/FUN_004be2b0_CompositeBlit.cpp`
- `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_helper_PortraitDraw_54ae10.cpp`
- `src/graphics.h` for `SDrawParam` layout.

## The Important Trap

`FUN_004be2b0_CompositeBuffer(x, y, w, h, text)` does not give the final
screen destination. In slot 23, it draws text into the `+0x64` scratch text
surface. The later `vtable+0x5c` calls blit rectangles from that scratch
surface to the live pane; those stack-built `SDrawParam` structs contain the
actual destination coordinates.

Treat the text path as two separate coordinate spaces:

1. `CompositeBuffer` rect: text draw/source cell inside the scratch surface.
2. `SDrawParam` before `vtable+0x5c`: final destination on the pane plus the
   source rectangle copied out of the scratch surface.

## Decoding Stack-Built SDrawParam

Use `src/graphics.h` as the byte map:

```text
0x00 drawmode
0x04 func
0x08 callback
0x0c data
0x10 originx
0x14 originy
0x18 clipx
0x1c clipy
0x20 clipwidth
0x24 clipheight
0x28 dx
0x2c dy
0x30 dwidth
0x34 dheight
0x38 sx
0x3c sy
0x40 swidth
0x44 sheight
0x48 color
0x4c intensity
0x50 zpos
0x52 normal
```

In Ghidra output, locals often alias and the pointer passed to `vtable+0x5c`
may move (`&acStack_e4`, `&stack0xffffff0c`, `&pcStack_104`). Re-anchor by
looking for a consistent 0x58-byte `SDrawParam` run and matching known values:
`drawmode=0x100` (`DM_TRANSPARENT`), `dwidth=0x32`, `dheight=0x0e`.

When Ghidra's stack variable names are too muddy, use retail disassembly
directly:

```sh
objdump -d --start-address=0x54b820 --stop-address=0x54ba80 data/Revenant.exe
objdump -d --start-address=0x54bbf0 --stop-address=0x54be15 data/Revenant.exe
```

For these calls the compiler commonly does:

```text
push arg4
push arg3
push source_surface
leal 0x28(%esp), %edx
push %edx
...
call *0x5c(%eax)
```

The `leal` happens before the final `push`, so after that final `push` the
`SDrawParam` base is at `0x2c(%esp)`. This makes the high-signal stores:

```text
0x2c(%esp) -> drawmode
0x54(%esp) -> dx
0x58(%esp) -> dy
0x5c(%esp) -> dwidth
0x60(%esp) -> dheight
0x64(%esp) -> sx
0x68(%esp) -> sy
0x6c(%esp) -> swidth
0x70(%esp) -> sheight
0x78(%esp) -> intensity
```

Some values are written before one or more pushes. Translate those writes by
the current `esp`, not by the final visible offset. The easiest check is to
land on a known pair like `dx=0x47`, `dy=0x07` for the player health value,
then decode the neighboring stores from that same base.

## Font Flags

`FUN_004be2b0_CompositeBlit.cpp` maps the low bits of the font/flag argument
to DrawText alignment:

- `font_id & 1`: left
- `font_id & 2`: center
- `font_id & 4`: right
- `font_id & 0x400`: draw black shadow passes before the colored pass

For TPlyrStatusBar:

- `0x401`: player stat values, left-aligned, shadowed
- `0x402`: name/level label, centered, shadowed
- `0x404`: target stat values, right-aligned, shadowed

The font pointer passed immediately after those flags is `DAT_0065abc4`.
Retail static init at `0x485db1..0x485dc5` stores `FONT.DEF "Small"` there.
The later `FONT.DEF "Numbers"` global is `DAT_0065c5c0`, not this status-bar
path.

Do not infer the stat-value color from `FONT.DEF "Numbers"`. Slot 23 supplies
the color separately before calling `FUN_004be2b0_CompositeBuffer`: the call
site pushes `0xff, 0xff, 0xff` through `FUN_00429950_BlitEffect_PackRGB` and
copies that packed value with `FUN_00419dd0_SetField`. The intended stat text
top pass is therefore white. If the direct TTF path looks gray or tinted, debug
the atlas/upload/blend path first; the glyph coverage texture should be white
RGB with coverage in alpha, then tinted by the requested color.

Current port caveat: `BuildTTFAtlas()` uses `stb_truetype` with 2x
oversampling, and `stb_truetype` does not execute TrueType hinting
instructions. Retail Windows GDI/Arial hinting snaps small-font stroke widths
onto full pixels, so the white top pass covers more of the black shadow. In the
unhinted path, many stroke pixels have partial alpha; after the black shadow
passes, those pixels blend to gray even though the requested top color is
white. Arimo is a closer Arial substitute for metrics/shapes, but it does not
make this path hinted.

The harness currently compensates by drawing the white top pass twice at the
same coordinates. With normal alpha blending that raises effective coverage to
`1 - (1 - coverage)^2`, which is closer to the fuller hinted strokes without
changing the recovered slot 23 color. This is a port-side rasterization
compensation, not a recovered extra retail `DrawTextA` call. Do not "fix" this
by changing the slot 23 color.

One known trap: `FONT.DEF` contains `WINFONT "Numbers" FONT "Arial" 12 COLOR
255 0 0`, but the slot 23 stat-value path does not rely on that color.

## Whole-Chip Coordinates

Do not treat `BackPanel`'s 128x64 bitmap as the full healthbar chip. It is only
the portrait/chrome surface. The bars extend past it.

The authoritative full chip width is `0xc1` pixels:

```text
player chip origin = 0
target chip origin = pane_w - 0xc1
```

This falls straight out of the target health bar call:

```text
target health x = pane_w - 0xc1
```

Inside a cached 0xc1-wide chip surface, target-local coordinates become:

```text
BackPanel local x = 0xc1 - 0x80 = 0x41
target health bar local x = 0x00
target mana bar local x   = 0x30
target fatigue local x    = 0x48
target value cell local x = 0x41
target name cell local x  = 0x7d
```

The player side uses the recovered pane-local coordinates directly:

```text
BackPanel local x = 0x00
player bars local x = 0x44
player value cell local x = 0x47
player name cell local x = 0x00
```

This coordinate conversion is the reason a 128px-only target panel makes the
bars look "mostly wrong": their true retail origin is left of the BackPanel.

## Bar Helper Reconstruction

`FUN_0054a5d0_TPlyrStatusBar_BarRender_Helper.cpp` proves the caller x/y are
the raw retail bar destination, not source coordinates:

```text
software path: FUN_00414d70(x, y, ...)
hardware path: SDrawParam.dx = x; SDrawParam.dy = y
```

The trap is the composition layer. Slot 7 and slot 23 first draw the cached
player chrome/effect surface through `FUN_00438d80(..., 4, 4)`, then draw the
bars at raw screen coordinates such as `(0x44,0x0f)`. If the port reproduces
that two-stage path exactly, use the raw coordinates unchanged.

If the port draws the chrome normalized to pane-local `(0,0)` instead, convert
the raw retail bar destinations into chrome-local coordinates by subtracting
`(4,4)`. Otherwise the bars land exactly `+4,+4` relative to the visible chip.
That was the current harness bug.

The helper is also not a simple filled-strip draw. Port the four-slice helper
math:

```text
segment 1: left cap, width=cap_w
segment 2: interior A, width depends on fill
segment 3: interior B, width depends on fill
segment 4: right tail/cap, width=(tail_x - max_w) + cap_w
```

Retail call-site arguments, followed by the current normalized harness
coordinates:

```text
health  raw dst=(0x44,0x0f), normalized=(0x40,0x0b), src=(2,0x01), dim=(2,0x31), tail_x=0x7d, h=0x11, max_w=0x77, cap=6
mana    raw dst=(0x44,0x1f), normalized=(0x40,0x1b), src=(2,0x13), dim=(2,0x42), tail_x=0x4d, h=0x0c, max_w=0x45, cap=4
fatigue raw dst=(0x44,0x2c), normalized=(0x40,0x28), src=(2,0x22), dim=(2,0x51), tail_x=0x35, h=0x0c, max_w=0x2d, cap=4
```

For target bars, convert the x coordinate through the whole-chip origin, then
apply the same current-harness `-4` screen-to-chrome normalization:

```text
health  raw local x = 0xc1 - 0xc1 = 0x00, normalized x = -0x04
mana    raw local x = 0xc1 - 0x91 = 0x30, normalized x = 0x2c
fatigue raw local x = 0xc1 - 0x79 = 0x48, normalized x = 0x44
```

## Proven Stat Value Cells

Each HP/MP/Fatigue value uses a 50x14 cell.

Player final destinations:

```text
health  dst=(0x47, 0x07), src=(0x00, 0x00), size=0x32 x 0x0e
mana    dst=(0x47, 0x17), src=(0x00, 0x0e), size=0x32 x 0x0e
fatigue dst=(0x47, 0x24), src=(0x00, 0x1c), size=0x32 x 0x0e
```

Target final destinations:

```text
health  dst=(pane_w - 0x80, 0x07), src=(0x00, 0x2a), size=0x32 x 0x0e
mana    dst=(pane_w - 0x80, 0x17), src=(0x00, 0x38), size=0x32 x 0x0e
fatigue dst=(pane_w - 0x80, 0x24), src=(0x00, 0x46), size=0x32 x 0x0e
```

Do not mirror the player `0x47` coordinate inside the 128-wide target panel.
Retail places the target value cell at the left edge of the right panel
(`pane_w - 0x80`) and right-aligns the text inside that cell via font id
`0x404`.

## Name/Level Scratch Cells

The name/level label uses `font_id 0x402` (center, shadowed), but it is a
separate 64x64 scratch-cell path. Its final blit positions decode as:

```text
player name  dst=(0x00, 0x36),       src=(0x40, 0x00), size=0x40 x 0x40
target name  dst=(pane_w - 0x44, 0x36), src=(0x40, 0x40), size=0x40 x 0x40
```

Do not mix these with the stat values. The stat values are the healthbar chip
numbers.

## Direct Renderer Contract

The modern code does not need to allocate every same scratch surface as retail,
but it must still preserve the same coordinate-space contract. Compose the chip
into a fixed-size render target first, then blit that completed surface during
the HUD swapchain pass. Do not draw bitmaps through one path and glyphs directly
to the swapchain through another path; window-scale changes will expose that as
text and bitmap art moving/scaling differently.

1. Use the recovered final `SDrawParam` destination cell as the authoritative
   output box.
2. Apply the recovered DrawText alignment bits inside that box.
3. Clip glyphs to the recovered cell size (`0x32 x 0x0e` for stat values,
   `0x40 x 0x40` for name/level).
4. Use the color supplied by the slot 23 call site, not a guessed `FONT.DEF`
   default.
5. Apply the `0x400` shadow behavior as three black passes at base, `+1x`, and
   `+1y`, then the colored pass at base.

This gives a direct TTF implementation equivalent to "draw into scratch cell,
then blit the cell to the final pane" without baking guessed baselines or
screen positions into the glyph walker, while still keeping the final HUD draw
as a surface blit.

## Non-Authoritative Capture Check

The comparison image is for transcription/regression checks only. Do not use
it to derive coordinates. Coordinates come from `/recon` plus retail
disassembly.

Use the comparison tool at native integer scale:

```sh
tools/ui/snap_compare.py ui-plyrstatusbar docs/ui/plyr_stats_panel.png \
  --frames 1 --crop 0,0,198,118 --ref-zoom 1 --current-zoom 2 \
  --out /tmp/plyr_statusbar_side_by_side.png
```

The checked-in `docs/ui/plyr_stats_panel.png` reference is already a 2x
player-chip crop. Keep it at `--ref-zoom 1` and zoom the current 1x game
capture to `--current-zoom 2` for a same-scale board. For the target chip on
a 640-wide test capture, use `--crop 447,0,193,118` (`640 - 0xc1 = 447`).

The tool auto-detects the game content top in macOS window captures before
applying `--crop`, then lays reference/current side by side with
nearest-neighbor zoom. If AppKit capture behavior changes, override with
`--chrome-top <pixels>` or `--chrome-top 0` to keep the full window.
