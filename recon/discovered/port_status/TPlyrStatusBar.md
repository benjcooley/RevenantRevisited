# TPlyrStatusBar port status

Retail vtable: **`cls_0x5a54e4`** (31 slots). Global instance: `0x65a8c0`. Recon body decompilations live in `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_*.cpp`. Our source: none yet — class is forward-declared at [src/revtypes.h:1166](../../../../src/revtypes.h) (`_CLASSDEF(TPlyrStatusBar)`) but no `.h`/`.cpp` exists. **Needs full reconstruction.**

The character status panel (upper-corner per-character HUD with health/stamina/mana bars + portrait). Per the Wave-1A brief and CLASSIC_HUD_REFERENCE: one class with TWO global instances expected — player (left) and current-target (right). The global @ 0x65a8c0 is the player instance. The target instance address is still unknown (look for adjacent global with same vtable).

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (31 slots, all addresses identified — methods not yet ported)

| Slot | Method | Retail addr | Our source | Status | Notes |
|---|---|---|---|---|---|
| 0 | `Initialize` | 0x549740 | — | ⚫ not-started | Allocates 5 surfaces (1 sprite 0x80x0x80 at +0x64, 1 0x28x0x28 at +0x60, 3 mosaic 0x80x0x40 at +0x6c/+0x70/+0x74) plus a portrait sprite (FUN_0046d710 of DAT_005e5724 = portrait name). Resolution-aware via DAT_006680c8 (NoTex/hires toggle). Body in `cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp`. |
| 1 | (likely Close/Dtor) | 0x549d40 | — | ⚫ not-started | Not yet extracted. |
| 2 | (TPane shared, likely Pulse) | 0x491bd0 | — | ⚫ not-started | Shared with TSidePane / TBottomPane / TSideTabsPane — likely TPane base. |
| 3..30 | (mix of base defaults + per-class overrides) | various | — | ⚫ not-started | Not yet extracted/labeled. |

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
