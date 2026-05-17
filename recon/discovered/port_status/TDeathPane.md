# TDeathPane port status

Retail vtable: **`cls_0x5b93c4`** (47-slot TButtonPane-with-DEF-intermediate subclass at vtable address 0x5b93c4). Identified by Wave-4B (2026-05-16) — see [B.r12 brief](../../../docs/ui/briefs/B_r12_oog_closeout.md) §B.r17 for the golden-path evidence chain (string anchor `"Trouble initializing Death pane"` at 0x5e3ec8 + matching src/death.cpp Initialize behaviour).

Our source: [src/death.h](../../../src/death.h), [src/death.cpp](../../../src/death.cpp). Pre-release is 31+85 lines — minimal `TDeathPane : public TButtonPane` with 3 virtuals (Initialize, Close, DrawBackground) and 3 free-function button handlers (DeathBtnRestart, DeathBtnLoad, DeathBtnExit), all of which currently just call `SaveGame.ReadGame() + DeathPane.Close()`.

**Caution — class-level rename deferred:** `recon/classes_original/cls_0x5b93c4_TPlayer.cpp` carries a misleading `_TPlayer` filename suffix (Ghidra inheritance-flattening + method-intrusion artifact; TPlayer is actually `cls_0x5b4f30`). The file mixes TDeathPane leaf methods, inherited TButtonPane-with-DEF intermediate (cls_0x5a45c8) methods in the 0x435xxx/0x436xxx/0x437xxx range, and 3 spurious-looking intrusions (0x52ff40, 0x538e40, 0x5391a0). Class-level rename blocked until cls_0x5a45c8 is identified and its inherited methods are factored out.

**Activation differs from pre-release:** retail introduces a `TDeathScreen` wrapper (`cls_0x5b9374`, see separate [TDeathScreen.md](TDeathScreen.md)) that loads `death.dat` first and then invokes TDeathPane::Initialize on the global instance at DAT_0066f500. Pre-release src/ called `DeathPane.Initialize()` directly with no screen wrapper.

Status values: ✅ vetted · 🟡 partial · 🔴 incomplete · ⚪ out-of-scope · ⚫ not-started. See [README.md](README.md).

## Vtable (47 slots — only leaf overrides documented; base slots inherited from TPane / TButtonPane-with-DEF intermediate)

| Slot | Method (src) | Retail addr | Our source | Status | Notes |
|---:|:---|:---|:---|:---|:---|
| 0 | `Initialize` | 0x005339b0 | `src/death.cpp:36` | 🟡 partial | Body decompiled in `recon/discovered/cls_0x5b93c4_TDeathPane_Initialize_5339b0.cpp`. Calls base init at 0x434e40 (TButtonPane-intermediate), creates 3 buttons via cls_0x5a3c68 ctor with labels Restart/Load/Exit. Emits `"Trouble_initializing_Death_pane"` (via parent screen) if death.dat fails. |
| 4 | `Close` | 0x00533c00 | `src/death.cpp:59` | 🟡 partial | Thin wrapper: calls cls_0x5b98b8::virt_meth_0x434f30 (TButtonPane intermediate Close). |
| 80 | `DrawBackground` | 0x00533c10 | `src/death.cpp:73` | 🟡 partial | IsDirty guard then Put `Bitmap("background")` of death.dat. |
| 124 | scalar-dtor | 0x00533da0 | implicit | ⚫ not-started | Standard MSVC scalar-deleting dtor wrapper. |
| (ctor) | `TDeathPane` | 0x00533cc0 | `src/death.h:22` | 🟡 partial | Sets pane to 640x480 = FRAMEMAPPANE dims, then routes vftptr through TPane base → final TDeathPane vtable. |
| (dtor) | `~TDeathPane` | 0x00533d20 | implicit | ⚫ not-started | |

All other slots (3, 5, 6, 7, 8, 9, 10..15, 16, 17, 18, 19..30, 31..184 except 184) are inherited from the TPane (cls_0x5a4494) and TButtonPane-with-DEF (cls_0x5a45c8) bases — see [TPane.md](TPane.md) and the eventual TButtonPane intermediate port-status.

## Struct

408-byte instance + small extension (need DumpVtable on cls_0x5b93c4 to get the full struct definition). Inferred field layout (from Initialize / Close / DrawBackground bodies): standard TPane base fields at 0x0..0x88, TButtonPane intermediate fields at 0x88..0x148, then a small `deathdata` pointer (likely at mbr_0x180 by analogy with TScrollPane/TBookPane — needs confirmation).

Global instance at DAT_0066f500 (single, statically-allocated).

## Visual test plan

- `--test=ui-death`: render the death screen with a synthesized TPlayer in dead state. Compare against retail screenshot (per CLASSIC_HUD_REFERENCE §6).
- Validate the 3 buttons (Restart/Load/Exit) hit-test and dispatch correctly.

## Open questions

1. Field layout at 0x180 (deathdata pointer) needs confirmation via DumpVtable + struct extraction.
2. The 0x148-byte button ctor (cls_0x5a3c68) is the retail TButton — already noted as the shared button factory across TDeathPane / TLogoScreen / TQuickSpellPane / etc. Identifying cls_0x5a3c68 = TButton at the class level is a sibling cleanup task.
