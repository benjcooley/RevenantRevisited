# TStatPane port status

Retail vtable: **unknown** (pending). Global instance: **`0x65b140`**. Init function: `FUN_00546b50`. Recon body in [cls_unknown_StatPane_Initialize_546b50.cpp](../cls_unknown_StatPane_Initialize_546b50.cpp).

Our source: [src/statpane.h](../../../src/statpane.h) — already exists in pre-release src. Need to cross-reference src class shape against retail to see how much divergence exists.

Identified by Wave-2B (2026-05-16) — see [B.r6 brief Step 2](../../../docs/ui/briefs/B_r6_target_charpane_and_base.md#step-2--two-stat-pane-inits-disambiguated-two-different-classes).

The sidebar **Stats tab** content pane. When the player selects the "Stats" tab on the right sidebar (TSideTabsPane), this pane is shown.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Init function fingerprint (FUN_00546b50, 537 bytes)

- Calls `FUN_00434e40()` — intermediate-class init (cls_0x5a45c8 base init: TPane init + extra fields including a global render-context pointer at [0x19]). This means TStatPane is **`TStatPane : cls_0x5a45c8 : TPane`** (3-level inheritance), where cls_0x5a45c8 is probably `TButtonPane` or similar mid-level pane.
- Allocates 1 mosaic surface (sized via this->newwidth × this->newheight, alloc-mode 0x200) at field [99] (0x18c offset)
- Allocates 1 sprite surface, loads sprite **"RingT"** at field [0x62] (0x188) — the Ring portrait icon
- Loads asset **"statpane.def"** (the DEF widget-engine definition file) into field [0x68] / [0x69]
- On asset-load failure: `"Unable to load STATPANE DEF"` error logged
- Zeros fields [0x5f, 0x61, 0x65, 0x67, 0x6a, 0x6b, 0x6c, 100] = misc init/dirty state
- Final call: `this->vtable[0x2e](DAT_00667fcc)` — vtable[46] called with a global pointer; likely "SetBoundCharacter" (DAT_00667fcc is probably the player character ptr)

## Field layout (so far)

```
this+0x000  : vptr
this+0x004..0x018c : (inherited from cls_0x5a45c8 → TPane, including newx/y/w/h and is_open)
                     specifically: this+0x18c (= param_1[99]) = mosaic surface ptr
this+0x188  : portrait sprite ptr ("RingT")  -- param_1[0x62]
this+0x190+ : DEF buffer pointer + size      -- param_1[0x68], [0x69]
this+0x1a4+ : misc init/dirty state          -- param_1[0x5f, 0x61, ...]
```

(Field offsets pending vtable wire site discovery — once we know the class's actual sizeof, the offset alignment is locked.)

## Vtable

Not yet dumped. To find it: look for the `MOV [0x65b140], <vtable>` instruction (the static initializer that wires the global's vtable). Wave-1A confirmed this pattern at 0x480420..0x480438 for siblings; the wire for 0x65b140 is presumably nearby. Need: `FindBytes 0x65b140` then check the instructions at each hit for `MOV [hit], <imm32>` pattern.

## Open questions

- Find the vtable address (cls_0xXXXX) for TStatPane.
- Verify pre-release `src/statpane.h` class structure against retail field layout.
- Identify the role of `statpane.def` — is it the layout definition for stat bars + values? Names? Both?
- What does `vtable[0x2e]` do? Is it `SetBoundCharacter(TCharacter*)`? Check by extracting the slot body and comparing to the field-write pattern.

## Related

- TSideTabsPane (cls_0x5a5750 @ 0x65be50) — switches between this pane and other sidebar mode panes (EquipPane, SpellPane, InventoryPane, AutoMapPane).
- DAT_0065c6f4 = `StatsPane.dat` — the asset file (loaded by TPlayScreen::Initialize).
- Pre-release: [src/statpane.h](../../../src/statpane.h).
- Brief: [B.r6](../../../docs/ui/briefs/B_r6_target_charpane_and_base.md).
