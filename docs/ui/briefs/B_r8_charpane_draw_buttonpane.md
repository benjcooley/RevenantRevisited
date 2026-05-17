# B.r8 — TPlyrStatusBar two-pass-draw verification + TButtonPane base ID

**Wave:** 3A
**Date:** 2026-05-16
**Status:** COMPLETE — single-instance two-pass-draw **CONFIRMED**; TButtonPane base **CONFIRMED = cls_0x5a45c8**; 4 conservative renames applied.

## TL;DR

1. **Single-instance two-pass-draw hypothesis: CONFIRMED.** TPlyrStatusBar slot 23 (FUN_0054af20, 3909 bytes) explicitly draws TWO sets of stat bars in one method body:
   - **Left pass** uses `DAT_00667fcc` (the global current-player character pointer) at fixed x-coords 0x44 (= 68)
   - **Right pass** uses `piStack_e8 = DAT_00667fcc[0x38][0x11]` (the player's current target's character pointer, derived via two-level indirection) at MIRRORED x-coords `width - 0x88 / -0x91 / -0x79`
   - Same source-swap pattern appears in slot 7 (FUN_0054ab80) as a smaller partial-redraw
   - The face-off framing IS implemented exactly as `CLASSIC_HUD_REFERENCE.md` §2 hypothesized — same widget, two paint sub-passes, two source-character pointers, no second instance
2. **TButtonPane base class: CONFIRMED = `cls_0x5a45c8`.** Inherits from TPane (cls_0x5a4494), confirmed by:
   - 8+ vtable slot identities shared with cls_0x5a5a30 (TQuickSpellPane, known-derived per Wave-2A + src/spellpane.h)
   - Slot 0 body (FUN_00434e40) matches src/button.h::TButtonPane::Initialize semantics (call TPane::Initialize, zero Buttons-array storage)
   - Used as intermediate in canonical MSVC 3-stage ctor pattern (cls_0x5a5ae8_ctor_488620.cpp)
   - Multiple derived classes use it as base (TQuickSpellPane, TBottomBarPane, the cls_0x5a5ae8 ScrollPane)
3. **FUN_0041c7f0 is NOT the TButtonPane ctor** as B.r5 speculated — it's `TPointerArray<TButton, MAXBUTTONS>::TPointerArray()` (allocates `param_2 << 2` bytes for the button-pointer array, zero-fills). The TButtonPane base ctor's vtable write is OPTIMIZED OUT in TQuickSpellPane / TBottomBarPane (single-inheritance with immediate leaf overwrite); the compiler collapses the chain.
4. **Four conservative renames applied** (recon/discovered/renames/agent_ui_wave3_charpane_buttonbase.txt):
   - `cls_0x5a45c8 -> cls_0x5a45c8_TButtonPane`
   - `FUN_00434e40 -> FUN_00434e40_TButtonPane_Initialize`
   - `FUN_00434f30 -> FUN_00434f30_TButtonPane_Close`
   - `FUN_00549d40 -> FUN_00549d40_TPlyrStatusBar_Close`

## Step 1 — TPlyrStatusBar vtable diff vs TPane base

Full dump saved to [recon/discovered/cls_0x5a54e4_TPlyrStatusBar_vtable.txt](../../../recon/discovered/cls_0x5a54e4_TPlyrStatusBar_vtable.txt).

