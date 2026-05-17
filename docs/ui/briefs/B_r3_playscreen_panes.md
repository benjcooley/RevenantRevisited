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
- **Next:** re-launched wave with updated AGENT_PROTOCOL.md guidance.
