# B.r5 — Bottom-area pane cluster: QuickSpell / BarInv / BottomBar (Wave-2A)

**Status:** COMPLETE — three class identifications applied with 3+ lines of evidence each.

**Bottom line up front:**

1. **TQuickSpellPane** (the 4-slot spell-ring strip at the bottom) is **`cls_0x5a5a30`** — a TButtonPane subclass, matches `src/spellpane.h`'s `class TQuickSpellPane : public TButtonPane`.
2. **TBarInvPane** (a NEW retail class, no pre-release decl) is **`cls_0x5a56d4`** — the potion/key slot row, leaf-TPane shape, loads `"BarInvBox"` sprite.
3. **TBottomBarPane** (also NEW, distinct from the Wave-1A `TBottomPane` container) is **`cls_0x5a5808`** — a TButtonPane subclass; loads `"UtilityBar"` + `"BarEndCap"` sprites and likely hosts the menu/action buttons that sit between the spell-ring and the potion slots.

All three sit physically inside the `TBottomPane` container (Wave-1A: `cls_0x5a5468`, trivial init), wired up by `TPlayScreen::Initialize` as three sibling globals after the container.

Wave-2A also resolved the **real TTextBar leaf class** (`cls_0x5a5560`) — that lives in B.r4's appended section, not here.

---

## Per-class evidence

### TQuickSpellPane (`cls_0x5a5a30`)

