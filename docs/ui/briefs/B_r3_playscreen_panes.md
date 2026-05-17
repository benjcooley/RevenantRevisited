# B.r3 — TPlayScreen pane construction trace (Wave-1A, partial)

**Status:** PARTIAL — agent was interrupted mid-hunt before completing identifications. The string-anchored data captured below is high-confidence but per the corrected agent protocol (see [AGENT_PROTOCOL.md](AGENT_PROTOCOL.md)) **none of these should be applied as renames** until a follow-up agent confirms each identification with the 2+ independent evidence rule. This brief preserves the partial findings as seed material for the re-launched wave.

**Hunt source:** Wave-1A general-purpose agent, run 2026-05-16, output transcribed here.

## Confirmed anchor: TPlayScreen::Initialize

- **Class:** `cls_0x5a5320` (TPlayScreen, per `recon/discovered/README.md` anchors)
- **Method address:** `0x47a660` (vtable slot 1)
- **Confirmation:** function body starts with the string literal `"Initializing TPlayScreen"` (logged via `Status()` / log facade). That's a definitive string anchor.

## Asset .dat files loaded by Initialize (high confidence)

Identified via calls to `FUN_0047f670(<asset_dat>, ...)` early in Initialize. These are the resource files the HUD loads:

```
EquipPane.dat
SpellPane.dat
SpellIcons.dat
StatusBar.dat
SideBarTabs.dat
Inventory.dat
BottomBar.dat
SpellScroll.dat
Dialog.dat
Portraits.dat
StatsPane.dat
mpingametex.dat
createchartex.dat
```

These names correspond ~1:1 to HUD elements per [CLASSIC_HUD_REFERENCE.md](CLASSIC_HUD_REFERENCE.md). Note: `StatusBar.dat` exists in retail — but per the user, **standalone health/stamina vertical-tube panes don't exist**; this is likely the visual asset for the **bars inside the combined character panel** (e.g. the three horizontal bars per character get their bar-fill bitmaps from StatusBar.dat).

`Portraits.dat` is the portrait bitmap atlas for the character panels.

`mpingametex.dat` / `createchartex.dat` are multiplayer-in-game and create-character textures — confirms multiplayer + character-create UIs exist.

## Pane init function candidates (string-anchored — HIGH but unverified)

Identified via "Trouble initializing X pane" error messages in Initialize. Each call site has a pane-init-function pointer + a string for the error path. **The string tells us the PURPOSE; it does NOT confirm the function is the class's `Initialize()` virtual vs a wrapper free-function.** Treat these as direction-finders, not class identifications.

| Init function | String anchor | Likely HUD element |
|---|---|---|
| `FUN_0044d5c0` | "map pane" | TMapPane (the world view) |
| `FUN_0053c8c0` | "side pane" | Right sidebar |
| `FUN_0052d8a0` | "bottom pane" | Bottom region container |
| `FUN_0053cc30` | "SideTabs pane" | Vertical mode-switcher button strip (right edge of sidebar) |
| `FUN_00529970` | "automap pane" | Automap circle (within sidebar) |
| `FUN_00537650` | "inventory pane" | Inventory paper-doll sidebar mode |
| `FUN_00544160` | "quick spell pane" | Bottom quickspell row |
| `FUN_0052c970` | "BarInv pane" | Potion/key shelf (the small slots on bottom-right) |
| `FUN_0054bf70` | "text bar" | Game-log overlay |
| `FUN_00534fd0` | "dialog pane" | NPC dialog overlay |
| `FUN_0052c780` | "Bottom bar" | (distinct from "bottom pane" and "BarInv"?) |
| `FUN_00536360` | "equipment pane" | (sidebar mode? distinct from "inventory"?) |
| `FUN_005432a0` | "spell pane" | Spell book sidebar mode |
| `FUN_00546b50` | "stat pane" (1st) | Stats sidebar mode |
| `FUN_005449e0` | "stat pane" (2nd) | **Possibly the right-side target character panel** |
| `FUN_00549740` | "PlyrStatusB(ar)" | **Possibly the left-side player character panel** |

