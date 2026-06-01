# TPlyrStatusBar — Reconstruction Burndown
Spec: TPlyrStatusBar_SPEC.md   Status: done (visually verified 2026-05-20)

Clean-room rebuild of `src/uiplyrstatusbartest.cpp` (`--test=ui-plyrstatusbar`),
built ONLY from TPlyrStatusBar_SPEC.md. Items checked off only after landed AND
visually confirmed (player vs docs/ui/plyr_stats_panel.png; target vs the spec
mirror — captured live, both sides examined element-by-element).

## Assets to load            (spec §2)
- [x] statusbarnotex.dat:BackPanel 128x64 → PTBitmap (chip chrome)        (§2)
- [x] statusbarnotex.dat:Bars 128x128 → PTBitmap (4-section fill atlas)   (§2)
- [x] statusbarnotex.dat:HealthIcon 24x24 (alpha) → PTBitmap             (§2)
- [x] statusbarnotex.dat:ManaIcon 24x24 (alpha) → PTBitmap               (§2)
- [x] statusbarnotex.dat:FatigueIcon 24x24 (alpha) → PTBitmap            (§2)
- [x] statusbarnotex.dat:Ring 44x44 (alpha) → PTBitmap                   (§2)
- [x] portraits.dat:LockeFace 30x30 → PTBitmap (placeholder; real=I3D)   (§2)

## Surfaces / render target  (spec §3)
- [x] compose whole pane into ONE fixed RT (pane_w x 0x70=112); DrawSurface once (§3)
- [x] chip width 0xc1=193; player origin screen 0; target origin pane_w-0xc1 (§3)
- [x] target chrome composites at chip-local 0x41 → screen pane_w-0x80+x_in_chrome (§3)

## Static elements           (spec §4)  [both player + target mirror]
- [x] portrait placeholder centered on Ring anchor                       (§4)
- [x] BackPanel chrome-local (0,0) 128x64 opaque                         (§4)
- [x] Ring centered: player tl (4,9); target screen tl (pane_w-0x34,9)   (§4)
- [x] HealthIcon player (43,3) / target screen (pane_w-0x46,3)           (§4)
- [x] ManaIcon   player (43,17)/ target screen (pane_w-0x46,17)          (§4)
- [x] FatigueIcon player (43,32)/ target screen (pane_w-0x46,32)         (§4)
- [x] HP bar player (68,15) w0x77 tailX0x7d / target (pane_w-0xc1,15)    (§4/§6)
- [x] MP bar player (68,31) w0x45 tailX0x4d / target (pane_w-0x91,31)    (§4/§6)
- [x] FT bar player (68,44) w0x2d tailX0x35 / target (pane_w-0x79,44)    (§4/§6)
- [x] HP/MP/FT value cells player (0x47,..) / target (pane_w-0x80,..)    (§4/§8)
- [x] name+level cell player (0x00,0x36) / target (pane_w-0x44,0x36)     (§4/§8)

## Draw order                (spec §5)
- [x] clear chip RT (transparent)                                       (§5)
- [x] draw_side player (chrome+ring+portrait+icons, then bars, then text)(§5)
- [x] draw_side target only if fade>0                                    (§5)
- [x] blit chip RT to HUD once                                           (§5)

## Algorithms                (spec §6)
- [x] 6a chrome+icon compose: portrait, BackPanel, Ring centered, 3 icons(§6a)
- [x] 6b bar slice fill (bright|dim split at fillW), dir-mirror          (§6b)
- [x] fillW = value*fullW/max; bar extent = tailX (incl. pointed tail)   (§6b)

## Effects / shadows         (spec §7)
- [x] bar drop shadow: player (+4,+4) down-right; target (-4,+4) down-left(§7)
- [x] text 3-pass black shadow (DrawTextShadowedToTarget)                (§7)
- [x] icons/Ring alpha (no magenta key); BackPanel/Bars opaque          (§7)

## Text                      (spec §8)
- [x] HP/MP/FT value "%d" white; player LEFT, target RIGHT, 0x32x0x0e cell(§8)
- [x] name+level "%s\nLevel %d" white CENTER, 2 lines TOP-stacked in 0x40 cell(§8)
- [x] name line1 at cell top (0x36), line2 +lineHeight (NOT vcentered)   (§8 v-align)
- [x] no per-element vertical nudge (font.cpp kGdiTopLeading handles it)  (§8)
- [x] font = LiberationSans ~12px (Small)                                (§8)

## Animation                 (spec §9, 24Hz)
- [x] player fade ramp 0..6 → target 6; alpha=fade*255/6                 (§9)
- [x] target fade ramp 0..6 → 6 when target present else 0; gated 24Hz   (§9)

## Input / dispatch          (spec §10)
- [x] none — non-interactive HUD readout                                 (§10)

## New port primitives needed (spec §13)
- [x] none — all exist (DrawBitmap*ToTarget, DrawText*ToTarget). I3D portrait
      deferred → LockeFace placeholder.                                  (§13)

## Errors to close            (spec §0)
- [x] #1 text Y: cell at recovered (x,y,0x32,0x0e), top-aligned in cell  (§0)
- [x] #2 name/level: single 2-line block, CENTER horiz, TOP stacked      (§0)
- [x] #3 bar fill: bright/dim split at fillW per §6 table                (§0)
- [x] #4 shadow dir: player +4,+4; target -4,+4 (per-side)              (§0)
- [x] #5/#6 target right panel: 0x41 chrome origin applied (pane_w-0x80) (§0)
- [x] #7 name/level TOP-aligned, not vcentered                          (§0)

## Spec gaps found during build (feed back to forensics)
- §6b bar 4-slice exact source-rect mapping is UNCONFIRMED-D (MEDIUM). I built
  a clean 2-slice reading (bright = leftmost fillW from brightY band; dim tail =
  remainder up to tailX from dimY band; dir<0 swaps sides). Used `tailX` (not the
  prior file's 2*capW inner model) so the pointed tip past `fullW` renders; this
  matches the reference proportions. The literal `capW`/`(a)`/`capX2` columns in
  §6 were NOT needed for a faithful result and their precise role stays open.
- §8 says font "~12px"; 12px LiberationSans matches the reference well. No exact
  px in the spec — would be good to pin.
- Portrait is the LockeFace placeholder; real chars\locke.i3d head needs the I3D
  decoder (deferred per §2/§14.3). Honest stand-in, flagged.
</content>