| Aspect | Value |
|---|---|
| Retail string anchor | `"Trouble initializing quick spell pane"` @ `0x5d72b0` |
| Call site in TPlayScreen::Initialize | `0x47abab` (`MOV ECX, 0x65c6f8; CALL 0x544160`) |
| Global instance | `0x65c6f8` |
| Vtable wire | ctor stub at `0x00480ad0`: `MOV ECX, 0x65c6f8; CALL FUN_00488580` |
| Constructor body (`FUN_00488580`) | sets base vtable `0x5a4494`, rect `0x1a4×0x3c` (420×60), calls `FUN_0041c7f0(0x10,0x10)` (TButtonPane base ctor), writes derived vtable `0x5a5a30` |
| Vtable @ `0x5a5a30` | 35 slots, **TButtonPane shape** (slot 1 = `0x434f30`, slot 2 = `0x435010` — NOT TPane's `0x491970`/`0x491bd0`) |
| Slot 0 (Initialize) | `0x544160` |
| Init body | allocates 4× `0x150`-byte entries (vtable `&PTR_FUN_005b9c54`), each loaded with **`RingU` / `RingD` / `RingG`** sprites (up/down/glow spell-ring graphics), registered via `FUN_00436ae0` |
| src cross-reference | `src/spellpane.h: class TQuickSpellPane : public TButtonPane` with `QUICKSPELLX/Y/WIDTH/HEIGHT` rect template — matches retail's 420×60 + TButtonPane base |
| Confidence | **98%** — 3+ lines of evidence (string + vtable shape + src forward-decl + RingU/D/G sprite anchors) |

The `0x150`-byte sub-entries are likely `TQuickSpellEntry` (the per-button data records) — pre-release `spellpane.h` has a sibling helper class for these; needs comparison against `src/spellpane.{h,cpp}` to identify the matching pre-release type if any.

Extraction file: `recon/discovered/FUN_00544160_TQuickSpellPane_init.cpp`
Constructor file: `recon/discovered/cls_0x5a5a30_TQuickSpellPane_ctor_488580.cpp`

### TBarInvPane (`cls_0x5a56d4`)

| Aspect | Value |
|---|---|
| Retail string anchor | `"Trouble initializing BarInv pane"` @ `0x5d72dc` |
| Call site in TPlayScreen::Initialize | `0x47abd4` (`MOV ECX, 0x65b028; CALL 0x52c970`) |
| Global instance | `0x65b028` |
| Vtable wire | **direct** static-init write at `0x00480ce8` and `0x00480d00`: `MOV dword ptr [0x0065b028], 0x5a56d4` |
| Vtable @ `0x5a56d4` | 31 slots, **leaf-TPane shape** (slot 1 = `0x491970`, slot 2 = `0x491bd0`, slot 9 = `0x491a80`, slot 16 = `0x491bb0` — same as TBottomPane / TSidePane / TPlyrStatusBar from Wave-1A) |
| Slot 0 (Initialize) | `0x52c970` |
| Init body (129 bytes) | calls `FUN_00491900()` (TPane base init), loads **`"BarInvBox"`** sprite, configures slot-grid count `(DAT_0065b644 - 0xdc) / 0x2d` (screen-width-derived slot count), calls some method at offset `+0x20` and `+0x2c` of own vtable (state setup) |
| src cross-reference | **NO** pre-release `_CLASSDEF` — NEW retail class. Class name inferred from the retail error string `"BarInv pane"` |
| Confidence | **98%** for class identity, **~85%** for the name "TBarInvPane" specifically (could equally be `TBarInventoryPane`, `TItemBarPane`, …) |

This is the small-slot strip that holds the **potions / keys / consumables** (the row of icon boxes between the spell ring and the side-bar) per `CLASSIC_HUD_REFERENCE.md`. The width-derived slot count is consistent with the layout absorbing resolution differences — fits the project's resolution-modes doc.

Extraction file: `recon/discovered/FUN_0052c970_BarInv_init.cpp`

### TBottomBarPane (`cls_0x5a5808`)

| Aspect | Value |
|---|---|
| Retail string anchor | `"Trouble initializing Bottom bar"` @ `0x5d7344` |
| Call site in TPlayScreen::Initialize | `0x47ac72` (`MOV ECX, 0x65b638; CALL 0x52c780`) |
| Global instance | `0x65b638` |
| Vtable wire | ctor stub at `0x00480550`: `MOV ECX, 0x65b638; CALL FUN_00487da0` |
| Constructor body (`FUN_00487da0`) | **BYTE-IDENTICAL** to TQuickSpellPane ctor at `0x00488580` except for the final vtable address. Sets base vtable `0x5a4494`, rect `0x1a4×0x3c` (420×60), calls `FUN_0041c7f0(0x10,0x10)` (TButtonPane base ctor), writes derived vtable `0x5a5808` |
| Vtable @ `0x5a5808` | 35+ slots, **TButtonPane shape** (slot 2 = `0x435010`, like TQuickSpellPane + TSideTabsPane) |
| Slot 0 (Initialize) | `0x52c780` |
| Init body (59 bytes) | calls `FUN_00434e40()` (likely TButtonPane base init), loads **`"UtilityBar"`** sprite into `this+0x180`, loads **`"BarEndCap"`** sprite into `this+0x184`. The actual buttons must be registered through a different path (possibly base-class auto-registration via the ctor's `FUN_0041c7f0` call, or via a sibling method not yet found) |
| src cross-reference | **NO** pre-release `_CLASSDEF(TBottomBarPane)`. The "Bottom bar" string in `revtypes.h:1167` corresponds to `TBottomPane` — but TBottomPane (cls_0x5a5468 from Wave-1A) is the CONTAINER with a trivial Initialize. This pane is something else, sitting inside / alongside the container. NEW retail class. |
| Confidence | **98%** for class identity (distinct from `cls_0x5a5468`, confirmed by separate vtable + separate global + separate init); **~85%** for the name "TBottomBarPane" specifically |

This is the **utility / action button row** that sits across the bottom (action buttons + decorative end-caps). Per `CLASSIC_HUD_REFERENCE.md`, the bottom area has multiple banded sub-regions — TBottomPane (container) + TQuickSpellPane (spell ring) + TBarInvPane (potion slots) + TBottomBarPane (utility/action buttons). All four classes are now identified.

Extraction files:
- `recon/discovered/FUN_0052c780_BottomBar_init.cpp`
- `recon/discovered/cls_0x5a5808_TBottomBarPane_ctor_487da0.cpp`

---

## Combined bottom-area architecture map (Wave-1A + Wave-2A)

```
TBottomPane (cls_0x5a5468)              <- Wave-1A; trivial init; CONTAINER
 ├── TQuickSpellPane (cls_0x5a5a30)     <- Wave-2A; TButtonPane subclass; 420×60; 4 spell-ring buttons (RingU/D/G)
 ├── TBarInvPane     (cls_0x5a56d4)     <- Wave-2A; leaf TPane; potion/key slot strip; "BarInvBox" sprite
 ├── TBottomBarPane  (cls_0x5a5808)     <- Wave-2A; TButtonPane subclass; 420×60; "UtilityBar"/"BarEndCap" sprites
 └── TTextBar        (cls_0x5a5560)     <- Wave-2A (B.r4 appendix); leaf TPane; 406×198 transparent overlay band; 3 mosaic surfaces
```

Note: TTextBar is initialized as a sibling to the four bottom-area panes in `TPlayScreen::Initialize`, but visually it overlays the top half of the screen (the game-log/scrollback area), not the bottom. The sibling-in-init relationship is purely sequential, not spatial.

---

## Candidates with weak evidence (NOT in renames)

| Symbol | Tentative ID | Why deferred |
|---|---|---|
| `cls_0x5a4494` (base vtable used by all three ctors here) | TPane base | High confidence by use pattern, but Wave-2A only TOUCHED this address (didn't dump or fingerprint). Wave-2B is supposed to do TPane base identification — deferring there. |
| `FUN_0041c7f0` (called by both TQuickSpellPane and TBottomBarPane ctors with args `0x10, 0x10`) | TButtonPane base ctor | Likely correct, but unverified. The `0x10, 0x10` args could be button-cell width/height. |
| Specific overridden vtable slots of TQuickSpellPane / TBarInvPane / TBottomBarPane | per-method virtuals (Draw, Update, ButtonClicked, …) | Identification by signature pending — Wave-2A focused on class IDs, not per-method mapping. |
| `0x150`-byte sub-entries inside FUN_00544160 (vtable `&PTR_FUN_005b9c54`) | TQuickSpellEntry or similar per-button record class | Likely a sibling helper class in `src/spellpane.h`, but the name and class identity need a separate small extraction. |

---

## Merging suspicions (Rule 2)

None for these three classes — all three vtables are clean shapes:

- **TBarInvPane** (cls_0x5a56d4) = pure leaf-TPane (same shape as the four Wave-1A panes)
- **TQuickSpellPane** (cls_0x5a5a30) = TButtonPane shape (matches TSideTabsPane from Wave-1A, which also derives from TButtonPane)
- **TBottomBarPane** (cls_0x5a5808) = TButtonPane shape; ctor is **byte-identical** to TQuickSpellPane's ctor except for the derived vtable address, which is strong structural evidence they share a base class

Method counts (35-ish) are reasonable for TButtonPane subclasses (more virtuals than a pure TPane leaf because of button-related additions); no method-count explosion suggests merging.

---

## Hunt log

**2026-05-16 (Wave-2A):**

- Read `AGENT_PROTOCOL.md`, `recon/discovered/README.md`, B.r3, B.r4. Inherited Wave-1A's pane-init protocol and Wave-1C's TTextBar partial.
- Extracted `FUN_00544160`, `FUN_0052c970`, `FUN_0052c780` via `DecompileAddr.java`. Confirmed FUN_0052c970 + FUN_0052c780 are thiscall methods (param_1 = this); FUN_00544160 is a free-style helper that allocates 4 sub-objects.
- Disassembled `TPlayScreen::Initialize @ 0x47a800..0x47ad00` via `Disasm.java`. Captured the 4 `MOV ECX, <GLOBAL>; CALL <INIT>` pairs (globals: 0x65c6f8 / 0x65b028 / 0x65c5d0 / 0x65b638).
- For each global: `RefsTo.java` to find WRITE-mode references.
  - `0x65c5d0` (TTextBar): one WRITE at `0x00480725` — global ctor stub doing direct vtable assignment + rect init.
  - `0x65b028` (TBarInvPane): two WRITES at `0x00480ce8` + `0x00480d00` — both direct `MOV [global], 0x5a56d4` (idempotent init).
  - `0x65c6f8` (TQuickSpellPane) + `0x65b638` (TBottomBarPane): NO direct WRITES — vtable wired via a constructor invoked through `MOV ECX, <global>; CALL <ctor>` sequences in the same 0x00480000-region.
- Decompiled the relevant ctor stubs (`FUN_00488580`, `FUN_00487da0`) — both single-inheritance ctor pattern: write base vtable → call base-class ctor → write derived vtable. The two are byte-identical except for the derived address, confirming they share a base class (TButtonPane).
- Dumped vtables for all four leaf classes (`DumpVtable.java 0x5a5560/0x5a5a30/0x5a56d4/0x5a5808 35`). Slot 0 of each = the corresponding init function. Shape analysis:
  - cls_0x5a5560 (TTextBar) + cls_0x5a56d4 (TBarInvPane) = leaf-TPane shape (slot 2 = 0x491bd0)
  - cls_0x5a5a30 (TQuickSpellPane) + cls_0x5a5808 (TBottomBarPane) = TButtonPane shape (slot 2 = 0x435010)
- Extracted TTextBar slot 1 (`FUN_0054c3d0`) — confirmed `TTextBar::Close` (frees the 3 mosaic surfaces + text buffer that Initialize sets up).
- Wrote `agent_ui_wave2_textbar_bottom.txt` (renames) + this brief + appended Wave-2A section to B.r4 + updated `port_status/TTextBar.md` + created `port_status/TQuickSpellPane.md` and `port_status/TBarInvPane.md`.

**Time on task:** ~25 min wall-clock. One Ghidra lock collision early; resolved by clearing stale `.lock` files.

**Pending for Wave-3+:**

- Per-method extraction for TTextBar slots 7/13/19/20/23/28 — should yield Print / Clear / Pulse / Animate / DrawBackground / SetHealthDisplay / SetLevels / ClearHealthDisplay.
- Per-method extraction for TQuickSpellPane / TBarInvPane / TBottomBarPane custom virtuals (slot ranges identified in this brief).
- TPane base identification (cls_0x5a4494) — flagged for Wave-2B per the dispatch description.
- TButtonPane base identification — likely sibling of cls_0x5a4494 once TPane is locked.
- Port back to `src/` once the per-class virtual surface is mapped — TBarInvPane and TBottomBarPane are NEW classes; need new `src/barinvpane.{h,cpp}` and `src/bottombarpane.{h,cpp}` files.