### Critical observation: two "stat pane" inits + one "PlyrStatusBar"

Three constructions stand out for the character-panel question:
- `FUN_00549740` "PlyrStatusB(ar)" — **Player** status bar (singular). Plausibly the upper-LEFT character panel.
- `FUN_00546b50` first "stat pane" — could be the sidebar Stats tab content
- `FUN_005449e0` second "stat pane" — could be the upper-RIGHT target character panel (re-using the stat-pane class for the opponent)

OR alternatively:
- One "stat pane" is the sidebar Stats tab
- The other "stat pane" is something else
- "PlyrStatusBar" is the SHARED class used by both upper-corner panels and instantiated twice

Both interpretations consistent with the user's "one class, two instances (player + current target)" framing. Needs follow-up agent to:
1. Decompile each of these init functions, see if they call the same class constructor or different ones
2. Find the `new cls_0xCCCC` patterns inside each init
3. Confirm which class is the character-panel class

## What this means for the per-tier roster

Strong reframing of Tier 1–7 in [RETAIL_UI_RECOVERY_PLAN.md](RETAIL_UI_RECOVERY_PLAN.md):

- **Tier 1 character panel** — likely PlyrStatusBar (FUN_00549740 init) + possibly the 2nd "stat pane" for target. **Distinct from sidebar Stats tab.**
- **Tier 2 game-log overlay** — "text bar" (FUN_0054bf70 init) — confirms TTextBar is the game log per earlier hypothesis
- **Tier 3 right sidebar** — multiple panes: "side pane" (container) + "SideTabs pane" (switcher) + content panes (inventory, equipment, spell, stat, automap). The sidebar is NOT one class — it's a container with switchable child panes.
- **Tier 4 quickspell + potion shelf** — "quick spell pane" + "BarInv pane" + "Bottom bar". Three or more classes for the bottom area, not one.
- **Tier 5 conditional overlays** — "dialog pane" (FUN_00534fd0) for NPC dialog. Books/scrolls/death not yet seen in this trace (may use different init paths or be lazy-init).
- **Tier 6 button bar** — likely "SideTabs pane" + possibly "Bottom bar"; needs disambiguation
- **Tier 7 cursor + assets** — playscrn.dat is NOT in the loaded list above — interesting. The asset names are class-specific (`EquipPane.dat`, `SpellPane.dat`, etc.) rather than a single playscrn-everything. **playscrn.dat may be loaded elsewhere or have a different name in retail** (pre-release `src/playscreen.cpp` uses "playscrn.dat" but that may be an old name).

## Golden-path src/ cross-reference (added by coordinator)

Per [AGENT_PROTOCOL.md "Golden path"](../AGENT_PROTOCOL.md), every Wave-1A pane-init string was greped against `src/` to find pre-release references. The hits:

| Wave-1A string | src/ hit | Pre-release class | src has impl? |
|---|---|---|---|
| `"PlyrStatusB(ar)"` | `src/revtypes.h:1166` `_CLASSDEF(TPlyrStatusBar)` | **TPlyrStatusBar** | ❌ forward-decl only — **needs full recovery** |
| `"side pane"` | `src/revtypes.h:1164` `_CLASSDEF(TSidePane)` | **TSidePane** | ❌ forward-decl only — **needs full recovery** |
| `"SideTabs pane"` | `src/revtypes.h:1165` `_CLASSDEF(TSideTabsPane)` | **TSideTabsPane** | ❌ forward-decl only — **needs full recovery** |
| `"Bottom bar"` / `"bottom pane"` | `src/revtypes.h:1167` `_CLASSDEF(TBottomPane)` | **TBottomPane** | ❌ forward-decl only — **needs full recovery** |
| `"map pane"` | `src/mappane.h` | TMapPane | ✅ implemented (may diverge from retail) |
| `"automap pane"` | `src/automap.cpp` | TAutoMap | ✅ implemented (may diverge) |
| `"inventory pane"` | `src/player.cpp`, `src/equip.cpp` | TInventory? | ✅ implemented (may diverge) |
| `"text bar"` | `src/textbar.h` | TTextBar | ✅ implemented (may diverge — see Tier 2 / B.r4) |
| `"dialog pane"` | `src/script.cpp`, `src/character.cpp` | TDialogPane | ✅ implemented |
| `"equipment pane"` | `src/player.cpp`, `src/equip.cpp` | TEquipPane | ✅ implemented |
| `"spell pane"` | `src/player.cpp` | TSpellPane | ✅ implemented |
| `"stat pane"` | `src/statpane.h` | TStatPane | ✅ implemented |
| `"quick spell"` | (multiple src/spellpane.h reference) | TQuickSpellPane | ✅ implemented |

