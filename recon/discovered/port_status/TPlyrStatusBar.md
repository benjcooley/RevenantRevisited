# TPlyrStatusBar port status

Retail vtable: **`cls_0x5a54e4`** (31 slots). Global instance: `0x65a8c0`. Recon body decompilations live in `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_*.cpp`. Our source: none yet — class is forward-declared at [src/revtypes.h:1166](../../../../src/revtypes.h) (`_CLASSDEF(TPlyrStatusBar)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The character status panel (upper-corner per-character HUD with health/stamina/mana bars + portrait). Per the Wave-1A brief and CLASSIC_HUD_REFERENCE: one class with TWO global instances expected — player (left) and current-target (right). The global @ 0x65a8c0 is the player instance. The target instance address is still unknown (look for adjacent global with same vtable).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (31 slots, all overrides extracted as of Wave-3A 2026-05-16)

| Slot | Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|---|
| 0 | `Initialize` | 0x549740 | — | 🟡 partial | Allocates 5 surfaces (1 sprite 0x80x0x80 at +0x64, 1 0x28x0x28 at +0x60, 3 mosaic 0x80x0x40 at +0x6c/+0x70/+0x74) plus a portrait sprite (FUN_0046d710 of DAT_005e5724 = portrait name). Resolution-aware via DAT_006680c8 (NoTex/hires toggle). Body in `cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp`. |
| 1 | `Close` | 0x549d40 | — | 🟡 partial | Frees the 5 surfaces allocated by Initialize via destructor-with-delete calls, then calls FUN_00491970_TPane_Close. Body in `cls_0x5a54e4_TPlyrStatusBar_Close_549d40.cpp`. Renamed Wave-3A. |
| 2 | (TPane base — likely `PaneResized`/`SetClipRect`) | 0x491bd0 | — | ⚫ not-started | Inherited from TPane base. |
| 3..6 | TPane base stubs | 0x444f40..0x70 | — | ⚫ not-started | Inherited. |
| 7 | (override — likely `Update`) | 0x54ab80 | — | ⚫ not-started | 645-byte partial two-pass bar redraw (LEFT=DAT_00667fcc, RIGHT=DAT_00667fcc->target). Body in `cls_0x5a54e4_TPlyrStatusBar_slot7_partialdraw_54ab80.cpp`. TButtonPane also overrides slot 7 (0x436010) — slot is likely `Update` per src::TButtonPane override list. |
| 8 | TPane base stub | 0x444f90 | — | ⚫ not-started | Inherited. |
| 9 | TPane base shared | 0x491a80 | — | ⚫ not-started | Inherited from TPane. |
| 10..15 | TPane base stubs | 0x444fb0..0x445000 | — | ⚫ not-started | Inherited. |
| 16 | TPane base shared | 0x491bb0 | — | ⚫ not-started | Inherited. |
| 17, 18 | TPane base stubs | 0x445020, 0x445030 | — | ⚫ not-started | Inherited. |
| 19 | (override — likely `DrawBackground`/`Pulse`) | 0x549da0 | — | ⚫ not-started | 178-byte state-counter update (animates +0xd4 / +0xdc / +0xd8 / +0xe0 toward 6 based on player/target presence). NO drawing in this body. Body in `cls_0x5a54e4_TPlyrStatusBar_slot19_state_549da0.cpp`. |
| 20 | (override — likely `Animate(bool draw)`) | 0x549e60 | — | ⚫ not-started | 571-byte. Manages portrait surface (recreates on player/target swap via DAT_00667fcc rebind check), calls FUN_0054a0a0 + FUN_0054a310 helpers (likely "paint player side" / "paint target side"). Body in `cls_0x5a54e4_TPlyrStatusBar_slot20_Animate_549e60.cpp`. |
| 21, 22 | TPane base stubs | 0x4451e0, 0x445060 | — | ⚫ not-started | Inherited. |
| 23 | (override — OPEN; body is THE big two-pass paint) | 0x54af20 | — | ⚫ not-started | **3909 bytes — the smoking-gun two-pass-draw body.** Renders text values (health/mana/stamina) for player at fixed x=0 + portrait at (0x40, 0x00), then for target at mirrored x + portrait at (0x40, 0x40), then triple bar fill on both sides via FUN_0054a5d0. Body in `cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp`. Slot-23 → src-name mapping is OPEN (see B.r8 §6). |
| 24..30 | TPane base stubs | 0x445080..0x4450c0 | — | ⚫ not-started | Inherited. |

## Architecture: single-instance two-pass-draw — **CONFIRMED 2026-05-16 by Wave-3A**

The B.r6 Alternative A hypothesis is decisively confirmed (see [B.r8](../../../docs/ui/briefs/B_r8_charpane_draw_buttonpane.md) for full evidence):

- **Single instance @ 0x65a8c0** (no second global, no heap-allocated sibling)
- **No `SetSource()` API on the class.** Character binding is *implicit through the global pointer `DAT_00667fcc`*, NOT explicit
- **Slot 23 body draws BOTH sides in one method**:
  - LEFT pass: reads from `DAT_00667fcc` (player), paints at fixed x-coords (0, 0x40, 0x44)
  - RIGHT pass: reads from `DAT_00667fcc[0x38][0x11]` (target derived via player→target→character chain), paints at MIRRORED x-coords (pane_width - 0x80, -0x88, -0x91, -0x79)
- **Slot 7 has the same two-pass pattern** as a partial-redraw variant
- **Slot 19 + slot 20** each early-return if `DAT_00667fcc == 0` (gates entire draw on player presence); slot 23 right-side block early-returns if target is null AND fallback lookup fails (gates target panel on target presence — per spec)
- **Fade-in/out timing.** Slot 19 ramps animation counters (`+0xd4` left, `+0xdc` right) toward 6 each tick — slot 23 multiplies these counters by 0xff/6 for the bar fill alpha. This is the smooth-transition mechanism per CLASSIC_HUD_REFERENCE.md §2 ("smooth target swap matters").

## Surface layout (from Initialize)

```
this+0x60  : small 0x28x0x28 sprite     (portrait corner?)
this+0x64  : 0x80x0x80 sprite (large)   (portrait?)
this+0x68  : portrait-sized sprite (loaded from DAT_005e5724 name string)
this+0x6c  : 0x80x0x40 mosaic surface   (bar?)
this+0x70  : 0x80x0x40 mosaic surface   (bar?)
this+0x74  : 0x80x0x40 mosaic surface   (bar?)
this+0x94..0xa4 : cached LEFT (player) stat values
this+0xa8..0xb8 : cached RIGHT (target) stat values
this+0xbc..0xd0 : cached extra-stats + name/level hashes
this+0xd4..0xe0 : per-side animation counters (left+right fade)
```

Three 0x80x0x40 mosaic surfaces = consistent with three horizontal bars per character (health/stamina/mana).

## Instances

## Surface layout (from Initialize)

```
this+0x60  : small 0x28x0x28 sprite     (portrait corner?)
this+0x64  : 0x80x0x80 sprite (large)   (portrait?)
this+0x68  : portrait-sized sprite (loaded from DAT_005e5724 name string)
this+0x6c  : 0x80x0x40 mosaic surface   (bar?)
this+0x70  : 0x80x0x40 mosaic surface   (bar?)
this+0x74  : 0x80x0x40 mosaic surface   (bar?)
this+0x94..0xe0 : zero-initialized state (current values, dirty flags, anim state)
```

Three 0x80x0x40 mosaic surfaces = consistent with three horizontal bars per character (health/stamina/mana).

## Instances

| Instance | Global address | Vtable wire site | Notes |
|---|---|---|---|
| Player (left) | `0x65a8c0` | `0x48069c MOV [0x65a8c0], 0x5a54e4` | Wave-1A confirmed |
| Target (right) | **NONE FOUND** | — | Wave-2B searched the binary exhaustively for additional `0x5a54e4` 4-byte LE patterns; only the player wire exists. See [B.r6](../../../docs/ui/briefs/B_r6_target_charpane_and_base.md#step-1--right-side-target-tplyrstatusbar-instance-hunt-null-result) |

**The "two global instances" hypothesis is FALSIFIED at the global level.** Three alternative architectures remain plausible for the upper-right target panel (B.r6 Step 1):

- **A (most likely)**: single TPlyrStatusBar instance, called twice per frame from TPlayScreen with different stat-source pointers (player vs current target) → would mean the Animate slot takes a character* parameter OR reads a swappable member set before each draw call.
- **B**: distinct sibling class for the target panel (e.g. `TTargetStatusBar`) with its own vtable + global — no candidate identified yet.
- **C**: heap-allocated lazily when player acquires a target — no evidence either way.

**Recommended Wave-3 hunt:** extract slot 19 (0x549da0) and slot 20 (0x549e60) — the two TPlyrStatusBar-specific draw-region overrides — to check whether they accept or consult a character source. If they do, Alternative A is confirmed.

## Open questions

- Which method binds the panel to a character (player vs current target)? See "Instances" above — likely a parameter to slot 19/20 rather than a SetSource method.
- Where does the per-frame Draw/Animate happen?

## Related

- TPlayer stats: `cls_0x5b4f30` (already identified).
- Asset: `StatusBar.dat` / `Portraits.dat` (loaded in TPlayScreen::Initialize).
- Forward-decl: [src/revtypes.h:1166](../../../../src/revtypes.h).
