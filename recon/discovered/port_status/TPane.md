# TPane port status

Retail vtable: **`cls_0x5a4494`** (32 slots). Confirmed by Wave-2B (2026-05-16) as the **TPane base class** — see [B.r6 brief](../../../docs/ui/briefs/B_r6_target_charpane_and_base.md) for the 3-line evidence chain (31 distinct base-vtable wire sites + slot inheritance match against 4 confirmed leaf panes + slot-0/slot-1 body semantics matching `src/screen.h::TPane`).

Our source: [src/screen.h](../../../src/screen.h) lines 78-374 — substantial existing TPane definition (pre-release era). The retail layout matches the src class structure closely but the retail port itself still needs writing as panes are reconstructed.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (32 slots — base method bodies decompiled where high-confidence)

| Slot | Method (inferred) | Retail addr | Our source | Status | Notes |
|---:|:---|:---|:---|:---|:---|
| 0 | `Initialize` | 0x00491900 | `src/screen.h:150` | 🟡 partial | Body decompiled in `cls_0x5a4494_TPane_Initialize_491900.cpp` — matches src TPane::Initialize semantics (copy new->cur rect, zero render state, call vtable[3]=CreateBackgroundBuffers, set isopen). |
| 1 | `Close` | 0x00491970 | `src/screen.h:152` | 🟡 partial | Body decompiled in `cls_0x5a4494_TPane_Close_491970.cpp` — matches src TPane::Close (call vtable[4]=FreeBackgroundBuffers, clear isopen). |
| 2 | ? (`PaneResized`?) | 0x00491bd0 | — | ⚫ not-started | Shared base body across most leaf panes. Non-trivial size. |
| 3 | `CreateBackgroundBuffers` | 0x00444f40 | `src/screen.h:189` (default `{}`) | ⚫ not-started | Stub default body, matches src. |
| 4 | `FreeBackgroundBuffers` | 0x00444f50 | `src/screen.h:191` (default `{}`) | ⚫ not-started | Stub default body, matches src. |
| 5 | `SetBackgroundBuffer`? | 0x00444f60 | — | ⚫ not-started | Need body extract. |
| 6 | `ClearBackgroundBuffer`? | 0x00444f70 | — | ⚫ not-started | Need body extract. |
| 7 | ? (overridden by TPlyrStatusBar) | 0x00444f80 | — | ⚫ not-started | TPlyrStatusBar overrides this slot at 0x0054ab80. Suggests an early-virtual that pane subclasses commonly override (`SetClipRect`? `Update`?). |
| 8 | ? | 0x00444f90 | — | ⚫ not-started | |
| 9 | ? (non-trivial) | 0x00491a80 | — | ⚫ not-started | Shared body across leaf panes. |
| 10..15 | ? (sequence of `{}` stubs) | 0x00444fb0..0x00445000 | — | ⚫ not-started | Likely the MouseClick/Move/KeyPress/CharPress/Joystick cluster (all default `{}` in src). |
| 16 | ? (non-trivial) | 0x00491bb0 | — | ⚫ not-started | Shared body. |
| 17, 18 | ? | 0x00445020, 0x00445030 | — | ⚫ not-started | |
| 19 | ? (overridden by TPlyrStatusBar) | 0x00445050 | — | ⚫ not-started | TPlyrStatusBar overrides at 0x549da0. Strong candidate for `Animate(bool draw)`. |
| 20 | ? (overridden by TPlyrStatusBar) | 0x004451d0 | — | ⚫ not-started | TPlyrStatusBar overrides at 0x549e60. Strong candidate for `Overlay(bool draw)`. |
| 21..30 | tail | various | — | ⚫ not-started | |
| 31 | ? | 0x00434e40 | — | ⚫ not-started | Outside the standard pane-vtable region; likely a scalar deleting destructor thunk OR an off-by-one in DumpVtable's slot count. |

## Field layout (inferred from src/screen.h + slot-0/slot-1 body access patterns)

```
this+0x00 : vptr
this+0x04 : x         (current frame)       <- param_1[1]
this+0x08 : y                                <- param_1[2]
this+0x0c : width                            <- param_1[3]
this+0x10 : height                           <- param_1[4]
this+0x14 : newx      (next frame)           <- param_1[5]
this+0x18 : newy                             <- param_1[6]
this+0x1c : newwidth                         <- param_1[7]
this+0x20 : newheight                        <- param_1[8]
this+0x24 : scrollx                          <- param_1[9]
this+0x28 : scrolly                          <- param_1[10]
this+0x2c : oldscrollx                       <- param_1[0xb]
this+0x30 : oldscrolly                       <- param_1[0xc]
this+0x34 : newscrollx                       <- param_1[0xd]
this+0x38 : newscrolly                       <- param_1[0xe]
this+0x40 : isopen flag (1 = initialized)    <- param_1[0x10]
this+0x48..0x54 : misc render state          <- param_1[0x12..0x15]
this+0x58 : backgroundbuffer (-1 = none)     <- param_1[0x16]
this+0x5c : ???                              <- param_1[0x17]
```

(These offsets are consistent across all leaf panes' field accesses observed so far.)

## Open questions

- Slots 2, 7, 9, 16, 19, 20 need body extraction to confirm src-name mapping.
- The cls_0x5a45c8 intermediate (used by stat-pane class family) is NOT a TPane subclass directly via vtable wiring patterns — its relationship needs disentangling. May be a `TButtonPane : TPane` mid-level, or a non-pane base shared by some buttons-and-bgr UI elements.
- Vtable slot 31 = 0x00434e40 — this is the base-init helper called from FUN_00546b50, FUN_00549740, etc. as a NON-virtual call. Its presence at slot 31 of the cls_0x5a4494 vtable is suspicious — could be a DumpVtable.java off-by-one (the script dumps `slots` slots, not `slots-1`). Recheck by reading raw memory at 0x5a4510..0x5a4514 to verify it's actually a vtable slot vs adjacent data.

## Related

- Leaf panes confirmed to derive from this base: TPlyrStatusBar (cls_0x5a54e4), TSidePane (cls_0x5a53ec), TSideTabsPane (cls_0x5a5750), TBottomPane (cls_0x5a5468).
- src/screen.h class TPane line 83.
- Brief: [B.r6](../../../docs/ui/briefs/B_r6_target_charpane_and_base.md).