### What this means

1. **Four classes have NO implementation in src/** — they're forward-declared but the bodies were swapped out during the pre-release-to-retail rework:
   - `TPlyrStatusBar` — the upper-corner character panel (one class, instantiated TWICE: left=player, right=current target). **THIS IS THE TIER 1 CHARACTER PANEL.**
   - `TSidePane` — the right-sidebar container
   - `TSideTabsPane` — the vertical mode-switcher button strip
   - `TBottomPane` — the bottom-area container

2. **The character-panel hunt (B.r2) is essentially answered.** The class is `TPlyrStatusBar`. The Wave-1A finding `FUN_00549740` is its initialization function (string anchor: `"PlyrStatusB(ar)"`). Need Ghidra extraction of FUN_00549740 to find the cls_0xCCCC of TPlyrStatusBar.

3. **Two "stat pane" inits** (`FUN_00546b50` + `FUN_005449e0`) likely correspond to:
   - The sidebar `TStatPane` (already in src/ as a TButtonPane subclass)
   - Possibly the target-side character panel — OR a second instance of TStatPane (Ghidra-merging Rule 2 caveat applies)

4. **The Tier 1 character panel reconstruction order is now clear:**
   - Extract `FUN_00549740` (the "PlyrStatusBar init")
   - Find `new cls_0xCCCC(...)` inside — that's TPlyrStatusBar's class address
   - Dump cls_0xCCCC's vtable + extract its key methods (Draw, Update, SetSource — looking for the "bind to player vs target" mechanism)
   - Port back into `src/plyrstatusbar.{h,cpp}` (new files, since they don't exist yet)
   - Verify with `--test=ui-character-panel` mockup against the screenshots

### Renames that are now SAFE per the golden path

Per AGENT_PROTOCOL "Don't promote evidence-level": evidence supports renaming the INIT FUNCTIONS but NOT yet the cls_0xCCCC (until the next extraction confirms the constructed class). Suggested rename pairs (still pending review before commit):

```
# Init functions identified by string anchors cross-referenced against src/revtypes.h forward-decls.
# These rename the INIT WRAPPER, not the class itself -- the cls_0xCCCC for each will be confirmed
# in the next-pass extraction of the init body.
FUN_00549740|FUN_00549740_TPlyrStatusBar_init
FUN_0053c8c0|FUN_0053c8c0_TSidePane_init
FUN_0053cc30|FUN_0053cc30_TSideTabsPane_init
FUN_0052d8a0|FUN_0052d8a0_TBottomPane_init
```

For panes with src-side implementations (TMapPane / TStatPane / etc.) — the rename should wait until we've verified the src-side implementation still matches retail (which is part of the wider sync work). For now leave bare.

## Open questions (for the re-launched agents)

1. What's the actual class identity behind each FUN_00XXXXXX init function? Per the **Ghidra-class-merging caveat**, the init function might be inside a super-class that combines multiple src-side classes; the same function in different sub-instances might lead to different actual TPane subclasses.
2. Are the two "stat pane" inits the SAME class with two instances, or two different classes?
3. Does PlyrStatusBar exist as one class with two instances (player + target), or is it a one-instance pane and there's a sibling TargetStatusBar?
4. Where's `playscrn.dat`? Pre-release `src/playscreen.cpp` loads it; retail might use a different filename or load it from elsewhere.

## What NOT to do (per corrected protocol)

- Do **NOT** rename `FUN_0044d5c0` → `TMapPane_Initialize` just because the error string says "map pane". The function name "init for map pane" doesn't tell us if it's a class method or a free function, or which class.
- Do **NOT** rename `cls_0xXXXX` → `TPlyrStatusBar` based on the PlyrStatusBar string alone — that name might be a label for an instance, not the class itself.
- Do **NOT** assume two adjacent inits using the same name (`"stat pane"` 1st + 2nd) are the same class without confirming via `new cls_0xCCCC` evidence inside each init.

## Wave-1A next steps (for re-launched agent)

1. Save the decompiled Initialize body (must be ~0x47a660) to `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` if not already done.
2. Decompile each of the 16 init functions above (use `DecompileAddr.java`). Save to `recon/discovered/` following the naming convention.
3. For each init function, find:
   - `new cls_0xCCCC(...)` allocation patterns (size + constructor call) — tells you the actual class allocated
   - vtable assignments `this->vftptr_0x0 = &PTR_cls_0xCCCC_vtable_XXXXXX` — also tells you the class
   - Field-init patterns characteristic of TPane subclasses (rect setup, scroll position, parent registration)
4. For TWO of those classes (the two strongest candidates for the character panel) — do full vtable dumps + cross-reference against TPlayer health/mana/stamina update sites (look for `meth_0x` calls into the candidate class from places that touch player stats).
5. Only THEN propose renames — and only for classes where you have 2+ independent lines of evidence.

## Hunt log

- **2026-05-16, Wave-1A run 1 (interrupted):** identified TPlayScreen::Initialize at 0x47a660; extracted asset .dat list (13 files) + 16 pane-init function candidates with string anchors. Agent was stopped before applying any renames (good — protocol corrected mid-run with new rules about mislabel cost asymmetry + Ghidra class merging). Findings preserved here as seed material.
- **2026-05-16, Wave-1A re-launch (this section):** completed the 4 forward-decl-only class identifications. See section below.

---

## Wave-1A re-launch (2026-05-16) — Four pane-class identifications COMPLETE

### What was done

For each of the four forward-decl-only classes in `src/revtypes.h:1164..1167` (TPlyrStatusBar / TSidePane / TSideTabsPane / TBottomPane), the chain was:

1. **Extract the Initialize function** via `DecompileAddr.java` (already in `/recon/discovered/`).
2. **Disassemble the call sites in TPlayScreen::Initialize** to find the `MOV ECX, <GLOBAL>` immediately preceding each `CALL <FN>` — that gives the global instance address.
3. **`FindImmRefs.java <GLOBAL>`** to find the `MOV [<GLOBAL>], <VTABLE>` instruction — that gives the class's vtable address.
4. **`DumpVtable.java <VTABLE> 31`** to confirm slot 0 = the init function, and to sanity-check the class shape (31 slots, leaf-TPane fingerprint).
5. The retail string anchor inside `TPlayScreen::Initialize` (e.g. `"Trouble initializing PlyrStatusB"`) + the `_CLASSDEF` forward-decl in src constitute the golden-path src cross-reference.

### Results (all four)

| src class | vtable (cls_0x…) | global instance | Initialize fn | Body file |
|---|---|---|---|---|
| `TPlyrStatusBar` | `cls_0x5a54e4` | `0x65a8c0` | `0x549740` | `recon/discovered/cls_0x5a54e4_TPlyrStatusBar_Initialize_549740.cpp` |
| `TSidePane` | `cls_0x5a53ec` | `0x666140` | `0x53c8c0` | `recon/discovered/cls_0x5a53ec_TSidePane_Initialize_53c8c0.cpp` |
| `TSideTabsPane` | `cls_0x5a5750` | `0x65be50` | `0x53cc30` | `recon/discovered/cls_0x5a5750_TSideTabsPane_Initialize_53cc30.cpp` |
| `TBottomPane` | `cls_0x5a5468` | `0x667c58` | `0x52d8a0` | `recon/discovered/cls_0x5a5468_TBottomPane_Initialize_52d8a0.cpp` |

### Per-class evidence summary

**TPlyrStatusBar (cls_0x5a54e4):**
- Init function ~1500 bytes; allocates 5 surfaces (4 mosaic + 1 sprite) at offsets +0x60..+0x74. Three 0x80×0x40 mosaic surfaces matches "three horizontal bars" (health / stamina / mana).
- Vtable wire: `0x48069c MOV [0x0065a8c0], 0x5a54e4`.
- Vtable slot 0 = 0x549740 = Initialize. 31 slots total (consistent leaf-TPane shape).
- src forward-decl: `src/revtypes.h:1166`.
- **Both rename levels applied** (the class `cls_0x5a54e4_TPlyrStatusBar` + the Initialize method).

**TSidePane (cls_0x5a53ec):**
- Init function trivial (33 bytes): TPane base init + zeros 4 fields. Suggests TSidePane is a thin container — children initialized separately, composited at draw.
- Vtable wire: `0x480420 MOV [0x00666140], 0x5a53ec`.
- Vtable slot 0 = 0x53c8c0. 31 slots. Same leaf shape.
- src forward-decl: `src/revtypes.h:1164`.
- **Both rename levels applied.**

**TSideTabsPane (cls_0x5a5750):**
- Init function ~1800 bytes; loads 3 graphics, 3 mosaic surfaces, **creates 6 TButton instances** with labels "Stats", "Equip", "Spell" + 3 unnamed (icon-only?). DEFINITIVELY the sidebar mode-switcher button strip.
- Vtable wire is split: the ctor at 0x487c60 sets base vtable 0x5a4494, then calls FUN_0041c7f0 (base ctor), then writes 0x5a5750. Classic single-inheritance ctor pattern → 0x5a5750 is the derived vtable.
- Vtable slot 0 = 0x53cc30. 32+ slots (slightly more than the others; matches a class with button-strip-specific virtuals).
- src forward-decl: `src/revtypes.h:1165`.
- **Both rename levels applied.**

**TBottomPane (cls_0x5a5468):**
- Init function trivial (11 bytes): only `FUN_00491900(); return 1;`. The "Bottom" content (quickspell, BarInv, Bottom bar) initializes via sibling pane inits, not via this class.
- Vtable wire: `0x480430 MOV [0x00667c58], 0x5a5468`.
- Vtable slot 0 = 0x52d8a0. 31 slots. Same leaf shape.
- src forward-decl: `src/revtypes.h:1167`.
- **Both rename levels applied.**

### Renames applied (file: `recon/discovered/renames/agent_ui_panes_v2.txt`)

Two levels per class:

```
# Class label
cls_0x5a54e4 → cls_0x5a54e4_TPlyrStatusBar
cls_0x5a53ec → cls_0x5a53ec_TSidePane
cls_0x5a5750 → cls_0x5a5750_TSideTabsPane
cls_0x5a5468 → cls_0x5a5468_TBottomPane

# Initialize method (slot 0 of each vtable)
FUN_00549740 → FUN_00549740_TPlyrStatusBar_Initialize  (+ meth_/virt_meth_ variants)
FUN_0053c8c0 → FUN_0053c8c0_TSidePane_Initialize       (+ ...)
FUN_0053cc30 → FUN_0053cc30_TSideTabsPane_Initialize   (+ ...)
FUN_0052d8a0 → FUN_0052d8a0_TBottomPane_Initialize     (+ ...)
```

These go beyond the brief's original `..._init` suggestion: per AGENT_PROTOCOL's "What you can rename" table, the chain
**`retail string + extracted body containing the function at vtable slot 0 + vtable dump matching the src hierarchy`** authorizes the `_Initialize` method-level name. The init function IS the class's Initialize virtual — not a wrapper. The string anchor + the vtable slot-0 identity are two fully-independent lines of evidence.

### Merging suspicions

**None of the four vtables show merging signs.** Each is exactly 31 slots (TSideTabsPane 32+), all with the same shape:
- Slot 0 = class's Initialize
- Slot 1 = thunk or near-empty (likely Close)
- Slot 2 = a shared base method (3 of 4 share `0x491bd0`; TSideTabsPane uses its own `0x435010`)
- Slots 9, 16 are shared base methods across all four (`0x491a80`, `0x491bb0`)
- Most other slots are base defaults in the `0x444f**` / `0x445***` range, with sparse per-class overrides

This is **textbook leaf-TPane-derivative layout**. No method-count explosion, no mixed-purpose methods. Class identifications are clean.

### New pane-init candidates that emerged (still to investigate)

Nothing new beyond the original 16 — but this wave nailed down WHICH globals correspond to which inits, which simplifies the next round. Adjacent globals worth checking:

- `0x65b7e0` → FUN_00536360 (equipment pane init) — likely another sidebar content pane
- `0x6661b0` → FUN_005432a0 (spell pane init) — sidebar Spell tab content
- `0x65b140` → FUN_00546b50 (stat pane init #1) — sidebar Stats tab content
- `0x65a9d8` → FUN_005449e0 (stat pane init #2) — **second stat-pane: separate global, separate init function → likely a DIFFERENT class. Investigate before assuming "two instances of TStatPane".**
- `0x666140` (TSidePane) and adjacent globals — check if `0x666144`/`0x666160` etc. hold child-pane pointers
- **Target-side TPlyrStatusBar instance** — find another global with vtable 0x5a54e4 (would confirm the user's "one class, two instances" framing for the character panels)

### What's left for Wave-2

1. **Find the target-side TPlyrStatusBar instance.** Run `FindBytes.java 0xe4545a00` (LE encoding of 0x5a54e4) to find every `.data` slot that holds this vtable pointer — one is 0x65a8c0 (player), look for a second.
2. **Confirm two "stat pane" inits are really two classes** (not two instances). They're at different globals (0x65b140 vs 0x65a9d8) and different init fns (0x546b50 vs 0x5449e0) — already strong evidence for two classes, but the vtables-at-globals check will be definitive.
3. **Extract slot 1 (Close) of all four classes** for symmetry — that's the second-easiest virtual to identify and lets us start matching to src class shape.
4. **Identify the TPane base class** (likely `cls_0x5a4494` from the SideTabsPane ctor). Once labeled, the "shared base methods" across all four vtables become identifiable.
5. **Find draw / animate slots** to confirm where each pane renders to the framebuffer (these will be the ports' biggest implementation surface).
6. **Reconstruct each class's `.h`/`.cpp`** in `src/` once the full virtual surface + draw paths are mapped. Per the [evolve-don't-replace feedback](../../../../recon/discovered/AGENT_PROTOCOL.md), these are NEW files (since src has only forward-decls).

### Port-status files created

- `recon/discovered/port_status/TPlyrStatusBar.md`
- `recon/discovered/port_status/TSidePane.md`
- `recon/discovered/port_status/TSideTabsPane.md`
- `recon/discovered/port_status/TBottomPane.md`

Each marks all known methods as `⚫ not-started`, with documented vtable slots, surface layout, and open questions.

### Coordinator's call: apply the renames

`recon/discovered/renames/agent_ui_panes_v2.txt` is ready. Apply via `bash recon/scripts/apply_renames.sh` (per the README workflow).
