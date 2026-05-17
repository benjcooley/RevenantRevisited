# TDeathScreen port status

Retail vtable: **`cls_0x5b9374`** (TScreen subclass — single member `cls_0x5a5ed4_TScreen` at offset 0, length 112). Identified by Wave-4B (2026-05-16) — see [B.r12 brief](../../../docs/ui/briefs/B_r12_oog_closeout.md) §B.r17.

**No pre-release equivalent.** TDeathScreen is a retail-era addition. In `src/death.cpp` the TDeathPane is `AddPane`'d directly onto PlayScreen with no intervening screen wrapper; retail factored out the screen-lifecycle (asset load + pane init dispatch) into a TScreen subclass that owns the global TDeathPane instance at DAT_0066f500.

## Responsibilities

1. Load `death.dat` into a global TMulti slot (DAT_0066f6f4) via FUN_0047f670("death.dat", ...).
2. Invoke TDeathPane::Initialize on the global TDeathPane (cls_0x5b93c4) instance at DAT_0066f500.
3. On failure (TDeathPane::Initialize returns 0) emit Status `"Trouble initializing Death pane"`.
4. (Likely also handles Close / Animate / Pulse via TScreen's base virtuals — slot identification pending vtable dump.)

## Vtable (only Initialize identified so far)

| Slot | Method (inferred) | Retail addr | Status | Notes |
|---:|:---|:---|:---|:---|
| 0 | (TScreen vtable slot — needs confirm) | 0x005338a0 | 🟡 partial | Body in `recon/discovered/cls_0x5b9374_TDeathScreen_Initialize_5338a0.cpp`. Loads death.dat, invokes TDeathPane::Initialize, prints Status on fail. |
| ? | ctor | 0x00533c60 | 🟡 partial | Trivial: chains to TScreen ctor and sets vftptr to cls_0x5b9374's vtable. Body in `recon/classes_original/cls_0x5b9374.cpp`. |
| ? | scalar-dtor | 0x00533c90 | ⚫ not-started | |

**Next step:** DumpVtable @ 0x5b9374 to enumerate full slot layout + confirm TScreen base inheritance. Cross-reference against cls_0x5a5ed4_TScreen vtable.

## Global instance

Wired at runtime in TLogoScreen / TPlayScreen activation paths (need to grep call sites). The class only needs one instance per running game.

## Visual test plan

Coupled to TDeathPane (see [TDeathPane.md](TDeathPane.md) test plan). No independent visual test mode is meaningful.

## Open questions

1. Full vtable layout.
2. Activation path: who instantiates TDeathScreen? Most likely the player-death dispatcher in TPlayer or TPlayScreen — needs cross-reference grep on DAT_0066f500 / FUN_005338a0.
3. Whether the class is named `TDeathScreen` in the lost retail source or has a different name (e.g. `TDeathMenu`, `TGameOverScreen`). Conservative naming used pending user confirmation.
