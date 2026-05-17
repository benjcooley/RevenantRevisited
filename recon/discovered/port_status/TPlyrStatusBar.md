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

## Open questions

- Where is the TARGET-side instance allocated? (Need to find another global with vtable 0x5a54e4.)
- Which method binds the panel to a character (player vs current target)? (Look for SetSource / Update slots.)
- Where does the per-frame Draw/Animate happen?

## Related

- TPlayer stats: `cls_0x5b4f30` (already identified).
- Asset: `StatusBar.dat` / `Portraits.dat` (loaded in TPlayScreen::Initialize).
- Forward-decl: [src/revtypes.h:1166](../../../../src/revtypes.h).