| Slot | TPane base (cls_0x5a4494) | TPlyrStatusBar (cls_0x5a54e4) | Override? | Body file (this wave) |
|---:|:---:|:---:|:---:|:---|
|  0 | 0x00491900 (Initialize) | **0x00549740** | ✅ (Wave-1A) | cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp |
|  1 | 0x00491970 (Close)      | **0x00549d40** | ✅ (Wave-3A) | cls_0x5a54e4_TPlyrStatusBar_Close_549d40.cpp |
|  2 | 0x00491bd0 (?)          | 0x00491bd0     | inherited | — |
|  3..6 | 0x00444f40..0x00444f70 (stubs) | (same)  | inherited | — |
|  7 | 0x00444f80 (stub)       | **0x0054ab80** | ✅ | cls_0x5a54e4_TPlyrStatusBar_slot7_partialdraw_54ab80.cpp |
|  8 | 0x00444f90 (stub)       | 0x00444f90     | inherited | — |
|  9 | 0x00491a80 (?)          | 0x00491a80     | inherited | — |
| 10..15 | 0x00444fb0..0x00445000 (stubs) | (same)  | inherited | — |
| 16 | 0x00491bb0 (?)          | 0x00491bb0     | inherited | — |
| 17, 18 | 0x00445020, 0x00445030 (stubs) | (same) | inherited | — |
| 19 | 0x00445050 (stub)       | **0x00549da0** | ✅ | cls_0x5a54e4_TPlyrStatusBar_slot19_state_549da0.cpp |
| 20 | 0x004451d0 (stub)       | **0x00549e60** | ✅ | cls_0x5a54e4_TPlyrStatusBar_slot20_Animate_549e60.cpp |
| 21 | 0x004451e0 (stub)       | 0x004451e0     | inherited | — |
| 22 | 0x00445060 (stub)       | 0x00445060     | inherited | — |
| 23 | 0x00445070 (stub)       | **0x0054af20** | ✅ | cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp |
| 24..30 | 0x00445080..0x004450c0 (stubs) | (same) | inherited | — |
| 31 | 0x00434e40 (off-by-one — actually TButtonPane Initialize, in adjacent .data) | 0x0054bf70 (off-by-one — TTextBar init) | — | n/a |

**Five overrides total**: slots 0, 1, 7, 19, 20, 23. (DumpVtable's slot 31 is an off-by-one into adjacent class .data; both TPane and TPlyrStatusBar vtables are 31 slots wide. See B.r6 Step 3 caveat.)

## Step 2 — Extracted draw / state method bodies (per-slot fingerprint)

### Slot 0 — Initialize (FUN_00549740) — UNCHANGED (Wave-1A)
1500-byte init: allocates 5 surfaces (4 mosaic + 1 sprite for portrait) at fields +0x60..+0x74. See cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp.

### Slot 1 — Close (FUN_00549d40) — NEW THIS WAVE
90 bytes. Frees the EXACT 5 surfaces Initialize allocates (fields +0x64/+0x60/+0x6c/+0x70/+0x74/+0x68) via `(**(vtable))(1)` destructor-with-delete calls, then calls FUN_00491970_TPane_Close (inherited base Close). Canonical Initialize/Close symmetric-pair body — **renamed**.

### Slot 7 — partial two-pass paint (FUN_0054ab80) — NEW THIS WAVE
645 bytes. Same two-pass pattern as slot 23 but smaller: only redraws the LEFT and RIGHT bar set if state flags `param_1[0xd4]` (left) and `param_1[0xdc]` (right) are nonzero. Calls `FUN_00414d70` (mosaic fill) + `FUN_0054a5d0` (bar paint helper) three times per side (= 3 bars: health / mana / stamina). Strong **Update** candidate (since TButtonPane also overrides slot 7) — but not renamed; needs golden-path src cross-ref.

### Slot 19 — state counter update (FUN_00549da0) — NEW THIS WAVE
178 bytes. Early-return if `DAT_00667fcc == 0`. Reads target via `piVar1 = DAT_00667fcc[0xe0]; iVar2 = (target_type==3||==0x19) ? piVar1[0x44] : 0;`. Updates animation state counters at fields +0xd4, +0xd8 (left side) and +0xdc, +0xe0 (right side), each ramped toward 6. Pure state-management; no actual draw calls. Strong **DrawBackground** or **Pulse** candidate — but the body has no drawing AND no AI; pre-paint state-prep hook is the best fit.

### Slot 20 — Animate (FUN_00549e60) — NEW THIS WAVE
571 bytes. Early-return if `DAT_00667fcc == 0`. Manages the portrait surface (recreates when `DAT_00667fcc != param_1[0x25]` indicating player-swap, or when target binding changes). Loads sprite via `FUN_0046d710(&DAT_005e572c)` or `&DAT_005e5734` based on hi-res toggle `DAT_006680c8`. Then calls `FUN_0054a0a0()` AND `FUN_0054a310()` — almost certainly the per-side "paint player portrait+bars" and "paint target portrait+bars" helpers. Clears the bound-character cached state. Strong **Animate(bool draw)** candidate (slot 20 = Animate in TButtonPane vtable too).

### Slot 23 — FULL two-pass paint (FUN_0054af20) — NEW THIS WAVE — **THE SMOKING GUN**
3909 bytes. The decisive evidence for the two-pass-draw architecture.

```
Early-return if DAT_00667fcc == 0.
Derive piStack_e8 = DAT_00667fcc[0x38]; *target check; piStack_e8 = target_obj[0x11];  // target's TCharacter

// === LEFT (PLAYER) PASS ===
// Three "value changed?" tests against DAT_00667fcc's getter vtable methods:
//   vtable[0x1c0] -> health   (cmp vs param_1[0x98])
//   vtable[0x1d0] -> mana     (cmp vs param_1[0x9c])
//   vtable[0x1c8] -> stamina  (cmp vs param_1[0xa0])
// On change: format value string, draw text into auStack_80 buffer, blit at fixed coords:
//   (0x00, 0x00, 0x32, 0x0e)   <- health number, left side, top row
//   (0x00, 0x0e, 0x32, 0x0e)   <- mana number, left side, second row
//   (0x00, 0x1c, 0x32, 0x0e)   <- stamina number, left side, third row
// Player name string also rendered if changed (calls FUN_0054ae10 at (0x40, 0x00) -- portrait area)

// === RIGHT (TARGET) PASS ===  (only if piStack_e8 != 0)
// Same three value-changed tests against piStack_e8's getter vtable methods (same offsets).
// On change: draw target name/level/values at coords with x=0 (will be repositioned by Animate's coordinate frame)
// AND draws target portrait/banner at (0x40, 0x40) -- right side panel area

// === BAR FILL (BOTH SIDES) ===
// If left-side animation counter (param_1[0xd4]) != 0:
//   Call mosaic mask + FUN_0054a5d0(...) THREE times with x=0x44 (LEFT FIXED), feeding from DAT_00667fcc
// If right-side animation counter (param_1[0xdc]) != 0:
//   Call mosaic mask + FUN_0054a5d0(...) THREE times with x = param_1[0xc] + -0x88/-0x91/-0x79
//   (= width - 0x88, MIRRORED across the screen), feeding from piStack_e8
//   This is the "right panel" data path
```

**Field offsets reveal the layout:**
- `param_1[0x94] / [0x98] / [0x9c] / [0xa0] / [0xa4]` = cached LEFT (player) stat values
- `param_1[0xa8] / [0xac] / [0xb0] / [0xb4] / [0xb8] / [0xbc]` = cached RIGHT (target) stat values
- `param_1[0xc4..0xd0]` = cached extra-stats (when special-flag set)
- `param_1[0xd4]` = LEFT bar animation counter; `param_1[0xdc]` = RIGHT bar animation counter (set in slot 19)

**The right-pane x-coordinates are computed from `param_1[0xc]` = pane width**, NOT the screen width. This is critical: the panel itself is wide enough to span the full screen, with left-anchored player content at x=0..0x80 and right-anchored target content at x=(width-0x80)..(width). The pane width-derived layout means the panel auto-scales to resolution per the project's resolution-modes doc.

## Step 3 — Single-instance two-pass-draw verdict

### CONFIRMED: single-instance, two-pass draw with stat-source swap

The CLASSIC_HUD_REFERENCE.md §2 hypothesis (corrected by Wave-2B) is **vindicated by the bodies**. Specifically:

| Hypothesis component | Evidence |
|---|---|
| Single instance | Wave-2B FindBytes proved no second global wire site; slot 23 body explicitly references the ONE instance (`param_1`) and operates on its own cached fields. |
| Two paint sub-passes per frame | Slot 23 body has TWO distinct draw blocks (player-side, target-side) separated by a target-null check. Slot 7 has the same shape (partial redraw variant). |
| Stat-source swap | Player source = global `DAT_00667fcc`; target source = `DAT_00667fcc[0x38][0x11]` (target object's character ref). No swap of a *pointer field on the instance* -- the swap happens via two separate variable bindings in the same function. |
| Mirrored layout (face-off framing) | LEFT bars drawn at x = 0x44 (fixed); RIGHT bars drawn at x = pane_width + offset (mirrored). Confirms the upper-left + upper-right face-off layout from sample_screen_3.jpg. |
| Target panel hides when no target | Both slot 19 and slot 20 early-return if `DAT_00667fcc == 0`. Slot 23's right-side block early-returns to `LAB_0054be15` if `piStack_e8 == 0` AND the fallback lookup fails. Per spec. |
| Smooth target swap | Slot 20's portrait surface mgmt explicitly handles target rebinding (re-creates portrait when target changes). The animation counters in slot 19 (`+0xd4`, `+0xdc`) ramp toward 6, suggesting a fade-in/out as the right panel appears/disappears. |

### NOT a "first pass with player, swap source, second pass with target" pattern

The simpler "swap one pointer member, call same draw twice" pattern is NOT what retail does. Instead, retail uses a **single composite function that knows about both sides**, with all coordinate math and source-pointer math compiled into one block. This is more efficient (no virtual-call overhead, single cache pass through the surface fields) but less flexible than the parametric version.

**For the port:** the modernized C++ implementation can EITHER mirror this composite shape (one `Draw()` that walks both sources) OR refactor into two parametric passes (cleaner code). The user's "evolve don't replace" guidance plus the existing pre-release `src/textbar.cpp::SetHealthDisplay` API (already a parametric "draw bar with given name+health" API) suggests the parametric refactor is the right modern shape.

### NOT TPlyrStatusBar's bind-target API

There is no `SetSource()` / `SetCharacter()` / `SetTarget()` method visible in any TPlyrStatusBar virtual. The class doesn't BIND to a character at all — it READS the global `DAT_00667fcc` (current-player) and derives the target via player's target field. **The character binding is implicit through the global pointer, not explicit.**

## Step 4 — TButtonPane base identification

### Conclusion: TButtonPane base = `cls_0x5a45c8`

Full vtable dump saved to [recon/discovered/cls_0x5a45c8_TButtonPane_vtable.txt](../../../recon/discovered/cls_0x5a45c8_TButtonPane_vtable.txt).

### Evidence chain (4 independent lines)

**A. Vtable inheritance match with confirmed-derived class.** cls_0x5a5a30 (TQuickSpellPane, locked Wave-2A from "quick spell pane" string anchor + src/spellpane.h forward decl `class TQuickSpellPane : public TButtonPane`) shares the following vtable slots byte-identically with cls_0x5a45c8:

```
slot  1 = 0x00434f30  slot  9 = 0x00491a80  slot 22 = 0x00435f90
slot  2 = 0x00435010  slot 10 = 0x00445210  slot 23 = 0x00436090
slot  7 = 0x00436010  slot 16 = 0x00491bb0  slot 27 = 0x004361f0
                      slot 19 = 0x00435d70  slot 28 = 0x00436460
                                            slot 29 = 0x00436340
```

12 shared slots = textbook single-inheritance vtable. TQuickSpellPane only overrides its own leaf-specific slots (0, 20, 21, 25, 26, 31+).

**B. Slot-0 body matches src::TButtonPane::Initialize.** FUN_00434e40 body (saved Wave-2B as cls_0x5a45c8_intermediate_init_434e40.cpp):
1. First call: `FUN_00491900_TPane_Initialize()` — chain to base init
2. Zeros 0x100-byte buffer at field [0x26] — the TPointerArray<TButton, MAXBUTTONS> storage (per src/button.h:124 `TPointerArray<TButton, MAXBUTTONS> Buttons`)
3. Zeros remaining instance state (fields 0x18, 0x1a..0x33, 0x54..0x56, 0x5e — button-management bookkeeping)
4. Calls own vtable[11] = 0x444fc0 — the post-init pulse / setup

Exactly the shape `bool TButtonPane::Initialize()` would have per src/button.h.

**C. Canonical MSVC 3-stage ctor pattern.** cls_0x5a5ae8_ctor_488620.cpp shows:
```c
*param_1 = &PTR_FUN_005a4494;   // TPane base vtable
FUN_0041c7f0(0x10, 0x10);       // TPointerArray<TButton> ctor (not TButtonPane ctor!)
*param_1 = &PTR_FUN_005a45c8;   // intermediate write -- THIS IS THE TButtonPane VTABLE
... per-leaf field setup ...
*param_1 = &PTR_FUN_005a5ae8;   // leaf vtable
```

Three vtable writes = three inheritance levels = `cls_0x5a4494` (TPane) → `cls_0x5a45c8` (TButtonPane) → `cls_0x5a5ae8` (scroll-pane leaf).

**D. Multiple derived classes share cls_0x5a45c8 as base.** Confirmed via vtable inspection:
- TQuickSpellPane (`cls_0x5a5a30`) — 12 shared slots
- TBottomBarPane (`cls_0x5a5808`) — byte-identical ctor to TQuickSpellPane (per B.r5) — implies same base
- The scroll/parchment-viewer class (`cls_0x5a5ae8`) — explicit 3-stage write in its ctor
- The 1st "stat pane" (cls_unknown, FUN_00546b50 Init, global @0x65b140) — Init body starts with `FUN_00434e40()` (TButtonPane Initialize call)

Four derived classes use cls_0x5a45c8 as base = base class pattern.

### About FUN_0041c7f0 (corrects B.r5 speculation)

B.r5 tentatively flagged `FUN_0041c7f0` as "TButtonPane base ctor candidate". That identification is **wrong** — FUN_0041c7f0 body (extracted this wave):
```c
undefined4 * __thiscall FUN_0041c7f0(undefined4 *param_1, int param_2, undefined4 param_3) {
  param_1[2] = param_2;        // size = MAXBUTTONS
  param_1[3] = param_3;        // extra arg
  param_1[4] = malloc(param_2 << 2);   // allocate param_2 pointers
  // zero-fill loop
}
```

This is `TPointerArray<TButton, MAXBUTTONS>::TPointerArray()` — the Buttons-array ctor, NOT TButtonPane's. The `0x10, 0x10` args are MAXBUTTONS (=16) and an extra param. The TButtonPane base ctor's vtable-write step is OPTIMIZED OUT by the compiler in TQuickSpellPane / TBottomBarPane / TBottomPane — single-inheritance with immediate leaf-vtable overwrite collapses the chain (no point writing the base vtable when you're about to overwrite it).

This explains why B.r5 saw "byte-identical ctors except for the final vtable address" between TQuickSpellPane and TBottomBarPane — they're both TButtonPane subclasses with their TButtonPane vtable write optimized out, leaving only the TPane base + TPointerArray<TButton> init + leaf vtable steps.

Renamed FUN_0041c7f0 → none yet (the rename should be `TPointerArray_TButton_ctor` but the file is small enough that documentation here suffices; a focused TPointerArray pass should rename it together with sibling template instantiations).

## Step 5 — Renames applied this wave

See [recon/discovered/renames/agent_ui_wave3_charpane_buttonbase.txt](../../../recon/discovered/renames/agent_ui_wave3_charpane_buttonbase.txt).

Four pairs:
| Bare | Renamed | Lines of evidence |
|---|---|---|
| `cls_0x5a45c8` | `cls_0x5a45c8_TButtonPane` | 4 (vtable inheritance + slot-0 body + 3-stage ctor + multi-derivative use) |
| `FUN_00434e40` | `FUN_00434e40_TButtonPane_Initialize` | 2 (vtable slot 0 + src semantics match) |
| `FUN_00434f30` | `FUN_00434f30_TButtonPane_Close` | 1.5 (slot 1 + slot-position pairing convention -- borderline; body not yet extracted) |
| `FUN_00549d40` | `FUN_00549d40_TPlyrStatusBar_Close` | 2 (vtable slot 1 + body inverts Initialize allocation list) |

## Step 6 — Candidates with weak evidence (NOT renamed)

| Symbol | Tentative ID | Why not renamed |
|---|---|---|
| TPlyrStatusBar slot 7 (FUN_0054ab80) | `Update` | Slot-position alone (TButtonPane also overrides 7) + body shape doesn't disambiguate from a "partial repaint" hook. Wave-4 should extract the equivalent TButtonPane slot 7 (0x436010) and compare. |
| TPlyrStatusBar slot 19 (FUN_00549da0) | `DrawBackground` or `Pulse` | Body is state-update with NO draw calls — fits either. TButtonPane slot 19 (0x435d70) is per src::TButtonPane the DrawBackground override; cross-check by reading 0x435d70's body. |
| TPlyrStatusBar slot 20 (FUN_00549e60) | `Animate(bool draw)` | Body matches Animate semantics (per-frame surface management + content rendering). TButtonPane slot 20 (0x435de0) reinforces this — Animate is the per-frame draw. |
| TPlyrStatusBar slot 23 (FUN_0054af20) | OPEN | Body is unmistakably the big two-pass paint, but src vtable slot 23 = `MouseMove` per pre-Revisited declaration order — which contradicts the body shape. Either (a) the retail vtable order diverges from current src/screen.h order, (b) slot 23 is the actual "do everything" main draw with Animate as a small wrapper, or (c) the slot 19/20/23 trio is `DrawBackground/Pulse/Animate` in some order that doesn't match src. Needs src vs retail slot-mapping reconciliation. |
| FUN_0054a0a0, FUN_0054a310, FUN_0054ae10, FUN_0054a5d0 | per-side draw helpers / bar paint kernel | Called from slot 20 + slot 23. Wave-4 should extract these to confirm one is "draw player side", one is "draw target side", one is bar-fill kernel. |
| FUN_00452690 | object-by-mapindex registry lookup | Called in slot 23 as fallback target lookup. Plausibly the global object registry's `GetObject(mapindex)` -- worth confirming via a focused extraction. |
| DAT_00667fcc | global current-player character pointer | Already documented in port_status/TPlyrStatusBar.md. Cross-check vs TPlayer/TCharacter field shape would confirm; offset [0x38]=0xe0 is TCharacter's target/lookat slot per recon/discovered/cls_0x5a7b98_*. |

## Step 7 — Merging suspicions

**None.** All vtables inspected have textbook leaf-or-base shapes:
- TPlyrStatusBar (cls_0x5a54e4): 31 slots, 5 overrides (clean leaf-TPane shape)
- TButtonPane (cls_0x5a45c8): 31 slots with 13 overrides vs TPane base (consistent with TButtonPane adding ~7 virtuals per src::button.h + a few internal helpers)
- TQuickSpellPane (cls_0x5a5a30): 35+ slots, 5 overrides vs TButtonPane base (clean leaf with extras for spell-button mgmt)

No method-count explosions, no mixed-purpose methods, no field-offset incompatibilities. Identifications are clean.

## Open questions for Wave-4

1. **Per-slot src-name mapping for slots 7/19/20/23.** Extract bodies of TButtonPane slot 7 (0x436010), slot 19 (0x435d70), slot 20 (0x435de0), slot 22 (0x435f90), slot 23 (0x436090), slot 25 (0x436530), slot 26 (0x436660), slot 27 (0x4361f0). The src/button.h override list (Initialize, Close, KeyPress, MouseClick, MouseMove, DrawBackground, Animate) tells us TButtonPane overrides 7 virtuals — match each retail body to a src virtual by shape. That nails down the TPane base slot table once and for all, after which TPlyrStatusBar's overrides are self-naming.
2. **Extract FUN_0054a0a0 + FUN_0054a310 + FUN_0054ae10 + FUN_0054a5d0** (the per-side draw helpers + bar-fill kernel). Likely takes one decompile pass each; gives the port the full draw pipeline shape.
3. **DAT_00667fcc identification.** Cross-reference its read sites + the TPlayer source-side hits to confirm it's `Player` (or `CurrentPlayer` if those are distinct globals).
4. **Confirm the `[0x38] -> [0x11]` indirection chain** is TCharacter::Target → TObject->mbr_0x44 (= the character pointer back-ref). Wave-1A's TCharacter identification should already cover the field-offset math.
5. **TBottomBarPane / TBottomPane button registration.** B.r5 noted the bottom panes' init bodies don't register their buttons explicitly; the buttons must come from somewhere (probably the FUN_0041c7f0 TPointerArray ctor pre-allocates the slots, and buttons get registered into them later from a config-driven init).

## Files produced this wave

Decompilations (under `recon/discovered/`):
- `cls_0x5a54e4_TPlyrStatusBar_Close_549d40.cpp`
- `cls_0x5a54e4_TPlyrStatusBar_slot7_partialdraw_54ab80.cpp`
- `cls_0x5a54e4_TPlyrStatusBar_slot19_state_549da0.cpp`
- `cls_0x5a54e4_TPlyrStatusBar_slot20_Animate_549e60.cpp`
- `cls_0x5a54e4_TPlyrStatusBar_slot23_TwoPassDraw_54af20.cpp`
- `cls_misc_TPointerArray_TButton_ctor_41c7f0.cpp`

Vtable dumps (under `recon/discovered/`):
- `cls_0x5a54e4_TPlyrStatusBar_vtable.txt` (annotated with override marks)
- `cls_0x5a45c8_TButtonPane_vtable.txt` (annotated with inheritance evidence)
- `cls_0x5a5a30_TQuickSpellPane_vtable.txt` (annotated with inheritance evidence)

Renames: `recon/discovered/renames/agent_ui_wave3_charpane_buttonbase.txt` (4 pairs).

Port-status updates:
- `recon/discovered/port_status/TPlyrStatusBar.md` — appended draw-method section + verified two-pass architecture
- `recon/discovered/port_status/TButtonPane.md` — NEW file
- `recon/discovered/port_status/TPane.md` — appended confirmed-virtuals notes

## Hunt log

- **2026-05-16, Wave-3A:** read prerequisites (AGENT_PROTOCOL, B.r3, B.r5, B.r6, src/screen.h, src/button.h, cls_0x5a4494_TPane_vtable.txt). Ran DumpVtable for TPlyrStatusBar (32 slots) — confirmed 5 overrides (slots 0, 1, 7, 19, 20, 23). Decompiled all 5 overridden slot bodies via DecompileAddr.java. Identified slot 23 as the BIG two-pass paint with DAT_00667fcc/target source swap + mirrored x-coords — single-instance two-pass-draw hypothesis CONFIRMED. Then extracted FUN_0041c7f0 — found it's actually the TPointerArray<TButton> ctor, not TButtonPane base. Dumped cls_0x5a45c8 vtable + cls_0x5a5a30 (TQuickSpellPane) vtable — confirmed 12+ shared slot identities → cls_0x5a45c8 IS TButtonPane base. Total Ghidra runs: 3 (1 collision, 30s wait, retry succeeded). Applied 4 conservative renames + documented all weak-evidence candidates in the brief. Time: ~30 min wall-clock.
